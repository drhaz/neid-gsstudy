#include <string.h>
#include <stdio.h>
#include "fitsio.h"

// TOBUILD: gcc -g -I/usr/local/opt/cfitsio3.410/include/ -L/usr/local/opt/cfitsio3.410/lib/ get_fits_pixels.c -o test -lm -lcfitsio

#define XDIM 127
#define YDIM 127
#define MAXAXES 3
#define null '\0'

double * load_pixels(char *);
void free_1D_matrix(double *);
void print_pixels(double *);

