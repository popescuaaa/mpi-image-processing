#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

int main(int argc, char const *argv[])
{
    char file_name[40] = "../PGM/darth-vader.pgm";
	int type = image_type(file_name);
	PGMImage *image;
	if(type == PGM_TYPE)
	{
		image = read_PGM_image(file_name);
	}
    
 	
    #ifdef  DEBUG
		FILE *out = fopen("out.pgm", "wb");

		fwrite(image -> image_type, sizeof(image -> image_type), sizeof(char), out);
		fwrite(&image -> width, 1, sizeof(int), out);
		fwrite(&image -> height, 1, sizeof(int), out);

		for (int i =0;i < image->width * image->height; i++) 
		{
			fwrite(&image -> image_content[i], 1, sizeof(char), out);
		}

		fclose(out);
    #endif

	printf("%d\n", image -> width);
	char *out = "test.pgm";
	write_image(image, NULL, out);
    free(image);
    return 0;
}


