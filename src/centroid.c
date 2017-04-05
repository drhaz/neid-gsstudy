#include <stdio.h>
#include <math.h>
#include <float.h>


#define MARGX	4
#define MARGY	4
#define FWHMSIG 2.35482005

extern char * timestamp(char * buf);

// A macro for the default access pattern to ODI image buffers.

#define array_index(image, x, y, winx, winy) ((image) + (y) * winx + (x))


/** Too function to help calculating moments of an image
 *
 *  @value: pixel value in image
 *  @x: x coordiante of pixel value
 *  @y: y coordiante of pixel value
 *
 *  Returns:
 *  @ sum_xf: sum in x first moment
 *  @ sim_yf: sum in y first moment
 *  @ sum_f: sum of total flux
 *  @ num: incrementing counter of elements.
 *
 */

inline void weight(float value, int x, int y, float *sum_xf, float *sum_yf, float *sum_f, int *num)
{
  *sum_xf += (float) (x * value);
  *sum_yf += (float) (y * value);
  *sum_f  += (float) (value);
  *num    += 1;
}


/**
 * Find the brightest pixel in a single star image, unsigned short.
 *
 */
inline void gc_findBrightestPixel_us(unsigned short *image, unsigned short **max,
                                   int winx, int winy, int xmarg, int ymarg )
{
  *max = image;
  unsigned short *ptr;

  int ii, jj;

  for (ii = xmarg; ii < winx - xmarg; ii++)
  {
    for (jj = ymarg; jj < winy - ymarg; jj++)
    {
      ptr = image + jj * winx + ii;
      if (*ptr > **(max))
      {
         *max = ptr;
      }
    }
  }
}

/**
 * Find the brightest pixel in a single star image, long
 */
inline void gc_findBrightestPixel_l(long *image, long **max,
                                   int winx, int winy, int xmarg, int ymarg )
{
  *max = image;
  long *ptr;

  int ii, jj;

  for (ii = xmarg; ii < winx - xmarg; ii++)
  {
    for (jj = ymarg; jj < winy - ymarg; jj++)
    {
      ptr = image + jj * winx + ii;
      if (*ptr > **(max))
      {
         *max = ptr;
      }
    }
  }
}

/**
 * Find the brightest pixel in a single star image, float
 */
inline void gc_findBrightestPixel_f(float *image, float **max,
                                   int winx, int winy, int xmarg, int ymarg )
{
  *max = image;
  float *ptr = image;
  while (ptr < image + winy*winx)  {
    if (*ptr > **max) { *max = ptr; }
    ptr++;
  }
  int pos = *max -image; 
  printf("max is %d y %d x %d\n",pos, pos/winx,pos%winx);
/*
  int ii, jj;

  for (ii = xmarg; ii < winx - xmarg; ii++)
  {
    for (jj = ymarg; jj < winy - ymarg; jj++)
    {
      ptr = image + jj * winx + ii;
      if (*ptr > **(max))
      {
         *max = ptr;
      }
    }
  }
*/
}

/**
 * Determine the centroid of the guide star in a picture.  
 */
void centroid_us(unsigned short * image, unsigned short * max, int winx, int winy, float sky, float skyvar,
	      float * xctr, float * yctr, float * sigma) 
{
  /* thresh - determines the threshold above which pixels 
   * must lie to be included in the centroid.
   */
  static const float thresh = 0.5;
  /* the Border defines how many pixels from the cell border
   * will be ignored
   */
	
  static const int Border = 2;
	
  float tmp;
  int max_row = (max - image) / winx;
  int max_col = (max - image) % winx;
 
  char buffer[256];
  char * next;

  next = timestamp(buffer);
  sprintf(next," winx %d centroid brightest: %d %d %d sky %f skyvar %f\n", winx, max_col +1, max_row +1, *max, sky, skyvar) ;
  printf(buffer); fflush(stdout);

  float sum_xf = 0;
  float sum_yf = 0;
  float sum_f = 0;

  int num = 0;

  int currentX, currentY;
	
  /* Loop over the four quadrants around the brightest pixel of the star and calculate 
   * the first moment of the flux distribution. However, we stop summing up once we the flux above
   * sky reaches the threshold level.
   */
  unsigned short * ptr = 0;
  for (currentY = max_row; currentY < winy - Border; currentY++)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx - Border; currentX++)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0 + Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
	  
  } // ready doing Flux * Y, Flux * X in the upper half;
  for (currentY = max_row-1; currentY > 0 + Border; currentY--)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx- Border; currentX++)
    {
      tmp = ((float)*ptr) - sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
	
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0+ Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
  }

  *xctr = (float)sum_xf / sum_f + 1; // fits coordinates start at 1/1
  *yctr = (float)sum_yf / sum_f + 1;
  *sigma = 2 * sqrt((float) num / 3.14159265) / FWHMSIG;

  next = timestamp(buffer);
  sprintf(next," xcenter %f ycenter %f\n", *xctr, *yctr);
  printf(buffer); fflush(stdout);
}

