#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio


// The main loop. Process one image in fits file.
int main(int argc, char *argv[])
{
    int ex;

    double **test = load_pixels(argv[1]);
    for(ex = 0; ex < 1000; ex++)
    {
        printf("%s:%i ", argv[1], ex);
        doCentroid(test[ex]);
    }

    printf("\n");

//    remove("frame2.fits");
//    writeFits("frame1.fits", test);
//    writeFits("frame2.fits", test_original);
}

