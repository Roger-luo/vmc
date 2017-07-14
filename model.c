#include "model.h"

////////////// Model related
//pratio
double ModelPratio(int *config, int *flips, int num_flips){
    return 1.;
}

//Do if accecpted.
void ModelAccept(int *config, int *flips, int num_flips, Statistics *st){
    int i;
    for(i=0; i<num_flips; i++){
        *(config+*(flips+i))*=-1;
    }
    st->num_accept+=1;
}

//Do if rejected
void ModelReject(int *config, int *flips, int num_flips, Statistics *st){
    return;
}

//Measuring the value of the local energy
//on the current state
void ModelMeasure(int *config, double complex *energy, int num_energy, Statistics *st){
    *(energy+num_energy)=(double complex)(*(config+2));
}
