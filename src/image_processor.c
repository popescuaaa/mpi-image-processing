#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"
#include "filters.h"

int main(int argc, char const *argv[])
{
    PGMImage *image = read_PGM_image("../PGM/darth-vader.pgm");

    unsigned char **tmp_matrix = image -> image_matrix;
    for (int line = 1; line < image -> height - 1; line ++)
    {
        for (int column = 1; column < image -> width - 1; column ++)
        {
            image -> image_matrix[line][column] =
                       smooth[0][0] * tmp_matrix[line - 1][column - 1] +
                       smooth[0][1] * tmp_matrix[line - 1][column] +
                       smooth[0][2] *tmp_matrix[line - 1][column + 1] +
                       smooth[1][0] * tmp_matrix[line][column - 1] +
                       smooth[1][1] * tmp_matrix[line][column] +
                       smooth[1][2] * tmp_matrix[line][column + 1] +
                       smooth[2][0] * tmp_matrix[line + 1][column - 1] +
                       smooth[2][1] * tmp_matrix[line + 1][column] +
                       smooth[2][2] * tmp_matrix[line + 1][column + 1];
        }
    }
    

    write_image(image, NULL, "out.pgm");
    return 0;
}



