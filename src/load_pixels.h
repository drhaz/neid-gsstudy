#include <string.h>
#include <stdio.h>
#include "fitsio.h"

// TOBUILD: gcc -g -I/usr/local/opt/cfitsio3.410/include/ -L/usr/local/opt/cfitsio3.410/lib/ get_fits_pixels.c -o test -lm -lcfitsio
// Does not nessesarily work, yet.

#define MAXAXES 3
#define null '\0'
double * load_pixels(char *filename);

