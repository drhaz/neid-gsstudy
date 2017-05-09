#ifndef PHOTOMETRY_H
#define PHOTOMETRY_H

/** Data structure that holds photometry & centroid information that has been extracted from a guide window.
 *
 */
typedef struct Photometry
{
  /** Estiamted sky background value */
  float skyValue;
  /** Estiamted sky variance */
  float skyVar;
  /** Estiamted signal to noise of star */
  float sigNoise;
  /** Better Estiamted signal to noise of star */
  float betterSigNoise;
  /** Estimated detection probability in sigma */
  float sigma;

  /** specified S/N floor for guide star photometry to be locked */
  float snThreshold;

  /** Background noise; TBD how different than skyVar */
  float backNoise;

  /** X centroid of guide star in window */
  float xctr;
  /** Y centroid of guide star in window */
  float yctr;
 
  /** Value of brightest pixel */
  float max;
  /** X location of brightest pixel */
  int maxx;
  /** Y location of brightest pixel */
  int maxy;

  /** array index to brightest pixel */
  int maxp; // index of max pixel

  /** FWHM in X (unbinned pixels) */
  float FWHM_X;

  /** FWHM in Y (unbinned pixels */
  float FWHM_Y;

  /** Total flux in star in ADU*/
  float Flux;

  /** Peak value; redundant w/ max ? */
  float Peak;

  /** Roundness estimate of star */
  float Roundness;

  /** XY moment of star */
  float MomentXY;

  /** time stamp of guide video */
  double tstamp;

} Photometry;
#endif /* PHOTOMETRY_H */
