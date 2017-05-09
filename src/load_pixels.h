#ifndef SOME_HEADER_FILE_H
#define SOME_HEADER_FILE_H

#include <string.h>
#include <stdio.h>
#include "fitsio.h"
#include "Photometry.h"
#include "utils.h"

// TOBUILD: gcc -g -I/usr/local/opt/cfitsio3.410/include/ -L/usr/local/opt/cfitsio3.410/lib/ get_fits_pixels.c -o test -lm -lcfitsio

#define XDIM 128
#define YDIM 128
#define MAXAXES 3
#define null '\0'

double * load_pixels(char *, int );

void free_1D_matrix(double *);
void print_pixels(double *);


#endif
