/**
    // Popescu Andrei-Gabriel
    // 333CA
    // Main header for image data reading


    Considering the binary forat th easiest way is to read 
    using fread function from stdio in c and then pass the whole 
    image structure to the main function

    // TODO: defensive programming over all parameters
    // TODO: remove this comment and add more esteic ones
    // TODO: line length is huge
    // TODO: remove duplicated code or think for alternative solutions
**/

#ifndef IMAGE_PROCESSOR
#define IMAGE_PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "types.h"

int image_type(char *image_file_name) 
{
    char PGM_type[] = ".pgm";
    char PNM_type[] = ".pnm";

    char* is_PGM = strstr(image_file_name, PGM_type);
    char* is_PNM = strstr(image_file_name, PNM_type);

    if (is_PNM != NULL)
    {
        printf("%s\n", PNM_FORMAT_SIGNAL);
        return PNM_TYPE;
    }
    else if (is_PGM != NULL)
    {
        printf("%s\n", PGM_FORMAT_SIGNAL);
        return PGM_TYPE;
    }
    else if (is_PNM == NULL && is_PGM == NULL) 
    {
        printf("%s\n", SUPPORTED_FORMATS_ERROR);
    }
    
    return DEFAULT_RETURN_TYPE;
}

PGMImage *read_PGM_image(char *image_file_name) 
{
    int line, column;
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

    unsigned char **image_matrix = (unsigned char **) malloc(image_structure -> height * sizeof(unsigned char *));
    for (line = 0; line < image_structure -> height; ++line) 
    {
        image_matrix[line] = (unsigned char *) malloc(image_structure -> width * sizeof(unsigned char));
    }

    for (line = 0; line < image_structure -> height; ++line) 
    {
        for (column = 0; column < image_structure -> width; ++column) 
        {
            image_matrix[line][column] = image_content[line + column];
        }
    }
    
    image_structure -> image_matrix = image_matrix;

    fclose(image);
    return image_structure;
}


PNMImage *read_PNM_image(char *image_file_name) 
{
    int line, column;
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

    Pixel *rgb_content = (Pixel *) malloc(image_structure -> width * image_structure -> height * sizeof(Pixel));
    fread(rgb_content, sizeof(Pixel), image_structure -> width * image_structure ->height, image);
    
    image_structure -> rgb_content = rgb_content;

    Pixel **rgb_image_matrix= (Pixel **) malloc(image_structure -> height * sizeof(Pixel *));
    for (line = 0; line < image_structure -> height; ++line) 
    {
        rgb_image_matrix[line] = (Pixel *) malloc(image_structure -> width * sizeof(Pixel));
    }

    for (line = 0; line < image_structure -> height; ++line) 
    {
        for (column = 0; column < image_structure -> width; ++column) 
        {
            rgb_image_matrix[line][column] = rgb_content[line + column];
        }
    }
    
    image_structure -> rgb_image_matrix = rgb_image_matrix;

    fclose(image);
    return image_structure;
}

#endif