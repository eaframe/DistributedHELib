#include "Distributed.h"
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <queue>
#include "timing.h"

int my_rank;
int numprocs;

queue<MPI::Request> request_queue;

long *vector_1 = NULL;
long vector_1_length = 0;
long *vector_2 = NULL;
long vector_2_length = 0;

void init_vector(long **vector, long *vector_length, long need_length) {
	if(*vector == NULL) {
		*vector = (long *) malloc(sizeof(long) * need_length);
		*vector_length = need_length;
	} else if (*vector_length < need_length) {
		*vector = (long *)realloc(vector, sizeof(long) * need_length); 
		*vector_length = need_length;
	}
}

void Startup(int argc, char **argv) {
	//setup for MPI
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI::Init(argc, argv);
	numprocs = MPI::COMM_WORLD.Get_size();
	my_rank = MPI::COMM_WORLD.Get_rank();
	MPI::Get_processor_name(processor_name, namelen);

//	cout << "Process " << my_rank << " on " << processor_name << " out of " << numprocs << endl;
	if(my_rank != 0) {
		while(true) {
			int operation;
			MPI::COMM_WORLD.Recv(&operation, 1, MPI::INT, 0, 0);
			
			if(operation == OP_SHUTDOWN) {
				Shutdown();
			} else if (operation == OP_ADD_TWO_VECTS ||
					operation == OP_SUB_TWO_VECTS ||
					operation == OP_MUL_TWO_VECTS) {
//				cout << "Process " << my_rank << " recving values" << endl;
				long ithPrime;
				long row_length;
				
				MPI::COMM_WORLD.Recv(&ithPrime, sizeof(long), MPI::BYTE, 0, 0);
				MPI::COMM_WORLD.Recv(&row_length, sizeof(long), MPI::BYTE, 0, 0);
				
				init_vector(&vector_1, &vector_1_length, row_length);
				init_vector(&vector_2, &vector_2_length, row_length);
				
				MPI::COMM_WORLD.Recv(vector_1, sizeof(long) * row_length, MPI::BYTE, 0, 0);
				MPI::COMM_WORLD.Recv(vector_2, sizeof(long) * row_length, MPI::BYTE, 0, 0);
				
//				cout << "Process " << my_rank << " computing results" << endl;
				for(long i = 0; i < row_length; i++) {
					if(operation == OP_ADD_TWO_VECTS) {
						vector_1[i] = AddMod(vector_1[i], vector_2[i], ithPrime);
					} else if (operation == OP_SUB_TWO_VECTS) {
						vector_1[i] = SubMod(vector_1[i], vector_2[i], ithPrime);
					} else if (operation == OP_MUL_TWO_VECTS) {
						vector_1[i] = MulMod(vector_1[i], vector_2[i], ithPrime);
					}
				}
				
//				cout << "Process " << my_rank << " send row back" << endl;
				MPI::COMM_WORLD.Send(vector_1, sizeof(long) * row_length, MPI::BYTE, 0, 0);
			} else if (operation == OP_ADD_ONE_VECT_ONE_NUM ||
					operation == OP_SUB_ONE_VECT_ONE_NUM ||
					operation == OP_MUL_ONE_VECT_ONE_NUM) {
//				cout << "Process " << my_rank << " recving values" << endl;
				long ithPrime;
				long row_length;
				long num;
				
				MPI::COMM_WORLD.Recv(&ithPrime, sizeof(long), MPI::BYTE, 0, 0);
				MPI::COMM_WORLD.Recv(&num, sizeof(long), MPI::BYTE, 0, 0);
				MPI::COMM_WORLD.Recv(&row_length, sizeof(long), MPI::BYTE, 0, 0);
				
				init_vector(&vector_1, &vector_1_length, row_length);
				
				MPI::COMM_WORLD.Recv(vector_1, sizeof(long) * row_length, MPI::BYTE, 0, 0);
				
//				cout << "Process " << my_rank << " computing results" << endl;
				for(long i = 0; i < row_length; i++) {
					if(operation == OP_ADD_ONE_VECT_ONE_NUM) {
						vector_1[i] = AddMod(vector_1[i], num, ithPrime);
					} else if (operation == OP_SUB_ONE_VECT_ONE_NUM) {
						vector_1[i] = SubMod(vector_1[i], num, ithPrime);
					} else if (operation == OP_MUL_ONE_VECT_ONE_NUM) {
						vector_1[i] = MulMod(vector_1[i], num, ithPrime);
					}
				}
				
//				cout << "Process " << my_rank << " send row back" << endl;				
				MPI::COMM_WORLD.Send(vector_1, sizeof(long) * row_length, MPI::BYTE, 0, 0);	
			}
		}
	}
	
//	cout << "MASTER HERE, CONTINUING" << endl;
}