/**
 * Determine the centroid of the guide star in a picture; float
 */
void centroid_f(float * image, float * max, int winx, int winy, float sky, float skyvar,
	      float * xctr, float * yctr, float * sigma) 
{
  /* thresh - determines the threshold above which pixels 
   * must lie to be included in the centroid.
   */
  static const float thresh = 0.5;
  /* the Border defines how many pixels from the cell border
   * will be ignored
   */
	
  static const int Border = 2;
	
  float tmp;
  int max_row = (max - image) / winx;
  int max_col = (max - image) % winx;
 
  char buffer[256];
  char * next;

  next = timestamp(buffer);
  sprintf(next," winx %d centroid brightest: %d %d max %f sky %f skyvar %f\n", winx, max_col +1, max_row +1, *max, sky, skyvar) ;
  printf(buffer); fflush(stdout);

  float sum_xf = 0;
  float sum_yf = 0;
  float sum_f = 0;

  int num = 0;

  int currentX, currentY;
	
  /* Loop over the four quadrants around the brightest pixel of the star and calculate 
   * the first moment of the flux distribution. However, we stop summing up once we the flux above
   * sky reaches the threshold level.
   */
  float * ptr = 0;
  for (currentY = max_row; currentY < winy - Border; currentY++)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx - Border; currentX++)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0 + Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
	  
  } // ready doing Flux * Y, Flux * X in the upper half;
  for (currentY = max_row-1; currentY > 0 + Border; currentY--)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx- Border; currentX++)
    {
      tmp = ((float)*ptr) - sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
	
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0+ Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
  }

  *xctr = (float)sum_xf / sum_f + 1; // fits coordinates start at 1/1
  *yctr = (float)sum_yf / sum_f + 1;
  *sigma = 2 * sqrt((float) num / 3.14159265) / FWHMSIG;

  next = timestamp(buffer);
  sprintf(next," xcenter %f ycenter %f\n", *xctr, *yctr);
  printf(buffer); fflush(stdout);
}

/**
 * Determine the centroid of the guide star in a picture; float
 */
void centroid_l(long * image, long * max, int winx, int winy, float sky, float skyvar,
	      float * xctr, float * yctr, float * sigma) 
{
  /* thresh - determines the threshold above which pixels 
   * must lie to be included in the centroid.
   */
  static const float thresh = 0.5;
  /* the Border defines how many pixels from the cell border
   * will be ignored
   */
	
  static const int Border = 2;
	
  float tmp;
  int max_row = (max - image) / winx;
  int max_col = (max - image) % winx;
 
  char buffer[256];
  char * next;

  next = timestamp(buffer);
  sprintf(next," winx %d centroid brightest: %d %d %d sky %f skyvar %f\n", winx, max_col +1, max_row +1, *max, sky, skyvar) ;
  printf(buffer); fflush(stdout);

  float sum_xf = 0;
  float sum_yf = 0;
  float sum_f = 0;

  int num = 0;

  int currentX, currentY;
	
  /* Loop over the four quadrants around the brightest pixel of the star and calculate 
   * the first moment of the flux distribution. However, we stop summing up once we the flux above
   * sky reaches the threshold level.
   */
  long * ptr = 0;
  for (currentY = max_row; currentY < winy - Border; currentY++)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx - Border; currentX++)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0 + Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
	  
  } // ready doing Flux * Y, Flux * X in the upper half;
  for (currentY = max_row-1; currentY > 0 + Border; currentY--)
  {
    ptr = array_index(image, max_col, currentY, winx, winy);
    for (currentX = max_col; currentX < winx- Border; currentX++)
    {
      tmp = ((float)*ptr) - sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr++;
    }
	
    ptr = array_index(image, max_col-1, currentY, winx, winy);
    for (currentX = max_col - 1; currentX >= 0+ Border; currentX--)
    {
      tmp = ((float)*ptr)-sky;
      if (tmp < thresh *(((float)*max) - sky))
        break;
      weight(tmp, currentX, currentY, &sum_xf, &sum_yf, &sum_f, &num);
      ptr--;
    }
  }

  *xctr = (float)sum_xf / sum_f + 1; // fits coordinates start at 1/1
  *yctr = (float)sum_yf / sum_f + 1;
  *sigma = 2 * sqrt((float) num / 3.14159265) / FWHMSIG;

  next = timestamp(buffer);
  sprintf(next," xcenter %f ycenter %f\n", *xctr, *yctr);
  printf(buffer); fflush(stdout);
}

