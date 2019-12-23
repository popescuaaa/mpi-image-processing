#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "image_processor.h"
#include "filters.h"

int send_size;
int start, finish;
int type_tag;
char **filters;
char *image_file_name;
char *current_filter;

void read_parameters(int argc, char *argv[]) 
{
    int current_element;
    image_file_name = argv[1];
    for (current_element = 0; current_element < argc; ++current_element)
    {
        filters[current_element + 2] = argv[current_element];
    }

    type_tag = image_type(image_file_name);
}


int main (int argc, char* argv[])
{
    read_parameters(argv);
    int i, rank, number_of_processes;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    

    MPI_Finalize();
    return 0;
}