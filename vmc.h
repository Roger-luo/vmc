#ifndef VMC_H
#define VMC_H
#include "general.h"
#include "randlib.h"
#include "model.h"
#include "statistics.h"

//////////////// Monte Carlo Simulation
//Metropolis algorithm.
extern bool MPDecision(double pratio);
extern double AnalyseEnergy(double complex *energy, int num_energy,
        int num_blocks, int num_spin);
//Single Step Monte Carlo.
extern inline void MonteCarloStep(int *config, unsigned int num_spin, 
        unsigned int *flips, unsigned int num_flips,
        bool mag0, Statistics *st);
//nflipss is the number of random spin flips to be done, it is automatically set to 1 or 2 depending on the hamiltonian
extern double RunMonteCarlo(unsigned int num_sweep_bath, unsigned int num_sweeps_run,
        int *config, unsigned int num_spin, double sweepfactor, 
        unsigned int num_flips, bool mag0);
//mpi run monte carlo.
double *MultiRun(unsigned int num_sweep_bath, unsigned int num_sweeps_run, unsigned int num_spin,
        unsigned int num_task_each_core, unsigned int num_flips, bool mag0, double sweepfactor,
        int argc, char **argv);

#endif // VMC_H
