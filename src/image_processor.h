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
    // TODO: wrong matrix bounding line + column
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
    int line; 
    int column;
    int pixel_index;

    PGMImage *image_structure = (PGMImage *) malloc(1 * sizeof(PGMImage));

    FILE *image = fopen(image_file_name, "rb");

    if (image == NULL)
    {
        printf("%s\n", FILE_ERROR_MESSAGE);
        exit(1);
    }    

    unsigned char *image_type = (unsigned char*) malloc(IMAGE_TYPE_SIZE * sizeof(char));
    fread(image_type, sizeof(unsigned char), IMAGE_TYPE_SIZE, image);
    printf("%s\n",image_type);
    image_structure -> image_type = image_type;
    printf("%s\n",image_structure -> image_type);
    unsigned char comment_line[45];
    fread(comment_line, sizeof(unsigned char), sizeof(comment_line) + 1, image);
    
    unsigned char width[5], height[5], max_val[5];
    unsigned char runner;

    fread(width, sizeof(unsigned char), WIDTH_DEFAULT_LENGTH, image); 

    image_structure -> width = atoi(width);
    
    fread(&runner, sizeof(unsigned char), 1, image); 

    fread(height, sizeof(unsigned char), HEIGHT_DEFAULT_LENGHT, image);
    
    image_structure -> height = atoi(height);
    
    fread(max_val, sizeof(unsigned char), MAX_VAL_DEFAULT_LENGHT, image); 
  
    image_structure -> max_val = atoi(max_val);

    unsigned char *image_content = (unsigned char *) malloc((image_structure -> width * image_structure -> height) * sizeof(unsigned char));
    fread(image_content, sizeof(unsigned char), image_structure -> width * image_structure -> height, image);
    
    image_structure -> image_content = image_content;

    unsigned char **image_matrix = (unsigned char **) malloc(image_structure -> height * sizeof(unsigned char *));
    for (line = 0; line < image_structure -> height; ++line) 
    {
        image_matrix[line] = (unsigned char *) malloc(image_structure -> width * sizeof(unsigned char));
    }


    pixel_index = 0;

    for (line = 0; line < image_structure -> height; ++line) 
    {
        for (column = 0; column < image_structure -> width; ++column) 
        {
            image_matrix[line][column] = image_content[pixel_index];
            pixel_index++;
        }
    }
    
    image_structure -> image_matrix = image_matrix;

    fclose(image);
    return image_structure;
}


PNMImage *read_PNM_image(char *image_file_name) 
{
    int line;
    int column;
    int pixel_index;

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

    pixel_index = 0;

    for (line = 0; line < image_structure -> height; ++line) 
    {
        for (column = 0; column < image_structure -> width; ++column) 
        {
            rgb_image_matrix[line][column] = rgb_content[pixel_index];
            pixel_index++;
        }
    }
    
    image_structure -> rgb_image_matrix = rgb_image_matrix;

    fclose(image);
    return image_structure;
}

