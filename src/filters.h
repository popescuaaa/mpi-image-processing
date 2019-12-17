#ifndef FILTERS
#define FILTERS

#include "constants.h"

const float smooth[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
    { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
    { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
};

const float approximative_gaussian_blur[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0},
    { 2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0},
    { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}
};

const float sharpen[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 0.0, -2.0 / 3.0, 0.0},
    { -2.0 / 3.0, 11.0 / 3.0, -2.0 / 3.0},
    { 0.0, -2.0 / 3.0, 0.0}
};

const float mean_removal[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { -1.0, -1.0, -1.0},
    { -1.0, 9.0, -1.0},
    { 1.0 / 9.0, 1.0 / 9/0, 1.0 / 9.0}
};

const float embross[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 0.0, 1.0, 0.0},
    { 0.0, 0.0, 0.0},
    { 0.0, -1.0, 0}
};


#endif