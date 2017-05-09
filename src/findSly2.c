#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include "Photometry.h"
#include "fitsio.h" /* for bitpix */
#include "centroid.h"

extern int doOTCorrect;
extern int doOTSquareShift;
extern int doOTSquareShiftDwell;
extern int doOTSquareShiftSize;

extern char * timestamp(char * buf);

#define FULL_WIDTH_HALF_MAX_STD 2.35482005
#define SATURATION_LIMIT 60000

#define array_index(image, x, y, winx, winy) ((image) + (y) * winx + (x))

void forxx1(float *max, float *min, Photometry  *photom, double *sums, double *nPixels, int *xx, int *yy)
{


			float imageValue = get_array_float(image, bitpix, xx, yy, winx,
					winy);

			if (imageValue > 0 && imageValue < SATURATION_LIMIT) {

				// check for the brightest/dimmest pixel
				if (xx > skyband + skydelta && xx < winx - skyband - skydelta && yy > skyband + skydelta
						&& yy < winy - skyband - skydelta) {
					//if (min > imageValue) min = imageValue;
                        if (max < imageValue) {
                            max = imageValue;
						photom->maxx = xx;
						photom->maxy = yy;
					}
				}

				// potentially we are in the left sky area
				if (xx >= skydelta && xx < skydelta + skyband) {

					if (yy >= skydelta && yy < skydelta + skyband) {
						sums[0] += imageValue; // lower left sky
						nPixels[0]++;
					} else if (yy < winy - skydelta
							&& yy >= winy - skyband - skydelta) {
						sums[2] += imageValue; // upper left
						nPixels[2]++;
					}

					// or in the right sky area:
				} else if (xx < winx - skydelta
						&& xx >= winx - skydelta - skyband) {

					if (yy >= skydelta && yy < skydelta + skyband) {
						sums[1] += imageValue; // lower right sky
						nPixels[1]++;
					} else if (yy < winy - skydelta
							&& yy >= winy - skyband - skydelta) {
						sums[3] += imageValue; // upper right sky
						nPixels[3]++;
					}
				}

            }
}

void median_filter_middle(int *max01, int *min01, int *max23, int *min23, int *max03, int *med03a, int *min03, int *med03b)
{

    if (sums[0] > sums[1]) {
        max01 = 0;
        min01 = 1;
    } else {
        max01 = 1;
        min01 = 0;
    }
 
    if (sums[2] > sums[3]) {
        max23 = 2;
        min23 = 3;
    } else {
        max23 = 3;
        min23 = 2;
    }
    
    if (sums[max01] > sums[max23]) {
        max03 = max01;
        med03a = max23;
    } else {
        max03 = max23;
        med03a = max01;
    }
   
    if (sums[min01] <= sums[min23]) {
        min03 = min01;
        med03b = min23;
    } else {
        min03 = min23;
        med03b = min01;
    }

}

