#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio
/*
float * dtofArray(double *pixels)
{
    int x = 0;
    int y = 0;

    float *z =  (float *) malloc((YDIM * XDIM) * sizeof(float));


    for (y = 0; y < ((YDIM * XDIM) - 1); y++)
    {
        z[y] = (float) pixels[y];
    }

    return z;
}

void arrayCMPFF(float *pixels1, float *pixels2)
{
    int x = 0; 
    int y = 0;


    for (y = 0; y < ((YDIM * XDIM) - 1); y++)
    {
        if( pixels1[y] != pixels2[y])
        {
            printf("-------------------------------------------------------->FAIL! %i\n", y);
        }
    }

}

void arrayCMPDD(double *pixels1, double *pixels2)
{
    int x = 0; 
    int y = 0;


    for (y = 0; y < ((YDIM * XDIM) -1); y++)
    {
        if( pixels1[y] != pixels2[y])
        {
            printf("-------------------------------------------------------->FAIL! %i\n", y);
        }
    }

}

void arrayCMPDF(double *pixels1, float *pixels2)
{
    int x = 0; 
    int y = 0;


    for (y = 0; y < ((YDIM * XDIM) - 1); y++)
    {
        if( pixels1[y] != (double) pixels2[y])
        {
            printf("-------------------------------------------------------->FAIL! %i\n", y);
        }
    }

}

double * copyDArray(double *pixels)
{
    int x = 0; 
    int y = 0;

    double *z =  (double *) malloc((YDIM * XDIM) * sizeof(double));


    for (y = 0; y < ((YDIM * XDIM) - 1); y++)
    {
        z[y] = pixels[y];
    }

    return z;
}

float * copyfArray(float *pixels)
{
    int x = 0; 
    int y = 0;

    float *z =  (float *) malloc((YDIM * XDIM) * sizeof(float));


    for (y = 0; y < ((YDIM * XDIM) - 1); y++)
    {
        z[y] = pixels[y];
    }

    return z;
}

void print_pixels(double *pixels)
{
    int x = 0; 
    int y = 0;
    
    for (y = 0; y <= (YDIM - 1); y++)
    {
        for (x = 0; x <= (XDIM -1); x++)
        {
            printf("%i, ", (int) pixels[ (128 * y) + x ] );
        }
            printf("\n");

    }

}

void free_1D_matrix(double *x)
{

    free(x);

    return;
}

double ** make_double_matrix(int xsize, int ysize)
{
    double **x = (double **) malloc(ysize * sizeof(double*));

    int i = 0;

    for (i = 0; i < (ysize - 1); i++) 
    {
        x[i] = (double *) malloc(xsize * sizeof(double));
    }

    return x;
}

void free_matrix(double **x, int ysize)
{

    int i = 0;

    for (i = 0; i < (ysize - 1); i++) 
    {
        free(x[i]);
    }

    free(x);

    return;
}
*/

void printPhotom(Photometry * photom) {
        printf( "photometry: \t momentx %f momenty %f flux %f back %f fwhm_x %f \n",
                photom->xctr, photom->yctr, photom->Flux, photom->skyValue,
                photom->FWHM_X);
}

void initPhotom(Photometry *photom)
{

    photom->skyValue = 0.0;    /** Estiamted sky background value */

    photom->skyVar = 0.0;    /** Estiamted sky variance */

    photom->sigNoise = 0.0;    /** Estiamted signal to noise of star */

    photom->sigma = 0.0;    /** Estimated detection probability in sigma */

    photom->snThreshold = 0.0;    /** specified S/N floor for guide star photometry to be locked */

    photom->backNoise = 0.0;    /** Background noise; TBD how different than skyVar */

    photom->xctr = 0.0;    /** X centroid of guide star in window */

    photom->yctr = 0.0;    /** Y centroid of guide star in window */

    photom->max = 0.0;    /** Value of brightest pixel */

    photom->maxx = 0;    /** X location of brightest pixel */
  
    photom->maxy = 0;    /** Y location of brightest pixel */

    photom->maxp = 0; // index of max pixel     array index to brightest pixel 

    photom->FWHM_X = 0.0;    /** FWHM in X (unbinned pixels) */

    photom->FWHM_Y = 0.0;    /** FWHM in Y (unbinned pixels */

    photom->Flux = 0.0;    /** Total flux in star in ADU*/

    photom->Peak = 0.0;    /** Peak value; redundant w/ max ? */

    photom->Roundness = 0.0;    /** Roundness estimate of star */

    photom->MomentXY = 0.0;    /** XY moment of star */

    photom->tstamp = 0.0;    /** time stamp of guide video */

}

void doCentroid(float *pixels)
{

    //printf("pixels: %X \n", pixels);
    //Photometry *photom = malloc(sizeof(Photometry));
    //printf("Photometry %i, photom %i\n", sizeof(Photometry), sizeof(*photom));
    Photometry photom;

    initPhotom(&photom);
    printf("Photometry %lu, photom %lu\n", sizeof(Photometry), sizeof(photom));

    //findSky(pixels, FLOAT_IMG, 128, 128, 1, 8, photom);
    findSky(pixels, FLOAT_IMG, 128, 128, 1, 8, &photom);

    //centroid(pixels, FLOAT_IMG, 3, 3, 120, 120, 128,128, photom);
    centroid(pixels, FLOAT_IMG, 3, 3, 120, 120, 128,128, &photom);

    //printPhotom(photom);
    printPhotom(&photom);


}

