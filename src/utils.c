#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

void printPhotom(Photometry * photom) {
        printf( "photometry: \t momentx %f momenty %f flux %f back %f fwhm_x %f sigNoise %f \n",
                photom->xctr, photom->yctr, photom->Flux, photom->skyValue,
                photom->FWHM_X, photom->betterSigNoise);
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

    photom->betterSigNoise = 0.0;

}

void doCentroid(double *pixels)
{

    //printf("pixels: %X \n", pixels);
    //Photometry *photom = malloc(sizeof(Photometry));
    //printf("Photometry %i, photom %i\n", sizeof(Photometry), sizeof(*photom));
    Photometry photom;

    initPhotom(&photom);

    //findSky(pixels, FLOAT_IMG, 128, 128, 1, 8, photom);
    findSky(pixels, DOUBLE_IMG, 128, 128, 1, 8, &photom);

    //centroid(pixels, FLOAT_IMG, 3, 3, 120, 120, 128,128, photom);
    centroid(pixels, DOUBLE_IMG, 3, 3, 120, 120, 128,128, &photom);

    //printPhotom(photom);
    printPhotom(&photom);


}

bool writeFits(char *filename, float *pixels)
{

    int status= 0;
    fitsfile *file_pointer;
    int naxis = 2;
    long naxes[2] = {XDIM,YDIM}, fpixel[2] = {1,1};
    bool isok = true;

    if (fits_create_file(&file_pointer, filename, &status)) 
    {
        isok = false;
        fits_report_error(stderr, status);
    }

    if (fits_create_img(file_pointer,  FLOAT_IMG, naxis, naxes, &status))
    {
        isok = false;
        fits_report_error(stderr, status);
    }

    if (fits_write_pix(file_pointer, TFLOAT, fpixel, YDIM * XDIM, pixels, &status))
    {
        isok = false;
        fits_report_error(stderr, status);
    }

    if(fits_close_file(file_pointer, &status))
    {
        isok = false;
         fits_report_error(stderr, status);
   }

    return isok;
}
