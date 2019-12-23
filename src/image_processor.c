#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"
#include "filters.h"

int main(int argc, char const *argv[])
{
    PNMImage *image = read_PNM_image("../PNM/lena.pnm");
    write_image(NULL, image, "test.pnm");
    return 0;
}



