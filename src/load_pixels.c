#include "load_pixels.h"

// TOBUILD: gcc -g -I/usr/local/include/ -L/usr/local/lib/ load_pixels.c -o testo -lm -lcfitsio

/* Make a double array. */
double * make_1D_double_matrix(int xsize, int ysize)
{
    double *x = malloc((ysize * xsize) * sizeof(double));

    return x;
}

/* Print the pixels in a readable format. */
void print_pixels(double *pixels)
{
    int x, y = 0;
    
    for (y = 0; y <= YDIM; y++)
    {
        for (x = 0; x <= XDIM; x++)
        {
            printf("%i, ", (int) pixels[ (128 * y) + x ] );
        }
            printf("\n");

    }

}

/* Free 1D array. */
void free_1D_matrix(double *x)
{

    free(x);

    return;
}

/* Make a 2D double array. */
double ** make_double_matrix(int xsize, int ysize)
{
    double **x = malloc(ysize * sizeof(double*));

    int i = 0;

    for (i = 0; i < ysize; i++) 
    {
        x[i] = malloc(xsize * sizeof(double));
    }

    return x;
}

/* Free 2D array. */
void free_matrix(double **x, int ysize)
{

    int i = 0;

    for (i = 0; i < ysize; i++) 
    {
        free(x[i]);
    }

    free(x);

    return;
}

/* Load the pixels from the fits file into a double array. */
double * load_pixels(char *filename)
{
    fitsfile *file_pointer; 
    int status = 0; 
    int bits_per_pixel;
    int number_of_axies;
    int index_incr;
    long naxes[3] = {1,1,1}, fpixel[3] = {1,1,1};
    double *pixels;
    double **return_pixels;
    double *return_1D_pixels;

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

            return_1D_pixels = make_1D_double_matrix(naxes[0], naxes[1]);

            /* get memory for 1 row */
            pixels = (double *) malloc(naxes[0] * sizeof(double));

            if ((pixels == NULL) || (return_1D_pixels == NULL))
            {
                printf("Memory allocation error\n");
                return(null);
            }

            /* loop over all the rows in the image, top to bottom */
            for (fpixel[1] = naxes[1]; fpixel[1] >= 1; fpixel[1]--)
            {
               if (fits_read_pix(file_pointer, TDOUBLE, fpixel, naxes[0], NULL, pixels, NULL, &status) )  /* read row of pixels */
                  break;  /* error */

               for (index_incr = 0; index_incr < naxes[0]; index_incr++)
               {
                   return_1D_pixels[ (128 * (fpixel[1] - 1)) + index_incr ] = pixels[index_incr];
               }
            }
            free(pixels);  /* Housekeeping! */
          }
        }
        fits_close_file(file_pointer, &status);
    }

    if (status)
    {
       fits_report_error(stderr, status); /* print error */
    }

    return(return_1D_pixels);
}

