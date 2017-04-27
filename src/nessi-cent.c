#include "centroid.h"  
#include "load_pixels.h"
#include <fitsio.h>
#include "Photometry.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

/* Make a float array. */
float * make_1D_float_matrix(int xsize, int ysize)
{
    float *x = malloc((ysize * xsize) * sizeof(float));

    return x;
}

/* Print the pixels in a readable format. */
float * dtofArray(double *pixels)
{
    int x, y = 0;

    float *z = make_1D_float_matrix(XDIM, YDIM);

    for (y = 0; y <= YDIM * XDIM; y++)
    {
        z[y] = (float) pixels[y];
    }

    return z;
}

static void printPhotom(Photometry * photom) {
        printf( "photometry: \t momentx %f momenty %f flux %f back %f fwhm_x %f \n",
                photom->xctr, photom->yctr, photom->Flux, photom->skyValue,
                photom->FWHM_X);
}

// The main loop. Process one image in fits file.
int main(int argc, char *argv[])
{
    double *test = load_pixels(argv[1]);

    float *test_float = dtofArray(test);
    float *max = test_float+9924;
    float xctr, yctr, sigma = 0.0;

    Photometry photom;

    findSky(test_float, FLOAT_IMG, 128, 128, 1, 8, &photom);

    centroid(test_float, FLOAT_IMG, 3, 3, 120, 120, 128,128, &photom);

    printPhotom(&photom);

    free_1D_matrix(test);
}

