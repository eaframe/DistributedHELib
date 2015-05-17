#ifndef _DISTRIBUTED_H_
#define _DISTRIBUTED_H_

#include "NumbTh.h"
#include <mpi.h>

const int OP_SHUTDOWN = 1;
const int OP_ADD_TWO_VECTS = 2;
const int OP_SUB_TWO_VECTS = 3;
const int OP_MUL_TWO_VECTS = 4;
const int OP_ADD_ONE_VECT_ONE_NUM = 5;
const int OP_SUB_ONE_VECT_ONE_NUM = 6;
const int OP_MUL_ONE_VECT_ONE_NUM = 7;

void Startup(int argc, char **argv);
void DistributeValuesTwoVectors(int operation, long ithPrime, long row_length, long *row, long *other_row);
void DistributeValuesOneVectorOneNum(int operation, long ithPrime, long row_length, long *row, long num);
void Shutdown();
void DistributedSync();

#endif
