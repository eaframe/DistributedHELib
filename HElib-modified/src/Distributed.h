#ifndef _DISTRIBUTED_H_
#define _DISTRIBUTED_H_

#include <mpi.h>

const int OP_SHUTDOWN = 1;

void Startup(int argc, char **argv);
void Shutdown();

#endif
