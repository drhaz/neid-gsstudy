/* Convert a Double Array to a Float Array. */
float * dtofArray(double *pixels);

/* Comapre Arrays. */
void arrayCMPFF(float *pixels1, float *pixels2);

/* Comapre Arrays. */
void arrayCMPDD(double *pixels1, double *pixels2);

/* Comapre Arrays. */
void arrayCMPDF(double *pixels1, float *pixels2);

/* Copy Array. */
double * copyDArray(double *pixels);

/* Copy Array. */
float * copyfArray(float *pixels);

/* Print the pixels in a readable format. */
void print_pixels(double *pixels);

/* Free 1D array. */
void free_1D_matrix(double *x);

/* Make a 2D double array. */
double ** make_double_matrix(int xsize, int ysize);

/* Free 2D array. */
void free_matrix(double **x, int ysize);

void printPhotom(Photometry * photom); 

void doCentroid(float *pixels);

