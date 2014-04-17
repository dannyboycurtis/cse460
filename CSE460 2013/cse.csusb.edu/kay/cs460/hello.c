/*************************************************
Compile and run:

$ mpicc -o hello hello.c
$ mpirun -np 4 -hostfile machines.0-3 hello

where machines.0-3 contains:

jb359-0
jb359-1
jb359-2
jb359-3

Alternatively run:

$ mpirun -np 4 -host jb359-0,jb359-1 hello
**************************************************/

#include <stdio.h>
#include <string.h>
#include "mpi.h"

main(int argc, char **argv)
{
	int my_rank, p, source, dest, tag = 7;
	char message[50];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (my_rank != 0) {
		sprintf(message, "Greetings from %d", my_rank);
		dest = 0;
		MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	} else {
		for (source = 1; source < p; source++) {
			MPI_Recv(message, 50, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
			printf("%s\n", message);
		}
	}

	MPI_Finalize();
}
