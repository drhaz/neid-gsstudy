#include <string.h>
#include <stdio.h>
#include "fitsio.h"

int main(int argc, char *argv[])
{
    fitsfile *file_pointer; 
    int status = 0; 
    int bits_per_pixel;
    int number_of_axies;
    int index_incr;
    long naxes[2] = {1,1}, fpixel[2] = {1,1};
    double *pixels;
    char format[20], header_format[20];

    if (!fits_open_file(&file_pointer, argv[1], READONLY, &status))
    {
        if (!fits_get_img_param(file_pointer, 2, &bits_per_pixel, &number_of_axies, naxes, &status) )
        {
          if (number_of_axies > 2 || number_of_axies == 0)
             printf("Error: only 1D or 2D images are supported\n");
          else
          {
            /* get memory for 1 row */
            pixels = (double *) malloc(naxes[0] * sizeof(double));

            if (pixels == NULL) 
            {
                printf("Memory allocation error\n");
                return(1);
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

            printf("\n      ");          /* print column header */
            for (index_incr = 1; index_incr <= naxes[0]; index_incr++)
            {
                printf(header_format, index_incr);
            }
            printf("\n"); 

            /* loop over all the rows in the image, top to bottom */
            for (fpixel[1] = naxes[1]; fpixel[1] >= 1; fpixel[1]--)
            {
               if (fits_read_pix(file_pointer, TDOUBLE, fpixel, naxes[0], NULL, pixels, NULL, &status) )  /* read row of pixels */
                  break;  /* error */

               printf(" %4d ",fpixel[1]);  /* print row number */

               for (index_incr = 0; index_incr < naxes[0]; index_incr++)
               {
                   printf(format, pixels[index_incr]);   /* print each pixel */
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

    return(status);
}