void DistributedSync() {
	while(!request_queue.empty()) {
		MPI::Request request = request_queue.front();
		request_queue.pop();
		
		if(!request.Test()) {
			request_queue.push(request);
		}
	}
}

int next_dest = 1;

int get_next_dest() {
	if(next_dest == numprocs) { next_dest = 1; }
	return next_dest++;
}

void DistributeValue(long value, int dest) {
//	cout << "Process " << my_rank << " distributing value to " << dest << endl;
	request_queue.push(MPI::COMM_WORLD.Isend(&value, sizeof(long), MPI::BYTE, dest, 0));
}

void DistributeVector(long *vector, long row_length, int dest) {
//	cout << "Process " << my_rank << " distributing row to " << dest << endl;
	request_queue.push(MPI::COMM_WORLD.Isend(vector, sizeof(long) * row_length, MPI::BYTE, dest, 0));
}

void DistributeValuesTwoVectors(int operation, long ithPrime, long row_length, long *row, long *other_row) {

	static FHEtimer *vecsDistributeSingleValuesTimer = 0;
	
	if(operation == OP_ADD_TWO_VECTS) {
		buildTimer(vecsDistributeSingleValuesTimer, "AddVecsDistributeSingleValues", FHE_AT);
	} else if (operation == OP_SUB_TWO_VECTS) {
		buildTimer(vecsDistributeSingleValuesTimer, "SubVecsDistributeSingleValues", FHE_AT);
	} else if (operation == OP_MUL_TWO_VECTS) {
		buildTimer(vecsDistributeSingleValuesTimer, "MulVecsDistributeSingleValues", FHE_AT);
	}

  	auto_timer vecsDistributeSingleValuesAutoTimer(vecsDistributeSingleValuesTimer);

	static FHEtimer *vecsDistributeWorkTimer = 0;
	
	if(operation == OP_ADD_TWO_VECTS) {
		buildTimer(vecsDistributeWorkTimer, "AddVecsDistributeWork", FHE_AT);
	} else if (operation == OP_SUB_TWO_VECTS) {
		buildTimer(vecsDistributeWorkTimer, "SubVecsDistributeWork", FHE_AT);
	} else if (operation == OP_MUL_TWO_VECTS) {
		buildTimer(vecsDistributeWorkTimer, "MulVecsDistributeWork", FHE_AT);
	}

  	auto_timer vecsDistributeWorkAutoTimer(vecsDistributeWorkTimer);

	int dest = get_next_dest();
	
//	cout << "Process " << my_rank << " distributing operation " << operation << " to " << dest << endl;
	MPI::COMM_WORLD.Send(&operation, 1, MPI::INT, dest, 0);
	
	vecsDistributeWorkAutoTimer.stop();
	
	static FHEtimer *vecsDistributeDataTimer = 0;
	
	if(operation == OP_ADD_TWO_VECTS) {
		buildTimer(vecsDistributeDataTimer, "AddVecsDistributeData", FHE_AT);
	} else if (operation == OP_SUB_TWO_VECTS) {
		buildTimer(vecsDistributeDataTimer, "SubVecsDistributeData", FHE_AT);
	} else if (operation == OP_MUL_TWO_VECTS) {
		buildTimer(vecsDistributeDataTimer, "MulVecsDistributeData", FHE_AT);
	}

  	auto_timer vecsDistributeDataAutoTimer(vecsDistributeDataTimer);
	
	DistributeValue(ithPrime, dest);
	DistributeValue(row_length, dest);
	DistributeVector(row, row_length, dest);
	DistributeVector(other_row, row_length, dest);
	
	vecsDistributeDataAutoTimer.stop();
	
	request_queue.push(MPI::COMM_WORLD.Irecv(row, sizeof(long) * row_length, MPI::BYTE, dest, 0));
	
	vecsDistributeSingleValuesAutoTimer.stop();
}

