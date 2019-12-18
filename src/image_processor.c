#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

int main(int argc, char const *argv[])
{
    char file_name[40] = "../PNM/baby-yoda.pnm";
	int type = image_type(file_name);
	PNMImage *image;
	if(type == PNM_TYPE)
	{
		image = read_PNM_image(file_name);
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
	char *out = "test.pnm";
	write_image(NULL, image, out);
    free(image);
    return 0;
}


