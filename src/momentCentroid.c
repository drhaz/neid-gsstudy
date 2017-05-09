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

//inline double get_array_float(void * image, int bitpix, int x, int y, int winx,
double get_array_float(void * image, int bitpix, int x, int y, int winx,
		int winy) {
	double val = 0;
	double * ptr_f;
	double * ptr_d;
	long * ptr_l;
	unsigned long * ptr_ul;
	short * ptr_s;
	unsigned short * ptr_us;
	switch (bitpix) {
	case DOUBLE_IMG:
		ptr_d = array_index(((double * )image), x, y, winx, winy);
		val = *ptr_d;
		break;
	case FLOAT_IMG:
		ptr_f = array_index(((double * )image), x, y, winx, winy);
		val = *ptr_f;
		break;
	case LONG_IMG:
		ptr_l = array_index(((long * )image), x, y, winx, winy);
		val = *ptr_l;
		break;
	case ULONG_IMG:
		ptr_ul = array_index(((unsigned long * )image), x, y, winx, winy);
		val = *ptr_ul;
		break;
	case SHORT_IMG:
		ptr_s = array_index(((short * )image), x, y, winx, winy);
		val = *ptr_s;
		break;
	case USHORT_IMG:
		ptr_us = array_index(((unsigned short * )image), x, y, winx, winy);
		val = *ptr_us;
		break;
	default:
		fprintf(stderr, "unknown image bitpix %d\n", bitpix);
		break;
	}
	return val;
}

