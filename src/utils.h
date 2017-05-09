void printPhotom(Photometry * photom); 

void doCentroid(double *pixels);

typedef int bool;
#define true 1
#define false 0

bool writeFits(char *filename, float *pixels);
