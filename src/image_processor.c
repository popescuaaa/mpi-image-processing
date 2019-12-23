#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"
#include "filters.h"

int main(int argc, char const *argv[])
{
    PGMImage *image = read_PGM_image("../PGM/darth-vader.pgm");
    write_image(image, NULL, "out.pgm");
    return 0;
}



