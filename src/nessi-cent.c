#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio


// The main loop. Process one image in fits file.
int main(int argc, char *argv[])
{
    float *test = load_pixels(argv[1]);
    float *test_original = load_pixels(argv[1]);

    doCentroid(test);
    doCentroid(test);
    printf("\n \n");

    doCentroid(test_original);
    printf("\n \n");

//    remove("frame1.fits");
//    remove("frame2.fits");
//    writeFits("frame1.fits", test);
//    writeFits("frame2.fits", test_original);
}

