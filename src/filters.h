#ifndef FILTERS
#define FILTERS

#include <string.h>
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
    { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
};

const float embross[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 0.0, 1.0, 0.0},
    { 0.0, 0.0, 0.0},
    { 0.0, -1.0, 0.0}
};

const float default[FILTER_HEIGHT][FILTER_WIDTH] = 
{
    { 0.0, 0.0, 0.0},
    { 0.0, 1.0, 0.0},
    { 0.0, 0.0, 0.0}
};


const float **get_filter_by_name(char name[]) 
{
  if (strcmp(name, "smooth") == 0)
  {
      return smooth;
  }
    
  if (strcmp(name, "blur") == 0)
  {
      return approximative_gaussian_blur;
  }
   
  if (strcmp(name, "sharpen") == 0)
  {
       return sharpen;
  }
   
  if (strcmp(name, "mean") == 0)
  {
       return mean_removal;
  }
   
  if (strcmp(name, "embross") == 0)
  {
      return embross;
  }

  /**
   *  Handle non-valid filter names
   **/ 
  return default;
}

#endif