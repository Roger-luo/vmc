#ifndef RANDLIB_H
#define RANDLIB_H
#include "general.h"

//Uniform random number in [0,1)
extern inline double Uniform01();
extern inline int RandomInt(int max_int);
//Seed random number generator.
extern inline void RandomSeed();

//Random spin flips (max 2 spin flips in this implementation)
//if mag0=true, when doing 2 spin flips the total magnetization is kept equal to 0
extern inline void SpinFlipPropose(int *config, int num_spin, int *flips, int num_flips, bool mag0);

//Initializes a random spin state
//if mag0=true, the initial state is prepared with zero total magnetization
extern inline int *GetRandomConfig(const int num_spin, const bool mag0);
#endif // RANDLIB_H
