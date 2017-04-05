
/**
 * type of image is determined by cfitsio coded bitpix
 */
void centroid(void * image, int bitpix, int minX, int minY, int maxX, int maxY, int winx, int winy, Photometry * photom);
void findSky(void * image, int bitpix, int winx, int winy, int skydelta, int skyband, Photometry * photom);
int init_photom(xmlDocPtr doc, Photometry * photometry, int photsize);
