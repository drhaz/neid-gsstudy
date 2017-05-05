#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

/* Load the pixels from the fits file into a double array. */
float * load_pixels(char *filename)
{
    fitsfile *file_pointer; 
    int status = 0; 
    int bits_per_pixel;
    int number_of_axies;
//    int index_incr;
    long naxes[3] = {1,1,1}, fpixel[3] = {1,1,1};
 //long naxes2[2] = {128,128}, fpixel2[2] = {1,1};
 //fitsfile *file_pointer2; 
    float *return_1D_pixels_f = null;

    if (!fits_open_file(&file_pointer, filename, READONLY, &status))
    {
        if (!fits_get_img_param(file_pointer, MAXAXES, &bits_per_pixel, &number_of_axies, naxes, &status) )
        {
printf("%i\n", bits_per_pixel);
          if (number_of_axies < MAXAXES)
          {
             printf("Error: only D images are supported\n");
          }
          else
          {

            return_1D_pixels_f  = (float *) malloc((naxes[0] * naxes[1]) * sizeof(float));

            fits_read_pix(file_pointer, TFLOAT, fpixel, (naxes[0] * naxes[1]), NULL, return_1D_pixels_f, NULL, &status);

            if (status)
            {
                fits_report_error(stderr, status); /* print error */
            }

          }
        }
        fits_close_file(file_pointer, &status);

    }


    printf("Internal doCentroid(return_1D_pixels_f) #2;\n");
    doCentroid(return_1D_pixels_f);

    printf("\n\n");

    return(return_1D_pixels_f);
}

