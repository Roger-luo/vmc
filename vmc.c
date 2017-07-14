#include "vmc.h"

//////////////// Monte Carlo Simulation
//Metropolis algorithm.
bool MPDecision(double pratio){
    if(pratio>Uniform01()){
        return true;
    }
    else{
        return false;
    }
}

double AnalyseEnergy(double complex *energy, int num_energy, int num_blocks, int num_spin){
    int blocksize=floor((double)(num_energy)/(double)(num_blocks));
    int i, j;
    double enmean=0;
    double enmeansq=0;
    double enmean_unblocked=0;
    double enmeansq_unblocked=0;

    for(i=0; i<num_blocks; i++){
        double eblock=0;
        for(j=i*blocksize;j<(i+1)*blocksize;j++){
            eblock+=creal(energy[j]);
            //Q:handle this error
            if(j>num_energy){
                printf("Error for # of energy.\n");
                exit(0);
            }

            double delta=creal(energy[j])-enmean_unblocked;
            enmean_unblocked+=delta/(double)(j+1);
            double delta2=creal(energy[j])-enmean_unblocked;
            enmeansq_unblocked+=delta*delta2;
        }
        eblock/=(double)(blocksize);
        double delta=eblock-enmean;
        enmean+=delta/(double)(i+1);
        double delta2=eblock-enmean;
        enmeansq+=delta*delta2;
    }

    enmeansq/=(double)(num_blocks-1);
    enmeansq_unblocked/=(double)((num_blocks*blocksize-1));

    double estav=enmean/(double)(num_spin);
    double esterror=sqrt(enmeansq/(double)(num_blocks))/(double)(num_spin);

    int ndigits=log10(esterror);
    if(ndigits<0){
        ndigits=-ndigits+2;
    }
    else{
        ndigits=0;
    }

    printf("# Estimated average energy per spin : %.4f+/-%.4f\n",estav,esterror);
    printf("# Error estimated with binning analysis consisting of %d bins \n",num_blocks);
    printf("# Block size is %d\n",blocksize);
    printf("# Estimated autocorrelation time is %.4f\n",0.5*blocksize*enmeansq/enmeansq_unblocked);
    return estav;
}

//Single Step Monte Carlo.
inline void MonteCarloStep(int *config, unsigned int num_spin, unsigned int *flips, unsigned int num_flips, bool mag0, Statistics *st){
    SpinFlipPropose(config, num_spin, flips, num_flips, mag0);
    double pratio=ModelPratio(config, flips, num_flips);
    //Metropolis-Hastings test
    if(MPDecision(pratio)){
        ModelAccept(config, flips, num_flips, st);
    }
    else{
        ModelReject(config, flips, num_flips, st);
    }
    st->num_moves+=1;
#ifdef DEBUG
    int i;
    for(i=0; i<num_spin; i++){
        printf("%2d",*(config+i));
    }
    printf("\n");

    int cum=0;
    for(i=0; i<num_spin; i++){
        cum+=*(config+i);
    }
    printf("mag = %d\n",cum);
#endif
}


//Run the Monte Carlo sampling
//nsweeps is the total number of sweeps to be done
//thermfactor is the fraction of nsweeps to be discarded during the initial equilibration
//sweepfactor set the number of single spin flips per sweeps to num_spin*sweepfactor
//nflipss is the number of random spin flips to be done, it is automatically set to 1 or 2 depending on the hamiltonian
double RunMonteCarlo(unsigned int num_sweep_bath, unsigned int num_sweeps_run,
        int *config, unsigned int num_spin, double sweepfactor, unsigned int num_flips, bool mag0){
    //checking input consistency
    //Q:error processing!
    if(num_flips==0){
      printf("# Error : The number of spin flips should >0\n");
      exit(0);
    }
    unsigned int *flips=malloc(sizeof(unsigned int)*num_flips);
    Statistics *st = StatisticsNew();
    unsigned int n, i, blocksize = 50;
    double complex *energy=malloc(sizeof(double complex)*num_sweeps_run);

    printf("# Starting Monte Carlo sampling\n");
    //printf("# Number of sweeps to be performed is %d\n",num_sweeps_run);

    //thermalization
    printf("# Thermalization... \n");
    StatisticsReset(st);
    for(n=0; n<num_sweep_bath; n++){
        for(i=0; i<num_spin*sweepfactor; i++){
            MonteCarloStep(config, num_spin, flips, num_flips, mag0, st);
        }
        // No Measurements
    }
    printf(" DONE \n");

    //sequence of sweeps
    StatisticsReset(st);
    printf("# Sweeping...\n");
    for(n=0; n<num_sweeps_run; n++){
        for(i=0; i<num_spin*sweepfactor; i++){
            MonteCarloStep(config, num_spin, flips, num_flips, mag0, st);
        }
        ModelMeasure(config, energy, n, st);
    }
    printf(" DONE \n");
    return AnalyseEnergy(energy, num_sweeps_run, blocksize, num_spin);
}

double *MultiRun(unsigned int num_sweep_bath, unsigned int num_sweeps_run, unsigned int num_spin,
        unsigned int num_task_each_core, unsigned int num_flips, bool mag0, double sweepfactor,
        int argc, char **argv){
    int RANK, SIZE;
    int i;
#ifdef USE_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
#else
    RANK=0;
    SIZE=1;
#endif
    double *englist_batch=malloc(sizeof(double)*num_task_each_core);
    double *englist;
    printf("SIZE = %d, num_task_each_core = %d.\n",SIZE, num_task_each_core);
    for(i=0; i<num_task_each_core; i++){
        RandomSeed();
        int *config=GetRandomConfig(num_spin, mag0);
        *(englist_batch+i)=RunMonteCarlo(num_sweeps_run, num_sweep_bath, config, num_spin, sweepfactor, num_flips, mag0);
        sleep(1);  //for debug!
    }
    if(RANK==0){
        englist=malloc(sizeof(double)*SIZE*num_task_each_core);
    }
    //Gather to ROOT = 0;
#ifdef USE_MPI
    MPI_Gather(englist_batch, num_task_each_core, MPI_DOUBLE, englist, num_task_each_core, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#else
    englist=englist_batch;
#endif
    if(RANK==0){
        printf("I Get List (SIZE = %d ) = ",SIZE);
        for(i=0; i<num_task_each_core*SIZE; i++){
            printf("%.4f ",*(englist+i));
        }
        printf("\n");
    }
#ifdef USE_MPI
    MPI_Finalize();
#endif
    return englist;
}
