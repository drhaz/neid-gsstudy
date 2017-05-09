#include "load_pixels.h"
#include "centroid.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

typedef struct nessi_frames
{

    double *frames[1000];

} nessi_frames;

/* Load the pixels from the fits file into a double array. */
double ** load_pixels(char *filename)
{
    fitsfile *file_pointer; 
    double *return_1D_pixels_f;
    int status = 0; 
    int ex;
    int bits_per_pixel;
    int number_of_axies;
    nessi_frames *nFrames;
    long naxes[3] = {1,1,1}, fpixel[3] = {1,1,1};

    nFrames  = (nessi_frames *) malloc(sizeof(nessi_frames));

    if (!fits_open_file(&file_pointer, filename, READONLY, &status))
    {
        if (!fits_get_img_param(file_pointer, MAXAXES, &bits_per_pixel, &number_of_axies, naxes, &status) )
        {
          if (number_of_axies < MAXAXES)
          {
             printf("Error: only D images are supported\n");
          }
          else
          {
            for(ex = 0; ex < 1000; ex++)
            {
               return_1D_pixels_f  = (double *) malloc((naxes[0] * naxes[1]) * sizeof(double));

                fpixel[2] = (ex + 1);

                fits_read_pix(file_pointer, TDOUBLE, fpixel, (naxes[0] * naxes[1]), NULL, return_1D_pixels_f, NULL, &status);

                if (status)
                {
                    fits_report_error(stderr, status); /* print error */
                }

               nFrames->frames[ex] = return_1D_pixels_f; 
            }
          }
        }
        fits_close_file(file_pointer, &status);

    }

    return( nFrames->frames);
}

