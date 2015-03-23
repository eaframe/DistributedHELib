#include "Distributed.h"
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int rank;
int numprocs;

void Startup(int argc, char **argv) {
	//setup for MPI
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI::Init(argc, argv);
	numprocs = MPI::COMM_WORLD.Get_size();
	rank = MPI::COMM_WORLD.Get_rank();
	MPI::Get_processor_name(processor_name, namelen);

	cout << "Process " << rank << " on " << processor_name << " out of " << numprocs << endl;

	if(rank != 0) {
		while(true) {
			int operation;
			MPI::COMM_WORLD.Recv(&operation, 1, MPI::INT, 0, 0);
			
			if(operation == OP_SHUTDOWN) {
				Shutdown();
			}
		}
	}
	
	cout << "MASTER HERE, CONTINUING" << endl;
}

void Shutdown() {

	if(rank == 0) {
		for(int i=1; i<numprocs; i++) {
			MPI::COMM_WORLD.Send(&OP_SHUTDOWN, 1, MPI::INT, i, 0);
		}
		
		cout << "Process " << rank << " finalizing" << endl;
		MPI::Finalize();
	} else {
		cout << "Process " << rank << " finalizing" << endl;
		MPI::Finalize();
		exit(0);
	}
}
