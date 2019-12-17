#ifndef IMAGE_PROCESSOR
#define IMAGE_PROCESSOR

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

typedef struct{
    int width;
    int height;
    char *image_type;
    char *image_content;
} Image;


 Image *read_image(char *image_file_name) 
 {
    Image *image_structure = (Image *) malloc(1 * sizeof(Image));

    FILE *image = fopen(image_file_name, "rb");

    if (image == NULL)
    {
        printf("%s\n", error_message);
        exit(1);
    }    

    char *image_type = (char*) malloc(2 * sizeof(char));
    image_type[0] = fgetc(image);
    image_type[1] = fgetc(image);
    
    image_structure->image_type = image_type;
    char comment_line[100];
    

    fclose(image);
    return image_structure;
}

#endif