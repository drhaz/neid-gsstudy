#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio


// The main loop. Process one image in fits file.
int main(int argc, char *argv[])
{
    double return_1D_pixels_f[(128 * 128) * sizeof(float)];
    double *test = load_pixels(argv[1], return_1D_pixels_f);
    //float *test_original = load_pixels(argv[1], return_1D_pixels_f);

    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(test);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    doCentroid(return_1D_pixels_f);
    printf("\n \n");

    //doCentroid(test_original);
    printf("\n \n");

//    remove("frame1.fits");
//    remove("frame2.fits");
//    writeFits("frame1.fits", test);
//    writeFits("frame2.fits", test_original);
}

