/**
 *  Apache spark structure:
 *  - master process or 0 will read the image and 
 *    then  will split it acroos all number of processes - 1 
 *    slaves 
 *  - they will notify th main process that the data is received
 *  - they will process the data
 *  - the main process will have a processing part too to save the 
 *    computational time
 *  - all the salves in the end will send their processed data 
 *    back to the master process
 *  - this process will be repeated for each individual filter 
 *  
 * 
 *  Note: this main code will be the one tested using unit tests and 
 *        and tested for correctness and scalability.
 *  
 * 
 **/ 

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_processor.h"
#include "filters.h"


int send_size;
int start, finish;
int type_tag;
char **filters;
char *image_file_name;
char *out_file_name;
char *current_filter;


int main (int argc, char* argv[])
{
    int rank;
    int number_of_processes;
  
    image_file_name = argv[1];
    out_file_name = argv[2];
    current_filter = argv[3];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    MPI_Status status;
    
    type_tag = image_type(image_file_name);
    
    if (rank == MASTER) 
    {
        /**
         *  Main process
         * - the image distribution will be made on height
         **/ 
       
        int line, column;
        int slave_index;
        int current_line_for_proccess;

        if (type_tag == PGM_TYPE)
        {
           PGMImage *image = read_PGM_image(image_file_name); 

           for (slave_index = 1; slave_index < number_of_processes; ++slave_index)
           {
               MPI_Send(
                   &(image -> width), 
                   1, 
                   MPI_INT, 
                   slave_index, 
                   WIDTH_TAG, 
                   MPI_COMM_WORLD
                );
           }

           send_size = (image -> height - 2) / number_of_processes;
           start = 0
           finish = start + send_size + 2;

           for (slave_index = 1; slave_index < number_of_processes - 1; ++slave_index)
           {
               /**
                * Send submatrix
                **/ 
                
               MPI_Send(
                   &(start), 
                   1, 
                   MPI_INT, 
                   slave_index, 
                   start, 
                   MPI_COMM_WORLD
                );

               MPI_Send(
                   &(finish), 
                   1, 
                   MPI_INT, 
                   slave_index, 
                   finish, 
                   MPI_COMM_WORLD
                );

               for (current_line_to_process = start; current_line_for_proccess < finish; ++current_line_for_proccess)
               {
                    MPI_Send(
                        image -> image_matrix[current_line_for_proccess], 
                        image -> width, 
                        MPI_UNSIUGNED_CHAR, 
                        slave_index, 
                        UNPROCESSED_MATRIX, 
                        MPI_COMM_WORLD
                    );
               }

               start += send_size;
               finish += send_size;
           }

            // send the rest to the last neighbour
            for (current_line_to_process = start; current_line_for_proccess < image -> height; ++current_line_for_proccess)
            {
                MPI_Send(
                    image -> image_matrix[current_line_for_proccess], 
                    image -> width, 
                    MPI_UNSIUGNED_CHAR, 
                    number_of_processes - 1, 
                    UNPROCESSED_MATRIX, 
                    MPI_COMM_WORLD
                );
            }

            /**
             *  Create a new matrix which will hold all processed lines
             *  received from slaves.
             * 
             **/ 
            unsigned char **matrix_copy = (unsigned char **) 
                                          malloc (image -> height * 
                                          sizeof(unsigned char *));

            for (line = 0; line < image -> height; ++line)
            {
                matrix_copy[line] = (unsigned char *) 
                                    malloc(image -> width *
                                    sizeof(unsigned char));
            }
            
            unsigned char **recv_matrix = (unsigned char **) 
                                          malloc ((send_size + 2) * 
                                          sizeof(unsigned char *));

            for (line = 0; line < (send_size + 2); ++line)
            {
                recv_matrix[line] = (unsigned char *) 
                                    malloc(image -> width *
                                    sizeof(unsigned char));
            }

            int current_line = 0;

            for (slave_index = 1; slave_index < number_of_processes - 1; ++slave_index)
            {
                /**
                 * Receive submatrix
                 * - and get only the core of ir between 1 -> send_size + 1
                 **/ 
                for (line = 0; line < send_size + 2; ++line) 
                {
                    MPI_Recv(
                        recv_matrix[line], 
                        image -> width, 
                        MPI_UNSIGNED_CHAR, 
                        slave_index, 
                        MPI_ANY_TAG, 
                        MPI_COMM_WORLD, 
                        status
                    );
                }

                for (line = 1; line < send_size - 1; ++line) 
                {
                    current_line++;

                    for (column = 0; column < image -> width; ++column) 
                    {
                        matrix_copy[current_line][column] = recv_matrix[line][column];
                    }
                }
                
            }
            
            /**
             * Receive submatrix from the last neighbour
             * 
             **/ 

            for (line = 0; line < send_size + 2; ++line) 
            {
                MPI_Recv(
                    recv_matrix[line], 
                    image -> width, 
                    MPI_UNSIGNED_CHAR, 
                    number_of_processes - 1, 
                    MPI_ANY_TAG, 
                    MPI_COMM_WORLD, 
                    status
                );
            }

            for (line = 1; line < send_size - 1; ++line) 
            {
                current_line++;
                
                for (column = 0; column < image -> width; ++column) 
                {
                    matrix_copy[current_line][column] = recv_matrix[line][column];
                }
            }

            image -> image_matrix = matrix_copy;
            write_image(image, NULL, out_file_name);

            printf("%s\n", MASTER_PROCESS_FINISHED);
        } 
        else
        {  
        

        }
    }
    else 
    {
        /**
         *  Slave process
         **/ 

        int line;
        int column;
        int received_width;
        int start_line_process;
        int finish_line_process;

        while(1)
        {
            MPI_Recv(
                &received_width, 
                1, 
                MPI_INT, 
                MASTER, 
                MPI_ANY_TAG, 
                MPI_COMM_WORLD, 
                &status
            );

            MPI_Recv(
                &start_line_process, 
                1, 
                MPI_INT, 
                MASTER, 
                MPI_ANY_TAG, 
                MPI_COMM_WORLD, 
                &status
            );

            MPI_Recv(
                &finish_line_process, 
                1, 
                MPI_INT, 
                MASTER, 
                MPI_ANY_TAG, 
                MPI_COMM_WORLD, 
                &status
            );

            /**
             *  Receive input lines in a matrix
             **/ 

            unsigned char **input_data = (unsigned char **) 
                                        malloc ((finish_line_process - start_line_process) * 
                                        sizeof(unsigned char *));

            for (line = 0; line < (finish_line_process - start_line_process); ++line)
            {
                input_data[line] = (unsigned char *) 
                                    malloc(received_width *
                                    sizeof(unsigned char));
            }


            int received_line_to_process;

            for (received_line_to_process = start_line_process; 
                received_line_to_process < finish_line_process; 
                ++received_line_to_process)
            {
                MPI_Recv(
                    input_data[received_line_to_process], 
                    received_width, 
                    MPI_UNSIGNED_CHAR, 
                    MASTER, 
                    MPI_ANY_TAG, 
                    MPI_COMM_WORLD, 
                    &status
                );
            }

            /**
             *  Apply the filter:
             *  - Every subprocess or slave will know about the current filter
             *    and will apply it
             **/ 
            
            unsigned char **output_data = (unsigned char **) 
                                        malloc ((finish_line_process - start_line_process) * 
                                        sizeof(unsigned char *));

            for (line = 0; line < (finish_line_process - start_line_process); ++line)
            {
                output_data[line] = (unsigned char *) 
                                    malloc(received_width *
                                    sizeof(unsigned char));
            }

            


        }
    }
    }
    
    MPI_Finalize(); 
    return 0;
}