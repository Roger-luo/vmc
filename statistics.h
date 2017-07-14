#ifndef STATISTICS_H
#define STATISTICS_H
#include "general.h"

///////////////// Statistic related operations
typedef struct Statistics{
    int num_accept;
    int num_moves;
} Statistics;

extern inline void StatisticsReset(Statistics *st);
extern inline double StatisticsAcceptance(Statistics *st);
extern inline Statistics *StatisticsNew();
#endif // STATISTICS_H
