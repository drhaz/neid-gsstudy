#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

/* Load the pixels from the fits file into a double array. */
double * load_pixels(char *filename, int frameNum)
{
    fitsfile *file_pointer; 
    double *return_1D_pixels_f;
    int status = 0; 
    int bits_per_pixel;
    int number_of_axies;
    long naxes[3] = {1,1,1}, fpixel[3] = {1,1,1};

    if (!fits_open_file(&file_pointer, filename, READONLY, &status))
    {
        if (!fits_get_img_param(file_pointer, MAXAXES, &bits_per_pixel, &number_of_axies, naxes, &status) )
        {
//printf("%i\n", bits_per_pixel);
          if (number_of_axies < MAXAXES)
          {
             printf("Error: only D images are supported\n");
          }
          else
          {

           // return_1D_pixels_f  = (float *) malloc((naxes[0] * naxes[1]) * sizeof(float));
           return_1D_pixels_f  = (double *) malloc((naxes[0] * naxes[1]) * sizeof(double));


            fpixel[2] = (frameNum + 1);

            fits_read_pix(file_pointer, TDOUBLE, fpixel, (naxes[0] * naxes[1]), NULL, return_1D_pixels_f, NULL, &status);

            if (status)
            {
                fits_report_error(stderr, status); /* print error */
            }

          }
        }
        fits_close_file(file_pointer, &status);

    }


//    printf("internal docentroid(return_1d_pixels_f) #2;\n");
//    docentroid(return_1d_pixels_f);
//
//   printf("\n\n");

    return(return_1D_pixels_f);
}