/**
 * Determine the sky level of a guide image and the varaince of the sky level.
 * The sky value is subtracted from the  picture.
*/
void psfCell_findSky_us(unsigned short *imageBuffer, int winx, int winy,  unsigned short ** max, float *skyValue, float *skyVar)
{
  static const int sky_border = 2;
  const int sectX = winx / (4);
  const int sectY = winy / (4);
  const short npix = sectX * sectY;

  gc_findBrightestPixel_us(imageBuffer, max, winx, winy, MARGX, MARGX);

  int i;

  float sum[4] = {0,0,0,0};
  float var[4] = {0,0,0,0};

  unsigned short *pos1, *pos2, *pos3, *pos4;
  int xx, yy;
  float fvar = 0;
  float fsum = 0;

  for (yy=0; yy< sectY; yy++)
  {
    pos1 = array_index(imageBuffer, sky_border, sky_border + yy, winx, winy);
				
    pos2 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    sky_border + yy, winx, winy);

    pos3 = array_index(imageBuffer, sky_border,
				    winy - 1 - sky_border - yy, winx, winy);

    pos4 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    winy - 1 - sky_border - yy, winx, winy);

    for (xx=0; xx<sectX; xx++)
    {
      sum[0] += *pos1;
      var[0] += *pos1 * *pos1;
      sum[1] += *pos2;
      var[1] += *pos2 * *pos2;
      sum[2] += *pos3;
      var[2] += *pos3 * *pos3;
      sum[3] += *pos4;
      var[3] += *pos4 * *pos4;
      pos1++; pos2++; pos3++; pos4++;
    }
  }

  //average sky value of the four
  // int median = 0;
  for (i = 0; i < 4; i++)
  {
    fsum += sum[i] / npix / 4;
    fvar += var[i]  /4;
  } 

  *skyValue = fsum;
  *skyVar = fvar / npix - fsum*fsum;
//  *skyVar = 0;
//  printf ("Sky value: %f +/- %f\n", *skyValue, *skyVar);   

  // sky correct ? really?
  pos1 = imageBuffer;
  // ?? count to winx*winy?
  // pos1 += winx*winy-1; ?
  for (yy = 0; yy < winx*winy; yy++)
  {
    //  *pos1 -= fsum;
    pos1++;
  }
  //*skyValue = 0;
}

/**
 * Determine the sky level of a guide image and the varaince of the sky level.
 * The sky value is subtracted from the  picture.
*/
void psfCell_findSky_l(long *imageBuffer, int winx, int winy,  long ** max, float *skyValue, float *skyVar)
{
  static const int sky_border = 2;
  const int sectX = winx / (4);
  const int sectY = winy / (4);
  const short npix = sectX * sectY;

  gc_findBrightestPixel_l(imageBuffer, max, winx, winy, MARGX, MARGX);

  int i;

  float sum[4] = {0,0,0,0};
  float var[4] = {0,0,0,0};

  long *pos1, *pos2, *pos3, *pos4;
  int xx, yy;
  float fvar = 0;
  float fsum = 0;

  for (yy=0; yy< sectY; yy++)
  {
    pos1 = array_index(imageBuffer, sky_border, sky_border + yy, winx, winy);
				
    pos2 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    sky_border + yy, winx, winy);

    pos3 = array_index(imageBuffer, sky_border,
				    winy - 1 - sky_border - yy, winx, winy);

    pos4 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    winy - 1 - sky_border - yy, winx, winy);

    for (xx=0; xx<sectX; xx++)
    {
      sum[0] += *pos1;
      var[0] += *pos1 * *pos1;
      sum[1] += *pos2;
      var[1] += *pos2 * *pos2;
      sum[2] += *pos3;
      var[2] += *pos3 * *pos3;
      sum[3] += *pos4;
      var[3] += *pos4 * *pos4;
      pos1++; pos2++; pos3++; pos4++;
    }
  }

  //average sky value of the four
  // int median = 0;
  for (i = 0; i < 4; i++)
  {
    fsum += sum[i] / npix / 4;
    fvar += var[i]  /4;
  } 

  *skyValue = fsum;
  *skyVar = fvar / npix - fsum*fsum;
