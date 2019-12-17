/**
 *  Popescu Andrei Gabriel
 *  333CA
 *  
 *  Types definitions for black and white images
 *  and RGB images. 
 * 
 */

#ifndef TYPES
#define TYPES

/**
 *  Black and White image structure
 *  PGM type images
 */

typedef struct
{
    int width;
    int height;
    int max_val;
    unsigned char *image_type;
    unsigned char *image_content;
} PGMImage;


/**
 *  Black and White image structure
 *  PNM type images
 */

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;

} Pixel;

typedef struct
{
    int width;
    int height;
    int max_val;
    unsigned char *image_type;
    Pixel *rgb_content;
} PNMImage;


#endif