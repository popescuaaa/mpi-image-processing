#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

int main(int argc, char const *argv[])
{
    char file_name[40] = "../PGM/darth-vader.pgm";
    Image *image = read_image(file_name);
    printf("%s\n", image->image_type);

    free(image);
    return 0;
}


