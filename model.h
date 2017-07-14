#ifndef MODEL_H
#define MODEL_H
#include "general.h"
#include "statistics.h"

////////////// Model related
//pratio
extern double ModelPratio(int *config, int *flips, int num_flips);
//Do if accecpted.
extern void ModelAccept(int *config, int *flips, int num_flips, Statistics *st);
//Do if rejected
extern void ModelReject(int *config, int *flips, int num_flips, Statistics *st);
//Measuring the value of the local energy
//on the current state
extern void ModelMeasure(int *config, double complex *energy, int num_energy, Statistics *st);
#endif //MODEL_H