void write_PGM_image(PGMImage *pgm_image, char *output_file_name)
{
    FILE *out = fopen(output_file_name, "wb");
    if (out == NULL) 
    {
        printf("%s\n", FILE_ERROR_MESSAGE);
        exit(1);
    }
    
    int line;
    int column;
    unsigned char width[WIDTH_DEFAULT_LENGTH];
    unsigned char height[HEIGHT_DEFAULT_LENGHT];
    unsigned char max_val[MAX_VAL_DEFAULT_LENGHT];
    unsigned char *delimiter = " ";
    unsigned char *comment_line = COMMENT_LINE_GIMP;
    unsigned char *separator = "\n";
    sprintf(width, "%d", pgm_image -> width);
    sprintf(height, "%d", pgm_image -> height);
    sprintf(max_val, "%d", pgm_image -> max_val);
   

    fwrite(pgm_image -> image_type, sizeof(unsigned char), IMAGE_TYPE_SIZE, out);
    fwrite(separator, sizeof(unsigned char), 1, out);
    fwrite(comment_line, sizeof(unsigned char), COMMENT_LINE_FIXED_LENGTH + 1, out);
    fwrite(width, sizeof(unsigned char), strlen(width), out);
    fwrite(delimiter, sizeof(unsigned char), 1, out);
    fwrite(height, sizeof(unsigned char), strlen(height), out);
    fwrite(separator, sizeof(unsigned char), 1, out);
    fwrite(max_val, sizeof(unsigned char), strlen(max_val), out);
    fwrite(separator, sizeof(unsigned char), 1, out);
    
    // for (line = 0; line < pgm_image -> height; ++line)
    // {
    //     fwrite(pgm_image -> image_matrix[line], sizeof(unsigned char), pgm_image -> width, out);   
    // }    
    fwrite(pgm_image -> image_content, sizeof(unsigned char), pgm_image -> width * pgm_image -> height, out);
    // fprintf(out, "%s\n", pgm_image -> image_type);
    // fprintf(out, "%s\n", COMMENT_LINE_GIMP);
    // fprintf(out, "%d %d\n", pgm_image -> width, pgm_image -> height);
    // fprintf(out, "%d\n", pgm_image -> max_val);
    // for(line  = 0; line < pgm_image -> height; ++line)
    // {
    //     for (column = 0; column < pgm_image -> width; ++column)
    //     {
    //        fprintf(out, "%u",(unsigned char) pgm_image -> image_matrix[line][column]);
    //     }
    // }

    fclose(out);
    printf("%s\n", FINISHED_WRITING);
}

void write_PNM_image(PNMImage *pnm_image, char *output_file_name)
{
    FILE *out = fopen(output_file_name, "w+b");
    if (out == NULL) 
    {
        printf("%s\n", FILE_ERROR_MESSAGE);
        exit(1);
    }
    
    int line;
    int column;
    unsigned char width[WIDTH_DEFAULT_LENGTH];
    unsigned char height[HEIGHT_DEFAULT_LENGHT];
    unsigned char max_val[MAX_VAL_DEFAULT_LENGHT];
    unsigned char *delimiter = " ";
    unsigned char *comment_line = COMMENT_LINE_GIMP;
    unsigned char *separator = "\n";
    unsigned char *image_type = (unsigned char*) malloc(IMAGE_TYPE_SIZE_OUT * sizeof(unsigned char));
    sprintf(width, "%d", pnm_image -> width);
    sprintf(height, "%d", pnm_image -> height);
    sprintf(max_val, "%d", pnm_image -> max_val);
    strcat(image_type, pnm_image -> image_type);
    strcat(image_type, separator);

    fwrite(image_type, sizeof(unsigned char), IMAGE_TYPE_SIZE_OUT, out);
    fwrite(comment_line, sizeof(unsigned char), COMMENT_LINE_FIXED_LENGTH + 1, out);
    fwrite(width, sizeof(unsigned char), strlen(width), out);
    fwrite(delimiter, sizeof(unsigned char), 1, out);
    fwrite(height, sizeof(unsigned char), strlen(height), out);
    fwrite(separator, sizeof(unsigned char), 1, out);
    fwrite(max_val, sizeof(unsigned char), strlen(max_val), out);
    fwrite(separator, sizeof(unsigned char), 1, out);
    
    for (line = 0; line < pnm_image -> height; ++line)
    {
        for (column = 0; column < pnm_image -> width; ++column)
        {
            fwrite(&pnm_image -> rgb_image_matrix[line][column].red, sizeof(unsigned char), RUNNER_DEFAUT_VALUE, out);
            
            fwrite(&pnm_image -> rgb_image_matrix[line][column].green, sizeof(unsigned char), RUNNER_DEFAUT_VALUE, out);
        
            fwrite(&pnm_image -> rgb_image_matrix[line][column].blue, sizeof(unsigned char), RUNNER_DEFAUT_VALUE, out);
        
        }
    }    

    fclose(out);
    free(image_type);
    printf("%s\n", FINISHED_WRITING);
}

void write_image(PGMImage *pgm_image, PNMImage *pnm_image, char* output_file_name)
{
    if (pgm_image == NULL)
    {
        printf("%s\n", WRITING_PNM_IMAGE);
        write_PNM_image(pnm_image, output_file_name);
    }

    if (pnm_image == NULL)
    {
        printf("%s\n", WRITING_PGM_IMAGE);
        write_PGM_image(pgm_image, output_file_name);
    }
}


#endif