void forxx2(double *v, double *variance, double *nPixels, void *image, int bitpix, int winx, int winy, int skydelta)
{
           v = get_array_float(image, bitpix, skydelta + xx, skydelta + yy,
                    winx, winy);
            if (v > 0 && v < SATURATION_LIMIT) {
                v -= sums[0];
                variance[0] += v * v;
                nPixels[0]++;
                //  printf("DEBUG Variance %d %d --> %f\n", xx, yy, v);
            }

            v = get_array_float(image, bitpix, winx - skydelta - xx,
                    skydelta + yy, winx, winy);
            if (v > 0 && v < SATURATION_LIMIT) {
                v -= sums[1];
                variance[1] += v * v;
                nPixels[1]++;
            }

            v = get_array_float(image, bitpix, skydelta + xx,
                    winy - skydelta - yy, winx, winy);
            if (v > 0 && v < SATURATION_LIMIT) {
                v -= sums[2];
                variance[2] += v * v;
                nPixels[2]++;
            }

            v = get_array_float(image, bitpix, winx - skydelta - xx,
                    winy - skydelta - yy, winx, winy);
            if (v > 0 && v < SATURATION_LIMIT) {
                v -= sums[3];
                variance[3] += v * v;
                nPixels[3]++;
}


/**
 * Determine the sky level of a guide image and the varaince of the sky level.
 * While looping over the image, the brightest pixel is identified.
 * The sky value is subtracted from the  picture.
 * bitpix determines the cfitsio type of image.
 */
void findSky(void *image, int bitpix, int winx, int winy, int skydelta,
		int skyband, Photometry * photom) {
	assert(skydelta + skyband <= winx);
	assert(skydelta + skyband <= winy);
	assert(skyband != 0); // otherwise div/0 err

	double sums[] = { 0, 0, 0, 0 }; // will hold the sky values in the corners
	double nPixels[4];

	float min = get_array_float(image, bitpix, 0, 0, winx, winy);

	float max = min;
	//int maxIndex = 0;
	// We do one pass over the whole array to find the peak value. In the
	// same pass we sum the flux in little squares in the corners to
	// estimate the sky.
	int xx = 0, yy = 0;
	for (yy = 0; yy < winy; yy++) {
		for (xx = 0; xx < winx; xx++) {
//			float imageValue = get_array_float(image, bitpix, xx, yy, winx,
//					winy);
//
//			if (imageValue > 0 && imageValue < SATURATION_LIMIT) {
//
//				// check for the brightest/dimmest pixel
//				if (xx > skyband + skydelta && xx < winx - skyband - skydelta && yy > skyband + skydelta
//						&& yy < winy - skyband - skydelta) {
//					//if (min > imageValue) min = imageValue;
//                      if (max < imageValue) {
//                           max = imageValue;
//						photom->maxx = xx;
//						photom->maxy = yy;
//					}
//				}
//
//				// potentially we are in the left sky area
//				if (xx >= skydelta && xx < skydelta + skyband) {
//
//					if (yy >= skydelta && yy < skydelta + skyband) {
//						sums[0] += imageValue; // lower left sky
//						nPixels[0]++;
//					} else if (yy < winy - skydelta
//							&& yy >= winy - skyband - skydelta) {
//						sums[2] += imageValue; // upper left
//						nPixels[2]++;
//					}
//
//					// or in the right sky area:
//				} else if (xx < winx - skydelta
//						&& xx >= winx - skydelta - skyband) {
//
//					if (yy >= skydelta && yy < skydelta + skyband) {
//						sums[1] += imageValue; // lower right sky
//						nPixels[1]++;
//					} else if (yy < winy - skydelta
//							&& yy >= winy - skyband - skydelta) {
//						sums[3] += imageValue; // upper right sky
//						nPixels[3]++;
//					}
//				}
//			}
            forxx1(float &max,float &min, photom, sums, nPixels, &xx, &yy)
		}
	}
	// store location and property of brightest pixel
	photom->max = max;

	// normalize the sky counts to the number of pixels involved.

	int ii;
	for (ii = 0; ii < 4; ii++)
		sums[ii] /= nPixels[ii];

	// median filter middle values from sums[0-3]
//	int max01, min01;
//	if (sums[0] > sums[1]) {
//		max01 = 0;
//		min01 = 1;
//	} else {
//		max01 = 1;
//		min01 = 0;
//	}
//	int max23, min23;
//	if (sums[2] > sums[3]) {
//		max23 = 2;
//		min23 = 3;
//	} else {
//		max23 = 3;
//		min23 = 2;
//	}
//	int max03, med03a;
//	if (sums[max01] > sums[max23]) {
//		max03 = max01;
//		med03a = max23;
//	} else {
//		max03 = max23;
//		med03a = max01;
//	}
//	int min03, med03b;
//	if (sums[min01] <= sums[min23]) {
//		min03 = min01;
//		med03b = min23;
//	} else {
//		min03 = min23;
//		med03b = min01;
//	}
//
	// max03 is max of sums[0-3]
	// med03a, med03b
	// min03 is min of sums[0-3]
	// printf("sky median filt max %d med03a %d med03b %d min %d\n",max03, med03a, med03b, min03);

	int max01, min01;
	int max23, min23;
	int max03, med03a;
	int min03, med03b;
    median_filter_middle(int *max01, int *min01, int *max23, int *min23, int *max03, int *med03a, int *min03, int *med03b);

	float skyValue = (sums[med03a] + sums[med03b]) / 2.0;
	photom->skyValue = skyValue;

	// now determine noise of sky in a second pass

	// Reset the nPixels for the second pass
	double variance[4];
	for (ii = 0; ii < 4; ii++)
		nPixels[ii] = 0;

	double v = 0;

	for (xx = 0; xx < skyband; xx++) {
		for (yy = 0; yy < skyband; yy++) {

//			v = get_array_float(image, bitpix, skydelta + xx, skydelta + yy,
//					winx, winy);
//			if (v > 0 && v < SATURATION_LIMIT) {
//				v -= sums[0];
//				variance[0] += v * v;
//				nPixels[0]++;
//				//	printf("DEBUG Variance %d %d --> %f\n", xx, yy, v);
//			}
//
//			v = get_array_float(image, bitpix, winx - skydelta - xx,
//					skydelta + yy, winx, winy);
//			if (v > 0 && v < SATURATION_LIMIT) {
//				v -= sums[1];
//				variance[1] += v * v;
//				nPixels[1]++;
//			}
//
//			v = get_array_float(image, bitpix, skydelta + xx,
//					winy - skydelta - yy, winx, winy);
//			if (v > 0 && v < SATURATION_LIMIT) {
//				v -= sums[2];
//				variance[2] += v * v;
//				nPixels[2]++;
//			}
//
//			v = get_array_float(image, bitpix, winx - skydelta - xx,
//					winy - skydelta - yy, winx, winy);
//			if (v > 0 && v < SATURATION_LIMIT) {
//				v -= sums[3];
//				variance[3] += v * v;
//				nPixels[3]++;
//			}

            forxx2(&v, variance, nPixels, image, bitpix, winx, winy, skydelta);
		}
	}
	// Normalize the variances with the square root of the number of pixels
	double MeanVariance = 0;
	for (ii = 0; ii < 4; ii++) {
		variance[ii] = nPixels[ii] > 0 ? sqrt(variance[ii] / nPixels[ii]) : 0;
		MeanVariance += variance[ii] / 4.;
		//printf ("DEBUG: Background estimate[%d]  % 7.2f +/- % 4.1f\n", ii, sums[ii], variance[ii] );
	}

	photom->backNoise = MeanVariance;

	photom->sigma = MeanVariance > 0 ? (max - skyValue) / MeanVariance : 0;
}
