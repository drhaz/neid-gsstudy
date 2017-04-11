
#include <string.h>
#include <stdio.h>
#include "fitsio.h"

int main(int argc, char *argv[]) 
{
    fitsfile *fptr;
    char card[FLEN_CARD];
    int status = 0, nkeys, ii;  /* MUST initialize status */

    if(argc < 2)
    {
        fprintf(stderr, "Error: Usage %s FITS_FILE\n", argv[0]);
	exit(0);
    }

    fits_open_file(&fptr, argv[1], READONLY, &status);
    fits_get_hdrspace(fptr, &nkeys, NULL, &status);
    for (ii = 1; ii <= nkeys; ii++) 
    {
        fits_read_record(fptr, ii, card, &status); /* read keyword */
        printf("%s\n", card);
    }

    printf("END\n\n");  /* terminate listing with END */
    fits_close_file(fptr, &status);

    if (status)          /* print any error messages */
        fits_report_error(stderr, status);
    return (status);
}
