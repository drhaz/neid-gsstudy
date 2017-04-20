#include "load_pixels.h"

// TOBUILD: gcc -g -I/usr/local/opt/cfitsio3.410/include/ -L/usr/local/opt/cfitsio3.410/lib/ load_pixels.c -o test -lm -lcfitsio
// Does not nessesarily work, yet.

int main(int argc, char *argv[])
{
    double *test = load_pixels(argv[1]);
    printf("hi");
}

double * make_1D_double_matrix(int xsize, int ysize)
{
    double *x = malloc((ysize * xsize) * sizeof(double*));

    return x;
}

void free_1D_matrix(double *x)
{

    free(x);

    return;
}

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
    char format[20], header_format[20];

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

            return_pixels = make_double_matrix(naxes[0], naxes[1]);
            return_1D_pixels = make_1D_double_matrix(naxes[0], naxes[1]);

            /* get memory for 1 row */
            pixels = (double *) malloc(naxes[0] * sizeof(double));

            if (pixels == NULL) 
            {
                printf("Memory allocation error\n");
                return(null);
            }

            if (bits_per_pixel > 0)  /* set the default output format string */
            {
               strcpy(header_format, " %7d");
               strcpy(format,   " %7.0f");
            } 
            else 
            {
               strcpy(header_format, " %15d");
               strcpy(format,   " %15.5f");
            }

            printf("print column header\n      ");          /* print column header */
            for (index_incr = 1; index_incr <= naxes[0]; index_incr++)
            {
                printf(header_format, index_incr);
            }
            printf("print column header\n"); 

            /* loop over all the rows in the image, top to bottom */
            for (fpixel[1] = naxes[1]; fpixel[1] >= 1; fpixel[1]--)
            {
               if (fits_read_pix(file_pointer, TDOUBLE, fpixel, naxes[0], NULL, pixels, NULL, &status) )  /* read row of pixels */
                  break;  /* error */

               printf("row number %4d ",fpixel[1]);  /* print row number */

               for (index_incr = 0; index_incr < naxes[0]; index_incr++)
               {
                   printf(format, pixels[index_incr]);   /* print each pixel */
                   return_pixels[( fpixel[1] - 1) ][index_incr] = pixels[index_incr];
                   return_1D_pixels[ (128 * (fpixel[1] - 1)) + index_incr ] = pixels[index_incr];
               }
               printf("\n");
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

    //return(status);
    return(return_1D_pixels);
}