void centroid(void * image, int bitpix, int minX, int minY, int maxX, int maxY,
		int winx, int winy, Photometry * photom) {
	double SigmaThres = 3.;
	double sky = photom->skyValue;
	double peakflux = photom->max - sky;
	double noise = photom->backNoise;

	double totalFlux = 0;
	long nPixels = 0;
	double max = 0;
	double stellarFlux = 0;

	if (peakflux < 0) {
		//printf("Peak value of star is <0. That is fishy. Continuing anyway.");
	}
	// check boundaries
	if (minX < 0)
		minX = 0;
	if (maxX > winx)
		maxX = winx;
	if (minY < 0)
		minY = 0;
	if (maxY > winy)
		maxY = winy;

	// One sanity check: is the threshold compatible with the noise?
	double dr = peakflux;
	photom->sigNoise = 0;
	if (noise > 0) {
		dr /= noise;
		photom->sigNoise = dr;
		if (dr < 5) {
			//printf("MomentAnalysis: Star has low S/N\n");
		}
	} else {
		printf("WARNING: BAD photometry!\n");
	}

	double minimumFlux = noise * SigmaThres;
	double momentXX = 0;
	double momentYY = 0;
	double momentXY = 0;
	double centerX = 0;
	double centerY = 0;

	// TODO: Why the -1?
	int xx = 0;
	for (xx = minX; xx < maxX - 1; xx++) {
		int yy = 0;
		for (yy = minY; yy < maxY - 1; yy++) {

			double pixelFlux = get_array_float(image, bitpix, xx, yy, winx,
					winy);
			// while we are iterating anyways we can as well improve the peak estimate.:
			if (pixelFlux > max) {
				max = pixelFlux;
			}
			pixelFlux -= sky;

			// Do some basic flux estimate
			if (pixelFlux >= minimumFlux) {
				// TODO: should this be summed outside the noise threshold?
				stellarFlux += pixelFlux;
				centerX += pixelFlux * (double) xx;
				centerY += pixelFlux * (double) yy;

				totalFlux += pixelFlux;
				nPixels++;
			}
		}
	}

	// Make sure the total flux is positive, otherwise you're in trouble!
	if (totalFlux > 0) {
		centerX = (centerX / totalFlux);
		centerY = (centerY / totalFlux);
	} else {
		centerX = 0;
		centerY = 0;
	}
	// Save for later
	photom->xctr = centerX;
	photom->yctr = centerY;

	// Now: Calculate higher moments
	for (xx = minX; xx < maxX - 1; xx++) {
		int yy = 0;
		for (yy = minY; yy < maxY - 1; yy++) {
			double flux = get_array_float(image, bitpix, xx, yy, winx, winy);
			flux -= sky;

			if (flux >= minimumFlux) {
				double dx = xx - centerX;
				double dy = yy - centerY;
				momentXX += flux * (double) (dx * dx);
				momentYY += flux * (double) (dy * dy);
				momentXY += flux * (double) (dx * dy);
			}
		}
	}

	if (totalFlux > 0) {
		momentXX /= totalFlux;
		momentYY /= totalFlux;
		momentXY /= totalFlux;
	} else {
		momentXX = 0;
		momentYY = 0;
		momentXY = 0;
	}

	double fwhmX = sqrt(momentXX) * FULL_WIDTH_HALF_MAX_STD;
	double fwhmY = sqrt(momentYY) * FULL_WIDTH_HALF_MAX_STD;

	photom->FWHM_X = fwhmX;
	photom->FWHM_Y = fwhmY;
	photom->Flux = stellarFlux;
	photom->Peak = max; // should be same as photom max
	photom->Roundness = (fwhmX - fwhmY) / (fwhmX + fwhmY);
	photom->MomentXY = momentXY;
    photom->betterSigNoise = photom->Flux / (sqrt((pow(nPixels * photom->skyVar, 2) +  photom->Flux)));
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
	double nPixels[4] = { 0.0, 0.0, 0.0, 0.0 };

	double min = get_array_float(image, bitpix, 0, 0, winx, winy);

	double max = min;
	//int maxIndex = 0;
	// We do one pass over the whole array to find the peak value. In the
	// same pass we sum the flux in little squares in the corners to
	// estimate the sky.
	int xx = 0, yy = 0;
	for (yy = 0; yy < winy; yy++) {
		for (xx = 0; xx < winx; xx++) {
			double imageValue = get_array_float(image, bitpix, xx, yy, winx,
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
	}
	// store location and property of brightest pixel
	photom->max = max;

	// normalize the sky counts to the number of pixels involved.

	int ii;
	for (ii = 0; ii < 4; ii++)
		sums[ii] /= nPixels[ii];

	// median filter middle values from sums[0-3]
	int max01, min01;
	if (sums[0] > sums[1]) {
		max01 = 0;
		min01 = 1;
	} else {
		max01 = 1;
		min01 = 0;
	}
	int max23, min23;
	if (sums[2] > sums[3]) {
		max23 = 2;
		min23 = 3;
	} else {
		max23 = 3;
		min23 = 2;
	}
	int max03, med03a;
	if (sums[max01] > sums[max23]) {
		max03 = max01;
		med03a = max23;
	} else {
		max03 = max23;
		med03a = max01;
	}
	int min03, med03b;
	if (sums[min01] <= sums[min23]) {
		min03 = min01;
		med03b = min23;
	} else {
		min03 = min23;
		med03b = min01;
	}

	// max03 is max of sums[0-3]
	// med03a, med03b
	// min03 is min of sums[0-3]
	// printf("sky median filt max %d med03a %d med03b %d min %d\n",max03, med03a, med03b, min03);

	double skyValue = (sums[med03a] + sums[med03b]) / 2.0;
	photom->skyValue = skyValue;

	// now determine noise of sky in a second pass

	// Reset the nPixels for the second pass
	double variance[4];
	for (ii = 0; ii < 4; ii++)
    {
		nPixels[ii] = 0;
		variance[ii] = 0.0;
    }

	double v = 0;

	for (xx = 0; xx < skyband; xx++) {
		for (yy = 0; yy < skyband; yy++) {

			v = get_array_float(image, bitpix, skydelta + xx, skydelta + yy,
					winx, winy);
			if (v > 0 && v < SATURATION_LIMIT) {
				v -= sums[0];
				variance[0] += v * v;
				nPixels[0]++;
				//	printf("DEBUG Variance %d %d --> %f\n", xx, yy, v);
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
