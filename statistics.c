#include "statistics.h"

///////////////// Statistic related operations
inline void StatisticsReset(Statistics *st){
    st->num_accept = 0;
    st->num_moves = 0;
}

inline double StatisticsAcceptance(Statistics *st){
    return (double)(st->num_accept)/(double)(st->num_moves);
}

inline Statistics *StatisticsNew(){
    Statistics *st = malloc(sizeof(Statistics));
    st->num_accept=0;
    st->num_moves=0;
    return st;
}