void DistributeValuesOneVectorOneNum(int operation, long ithPrime, long row_length, long *row, long num) {
	
	static FHEtimer *vecsDistributeSingleValuesTimer = 0;
	
	if(operation == OP_ADD_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeSingleValuesTimer, "AddVecNumDistributeSingleValues", FHE_AT);
	} else if (operation == OP_SUB_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeSingleValuesTimer, "SubVecNumDistributeSingleValues", FHE_AT);
	} else if (operation == OP_MUL_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeSingleValuesTimer, "MulVecNumDistributeSingleValues", FHE_AT);
	}

  	auto_timer vecsDistributeSingleValuesAutoTimer(vecsDistributeSingleValuesTimer);
	
	static FHEtimer *vecsDistributeWorkTimer = 0;
	
	if(operation == OP_ADD_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeWorkTimer, "AddVecNumDistributeWork", FHE_AT);
	} else if (operation == OP_SUB_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeWorkTimer, "SubVecNumDistributeWork", FHE_AT);
	} else if (operation == OP_MUL_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeWorkTimer, "MulVecNumDistributeWork", FHE_AT);
	}

  	auto_timer vecsDistributeWorkAutoTimer(vecsDistributeWorkTimer);

	int dest = get_next_dest();
	
//	cout << "Process " << my_rank << " distributing operation " << operation << " to " << dest << endl;
	MPI::COMM_WORLD.Send(&operation, 1, MPI::INT, dest, 0);
	
	vecsDistributeWorkAutoTimer.stop();
	
	static FHEtimer *vecsDistributeDataTimer = 0;
	
	if(operation == OP_ADD_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeDataTimer, "AddVecNumDistributeData", FHE_AT);
	} else if (operation == OP_SUB_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeDataTimer, "SubVecNumDistributeData", FHE_AT);
	} else if (operation == OP_MUL_ONE_VECT_ONE_NUM) {
		buildTimer(vecsDistributeDataTimer, "MulVecNumDistributeData", FHE_AT);
	}

  	auto_timer vecsDistributeDataAutoTimer(vecsDistributeDataTimer);
	
	DistributeValue(ithPrime, dest);
	DistributeValue(num, dest);
	DistributeValue(row_length, dest);
	DistributeVector(row, row_length, dest);
	
	vecsDistributeDataAutoTimer.stop();
	
	request_queue.push(MPI::COMM_WORLD.Irecv(row, sizeof(long) * row_length, MPI::BYTE, dest, 0));
	
	vecsDistributeSingleValuesAutoTimer.stop();
}

void Shutdown() {

	if(my_rank == 0) {
		for(int i=1; i<numprocs; i++) {
			MPI::COMM_WORLD.Send(&OP_SHUTDOWN, 1, MPI::INT, i, 0);
		}
		
//		cout << "Process " << my_rank << " finalizing" << endl;
		MPI::Finalize();
	} else {
//		cout << "Process " << my_rank << " finalizing" << endl;
		MPI::Finalize();
		exit(0);
	}
}
