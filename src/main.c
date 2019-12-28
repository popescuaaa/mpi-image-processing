/**
 *  Apache spark structure:
 *  - master process or 0 will read the image and 
 *    then  will split it across all number of processes - 1
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

void process_PGM_image_master(char *input_image_file_name,
                       char *output_image_file_name,
                       char *argv[],
                       int argc, 
                       int number_of_processes);

void process_PGM_image_slave(char *input_image_file_name,
                       char *output_image_file_name,
                       char *argv[],
                       int argc, 
                       int number_of_processes);

void process_PNM_image(char *input_image_file_name, char *argv[]);
void send_image_PGM(PGMImage *image, int destination, int start_line, int end_line);
void send_image_PNM(PNMImage *image, int destination, int start_line, int end_line);
PGMImage *receive_image_PGM(int source);
PNMImage *receive_image_PNM(int source);
void apply_filter(PGMImage *image, float **filter, int start_line, int end_line);

int main(int argc, char *argv[]) 
{
  int type;  
  int rank;
  int number_of_processes;
  char input_image_file_name[100];
  char output_image_file_name[100];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  
  
  if (rank == 0) 
  {
    /**
     *  Master process: which make a scatter of the image matrix to all
     *  the slave subprocesses
     **/ 
    if (argc < 4) {
      printf(
          "Usage: mpirun -np N ./%s input_image.pnm/.pgm output_image.pnm/.pgm filter_1 filter_2 ... filter_X\n",
          argv[0]);
      exit(-1);
    }

        input_image_file_name = argv[1];
        output_image_file_name = argv[2];
        type = image_type(input_image_file_name);
        
        if (type == PGM_TYPE)
        {
            /**
             *  Process PGM Image: P5
             **/ 

            process_PGM_image_master(input_image_file_name);
        }
        else
        {
            /**
             *  Process PNM Image: P6
             **/ 

            process_PNM_image(input_image_file_name);
        }
  } 
  else
  {

      /**
       *  Slave process
       **/ 
      type = image_type(input_image_file_name);
      
      if (type == PGM_TYPE)
      {
          /**
           *  Process PGM Image: P5
           **/ 

          process_PGM_image_slave(input_image_file_name);
      }
      else
      {
          /**
           *  Process PNM Image: P6
           **/ 

          process_PNM_image(input_image_file_name);
      }

       
  }
  
  MPI_Finalize();
  return 0;
}


/**
 * @param: input_image_file_name
 * 
 * Function that process the input and handle the whole process of receiving and 
 * sending data only for PGM as master process. Altough it seems duplicated code I 
 * prefer to use separate funstions for processing different type of images as this poject 
 * can be in such a way extended in a certain moment in time.
 **/ 

