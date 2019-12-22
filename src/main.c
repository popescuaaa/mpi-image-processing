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

    current_filter = filters[0];
    
    if (rank == 0)
    {
        
        if (type_tag == PGM_TYPE)
        {
            PGMImage image = read_PGM_image(image_file_name);
            int current_slave;
            for (current_slave = 1; current_slave < number_of_processes; ++current_slave)
            {
                MPI_Send(&(image -> width), 1, MPI_INT, current_slave, WIDTH_TAG, MPI_COMM_WORLD);
            }

            // kernel radius + 1
            send_size = (image -> height - 2) / number_of_processes;
            start = 0;
            finish = start + send_size + 2;

            for (current_slave = 1; current_slave < number_of_processes; ++current_slave)
            {
                send_submatrix_pgm(image, start, finish, current_slave, current_filter);

            }
        } 
        else
        {
            
        }
        
    }
    else 
    {

    }


    MPI_Finalize();
    return 0;
}