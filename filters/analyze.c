/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                               A N A L I Z E                                 %
%                                                                             %
%               Methods to Compute a Information about an Image               %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              December 1992                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Routine SignatureImage computes a digital signature from the image
%  pixels.  This signature uniquely identifies the image.  The digital
%  signature is from RSA Data Security MD5 Digest Algorithm described in
%  Internet draft [MD5], July 1992.
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n a l y z e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnalyzeImage computes the brightness and saturation mean and
%  standard deviation and stores these values as attributes of the image.
%
%  The format of the AnalyzeImage method is:
%
%      unsigned int AnalyzeImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
*/
//#define PRECISION "%.2f"
#define PRECISION "%.0f"
ModuleExport unsigned int AnalyzeImage(Image *image,
  const int argc,char **argv)
{
  double
    bsumX = 0.0,
    bsumX2 = 0.0,
    brightness_mean = 0.0,
    brightness_stdev = 0.0,
    ssumX = 0.0,
    ssumX2 = 0.0,
    saturation_mean = 0.0,
    saturation_stdev = 0.0,
    total_pixels = 0.0;

  double
    brightness,
    hue,
    saturation;

  int
    y;

  register int
    x;

  register PixelPacket
    *p;

  char
    text[MaxTextExtent];

  assert(image != (Image *) NULL);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    if (y == 0)
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute(image,"TopLeftColor",text);
      }
    if (y == (image->rows-1))
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute(image,"BottomLeftColor",text);
      }
    for (x=0; x < image->columns; x++)
    {
      TransformHSL(p->red,p->green,p->blue,&hue,&saturation,&brightness);
      brightness *= MaxRGB;
	    bsumX += brightness;
	    bsumX2 += brightness * brightness;
      saturation *= MaxRGB;
	    ssumX += saturation;
	    ssumX2 += saturation * saturation;
      total_pixels++;
      p++;
    }
    p--; /* backup one pixel to allow us to sample */
    if (y == 0)
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute(image,"TopRightColor",text);
      }
    if (y == (image->rows-1))
      {
        FormatString(text,"#%02x%02x%02x",p->red,p->green,p->blue);
        (void) SetImageAttribute(image,"BottomRightColor",text);
      }
  }
  if (total_pixels > 0.0)
    {
      brightness_mean = bsumX/total_pixels;
      FormatString(text,PRECISION,brightness_mean);
      (void) SetImageAttribute(image,"BrightnessMean",text);
      // This formula gives a slightly biased result
      brightness_stdev =
          sqrt(bsumX2/total_pixels - (bsumX/total_pixels*bsumX/total_pixels));
      FormatString(text,PRECISION,brightness_stdev);
      (void) SetImageAttribute(image,"BrightnessStddev",text);
      // Now the correction for bias.
      //stdev = stdev*sqrt((double)total_pixels/(double)(total_pixels-1));
      // Now calculate the standard deviation of the mean
      //  brightness_stdevmean = bstdev/sqrt((double)total_pixels);

      saturation_mean = ssumX/total_pixels;
      FormatString(text,PRECISION,saturation_mean);
      (void) SetImageAttribute(image,"SaturationMean",text);
      // This formula gives a slightly biased result
      saturation_stdev =
          sqrt(ssumX2/total_pixels - (ssumX/total_pixels*ssumX/total_pixels));
      FormatString(text,PRECISION,saturation_stdev);
      (void) SetImageAttribute(image,"SaturationStddev",text);
      // Now the correction for bias.
      //stdev = stdev*sqrt((double)total_pixels/(double)(total_pixels-1));
      // Now calculate the standard deviation of the mean
      //  saturation_stdevmean = sstdev/sqrt((double)total_pixels);
    }
  return(True);
}
