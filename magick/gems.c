/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                           GGGG  EEEEE  M   M SSSSS                          %
%                          G      E      MM MM SS                             %
%                          G GG   EEE    M M M  SSS                           %
%                          G   G  E      M   M    SS                          %
%                           GGGG  EEEEE  M   M SSSSS                          %
%                                                                             %
%                                                                             %
%                    Graphic Gems - Graphic Support Methods                   %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 August 1996                                 %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l p h a C o m p o s i t e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AlphaComposite implements the alpha compositing rules for combining
%  source and destination pixels to achieve blending and transparency effects
%  with graphics and images. The rules implemented by this class are a subset
%  of the Porter-Duff rules described in T. Porter and T. Duff, "Compositing
%  Digital Images", SIGGRAPH 84, 253-259.
%
%  The format of the AlphaComposite method is:
%
%      PixelPacket AlphaComposite(const CompositeOperator compose,
%        const PixelPacket *p,const double alpha,const PixelPacket *q,
%        const double beta)
%
%  A description of each parameter follows:
%
%    o color: method AlphaComposite returns the color resulting from the
%      compositing operation.
%
%    o compose: specifies an image composite operator.
%
%    o p: the source pixel.
%
%    o alpha: alpha component of the source pixel.
%
%    o q: the destination pixel.
%
%    o beta: alpha component of the destination pixel.
%
%
*/
MagickExport PixelPacket AlphaComposite(const CompositeOperator compose,
  const PixelPacket *p,const double alpha,const PixelPacket *q,
  const double beta)
{
  double
    blue,
    green,
    opacity,
    red;

  PixelPacket
    color;

  switch (compose)
  {
    case OverCompositeOp:
    default:
    {
      switch ((int) alpha)
      {
        case TransparentOpacity:
        {
          color=(*q);
          color.opacity=(Quantum) beta;
          return(color);
        }
        case OpaqueOpacity:
        {
          color=(*p);
          color.opacity=(Quantum) alpha;
          return(color);
        }
        default:
        {
          color.red=(Quantum) (((MaxRGB-alpha)*p->red+
            alpha/MaxRGB*(MaxRGB-beta)*q->red)/MaxRGB);
          color.green=(Quantum) (((MaxRGB-alpha)*p->green+
            alpha/MaxRGB*(MaxRGB-beta)*q->green)/MaxRGB);
          color.blue=(Quantum) (((MaxRGB-alpha)*p->blue+
            alpha/MaxRGB*(MaxRGB-beta)*q->blue)/MaxRGB);
          color.opacity=(Quantum) (alpha*beta/MaxRGB);
          return(color);
        }
      }
      break;
    }
    case InCompositeOp:
    {
      color.red=(Quantum) ((MaxRGB-alpha)/MaxRGB*p->red*(MaxRGB-beta)/MaxRGB);
      color.green=(Quantum)
        ((MaxRGB-alpha)/MaxRGB*p->green*(MaxRGB-beta)/MaxRGB);
      color.blue=(Quantum) ((MaxRGB-alpha)/MaxRGB*p->blue*(MaxRGB-beta)/MaxRGB);
      color.opacity=(Quantum)
        ((MaxRGB-alpha)/MaxRGB*alpha*(MaxRGB-beta)/MaxRGB);
      return(color);
    }
    case OutCompositeOp:
    {
      color.red=(Quantum) ((MaxRGB-alpha)/MaxRGB*p->red*beta/MaxRGB);
      color.green=(Quantum) ((MaxRGB-alpha)/MaxRGB*p->green*beta/MaxRGB);
      color.blue=(Quantum) ((MaxRGB-alpha)/MaxRGB*p->blue*beta/MaxRGB);
      color.opacity=(Quantum) ((MaxRGB-alpha)/MaxRGB*alpha*beta/MaxRGB);
      return(color);
    }
    case AtopCompositeOp:
    {
      red=((MaxRGB-alpha)/MaxRGB*p->red*(MaxRGB-beta)+
        (MaxRGB-beta)/MaxRGB*q->red*alpha)/MaxRGB;
      color.red=(Quantum) (red > MaxRGB ? MaxRGB : red);
      green=((MaxRGB-alpha)/MaxRGB*p->green*(MaxRGB-beta)+
        (MaxRGB-beta)/MaxRGB*q->green*alpha)/MaxRGB;
      color.green=(Quantum) (green > MaxRGB ? MaxRGB : green);
      blue=((MaxRGB-alpha)/MaxRGB*p->blue*(MaxRGB-beta)+
        (MaxRGB-beta)/MaxRGB*q->blue*alpha)/MaxRGB;
      color.blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
      opacity=((MaxRGB-alpha)/MaxRGB*alpha*(MaxRGB-beta)+
        (MaxRGB-beta)/MaxRGB*beta*alpha)/MaxRGB;
      color.opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity);
      return(color);
    }
    case XorCompositeOp:
    {
      red=((MaxRGB-alpha)/MaxRGB*p->red*beta+
        (MaxRGB-beta)/MaxRGB*q->red*alpha)/MaxRGB;
      color.red=(Quantum) (red > MaxRGB ? MaxRGB : red);
      green=((MaxRGB-alpha)/MaxRGB*p->green*beta+
        (MaxRGB-beta)/MaxRGB*q->green*alpha)/MaxRGB;
      color.green=(Quantum) (green > MaxRGB ? MaxRGB : green);
      blue=((MaxRGB-alpha)/MaxRGB*p->blue*beta+
        (MaxRGB-beta)/MaxRGB*q->blue*alpha)/MaxRGB;
      color.blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
      opacity=((MaxRGB-alpha)/MaxRGB*alpha*beta+
        (MaxRGB-beta)/MaxRGB*beta*alpha)/MaxRGB;
      color.opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity);
      return(color);
    }
    case PlusCompositeOp:
    {
      red=((MaxRGB-alpha)*p->red+(MaxRGB-beta)*q->red)/MaxRGB;
      color.red=(Quantum) (red > MaxRGB ? MaxRGB : red);
      green=((MaxRGB-alpha)*p->green+(MaxRGB-beta)*q->green)/MaxRGB;
      color.green=(Quantum) (green > MaxRGB ? MaxRGB : green);
      blue=((MaxRGB-alpha)*p->blue+(MaxRGB-beta)*q->blue)/MaxRGB;
      color.blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue);
      opacity=((MaxRGB-alpha)*alpha+(MaxRGB-beta)*beta)/MaxRGB;
      color.opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity);
      return(color);
    }
    case MinusCompositeOp:
    {
      red=((MaxRGB-alpha)*p->red-(MaxRGB-beta)*q->red)/MaxRGB;
      color.red=(Quantum) (red < 0 ? 0 : red);
      green=((MaxRGB-alpha)*p->green-(MaxRGB-beta)*q->green)/MaxRGB;
      color.green=(Quantum) (green < 0 ? 0 : green);
      blue=((MaxRGB-alpha)*p->blue-(MaxRGB-beta)*q->blue)/MaxRGB;
      color.blue=(Quantum) (blue < 0 ? 0 : blue);
      opacity=((MaxRGB-alpha)*p->opacity-(MaxRGB-beta)*q->opacity)/MaxRGB;
      color.opacity=(Quantum) (opacity < 0 ? 0 : opacity);
      return(color);
    }
    case AddCompositeOp:
    {
      red=p->red+q->red;
      color.red=(Quantum) (red > MaxRGB ? red-=MaxRGB : red);
      green=p->green+q->green;
      color.green=(Quantum) (green > MaxRGB ? green-=MaxRGB : green);
      blue=p->blue+q->blue;
      color.blue=(Quantum) (blue > MaxRGB ? blue-=MaxRGB : blue);
      opacity=alpha+beta;
      color.opacity=(Quantum) (opacity > MaxRGB ? opacity-=MaxRGB : opacity);
      return(color);
    }
    case SubtractCompositeOp:
    {
      double
        blue,
        green,
        opacity,
        red;

      red=p->red-q->red;
      color.red=(Quantum) (red < 0 ? red+=MaxRGB : red);
      green=p->green-q->green;
      color.green=(Quantum) (green < 0 ? green+=MaxRGB : green);
      blue=p->blue-q->blue;
      color.blue=(Quantum) (blue < 0 ? blue+=MaxRGB : blue);
      opacity=alpha-beta;
      color.opacity=(Quantum) (opacity < 0 ? opacity+=MaxRGB : opacity);
      return(color);
    }
    case MultiplyCompositeOp:
    {
      color.red=(unsigned long) (p->red*q->red/MaxRGB);
      color.green=(unsigned long) (p->red*q->red/MaxRGB);
      color.blue=(unsigned long) (p->red*q->red/MaxRGB);
      color.opacity=(unsigned long) (p->red*q->red/MaxRGB);
      return(color);
    }
    case DifferenceCompositeOp:
    {
      color.red=AbsoluteValue((int) p->red-(int) q->red);
      color.green=AbsoluteValue((int) p->green-(int) q->green);
      color.blue=AbsoluteValue((int) p->blue-(int) q->blue);
      color.opacity=(Quantum) fabs(alpha-beta);
      return(color);
    }
    case BumpmapCompositeOp:
    {
      color.red=(unsigned long) ((Intensity(*p)*q->red)/MaxRGB);
      color.green=(unsigned long) ((Intensity(*p)*q->green)/MaxRGB);
      color.blue=(unsigned long) ((Intensity(*p)*q->blue)/MaxRGB);
      color.opacity=(unsigned long) ((Intensity(*p)*beta)/MaxRGB);
      return(color);
    }
    case CopyCompositeOp:
    {
      color=(*p);
      return(color);
    }
    case CopyRedCompositeOp:
    {
      color=(*q);
      color.red=p->red;
      return(color);
    }
    case CopyGreenCompositeOp:
    {
      color=(*q);
      color.green=p->green;
      return(color);
    }
    case CopyBlueCompositeOp:
    {
      color=(*q);
      color.blue=p->blue;
      return(color);
    }
    case CopyOpacityCompositeOp:
    {
      color=(*q);
      color.opacity=(Quantum) alpha;
      return(color);
    }
    case ClearCompositeOp:
    {
      memset(&color,0,sizeof(color));
      return(color);
    }
    case DissolveCompositeOp:
    {
      color.red=(unsigned long)
        ((alpha*p->red+(MaxRGB-beta)*q->red)/MaxRGB);
      color.green=(unsigned long)
        ((alpha*p->green+(MaxRGB-beta)*q->green)/MaxRGB);
      color.blue=(unsigned long)
        ((alpha*p->blue+(MaxRGB-beta)*q->blue)/MaxRGB);
      color.opacity=(unsigned long)
        ((alpha*alpha+(MaxRGB-beta)*beta)/MaxRGB);
      return(color);
    }
    case DisplaceCompositeOp:
    {
      color=(*p);
      return(color);
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t r a s t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Contrast enhances the intensity differences between the lighter
%  and darker elements of the image.
%
%  The format of the Contrast method is:
%
%      void Contrast(const int sign,Quantum *red,Quantum *green,Quantum *blue)
%
%  A description of each parameter follows:
%
%    o sign: A positive value enhances the contrast otherwise it is reduced.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void Contrast(const int sign,Quantum *red,Quantum *green,
  Quantum *blue)
{
  double
    alpha,
    brightness,
    hue,
    saturation;

  /*
    Enhance contrast: dark color become darker, light color become lighter.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  alpha=0.5+MagickEpsilon;
  brightness+=
    alpha*sign*(alpha*(sin(MagickPI*(brightness-alpha))+1.0)-brightness);
  if (brightness > 1.0)
    brightness=1.0;
  else
    if (brightness < 0.0)
      brightness=0.0;
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d A f f i n e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandAffine computes the affine's expansion factor, i.e. the
%  square root of the factor by which the affine transform affects area. In an
%  affine transform composed of scaling, rotation, shearing, and translation,
%  returns the amount of scaling.
%
%  The format of the ExpandAffine method is:
%
%      double ExpandAffine(const AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o expansion: Method ExpandAffine returns the affine's expansion factor.
%
%    o affine: A pointer the the affine transform of type AffineMatrix.
%
%
*/
MagickExport double ExpandAffine(const AffineMatrix *affine)
{
  double
    expand;

  assert(affine != (const AffineMatrix *) NULL);
  expand=fabs(affine->sx*affine->sy)-fabs(affine->rx*affine->ry);
  return(sqrt(fabs(expand)));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e n e r a t e N o i s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GenerateNoise adds noise to a pixel.
%
%  The format of the GenerateNoise method is:
%
%      Quantum GenerateNoise(const Quantum pixel,const NoiseType noise_type)
%
%  A description of each parameter follows:
%
%    o pixel: A structure of type Quantum.
%
%    o noise_type:  The type of noise: Gaussian, multiplicative Gaussian,
%      impulse, laplacian, or Poisson.
%
%
*/
MagickExport Quantum GenerateNoise(const Quantum pixel,
  const NoiseType noise_type)
{
#define NoiseEpsilon  1.0e-5
#define NoiseMask  0x7fff
#define SigmaUniform  4.0
#define SigmaGaussian  4.0
#define SigmaImpulse  0.10
#define SigmaLaplacian 10.0
#define SigmaMultiplicativeGaussian  0.5
#define SigmaPoisson  0.05
#define TauGaussian  20.0

  double
    alpha,
    beta,
    sigma,
    value;

  alpha=(double) (rand() & NoiseMask)/NoiseMask;
  if (alpha == 0.0)
    alpha=1.0;
  switch (noise_type)
  {
    case UniformNoise:
    default:
    {
      value=(double) pixel+SigmaUniform*(alpha-0.5);
      break;
    }
    case GaussianNoise:
    {
      double
        tau;

      beta=(double) (rand() & NoiseMask)/NoiseMask;
      sigma=sqrt(-2.0*log(alpha))*cos(2.0*MagickPI*beta);
      tau=sqrt(-2.0*log(alpha))*sin(2.0*MagickPI*beta);
      value=(double) pixel+
        (sqrt((double) pixel)*SigmaGaussian*sigma)+(TauGaussian*tau);
      break;
    }
    case MultiplicativeGaussianNoise:
    {
      if (alpha <= NoiseEpsilon)
        sigma=MaxRGB;
      else
        sigma=sqrt(-2.0*log(alpha));
      beta=(rand() & NoiseMask)/NoiseMask;
      value=(double) pixel+
        pixel*SigmaMultiplicativeGaussian*sigma*cos(2.0*MagickPI*beta);
      break;
    }
    case ImpulseNoise:
    {
      if (alpha < (SigmaImpulse/2.0))
        value=0;
       else
         if (alpha >= (1.0-(SigmaImpulse/2.0)))
           value=MaxRGB;
         else
           value=pixel;
      break;
    }
    case LaplacianNoise:
    {
      if (alpha <= 0.5)
        {
          if (alpha <= NoiseEpsilon)
            value=(double) pixel-MaxRGB;
          else
            value=(double) pixel+SigmaLaplacian*log(2.0*alpha);
          break;
        }
      beta=1.0-alpha;
      if (beta <= (0.5*NoiseEpsilon))
        value=(double) pixel+MaxRGB;
      else
        value=(double) pixel-SigmaLaplacian*log(2.0*beta);
      break;
    }
    case PoissonNoise:
    {
      register int
        i;

      for (i=0; alpha > exp(-SigmaPoisson*pixel); i++)
      {
        beta=(double) (rand() & NoiseMask)/NoiseMask;
        alpha=alpha*beta;
      }
      value=i/SigmaPoisson;
      break;
    }
  }
  if (value < 0.0)
    return(0);
  if (value > MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O p t i m a l K e r n e l W i d t h                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOptimalKernelWidth computes the optimal kernel radius for a
%  convolution filter.  Start with the minimum value of 3 pixels and walk out
%  until we drop below the threshold of one pixel numerical accuracy,
%
%  The format of the GetOptimalKernelWidth method is:
%
%      int GetOptimalKernelWidth(const double radius,const double sigma)
%
%  A description of each parameter follows:
%
%    o width: Method GetOptimalKernelWidth returns the optimal width of
%      a convolution kernel.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%
*/

MagickExport int GetOptimalKernelWidth1D(const double radius,const double sigma)
{
  double
    normalize,
    value;

  int
    width;

  register int
    u;

  if (radius > 0.0)
    return((int) (2.0*ceil(radius)+1.0));
  for (width=5; ;)
  {
    normalize=0.0;
    for (u=(-width/2); u <= (width/2); u++)
      normalize+=exp((double) -(u*u)/(2.0*sigma*sigma));
    u=width/2;
    value=exp((double) -(u*u)/(2.0*sigma*sigma))/normalize;
    if ((int) (value*MaxRGB) <= 0)
      break;
    width+=2;
  }
  return(width-2);
}

MagickExport int GetOptimalKernelWidth2D(const double radius,const double sigma)
{
  double
    normalize,
    value;

  int
    width;

  register int
    u,
    v;

  if (radius > 0.0)
    return((int) (2.0*ceil(radius)+1.0));
  for (width=5; ;)
  {
    normalize=0.0;
    for (v=(-width/2); v <= (width/2); v++)
    {
      for (u=(-width/2); u <= (width/2); u++)
        normalize+=exp((double) -(u*u+v*v)/(sigma*sigma));
    }
    v=width/2;
    value=exp((double) -(v*v)/(sigma*sigma))/normalize;
    if ((int) (value*MaxRGB) <= 0)
      break;
    width+=2;
  }
  return(width-2);
}

MagickExport int GetOptimalKernelWidth(const double radius,const double sigma)
{
  return GetOptimalKernelWidth1D(radius,sigma);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H S L T r a n s f o r m                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HSLTransform converts a (hue, saturation, luminosity) to a
%  (red, green, blue) triple.
%
%  The format of the HSLTransformImage method is:
%
%      void HSLTransform(const double hue,const double saturation,
%        const double luminosity,Quantum *red,Quantum *green,Quantum *blue)
%
%  A description of each parameter follows:
%
%    o hue, saturation, luminosity: A double value representing a
%      component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void HSLTransform(const double hue,const double saturation,
  const double luminosity,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    g,
    r,
    v,
    x,
    y,
    z;

  /*
    Convert HSL to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  v=(luminosity <= 0.5) ? (luminosity*(1.0+saturation)) :
    (luminosity+saturation-luminosity*saturation);
  if ((saturation == 0.0) || (hue == -1.0))
    {
      *red=(Quantum) (MaxRGB*luminosity+0.5);
      *green=(Quantum) (MaxRGB*luminosity+0.5);
      *blue=(Quantum) (MaxRGB*luminosity+0.5);
      return;
    }
  y=2.0*luminosity-v;
  x=y+(v-y)*(6.0*hue-floor(6.0*hue));
  z=v-(v-y)*(6.0*hue-floor(6.0*hue));
  switch ((int) (6.0*hue))
  {
    case 0: r=v; g=x; b=y; break;
    case 1: r=z; g=v; b=y; break;
    case 2: r=y; g=v; b=x; break;
    case 3: r=y; g=z; b=v; break;
    case 4: r=x; g=y; b=v; break;
    case 5: r=v; g=y; b=z; break;
    default: r=v; g=x; b=y; break;
  }
  *red=(Quantum) (MaxRGB*r+0.5);
  *green=(Quantum) (MaxRGB*g+0.5);
  *blue=(Quantum) (MaxRGB*b+0.5);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u l l                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Hull implements the eight hull algorithm described in Applied
%  Optics, Vol. 24, No. 10, 15 May 1985, "Geometric filter for Speckle
%  Reduction", by Thomas R Crimmins.  Each pixel in the image is replaced by
%  one of its eight of its surrounding pixels using a polarity and negative
%  hull function.
%
%  The format of the Hull method is:
%
%      void Hull(const int x_offset,const int y_offset,const int polarity,
%        const unsigned int columns,const unsigned int rows,Quantum *f,
%        Quantum *g)
%
%  A description of each parameter follows:
%
%    o x_offset, y_offset: An integer value representing the offset of the
%      current pixel within the image.
%
%    o polarity: An integer value declaring the polarity (+,-).
%
%    o columns, rows: Specifies the number of rows and columns in the image.
%
%    o f, g: A pointer to an image pixel and one of it's neighbor.
%
%
*/
MagickExport void Hull(const int x_offset,const int y_offset,const int polarity,
  const unsigned int columns,const unsigned int rows,Quantum *f,Quantum *g)
{
  int
    y;

  register int
    x;

  register Quantum
    *p,
    *q,
    *r,
    *s;

  Quantum
    v;

  assert(f != (Quantum *) NULL);
  assert(g != (Quantum *) NULL);
  p=f+(columns+2);
  q=g+(columns+2);
  r=p+(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < (int) rows; y++)
  {
    p++;
    q++;
    r++;
    if (polarity > 0)
      for (x=0; x < (int) columns; x++)
      {
        v=(*p);
        if (*r > v)
          v++;
        *q=v;
        p++;
        q++;
        r++;
      }
    else
      for (x=0; x < (int) columns; x++)
      {
        v=(*p);
        if (v > (Quantum) (*r+1))
          v--;
        *q=v;
        p++;
        q++;
        r++;
      }
    p++;
    q++;
    r++;
  }
  p=f+(columns+2);
  q=g+(columns+2);
  r=q+(y_offset*((int) columns+2)+x_offset);
  s=q-(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < (int) rows; y++)
  {
    p++;
    q++;
    r++;
    s++;
    if (polarity > 0)
      for (x=0; x < (int) columns; x++)
      {
        v=(*q);
        if (((Quantum) (*s+1) > v) && (*r > v))
          v++;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    else
      for (x=0; x < (int) columns; x++)
      {
        v=(*q);
        if (((Quantum) (*s+1) < v) && (*r < v))
          v--;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    p++;
    q++;
    r++;
    s++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I d e n t i t y A f f i n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IdentityAffine initializes the affine transform to the identity
%  matrix.
%
%  The format of the IdentityAffine method is:
%
%      IdentityAffine(AffineMatrix *affine)
%
%  A description of each parameter follows:
%
%    o affine: A pointer the the affine transform of type AffineMatrix.
%
%
*/
MagickExport void IdentityAffine(AffineMatrix *affine)
{
  assert(affine != (AffineMatrix *) NULL);
  affine->sx=1.0;
  affine->rx=0.0;
  affine->ry=0.0;
  affine->sy=1.0;
  affine->tx=0.0;
  affine->ty=0.0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n t e r p o l a t e C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InterpolateColor applies bi-linear interpolation between a pixel and
%  it's neighbors.
%
%  The format of the InterpolateColor method is:
%
%      PixelPacket InterpolateColor(Image *image,const double x_offset,
%        const double y_offset)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o x_offset,y_offset: A double representing the current (x,y) position of
%      the pixel.
%
%
*/
MagickExport PixelPacket InterpolateColor(Image *image,const double x_offset,
  const double y_offset)
{
  double
    alpha,
    beta;

  PixelPacket
    color,
    p,
    q,
    r,
    s;

  register double
    x,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  x=x_offset+0.5;
  y=y_offset+0.5;
  if ((x < -1.0) || (x >= image->columns) || (y < -1.0) || (y >= image->rows))
    return(image->background_color);
  p=image->background_color;
  if ((x >= 0.0) && (y >= 0.0))
    p=GetOnePixel(image,(int) x,(int) y);
  q=image->background_color;
  if (((x+1.0) < image->columns) && (y >= 0.0))
    q=GetOnePixel(image,(int) (x+1.0),(int) y);
  r=image->background_color;
  if ((x >= 0.0) && ((y+1.0) < image->rows))
    r=GetOnePixel(image,(int) x,(int) (y+1.0));
  s=image->background_color;
  if (((x+1.0) < image->columns) && ((y+1.0) < image->rows))
    s=GetOnePixel(image,(int) (x+1.0),(int) (y+1.0));
  x-=floor(x);
  y-=floor(y);
  alpha=1.0-x;
  beta=1.0-y;
  color.red=(Quantum)
    (beta*(alpha*p.red+x*q.red)+y*(alpha*r.red+x*s.red)+0.5);
  color.green=(Quantum)
    (beta*(alpha*p.green+x*q.green)+y*(alpha*r.green+x*s.green)+0.5);
  color.blue=(Quantum)
    (beta*(alpha*p.blue+x*q.blue)+y*(alpha*r.blue+x*s.blue)+0.5);
  color.opacity=(Quantum)
    (beta*(alpha*p.opacity+x*q.opacity)+y*(alpha*r.opacity+x*s.opacity)+0.5);
  return(color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d u l a t e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Modulate modulates the hue, saturation, and brightness of an
%  image.
%
%  The format of the ModulateImage method is:
%
%      void Modulate(double percent_hue,double percent_saturation,
%        double percent_brightness,Quantum *red,Quantum *green,Quantum *blue)
%        blue)
%
%  A description of each parameter follows:
%
%    o percent_hue, percent_saturation, percent_brightness: A double value
%      representing the percent change in a component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
MagickExport void Modulate(double percent_hue,double percent_saturation,
  double percent_brightness,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    brightness,
    hue,
    saturation;

  /*
    Increase or decrease color brightness, saturation, or hue.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  brightness*=(0.01+MagickEpsilon)*percent_brightness;
  if (brightness < 0.0)
    brightness=0.0;
  else
    if (brightness > 1.0)
      brightness=1.0;
  saturation*=(0.01+MagickEpsilon)*percent_saturation;
  if (saturation < 0.0)
    saturation=0.0;
  else
    if (saturation > 1.0)
      saturation=1.0;
  if (hue != -1.0)
    {
      hue*=(0.01+MagickEpsilon)*percent_hue;
      if (hue < 0.0)
        hue+=1.0;
      else
        if (hue > 1.0)
          hue-=1.0;
    }
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P e r m u t a t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Permutate()
%
%  The format of the Permutate method is:
%
%      void Permutate(int n,int k)
%
%  A description of each parameter follows:
%
%    o n:
%
%    o k:
%
%
*/
MagickExport double Permutate(int n,int k)
{
  double
    r;

  register int
    i;

  r=1.0;
  for (i=k+1; i <= n; i++)
    r*=i;
  for (i=1; i <= (n-k); i++)
    r/=i;
  return(r);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m H S L                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformHSL converts a (red, green, blue) to a (hue, saturation,
%  luminosity) triple.
%
%  The format of the TransformHSL method is:
%
%      void TransformHSL(const Quantum red,const Quantum green,
%        const Quantum blue,double *hue,double *saturation,double *luminosity)
%
%  A description of each parameter follows:
%
%    o red, green, blue: A Quantum value representing the red, green, and
%      blue component of a pixel..
%
%    o hue, saturation, luminosity: A pointer to a double value representing a
%      component of the HSL color space.
%
%
*/
MagickExport void TransformHSL(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *saturation,double *luminosity)
{
  double
    b,
    delta,
    g,
    max,
    min,
    r;

  /*
    Convert RGB to HSL colorspace.
  */
  assert(hue != (double *) NULL);
  assert(saturation != (double *) NULL);
  assert(luminosity != (double *) NULL);
  r=(double) red/MaxRGB;
  g=(double) green/MaxRGB;
  b=(double) blue/MaxRGB;
  max=Max(r,Max(g,b));
  min=Min(r,Min(g,b));
  *hue=(-1.0);
  *saturation=0.0;
  *luminosity=(0.5+MagickEpsilon)*(min+max);
  delta=max-min;
  if (delta == 0.0)
    return;
  *saturation=delta/((*luminosity <= 0.5) ? (min+max) : (2.0-max-min));
  if (r == max)
    *hue=(g == min ? 5.0+(max-b)/delta : 1.0-(max-g)/delta);
  else
    if (g == max)
      *hue=(b == min ? 1.0+(max-r)/delta : 3.0-(max-b)/delta);
    else
      *hue=(r == min ? 3.0+(max-g)/delta : 5.0-(max-r)/delta);
  *hue/=6.0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U p s a m p l e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Upsample doubles the size of the image.
%
%  The format of the UpSample method is:
%
%      void Upsample(const unsigned int width,const unsigned int height,
%        const unsigned int scaled_width,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o width,height:  Unsigned values representing the width and height of
%      the image pixel array.
%
%    o scaled_width:  Specifies the final width of the upsampled pixel array.
%
%    o pixels:  An unsigned char containing the pixel data.  On output the
%      upsampled pixels are returned here.
%
%
*/
MagickExport void Upsample(const unsigned int width,const unsigned int height,
  const unsigned int scaled_width,unsigned char *pixels)
{
  register int
    x,
    y;

  register unsigned char
    *p,
    *q,
    *r;

  /*
    Create a new image that is a integral size greater than an existing one.
  */
  assert(pixels != (unsigned char *) NULL);
  for (y=0; y < (int) height; y++)
  {
    p=pixels+(height-1-y)*scaled_width+(width-1);
    q=pixels+((height-1-y) << 1)*scaled_width+((width-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (int) width; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      *(q+1)=(((int) *p)+((int) *(p+1))+1) >> 1;
    }
  }
  for (y=0; y < (int) (height-1); y++)
  {
    p=pixels+(y << 1)*scaled_width;
    q=p+scaled_width;
    r=q+scaled_width;
    for (x=0; x < (int) (width-1); x++)
    {
      *q=(((int) *p)+((int) *r)+1) >> 1;
      *(q+1)=(((int) *p)+((int) *(p+2))+((int) *r)+((int) *(r+2))+2) >> 2;
      q+=2;
      p+=2;
      r+=2;
    }
    *q++=(((int) *p++)+((int) *r++)+1) >> 1;
    *q++=(((int) *p++)+((int) *r++)+1) >> 1;
  }
  p=pixels+(2*height-2)*scaled_width;
  q=pixels+(2*height-1)*scaled_width;
  memcpy(q,p,2*width);
}
