#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "image_processor.h"

int main (int argc, char* argv[])
{
    int i, rank, number;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    char *image_file_name = argv[1];
    int type = image_type(image_file_name);
    

    MPI_Finalize();
    return 0;
}