/**
 * // Popescu Andrei-Gabriel
 * // 333CA
 * // Main header for image data reading
 *
 *
 *    Considering the binary forat th easiest way is to read 
 *    using fread function from stdio in c and then pass the whole 
 *    image structure to the main function
 *
 *    // TODO: defensive programming over all parameters
 *    // TODO: add param and return type for all functions
 * 
 */ 

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

    unsigned char *image_type = 
            (unsigned char*) malloc(IMAGE_TYPE_SIZE * sizeof(char));

    fscanf(image, "%s\n", image_type);

    image_structure -> image_type = image_type;

    unsigned char comment_line[45];
    
    fscanf(image, "%[^\n]%*c", comment_line);
    
    int width, height, max_val;

    fscanf(image, "%d %d\n", &width, &height); 

    image_structure -> width = width;
    
    image_structure -> height = height;
    
    fscanf(image, "%d\n", &max_val);
  
    image_structure -> max_val = max_val;

    unsigned char *image_content = 
               (unsigned char *) malloc((image_structure -> width * 
                                         image_structure -> height) * 
                                         sizeof(unsigned char));


    unsigned char **image_matrix = 
                (unsigned char **) malloc(image_structure -> height * 
                                          sizeof(unsigned char *));

    for (line = 0; line < image_structure -> height; ++line) 
    {
        image_matrix[line] = 
                (unsigned char *) malloc(image_structure -> width * 
                                         sizeof(unsigned char));
    }
    
    for (line = 0; line < image_structure -> height; ++line) 
    {
        fread(image_matrix[line], sizeof(unsigned char), image_structure -> width, image);    
        
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

    unsigned char *image_type = 
            (unsigned char*) malloc(IMAGE_TYPE_SIZE * sizeof(char));

    fscanf(image, "%s\n", image_type);

    image_structure -> image_type = image_type;

    unsigned char comment_line[45];
    
    fscanf(image, "%[^\n]%*c", comment_line);
    
    int width, height, max_val;

    fscanf(image, "%d %d\n", &width, &height); 

    image_structure -> width = width;
    
    image_structure -> height = height;
    
    fscanf(image, "%d\n", &max_val);
  
    image_structure -> max_val = max_val;

    Pixel *rgb_content = 
            (Pixel *) malloc(image_structure -> width * 
                             image_structure -> height * 
                             sizeof(Pixel));
    

    Pixel **rgb_image_matrix= 
            (Pixel **) malloc(image_structure -> height * 
                              sizeof(Pixel *));

    for (line = 0; line < image_structure -> height; ++line) 
    {
        rgb_image_matrix[line] = 
            (Pixel *) malloc(image_structure -> width * 
                             sizeof(Pixel));
    }

    
    for (line = 0; line < image_structure -> height; ++line) 
    {
            fread(rgb_image_matrix[line], sizeof(Pixel), image_structure -> width, image);
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
   

    fwrite(pgm_image -> image_type, 
           sizeof(unsigned char), 
           IMAGE_TYPE_SIZE, 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(comment_line, 
           sizeof(unsigned char), 
           COMMENT_LINE_FIXED_LENGTH, 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(width, 
           sizeof(unsigned char), 
           strlen(width), 
           out);

    fwrite(delimiter, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(height, 
           sizeof(unsigned char), 
           strlen(height), 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(max_val, 
           sizeof(unsigned char), 
           strlen(max_val), 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);
        
    for (line = 0; line < pgm_image -> height; ++line) 
    {
    
        fwrite(pgm_image -> image_matrix[line], sizeof(unsigned char), pgm_image -> width, out);
        
    }

    fclose(out);
    printf("%s\n", FINISHED_WRITING);
}

void write_PNM_image(PNMImage *pnm_image, char *output_file_name)
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
    sprintf(width, "%d", pnm_image -> width);
    sprintf(height, "%d", pnm_image -> height);
    sprintf(max_val, "%d", pnm_image -> max_val);
   

    fwrite(pnm_image -> image_type, 
           sizeof(unsigned char), 
           IMAGE_TYPE_SIZE, 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(comment_line, 
           sizeof(unsigned char), 
           COMMENT_LINE_FIXED_LENGTH, 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(width, 
           sizeof(unsigned char), 
           strlen(width), 
           out);

    fwrite(delimiter, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(height, 
           sizeof(unsigned char), 
           strlen(height), 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);

    fwrite(max_val, 
           sizeof(unsigned char), 
           strlen(max_val), 
           out);

    fwrite(separator, 
           sizeof(unsigned char), 
           1, 
           out);
  
    for (line = 0; line < pnm_image -> height; ++line) 
    {
        fwrite(pnm_image -> rgb_image_matrix[line], sizeof(Pixel), pnm_image -> width, out);
        
    }

    fclose(out);
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