#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "image_processor.h"

int main (int argc, char* argv[])
{
    int i, rank, number_of_processes;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    char *current_filter;
    char *image_file_name = argv[1];
    
    


    MPI_Finalize();
    return 0;
}