void process_PGM_image_master(char *input_image_file_name,
                       char *output_image_file_name,
                       char *argv[],
                       int argc, 
                       int number_of_processes)
{
    PGMImage *image = read_PGM_image(input_image_file_name);

    int start_line, end_line;
    int slave_index;

    /**
     * Scatter the image across all the slave processes
     **/ 

    for (slave_index = 1; slave_index < number_of_processes; ++slave_index) 
    {
      start_line = (slave_index * image -> height) / number_of_processes;
      end_line = ((slave_index + 1) * image -> height) / number_of_processes;

     
      start_line -= 1;
      if (slave_index != number_of_processes - 1)
        end_line++;

      send_image_PGM(
          image, 
          slave_index, 
          start_line, 
          end_line
        );
    }

    /**
     *  The first part of the image will the resposability of the 
     *  master process.
     **/ 

    start_line = 0;
    end_line = image -> height / number_of_processes;

    /**
     *  Take one line further to ensure that the distribution 
     *  is made according the rule send_size + kernel_radius - 1
     **/ 

    if (end_line != image -> height)
      end_line++;
    
    int filter_index;

    for (filter_index = 3; filter_index < argc; ++filter_index) 
    {
      char *current_filter = argv[filter_index];
      apply_filter(
          image, 
          current_filter, 
          start_line, 
          end_line
        );

      if (number_of_processes > 1) 
      {
        int primary_slave_index = 1;

        /**
         *  Send computed edge line to next process
         **/ 
        MPI_Send(
            image -> image_matrix[end_line - 2],
            image -> width,
            MPI_UNSIGNED_CHAR,
            primary_slave_index,
            DEFAULT_TAG,
            MPI_COMM_WORLD
        );

        /**
         *  Receive computed edge line from next process
         **/

        MPI_Recv(
            image -> image_matrix[end_line - 1],
            image -> width,
            MPI_UNSIGNED_CHAR,
            primary_slave_index,
            DEFAULT_TAG,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
      }
    }

    /**
     *  Receive data from all processes and reconstruct image
     **/

    for (slave_index = 1; slave_index < number_of_processes; ++slave_index) 
    {
      start_line = (slave_index * image -> height) / number_of_processes;
      end_line = ((slave_index + 1) * image -> height) / number_of_processes;

      PGMImage *computed_image = receive_image_PGM(slave_index);

      for (line = start_line; line < end_line; ++line) 
      {
        image -> image_matrix[line] = computed_image -> image_matrix[line - start_line + 1];
      }

    }

    write_image(image, NULL, output_image_file_name);
  
}

/**
 * @param: input_image_file_name
 * Function that process the input and handle the whole process of receiving and 
 * sending data only for PNM.
 * 
 **/ 
void process_PNM_image(char *input_image_file_name, char *argv[])
{
    
}

/**
 * 
 *  Slave process 
 * 
 **/ 
void process_PGM_image_slave( char *input_image_file_name,
                              char *output_image_file_name,
                              char *argv[],
                              int argc, 
                              int rank,
                              int number_of_processes)
{

    PGMImage *image = receive_image_PGM(MASTER);


    /**
     *  Apply filters.
     *  *****Updating the upper and lower edges each time if they exist
     **/
    int filter_index;
    for (filter_index = 3; filter_index < argc; ++filter_index) 
    {
      apply_filter(image, get_filter_by_name(argv[filter_index]), 0, image -> height);

      MPI_Recv(image -> image_matrix[0],
              image -> width,
              MPI_UNSIGNED_CHAR,
              rank - 1,
              DEFAULT_TAG,
              MPI_COMM_WORLD,
              MPI_STATUS_IGNORE);

      MPI_Send(image -> image_matrix[1],
              image -> width,
              MPI_UNSIGNED_CHAR,
              rank - 1,
              DEFAULT_TAG,
              MPI_COMM_WORLD);

      if (rank != number_of_processes - 1) 
      {
          MPI_Send(image -> image_matrix[image -> height - 2],
                  image -> width,
                  MPI_UNSIGNED_CHAR,
                  rank + 1,
                  DEFAULT_TAG,
                  MPI_COMM_WORLD);

          MPI_Recv(image -> image_matrix[image->height - 1],
                  image -> width,
                  MPI_UNSIGNED_CHAR,
                  rank + 1,
                  DEFAULT_TAG,
                  MPI_COMM_WORLD,
                  MPI_STATUS_IGNORE);
    }
}

/**
 *  Send a PGM Image to a fixed destination.
 * 
 * 
 **/ 
void send_image_PGM(PGMImage *image, int destination, int start_line, int end_line)
{
  MPI_Send(
    image->image_type, 
    2, 
    MPI_UNSIGNED_CHAR, 
    destination, 
    DEFAULT_TAG, 
    MPI_COMM_WORLD
  );

  MPI_Send(
    &(image -> width), 
    1, 
    MPI_INT, 
    destination, 
    DEFAULT_TAG, 
    MPI_COMM_WORLD
  );

  int _height = end_line - start_line;

  MPI_Send(
    &new_height, 
    1, 
    MPI_INT, 
    destination, 
    DEFAULT_TAG, 
    MPI_COMM_WORLD
  );

  MPI_Send(
    &(image -> max_val),
     1,
     MPI_INT,
     destination,
     DEFAULT_TAG,
     MPI_COMM_WORLD
    );

  int line;
  for (line = start_line; line < end_line; ++line) {
    MPI_Send(image -> image[line],
             image -> width,
             MPI_UNSIGNED_CHAR,
             destination,
             DEFAULT_TAG,
             MPI_COMM_WORLD
            );
  }

}

/**
 * Used to receive a pgm image from source
 * 
 **/
PGMImage *receive_image_PGM(int source)
{

 PGMImage *image = (PGMImage *) malloc (sizeof(PGMImage));

  MPI_Recv(
    image->image_type,
    2,
    MPI_INT,
    source,
    DEFAULT_TAG,
    MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);

  MPI_Recv(
    &(image -> width),
    1,
    MPI_INT,
    source,
    DEFAULT_TAG,
    MPI_COMM_WORLD,
    MPI_STATUS_IGNORE
    );

  MPI_Recv(
    &(image -> height),
      1,
      MPI_INT,
      source,
      DEFAULT_TAG,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

  MPI_Recv(
    &(image -> max_val),
      1,
      MPI_INT,
      sender,
      DEFAULT_TAG,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );

  int line;
  for (line = 0; line < image -> height; ++line) 
  {
    MPI_Recv(
      image -> image_matrix[line],
      image -> width,
      MPI_UNSIGNED_CHAR,
      source,
      DEFAULT_TAG,
      MPI_COMM_WORLD,
      MPI_STATUS_IGNORE);
  }

  return image;

}


void apply_filter(PGMImage *image, float** filter, int start_line, int end_line) 
{
  PGMImage result = image;

  int line;
  int column;

  for (line = start_line; line < end_line; ++line) 
  {
    for (column = 0; column < image -> width - 1; ++column) 
    {
      if (line == 0 || 
          column < 1 || 
          line == image -> height - 1|| 
          column > image -> width - 2) 
      {
        result -> image_matrix[line][column] = image -> image_matrix[line][column];
      } 
      else 
      {
        unsigned char **p = image -> image_matrix;

        unsigned char pixels[3][3] =
            {
              {
                p[line - 1][column - 1], 
                p[line - 1][column], 
                p[line - 1][column + 1]
              },

              {
                p[line][column - 1], 
                p[line][column], 
                p[line][column + 1]
              },

              {
                p[line + 1][column - 1], 
                p[line + 1][column], 
                p[line + 1][column + 1]
              }
            };
        
        float result = 0;
        for (int i = 0; i < 3; i++)
          {
            for (int j = 0; j < 3; j++)
              {
                  result += pixels[i][j] * filter[i][j];
              }
          }
          
        result -> image_matrix[line][column] = (unsigned char) result;
      }
    }
  }

  for (int i = 0; i < image -> height; i++)
    for (int j = 0; j < image -> width; j++)
      image -> image_matrix[i][j] = result -> image_matrix[i][j];
}