//  *skyVar = 0;
//  printf ("Sky value: %f +/- %f\n", *skyValue, *skyVar);   

  // sky correct ? really?
  pos1 = imageBuffer;
  // ?? count to winx*winy?
  // pos1 += winx*winy-1; ?
  for (yy = 0; yy < winx*winy; yy++)
  {
    //  *pos1 -= fsum;
    pos1++;
  }
  //*skyValue = 0;
}

/**
 * Determine the sky level of a guide image and the varaince of the sky level.
 * The sky value is subtracted from the  picture.
*/
void psfCell_findSky_f(float *imageBuffer, int winx, int winy,  float ** max, float *skyValue, float *skyVar)
{
  static const int sky_border = 2;
  const int sectX = winx / (4);
  const int sectY = winy / (4);
  const short npix = sectX * sectY;

  gc_findBrightestPixel_f(imageBuffer, max, winx, winy, MARGX, MARGX);

  int i;

  float sum[4] = {0,0,0,0};
  float var[4] = {0,0,0,0};

  float *pos1, *pos2, *pos3, *pos4;
  int xx, yy;
  float fvar = 0;
  float fsum = 0;

  for (yy=0; yy< sectY; yy++)
  {
    pos1 = array_index(imageBuffer, sky_border, sky_border + yy, winx, winy);
				
    pos2 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    sky_border + yy, winx, winy);

    pos3 = array_index(imageBuffer, sky_border,
				    winy - 1 - sky_border - yy, winx, winy);

    pos4 = array_index(imageBuffer, winx - 1 - sky_border - sectX,
				    winy - 1 - sky_border - yy, winx, winy);

    for (xx=0; xx<sectX; xx++)
    {
      sum[0] += *pos1;
      var[0] += *pos1 * *pos1;
      sum[1] += *pos2;
      var[1] += *pos2 * *pos2;
      sum[2] += *pos3;
      var[2] += *pos3 * *pos3;
      sum[3] += *pos4;
      var[3] += *pos4 * *pos4;
      pos1++; pos2++; pos3++; pos4++;
    }
  }

  //average sky value of the four
  // int median = 0;
  for (i = 0; i < 4; i++)
  {
    fsum += sum[i] / npix / 4;
    fvar += var[i]  /4;
  } 

  *skyValue = fsum;
  *skyVar = fvar / npix - fsum*fsum;
//  *skyVar = 0;
  printf ("Sky value: %f +/- %f\n", *skyValue, *skyVar);   

  // sky correct ? really?
  pos1 = imageBuffer;
  // ?? count to winx*winy?
  // pos1 += winx*winy-1; ?
  /*
  for (yy = 0; yy < winx*winy; yy++)
  {
    //  *pos1 -= fsum;
    pos1++;
  }
  */
  //*skyValue = 0;
}

float photometry_us(unsigned short *image, int winx, int winy,
      float sky, float xctr, float yctr, float sig)
{

  static int excludeBorder = 2;
  float flux = 0;
  
  unsigned short *pointer;
  
  int xx, yy;
  for (yy = 0; yy < winy-2; yy++) {
    pointer = image + winx * yy + excludeBorder;
    for (xx = 2; xx < winx-2; xx++) {
      flux += ((float)*pointer) - sky;
      pointer ++;
    }
  }
	 
  //  printf ("Phot: %f, %f\n", flux, sky);
  return 2.5 * log10 (flux);
}

float photometry_f(float *image, int winx, int winy,
      float sky, float xctr, float yctr, float sig)
{

  static int excludeBorder = 2;
  float flux = 0;
  
  float *pointer;
  
  int xx, yy;
  for (yy = 0; yy < winy-2; yy++) {
    pointer = image + winx * yy + excludeBorder;
    for (xx = 2; xx < winx-2; xx++) {
      flux += ((float)*pointer) - sky;
      pointer ++;
    }
  }
	 
  //  printf ("Phot: %f, %f\n", flux, sky);
  return 2.5 * log10 (flux);
}

float photometry_l(long *image, int winx, int winy,
      float sky, float xctr, float yctr, float sig)
{

  static int excludeBorder = 2;
  float flux = 0;
  
  long *pointer;
  
  int xx, yy;
  for (yy = 0; yy < winy-2; yy++) {
    pointer = image + winx * yy + excludeBorder;
    for (xx = 2; xx < winx-2; xx++) {
      flux += ((float)*pointer) - sky;
      pointer ++;
    }
  }
	 
  //  printf ("Phot: %f, %f\n", flux, sky);
  return 2.5 * log10 (flux);
}
