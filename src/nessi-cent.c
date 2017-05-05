#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio


// The main loop. Process one image in fits file.
int main(int argc, char *argv[])
{
    float *test = load_pixels(argv[1]);
    float *test_original = load_pixels(argv[1]);
    //float *test_original = copyfArray(test);

    //arrayCMPFF(test, test_original);

    //printf("\nFrom Main #1 %X \n", (int) test);
    doCentroid(test);
    doCentroid(test);
//    arrayCMPFF(test, test_original);
    printf("\n \n");

    //printf("\nFrom Main #2 %X \n", test_original);
    doCentroid(test_original);
 //   arrayCMPFF(test, test_original);
    printf("\n \n");
}

