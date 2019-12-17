/**
    // Popescu Andrei-Gabriel
    // 333CA
    // Main header for image data reading


    Considering the binary forat th easiest way is to read 
    using fread function from stdio in c and then pass the whole 
    image structure to the main function

    // TODO: defensive programming over all parameters
    // TODO: remove this comment and add more esteic ones
**/

#ifndef IMAGE_PROCESSOR
#define IMAGE_PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "types.h"


 PGMImage *read_PGM_image(char *image_file_name) 
 {
    PGMImage *image_structure = (PGMImage *) malloc(1 * sizeof(PGMImage));

    FILE *image = fopen(image_file_name, "rb");

    if (image == NULL)
    {
        printf("%s\n", FILE_ERROR_MESSAGE);
        exit(1);
    }    

    char *image_type = (char*) malloc(2 * sizeof(char));
    fread(image_type, 2, sizeof(char), image);
    image_structure -> image_type = image_type;

    unsigned char comment_line[45];
    fread(comment_line, sizeof(unsigned char), sizeof(comment_line) + 1, image);
    
    unsigned char width[5], height[5], max_val[5];
    unsigned char runner;

    fread(&runner, sizeof(unsigned char), 1, image); width[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[4] = runner;
   
    image_structure -> width = atoi(width);
    
    fread(&runner, sizeof(unsigned char), 1, image); 

    fread(&runner, sizeof(unsigned char), 1, image); height[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[4] = runner;
   
    image_structure -> height = atoi(height);
    

    fread(&runner, sizeof(unsigned char), 1, image); max_val[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[4] = runner;

    image_structure -> max_val = atoi(max_val);

    unsigned char *image_content = (unsigned char *) malloc((image_structure -> width * image_structure -> height) * sizeof(unsigned char));
    fread(image_content, sizeof(unsigned char), image_structure -> width * image_structure -> height, image);
    
    image_structure -> image_content = image_content;

    fclose(image);
    return image_structure;
}


PNMImage *read_PNM_image(char *image_file_name) 
 {
    PNMImage *image_structure = (PNMImage *) malloc(1 * sizeof(PNMImage));

    FILE *image = fopen(image_file_name, "rb");

    if (image == NULL)
    {
        printf("%s\n", FILE_ERROR_MESSAGE);
        exit(1);
    }    

    char *image_type = (char*) malloc(2 * sizeof(char));
    fread(image_type, 2, sizeof(char), image);
    image_structure -> image_type = image_type;

    unsigned char comment_line[45];
    fread(comment_line, sizeof(unsigned char), sizeof(comment_line) + 1, image);
    
    unsigned char width[5], height[5], max_val[5];
    unsigned char runner;

    fread(&runner, sizeof(unsigned char), 1, image); width[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); width[4] = runner;
   
    image_structure -> width = atoi(width);
    
    fread(&runner, sizeof(unsigned char), 1, image); 

    fread(&runner, sizeof(unsigned char), 1, image); height[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); height[4] = runner;
   
    image_structure -> height = atoi(height);
    

    fread(&runner, sizeof(unsigned char), 1, image); max_val[0] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[1] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[2] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[3] = runner;
    fread(&runner, sizeof(unsigned char), 1, image); max_val[4] = runner;

    image_structure -> max_val = atoi(max_val);

    unsigned char *image_content = (unsigned char *) malloc((image_structure -> width * image_structure -> height) * sizeof(unsigned char));
    fread(image_content, sizeof(unsigned char), image_structure -> width * image_structure -> height, image);
    
    image_structure -> image_content = image_content;

    fclose(image);
    return image_structure;
}

#endif