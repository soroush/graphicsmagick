/*
  Graphic Gems - Graphic Support Methods.
*/
#ifndef _MAGICK_GEM_H
#define _MAGICK_GEM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Graphic gems define declarations.
*/
extern MagickExport double
  ExpandAffine(const AffineMatrix *),
  Permutate(const long,const long);

extern MagickExport int
  GetOptimalKernelWidth(const double,const double),
  GetOptimalKernelWidth1D(const double,const double),
  GetOptimalKernelWidth2D(const double,const double);

extern MagickExport PixelPacket
  InterpolateColor(const Image *,const double,const double,ExceptionInfo *);

extern MagickExport Quantum
  GenerateNoise(const Quantum,const NoiseType),
  ScaleCharToQuantum(const unsigned char),
  ScaleIntToQuantum(const unsigned itn),
  ScaleIntensityToQuantum(const PixelPacket *),
  ScaleShortToQuantum(const unsigned short);

extern MagickExport unsigned char
  ScaleQuantumToChar(const Quantum);

extern MagickExport unsigned int
  ScaleQuantumToInt(const Quantum);

extern MagickExport unsigned short
  ScaleQuantumToShort(const Quantum);

extern MagickExport void
  Contrast(const int,Quantum *,Quantum *,Quantum *),
  HSLTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  HWBTransform(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  Hull(const long,const long,const unsigned long,const unsigned long,Quantum *,
    Quantum *,const int),
  IdentityAffine(AffineMatrix *),
  Modulate(const double,const double,const double,Quantum *,Quantum *,
    Quantum *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformHWB(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  Upsample(const unsigned long,const unsigned long,const unsigned long,
    unsigned char *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
