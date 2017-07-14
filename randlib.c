#include "randlib.h"

/////////////////// Random Number Generator //////////////
//Uniform random number in [0,1)
inline double Uniform01(){
    return ((double)rand())/(RAND_MAX+1.);
}

inline int RandomInt(int maximum){
    return rand()%maximum;
}

//Seed random number generator.
inline void RandomSeed(){
#ifdef USE_MPI
    int RANK;
    printf("MPI Version\n");
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    srand(time(NULL)+RANK);
#else
    printf("Non-MPI Version\n");
    srand(time(NULL));
#endif
}

//Random spin flips (max 2 spin flips in this implementation)
//if mag0=true, when doing 2 spin flips the total magnetization is kept equal to 0
inline void SpinFlipPropose(int *config, int num_spin, int *flips, int num_flips, bool mag0){
    *flips=RandomInt(num_spin);
    if(mag0){
        //flip 2 spins
        int flip_up=RandomInt(num_spin/2);
        int flip_dn=RandomInt(num_spin/2);
        //find flip1-th up spin
        int cum_up=0, cum_dn=0, i=0;
        while(cum_up<=flip_up || cum_dn<=flip_dn){
            if(*(config+i)==1){  //spin up
                if(cum_up++==flip_up){
                    *flips = i;    //store position of flip_up-th up spin.
                }
            }
            else{
                if(cum_dn++==flip_dn){
                    *(flips+1) = i;
                }
            }
            i++;
        }
    }
    else{
        //flip random num_flips spin.
        int iflip,i;
        int nf=0;
        while(nf<num_flips){
            iflip = RandomInt(num_spin-i);
            nf++;
            for(i=0; i<nf; i++){
                if(*(flips+i)==iflip){
                    nf--;
                    break;
                }
                *(flips+nf-1)=iflip;
            }
        }
    }
}

//Initializes a random spin state
//if mag0=true, the initial state is prepared with zero total magnetization
inline int *GetRandomConfig(const int num_spin, const bool mag0){
    unsigned int i, rs;
    int *config=malloc(sizeof(int)*num_spin);
    for(i=0;i<num_spin;i++){
        *(config+i)=(Uniform01()<0.5)?(-1):(1);
    }

    if(mag0){
        int magt=1;
        if(num_spin%2){
            printf("# Error : Cannot initializate a random state with zero magnetization for odd number of spins\n");
            exit(0);
        }
        while(magt!=0){
            magt=0;
            for(i=0;i<num_spin;i++){
                magt+=*(config+i);
            }
            if(magt>0){
                rs=RandomInt(num_spin);
                while(*(config+rs)<0){
                    rs=RandomInt(num_spin);
                }
                *(config+rs)=-1;
                magt-=1;
            }
            else if(magt<0){
                rs=RandomInt(num_spin);
                while(*(config+rs)>0){
                    rs=RandomInt(num_spin);
                }
                *(config+rs)=1;
                magt+=1;
            }
        }
    }
    return config;
}

