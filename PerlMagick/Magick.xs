/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                         PPPP   EEEEE  RRRR   L                              %
%                         P   P  E      R   R  L                              %
%                         PPPP   EEE    RRRR   L                              %
%                         P      E      R  R   L                              %
%                         P      EEEEE  R   R  LLLLL                          %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%                Object-oriented Perl interface to ImageMagick                %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                              Kyle Shorter                                   %
%                              John Cristy                                    %
%                             February 1997                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
% PerlMagick is an objected-oriented Perl interface to ImageMagick.  Use
% the module to read, manipulate, or write an image or image sequence from
% within a Perl script.  This makes PerlMagick suitable for Web CGI scripts.
%
*/

/*
  Include declarations.
*/
#if !defined(WIN32)
#define MagickExport
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <math.h>
#include <magick/api.h>
#undef tainted
#if !defined(WIN32)
#include <setjmp.h>
#else
#undef setjmp
#undef longjmp
#include <setjmpex.h>
#endif

#ifdef __cplusplus
}
#endif

/*
  Define declarations.
*/
#define ArrayReference  (char **) 4
#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)
#define DoubleReference  (char **) 2
#define EndOf(array)  (&array[NumberOf(array)])
#define False  0
#define ImageReference  (char **) 3
#define IntegerReference  (char **) 1
#define MaxArguments  24
#ifndef na
#define na  PL_na
#endif
#define NumberOf(array)  (sizeof(array)/sizeof(*array))
#define PackageName   "Image::Magick"
#define StringReference  (char **) 0
#ifndef sv_undef
#define sv_undef  PL_sv_undef
#endif
#define True  1

/*
  Typedef and structure declarations.
*/
typedef void
  *Image__Magick;  /* data type for the Image::Magick package */

struct PackageInfo
{
  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;
};

union ArgumentList
{
  long
    int_reference;

  double
    double_reference;

  char
    *string_reference;

  STRLEN
    length;

  Image
    *image_reference;

  SV
    *array_reference;
};

/*
  Static declarations.
*/
static char
  *BooleanTypes[] =
  {
    "False", "True", (char *) NULL
  },
  *ChannelTypes[] =
  {
    "Undefined", "Red", "Cyan", "Green", "Magenta", "Blue", "Yellow",
    "Opacity", "Black", "Matte", (char *) NULL
  },
  *ClassTypes[] =
  {
    "Undefined", "DirectClass", "PseudoClass", (char *) NULL
  },
  *ColorspaceTypes[] =
  {
    "Undefined", "RGB", "Gray", "Transparent", "OHTA", "XYZ", "YCbCr",
    "YCC", "YIQ", "YPbPr", "YUV", "CMYK", "sRGB", (char *) NULL
  },
  *CompositeTypes[] =
  {
    "Undefined", "Over", "In", "Out", "Atop", "Xor", "Plus", "Minus",
    "Add", "Subtract", "Difference", "Multiply", "Bumpmap", "Copy",
    "CopyRed", "CopyGreen", "CopyBlue", "CopyOpacity", "Clear", "Dissolve",
    "Displace", "Modulate", "Threshold", (char *) NULL
  },
  *CompressionTypes[] =
  {
    "Undefined", "None", "BZip", "Fax", "Group4", "JPEG", "LosslessJPEG",
    "LZW", "RLE", "Zip", (char *) NULL
  },
  *FilterTypess[] =
  {
    "Undefined", "Point", "Box", "Triangle", "Hermite", "Hanning",
    "Hamming", "Blackman", "Gaussian", "Quadratic", "Cubic", "Catrom",
    "Mitchell", "Lanczos", "Bessel", "Sinc", (char *) NULL
  },
  *GravityTypes[] =
  {
    "Forget", "NorthWest", "North", "NorthEast", "West", "Center",
    "East", "SouthWest", "South", "SouthEast", "Static", (char *) NULL
  },
  *ImageTypes[] =
  {
    "Undefined", "Bilevel", "Grayscale", "GrayscaleMatte" "Palette",
    "PaletteMatte", "TrueColor", "TrueColorMatte", "ColorSeparation",
    "ColorSeparationMatte", "Optimize", (char *) NULL
  },
  *IntentTypes[] =
  {
    "Undefined", "Saturation", "Perceptual", "Absolute", "Relative",
    (char *) NULL
  },
  *InterlaceTypes[] =
  {
    "Undefined", "None", "Line", "Plane", "Partition", (char *) NULL
  },
  *MethodTypes[] =
  {
    "Point", "Replace", "Floodfill", "FillToBorder", "Reset", (char *) NULL
  },
  *ModeTypes[] =
  {
    "Undefined", "Frame", "Unframe", "Concatenate", (char *) NULL
  },
  *NoiseTypes[] =
  {
    "Uniform", "Gaussian", "Multiplicative", "Impulse", "Laplacian",
    "Poisson", (char *) NULL
  },
  *PreviewTypes[] =
  {
    "Undefined", "Rotate", "Shear", "Roll", "Hue", "Saturation",
    "Brightness", "Gamma", "Spiff", "Dull", "Grayscale", "Quantize",
    "Despeckle", "ReduceNoise", "AddNoise", "Sharpen", "Blur",
    "Threshold", "EdgeDetect", "Spread", "Solarize", "Shade", "Raise",
    "Segment", "Swirl", "Implode", "Wave", "OilPaint", "Charcoal",
    "JPEG", (char *) NULL
  },
  *PrimitiveTypes[] =
  {
    "Undefined", "point", "line", "rectangle", "roundRectangle", "arc",
    "ellipse", "circle", "polyline", "polygon", "bezier", "path", "color",
    "matte", "text", "image", (char *) NULL
  },
  *ResolutionTypes[] =
  {
    "Undefined", "PixelsPerInch", "PixelsPerCentimeter", (char *) NULL
  },
  *StretchTypes[] =
  {
    "Normal", "UltraCondensed", "ExtraCondensed", "Condensed",
    "SemiCondensed", "SemiExpanded", "Expanded", "ExtraExpanded",
    "UltraExpanded", "Any", (char *) NULL
  },
  *StyleTypes[] =
  {
    "Normal", "Italic", "Oblique", "Any", (char *) NULL
  };

typedef struct _Arguments
{
  char
    *method,
    **type;
} Arguments;

static struct
  Methods
  {
    char
      *name;

    Arguments
      arguments[MaxArguments];
  } Methods[] =
  {
    { "comment", { {"comment", StringReference} } },
    { "label", { {"label", StringReference} } },
    { "AddNoise", { {"noise", NoiseTypes} } },
    { "Colorize", { {"fill", StringReference}, {"opacity", StringReference} } },
    { "Border", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"fill", StringReference},
      {"color", StringReference} } },
    { "Blur", { {"geometry", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Chop", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Crop", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Despeckle", },
    { "Edge", { {"radius", DoubleReference} } },
    { "Emboss", { {"geometry", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Enhance", },
    { "Flip", },
    { "Flop", },
    { "Frame", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"inner", IntegerReference},
      {"outer", IntegerReference}, {"fill", StringReference},
      {"color", StringReference} } },
    { "Implode", { {"amount", DoubleReference} } },
    { "Magnify", },
    { "MedianFilter", { {"radius", DoubleReference} } },
    { "Minify", },
    { "OilPaint", { {"radius", DoubleReference} } },
    { "ReduceNoise", { {"radius", DoubleReference} } },
    { "Roll", { {"geometry", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Rotate", { {"degree", DoubleReference},
      {"color", StringReference} } },
    { "Sample", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference} } },
    { "Scale", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference} } },
    { "Shade", { {"geometry", StringReference}, {"azimuth", DoubleReference},
      {"elevatation", DoubleReference}, {"color", BooleanTypes} } },
    { "Sharpen", { {"geometry", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Shear", { {"geometry", StringReference}, {"x", IntegerReference},
      {"y", DoubleReference}, {"color", StringReference} } },
    { "Spread", { {"radius", IntegerReference} } },
    { "Swirl", { {"degree", DoubleReference} } },
    { "Resize", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"filter", FilterTypess},
      {"blur", DoubleReference } } },
    { "Zoom", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"filter", FilterTypess},
      {"blur", DoubleReference } } },
    { "Annotate", { {"text", StringReference}, {"font", StringReference},
      {"point", DoubleReference}, {"density", StringReference},
      {"box", StringReference}, {"stroke", StringReference},
      {"fill", StringReference}, {"geometry", StringReference},
      {"sans", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"gravity", GravityTypes},
      {"translate", StringReference}, {"scale", StringReference},
      {"rotate", DoubleReference}, {"skewX", DoubleReference},
      {"skewY", DoubleReference}, {"strokewidth", IntegerReference},
      {"antialias", BooleanTypes}, {"family", StringReference},
      {"style", StyleTypes}, {"stretch", StretchTypes},
      {"weight", IntegerReference} } },
    { "ColorFloodfill", { {"geometry", StringReference},
      {"x", IntegerReference}, {"y", IntegerReference},
      {"fill", StringReference}, {"bordercolor", StringReference},
      {"fuzz", DoubleReference} } },
    { "Composite", { {"image", ImageReference}, {"compose", CompositeTypes},
      {"geometry", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"gravity", GravityTypes},
      {"opacity", DoubleReference}, {"tile", BooleanTypes},
      {"rotate", DoubleReference} } },
    { "Contrast", { {"sharp", BooleanTypes} } },
    { "CycleColormap", { {"display", IntegerReference} } },
    { "Draw", { {"primitive", PrimitiveTypes}, {"points", StringReference},
      {"method", MethodTypes}, {"stroke", StringReference},
      {"fill", StringReference}, {"strokewidth", DoubleReference},
      {"font", StringReference}, {"bordercolor", StringReference},
      {"x", DoubleReference}, {"y", DoubleReference},
      {"translate", StringReference}, {"scale", StringReference},
      {"rotate", DoubleReference}, {"skewX", DoubleReference},
      {"skewY", DoubleReference}, {"tile", ImageReference},
      {"pointsize", DoubleReference}, {"antialias", BooleanTypes},
      {"density", StringReference} } },
    { "Equalize", },
    { "Gamma", { {"gamma", StringReference}, {"red", DoubleReference},
      {"green", DoubleReference}, {"blue", DoubleReference} } },
    { "Map", { {"image", ImageReference}, {"dither", BooleanTypes} } },
    { "MatteFloodfill", { {"geometry", StringReference},
      {"x", IntegerReference}, {"y", IntegerReference},
      {"opacity", IntegerReference}, {"bordercolor", StringReference},
      {"fuzz", DoubleReference} } },
    { "Modulate", { {"factor", StringReference}, {"bright", DoubleReference},
      {"saturation", DoubleReference}, {"hue", DoubleReference} } },
    { "Negate", { {"gray", BooleanTypes} } },
    { "Normalize", },
    { "NumberColors", },
    { "Opaque", { {"color", StringReference}, {"fill", StringReference},
      {"fuzz", DoubleReference} } },
    { "Quantize", { {"colors", IntegerReference}, {"tree", IntegerReference},
      {"colorspace", ColorspaceTypes}, {"dither", BooleanTypes},
      {"measure", BooleanTypes}, {"global", BooleanTypes} } },
    { "Raise", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"raise", BooleanTypes} } },
    { "Segment", { {"geometry", StringReference}, {"cluster", DoubleReference},
      {"smooth", DoubleReference}, {"colorspace", ColorspaceTypes},
      {"verbose", BooleanTypes} } },
    { "Signature", },
    { "Solarize", { {"factor", DoubleReference} } },
    { "Sync", },
    { "Texture", { {"texture", ImageReference} } },
    { "Sans", { {"geometry", StringReference}, {"crop", StringReference},
      {"filter", FilterTypess} } },
    { "Transparent", { {"color", StringReference},
      {"opacity", IntegerReference}, {"fuzz", DoubleReference} } },
    { "Threshold", { {"threshold", DoubleReference} } },
    { "Charcoal", { {"geometry", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Trim", { {"fuzz", DoubleReference} } },
    { "Wave", { {"geometry", StringReference}, {"ampli", DoubleReference},
      {"wave", DoubleReference} } },
    { "Channel", { {"channel", ChannelTypes} } },
    { "Condense", },
    { "Stereo", { {"image", ImageReference} } },
    { "Stegano", { {"image", ImageReference}, {"offset", IntegerReference} } },
    { "Deconstruct", },
    { "GaussianBlur", { {"geometry", StringReference},
      {"radius", DoubleReference}, {"sigma", DoubleReference} } },
    { "Convolve", { {"coefficients", ArrayReference} } },
    { "Profile", { {"name", StringReference}, {"profile", StringReference} } },
    { "UnsharpMask", { {"geometry", StringReference},
      {"radius", DoubleReference}, {"sigma", DoubleReference},
      {"amount", DoubleReference}, {"threshold", DoubleReference} } },
    { "MotionBlur", { {"geometry", StringReference},
      {"radius", DoubleReference}, {"sigma", DoubleReference},
      {"angle", DoubleReference} } },
    { "OrderedDither", },
    { "Shave", { {"geometry", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference} } },
    { "Level", { {"level", StringReference}, {"black-point", DoubleReference},
      {"mid-point", DoubleReference}, {"white-point", DoubleReference} } },
    { "Clip", },
  };

/*
  Variable declarations.
*/
char
  *client_name = "PerlMagick";

static jmp_buf
  *error_jump;  /* long jump return for FATAL errors */

SV
  *error_list;  /* Perl variable for storing messages */

/*
  Forward declarations.
*/
static Image
  *SetupList(SV *,struct PackageInfo **,SV ***);

static int
  strEQcase(const char *,const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e P a c k a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClonePackageInfo makes a duplicate of the given info, or if info is
%  NULL, a new one.
%
%  The format of the ClonePackageInfo routine is:
%
%      struct PackageInfo *ClonePackageInfo(struct PackageInfo *info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method ClonePackageInfo returns a duplicate of the given
%      info, or if info is NULL, a new one.
%
%    o info: a structure of type info.
%
%
*/
static struct PackageInfo *ClonePackageInfo(struct PackageInfo *info)
{
  struct PackageInfo
    *cloned_info;

  cloned_info=(struct PackageInfo *) AcquireMemory(sizeof(struct PackageInfo));
  if (!info)
    {
      InitializeMagick(client_name);
      cloned_info->image_info=CloneImageInfo((ImageInfo *) NULL);
      cloned_info->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
      return(cloned_info);
    }
  *cloned_info=(*info);
  cloned_info->image_info=CloneImageInfo(info->image_info);
  cloned_info->quantize_info=CloneQuantizeInfo(info->quantize_info);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   c o n s t a n t                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method constant returns a double value for the specified name.
%
%  The format of the constant routine is:
%
%      double constant(char *name,int sans)
%
%  A description of each parameter follows:
%
%    o value: Method constant returns a double value for the specified name.
%
%    o name: The name of the constant.
%
%    o sans: This integer value is not used.
%
%
*/
static double constant(char *name,int sans)
{
  errno=0;
  switch (*name)
  {
    case 'B':
    {
      if (strEQ(name,"BlobError"))
        return(BlobError);
      if (strEQ(name,"BlobWarning"))
        return(BlobWarning);
      break;
    }
    case 'C':
    {
      if (strEQ(name,"CacheError"))
        return(CacheError);
      if (strEQ(name,"CacheWarning"))
        return(CacheWarning);
      if (strEQ(name,"CorruptImageError"))
        return(CorruptImageError);
      if (strEQ(name,"CorruptImageWarning"))
        return(CorruptImageWarning);
      break;
    }
    case 'D':
    {
      if (strEQ(name,"DelegateError"))
        return(DelegateError);
      if (strEQ(name,"DelegateWarning"))
        return(DelegateWarning);
      break;
    }
    case 'F':
    {
      if (strEQ(name,"FatalException"))
        return(FatalException);
      if (strEQ(name,"FileOpenError"))
        return(FileOpenError);
      if (strEQ(name,"FileOpenWarning"))
        return(FileOpenWarning);
      break;
    }
    case 'M':
    {
      if (strEQ(name,"MaxRGB"))
        return(MaxRGB);
      if (strEQ(name,"MissingDelegateError"))
        return(MissingDelegateError);
      if (strEQ(name,"MissingDelegateWarning"))
        return(MissingDelegateWarning);
      if (strEQ(name,"MissingPluginWarning"))
        return(MissingDelegateWarning);
      break;
    }
    case 'O':
    {
      if (strEQ(name,"Opaque"))
        return(OpaqueOpacity);
      if (strEQ(name,"OptionError"))
        return(OptionError);
      if (strEQ(name,"OptionWarning"))
        return(OptionWarning);
      break;
    }
    case 'P':
    {
      if (strEQ(name,"PluginWarning"))
        return(DelegateWarning);
      break;
    }
    case 'R':
    {
      if (strEQ(name,"ResourceLimitError"))
        return(ResourceLimitError);
      if (strEQ(name,"ResourceLimitWarning"))
        return(ResourceLimitWarning);
      break;
    }
    case 'S':
    {
      if (strEQ(name,"StreamError"))
        return(StreamError);
      if (strEQ(name,"StreamWarning"))
        return(StreamWarning);
      if (strEQ(name,"Success"))
        return(0);
      break;
    }
    case 'T':
    {
      if (strEQ(name,"Transparent"))
        return(TransparentOpacity);
      break;
    }
    case 'X':
    {
      if (strEQ(name,"XServerError"))
        return(XServerError);
      if (strEQ(name,"XServerWarning"))
        return(XServerWarning);
      break;
    }
  }
  errno=EINVAL;
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y P a c k a g e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPackageInfo frees a previously created info structure.
%
%  The format of the DestroyPackageInfo routine is:
%
%      DestroyPackageInfo(struct PackageInfo *info)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static void DestroyPackageInfo(struct PackageInfo *info)
{
  DestroyImageInfo(info->image_info);
  DestroyQuantizeInfo(info->quantize_info);
  LiberateMemory((void **) &info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t L i s t                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetList is recursively called by SetupList to traverse the
%  Image__Magick reference.  If building an reference_vector (see SetupList),
%  *current is the current position in *reference_vector and *last is the final
%  entry in *reference_vector.
%
%  The format of the GetList routine is:
%
%      GetList(info)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static Image *GetList(SV *reference,SV ***reference_vector,int *current,
  int *last)
{
  Image
    *image;

  if (!reference)
    return(NULL);
  switch (SvTYPE(reference))
  {
    case SVt_PVAV:
    {
      AV
        *av;

      Image
        *head,
        *previous;

      int
        n;

      register int
        i;

      /*
        Array of images.
      */
      previous=(Image *) NULL;
      head=(Image *) NULL;
      av=(AV *) reference;
      n=av_len(av);
      for (i=0; i <= n; i++)
      {
        SV
          **rv;

        rv=av_fetch(av,i,0);
        if (rv && *rv && sv_isobject(*rv))
          {
            image=GetList(SvRV(*rv),reference_vector,current,last);
            if (!image)
              continue;
            if (image == previous)
              {
                ExceptionInfo
                  *exception;

                exception=(&image->exception);
                image=CloneImage(image,0,0,True,exception);
                if (image == (Image *) NULL)
                  {
                    MagickWarning(exception->severity,exception->reason,
                      exception->description);
                    return(NULL);
                  }
              }
            image->previous=previous;
            *(previous ? &previous->next : &head)=image;
            for (previous=image; previous->next; previous=previous->next);
          }
      }
      return(head);
    }
    case SVt_PVMG:
    {
      /*
        Blessed scalar, one image.
      */
      image=(Image *) SvIV(reference);
      if (!image)
        return(NULL);
      image->previous=(Image *) NULL;
      image->next=(Image *) NULL;
      if (reference_vector)
        {
          if (*current == *last)
            {
              *last+=256;
              if (*reference_vector)
                ReacquireMemory((void **) & (*reference_vector),
                  *last*sizeof(*reference_vector));
              else
                *reference_vector=(SV **)
                  AcquireMemory(*last*sizeof(*reference_vector));
            }
        (*reference_vector)[*current]=reference;
        (*reference_vector)[++(*current)]=NULL;
      }
      return(image);
    }
  }
  (void) fprintf(stderr,"GetList: Invalid reference type %d\n",
    (int) SvTYPE(reference));
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P a c k a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPackageInfo looks up or creates an info structure for the given
%  Image__Magick reference.  If it does create a new one, the information in
%  oldinfo is used to initialize it.
%
%  The format of the GetPackageInfo routine is:
%
%      struct PackageInfo *GetPackageInfo(void *reference,
%        struct PackageInfo *oldinfo)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static struct PackageInfo *GetPackageInfo(void *reference,
  struct PackageInfo *oldinfo)
{
  char
    message[MaxTextExtent];

  struct PackageInfo
    *info;

  SV
    *sv;

  FormatString(message,"%s::A_%lx_Z",PackageName,(long) reference);
  sv=perl_get_sv(message,(TRUE | 0x02));
  if (!sv)
    {
      MagickWarning(ResourceLimitWarning,"Unable to create info variable",
        message);
      return(oldinfo);
    }
  if (SvREFCNT(sv) == 0)
    (void) SvREFCNT_inc(sv);
  if (SvIOKp(sv) && (info=(struct PackageInfo *) SvIV(sv)))
    return(info);
  info=ClonePackageInfo(oldinfo);
  sv_setiv(sv,(IV) info);
  return(info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o o k u p S t r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LookupStr searches through a list of strings matching it to string
%  and return its index in the list, or -1 for not found .
%
%  The format of the LookupStr routine is:
%
%      int LookupStr(char **list,const char *string)
%
%  A description of each parameter follows:
%
%    o status: Method LookupStr returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static int LookupStr(char **list,const char *string)
{
  int
    longest,
    offset;

  register char
    **p;

  offset=(-1);
  longest=0;
  for (p=list; *p; p++)
    if (strEQcase(string,*p) > longest)
      {
        offset=p-list;
        longest=strEQcase(string,*p);
      }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k E r r o r H a n d l e r                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickErrorHandler replaces ImageMagick's fatal error handler.  This
%  stores the message in a Perl variable,and longjmp's to return the error to
%  Perl.  If the error_flag variable is set, it also calls the Perl warn
%  routine.  Note that this doesn't exit but returns control to Perl; the
%  Image::Magick handle may be left in a bad state.
%
%  The format of the MagickErrorHandler routine is:
%
%      MagickErrorHandler(const ExceptionType error,const char *message,
%        const char *qualifier)
%
%  A description of each parameter follows:
%
%    o error: Specifies the numeric error category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
static void MagickErrorHandler(const ExceptionType error,const char *message,
  const char *qualifier)
{
  char
    text[MaxTextExtent];

  int
    error_number;

  error_number=errno;
  errno=0;
  FormatString(text,"Exception %d: %.1024s%s%.1024s%s%s%.64s%s",error,
    (message ? message : "ERROR"),
    qualifier ? " (" : "",qualifier ? qualifier : "",qualifier ? ")" : "",
    error_number ? " [" : "",error_number ? strerror(error_number) : "",
    error_number? "]" : "");
  if ((error_list == NULL) || (error_jump == NULL))
    {
      /*
        Set up message buffer.
      */
      warn("%s",text);
      if (error_jump == NULL)
        exit((int) error % 100);
    }
  if (error_list)
    {
      if (SvCUR(error_list))
        sv_catpv(error_list,"\n");
      sv_catpv(error_list,text);
    }
  longjmp(*error_jump,(int) error);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k W a r n i n g H a n d l e r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickWarningHandler replaces the ImageMagick warning handler.  This
%  stores the (possibly multiple) messages in a Perl variable for later
%  returning.
%
%  The format of the MagickWarningHandler routine is:
%
%      MagickWarningHandler(const ExceptionType warning,const char *message,
%        const char *qualifier)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
static void MagickWarningHandler(const ExceptionType warning,
  const char *message,const char *qualifier)
{
  char
    text[MaxTextExtent];

  int
    error_number;

  error_number=errno;
  errno=0;
  if (!message)
    return;
  FormatString(text,"Warning %d: %.1024s%s%.1024s%s%s%.64s%s",warning,
    message,qualifier ? " (" : "",qualifier ? qualifier : "",
    qualifier? ")" : "",error_number ? " [" : "",
    error_number ? strerror(error_number) : "",error_number ? "]" : "");
  if (error_list == NULL)
    {
      /*
        Set up message buffer.
      */
      warn("%s",text);
      if (error_list == NULL)
        return;
    }
  if (SvCUR(error_list))
    sv_catpv(error_list,"\n");  /* add \n separator between messages */
  sv_catpv(error_list,text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t A t t r i b u t e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetAttribute sets the attribute to the value in sval.  This can
%  change either or both of image or info.
%
%  The format of the SetAttribute routine is:
%
%      SetAttribute(struct PackageInfo *info,Image *image,char *attribute,
%        SV *sval)
%
%  A description of each parameter follows:
%
%    o status: Method SetAttribute returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static void SetAttribute(struct PackageInfo *info,Image *image,char *attribute,
  SV *sval)
{
  double
    blue,
    green,
    opacity,
    red;

  int
    sp;

  long
    x,
    y;

  PixelPacket
    *color,
    target_color;

  switch (*attribute)
  {
    case 'A':
    case 'a':
    {
      if (LocaleCompare(attribute,"adjoin") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid adjoin type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->adjoin=sp != 0;
          return;
        }
      if (LocaleCompare(attribute,"antialias") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid antialias type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->antialias=sp != 0;
          return;
        }
      break;
    }
    case 'B':
    case 'b':
    {
      if (LocaleCompare(attribute,"background") == 0)
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->background_color=target_color;
          for ( ; image; image=image->next)
            image->background_color=target_color;
          return;
        }
      if (LocaleCompare(attribute,"blue-primary") == 0)
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf",
              &image->chromaticity.blue_primary.x,
              &image->chromaticity.blue_primary.y);
          return;
        }
      if (LocaleCompare(attribute,"bordercolor") == 0)
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->border_color=target_color;
          for ( ; image; image=image->next)
            image->border_color=target_color;
          return;
        }
      break;
    }
    case 'C':
    case 'c':
    {
      if (LocaleCompare(attribute,"cache-threshold") == 0)
        {
          SetCacheThreshold(SvIV(sval));
          return;
        }
      if (LocaleCompare(attribute,"clip-mask") == 0)
        {
          Image
            *clip_mask;

          clip_mask=SetupList(SvRV(sval),&info,(SV ***) NULL);
          for ( ; image; image=image->next)
            SetImageClipMask(image,clip_mask);
          return;
        }
      if (LocaleNCompare(attribute,"colormap",8) == 0)
        {
          for ( ; image; image=image->next)
          {
            int
              i;

            if (image->storage_class == DirectClass)
              continue;
            i=0;
            (void) sscanf(attribute,"%*[^[][%d",&i);
            if (i > (int) image->colors)
              i%=image->colors;
            if (strchr(SvPV(sval,na),',') == 0)
              QueryColorDatabase(SvPV(sval,na),image->colormap+i);
            else
              {
                color=image->colormap+i;
                red=color->red;
                green=color->green;
                blue=color->blue;
                (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf%*[, ]%lf",&red,
                  &green,&blue);
                color->red=(Quantum)
                  ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
                color->green=(Quantum)
                  ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
                color->blue=(Quantum)
                  ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
              }
          }
          return;
        }
      if (LocaleCompare(attribute,"colorspace") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(ColorspaceTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid colorspace type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->colorspace=(ColorspaceType) sp;
          for ( ; image; image=image->next)
            RGBTransformImage(image,(ColorspaceType) sp);
          return;
        }
      if (LocaleCompare(attribute,"compression") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(CompressionTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid compression type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->compression=(CompressionType) sp;
          for ( ; image; image=image->next)
            image->compression=(CompressionType) sp;
          return;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare(attribute,"debug") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid debug type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->debug=sp != 0;
          return;
        }
      if (LocaleCompare(attribute,"delay") == 0)
        {
          for ( ; image; image=image->next)
            image->delay=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"density") == 0)
        {
          if (!IsGeometry(SvPV(sval,na)))
            {
              MagickWarning(OptionWarning,"Invalid geometry on density",
                SvPV(sval,na));
              return;
            }
          if (info)
            (void) CloneString(&info->image_info->density,SvPV(sval,na));
          for ( ; image; image=image->next)
          {
            int
              count;

            count=sscanf(info->image_info->density,"%lfx%lf",
              &image->x_resolution,&image->y_resolution);
            if (count != 2)
              image->y_resolution=image->x_resolution;
          }
          return;
        }
      if (LocaleCompare(attribute,"depth") == 0)
        {
          if (info)
            info->image_info->depth=SvIV(sval);
          for ( ; image; image=image->next)
            SetImageDepth(image,SvIV(sval));
          return;
        }
      if (LocaleCompare(attribute,"dispose") == 0)
        {
          for (; image; image=image->next)
            image->dispose=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"dither") == 0)
        {
          if (info)
            {
              sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) :
                SvIV(sval);
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid dither type",
                    SvPV(sval,na));
                  return;
                }
              info->image_info->dither=sp != 0;
            }
          return;
        }
      if (LocaleCompare(attribute,"display") == 0)
        {
          display:
          if (info)
            (void) CloneString(&info->image_info->server_name,SvPV(sval,na));
          return;
        }
      break;
    }
    case 'F':
    case 'f':
    {
      if (LocaleCompare(attribute,"filename") == 0)
        {
          if (info)
            (void) strncpy(info->image_info->filename,SvPV(sval,na),
              MaxTextExtent-1);
          for ( ; image; image=image->next)
            (void) strncpy(image->filename,SvPV(sval,na),MaxTextExtent-1);
          return;
        }
      if (LocaleCompare(attribute,"file") == 0)
        {
          if (info)
            info->image_info->file=(FILE *) IoIFP(sv_2io(sval));
          return;
        }
      if (LocaleCompare(attribute,"font") == 0)
        {
          if (info)
            (void) CloneString(&info->image_info->font,SvPV(sval,na));
          return;
        }
      if (LocaleCompare(attribute,"fuzz") == 0)
        {
          if (info)
            info->image_info->fuzz=SvNV(sval);
          for ( ; image; image=image->next)
            image->fuzz=SvNV(sval);
          return;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare(attribute,"gravity") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(GravityTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid gravity type",SvPV(sval,na));
              return;
            }
          for ( ; image; image=image->next)
            image->gravity=(GravityType) sp;
          return;
        }
      if (LocaleCompare(attribute,"green-primary") == 0)
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf",
              &image->chromaticity.green_primary.x,
              &image->chromaticity.green_primary.y);
          return;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleNCompare(attribute,"index",5) == 0)
        {
          long
            index;

          IndexPacket
            *indexes;

          PixelPacket
            *pixel;

          for ( ; image; image=image->next)
          {
            if (image->storage_class != PseudoClass)
              continue;
            x=0;
            y=0;
            (void) sscanf(attribute,"%*[^[][%ld%*[, ]%ld",&x,&y);
            pixel=GetImagePixels(image,(long) (x % image->columns),
              (long) (y % image->rows),1,1);
            if (pixel == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            (void) sscanf(SvPV(sval,na),"%ld",&index);
            if ((index >= 0) && (index < (int) image->colors))
              *indexes=(IndexPacket) index;
            (void) SyncImagePixels(image);
          }
          return;
        }
      if (LocaleCompare(attribute,"iterations") == 0)
        {
  iterations:
          for ( ; image; image=image->next)
            image->iterations=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"interlace") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(InterlaceTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid interlace value",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->interlace=(InterlaceType) sp;
          for ( ; image; image=image->next)
            image->interlace=(InterlaceType) sp;
          return;
        }
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare(attribute,"loop") == 0)
        goto iterations;
      break;
    }
    case 'M':
    case 'm':
    {
      if (LocaleCompare(attribute,"magick") == 0)
        {
          if (info)
            {
              ExceptionInfo
                exception;

              GetExceptionInfo(&exception);
              FormatString(info->image_info->filename,"%.1024s:",SvPV(sval,na));
              SetImageInfo(info->image_info,True,&exception);
              if (*info->image_info->magick == '\0')
                MagickWarning(OptionWarning,"Unrecognized image format",
                  info->image_info->filename);
              else
                for ( ; image; image=image->next)
                  (void) strncpy(image->magick,info->image_info->magick,
                    MaxTextExtent-1);
              DestroyExceptionInfo(&exception);
            }
          return;
        }
      if (LocaleCompare(attribute,"mattecolor") == 0)
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->matte_color=target_color;
          for ( ; image; image=image->next)
            image->matte_color=target_color;
          return;
        }
      if (LocaleCompare(attribute,"matte") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid matte type",SvPV(sval,na));
              return;
            }
          for ( ; image; image=image->next)
            image->matte=sp != 0;
          return;
        }
      if (LocaleCompare(attribute,"monochrome") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid monochrome type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->monochrome=sp != 0;
          return;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare(attribute,"page") == 0)
        {
          char
            *geometry;

          geometry=GetPageGeometry(SvPV(sval,na));
          if (info)
            (void) CloneString(&info->image_info->page,geometry);
          for ( ; image; image=image->next)
            (void) GetImageGeometry(image,geometry,False,&image->page);
          LiberateMemory((void **) &geometry);
          return;
        }
      if (LocaleCompare(attribute,"pen") == 0)
        {
          if (info)
            (void) QueryColorDatabase(SvPV(sval,na),&info->image_info->pen);
          return;
        }
      if (LocaleNCompare(attribute,"pixel",5) == 0)
        {
          PixelPacket
            *pixel;

          for ( ; image; image=image->next)
          {
            x=0;
            y=0;
            (void) sscanf(attribute,"%*[^[][%ld%*[, ]%ld",&x,&y);
            pixel=GetImagePixels(image,(long) (x % image->columns),
              (long) (y % image->rows),1,1);
            if (pixel == (PixelPacket *) NULL)
              break;
            SetImageType(image,TrueColorType);
            if (strchr(SvPV(sval,na),',') == 0)
              QueryColorDatabase(SvPV(sval,na),pixel);
            else
              {
                red=pixel->red;
                green=pixel->green;
                blue=pixel->blue;
                opacity=pixel->opacity;
                (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf%*[, ]%lf%*[, ]%lf",
                  &red,&green,&blue,&opacity);
                pixel->red=(Quantum)
                  ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
                pixel->green=(Quantum)
                  ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
                pixel->blue=(Quantum)
                  ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
                pixel->opacity=(Quantum) ((opacity < 0) ? 0 :
                  (opacity > MaxRGB) ? MaxRGB : opacity+0.5);
              }
            (void) SyncImagePixels(image);
          }
          return;
        }
      if (LocaleCompare(attribute,"pointsize") == 0)
        {
          if (info)
            (void) sscanf(SvPV(sval,na),"%lf",&info->image_info->pointsize);
          return;
        }
      if (LocaleCompare(attribute,"preview") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(PreviewTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid preview type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->preview_type=(PreviewType) sp;
          return;
        }
      break;
    }
    case 'Q':
    case 'q':
    {
      if (LocaleCompare(attribute,"quality") == 0)
        {
          if (info)
            info->image_info->quality=SvIV(sval);
          return;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare(attribute,"red-primary") == 0)
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf",
              &image->chromaticity.red_primary.x,
              &image->chromaticity.red_primary.y);
          return;
        }
      if (LocaleCompare(attribute,"render") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(IntentTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid rendering intent",
                SvPV(sval,na));
              return;
            }
         for ( ; image; image=image->next)
           image->rendering_intent=(RenderingIntent) sp;
         return;
       }
      break;
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare(attribute,"scene") == 0)
        {
          for ( ; image; image=image->next)
            image->scene=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"subimage") == 0)
        {
          if (info)
            info->image_info->subimage=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"subrange") == 0)
        {
          if (info)
            info->image_info->subrange=SvIV(sval);
          return;
        }
      if (LocaleCompare(attribute,"server") == 0)
        goto display;
      if (LocaleCompare(attribute,"size") == 0)
        {
          if (info)
            {
              if (!IsGeometry(SvPV(sval,na)))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on size",
                    SvPV(sval,na));
                  return;
                }
              (void) CloneString(&info->image_info->size,SvPV(sval,na));
            }
          return;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare(attribute,"tile") == 0)
        {
          if (info)
            (void) CloneString(&info->image_info->tile,SvPV(sval,na));
          return;
        }
      if (LocaleCompare(attribute,"texture") == 0)
        {
          if (info)
            (void) CloneString(&info->image_info->texture,SvPV(sval,na));
          return;
        }
      if (LocaleCompare(attribute,"type") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(ImageTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid image type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->type=(ImageType) sp;
          for ( ; image; image=image->next)
            SetImageType(image,(ImageType) sp);
          return;
        }
      break;
    }
    case 'U':
    case 'u':
    {
      if (LocaleCompare(attribute,"unit") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(ResolutionTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid resolution unit",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->units=(ResolutionType) sp;
          for ( ; image; image=image->next)
            image->units=(ResolutionType) sp;
          return;
        }
      break;
    }
    case 'V':
    case 'v':
    {
      if (LocaleCompare(attribute,"verbose") == 0)
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid verbose type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->verbose=sp != 0;
          return;
        }
      if (LocaleCompare(attribute,"view") == 0)
        {
          if (info)
            (void) CloneString(&info->image_info->view,SvPV(sval,na));
          return;
        }
      break;
    }
    case 'W':
    case 'w':
    {
      if (LocaleCompare(attribute,"white-primary") == 0)
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf%*[, ]%lf",
              &image->chromaticity.white_point.x,
              &image->chromaticity.white_point.y);
          return;
        }
      break;
    }
    default:
      break;
  }
  if (image == (Image *) NULL)
    MagickWarning(OptionWarning,"Invalid attribute",attribute);
  for ( ; image; image=image->next)
    (void) SetImageAttribute(image,attribute,SvPV(sval,na));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t u p L i s t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetupList returns the list of all the images linked by their
%  image->next and image->previous link lists for use with ImageMagick.  If
%  info is non-NULL, an info structure is returned in *info.  If
%  reference_vector is non-NULL,an array of SV* are returned in
%  *reference_vector.  Reference_vector is used when the images are going to be
%  replaced with new Image*'s.
%
%  The format of the SetupList routine is:
%
%      Image *SetupList(SV *reference,struct PackageInfo **info,
%        SV ***reference_vector)
%
%  A description of each parameter follows:
%
%    o status: Method SetAttribute returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static Image *SetupList(SV *reference,struct PackageInfo **info,
  SV ***reference_vector)
{
  Image
    *image;

  int
    current,
    last;

  if (reference_vector)
    *reference_vector=NULL;
  if (info)
    *info=NULL;
  current=0;
  last=0;
  image=GetList(reference,reference_vector,&current,&last);
  if (info && (SvTYPE(reference) == SVt_PVAV))
    *info=GetPackageInfo((void *) reference,(struct PackageInfo *) NULL);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   s t r E Q c a s e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method strEQcase compares two strings and returns 0 if they are the
%  same or if the second string runs out first.  The comparison is case
%  insensitive.
%
%  The format of the strEQcase routine is:
%
%      int strEQcase(const char *p,const char *q)
%
%  A description of each parameter follows:
%
%    o status: Method strEQcase returns zero if strings p and q are the
%      same or if the second string runs out first.
%
%    o p: a character string.
%
%    o q: a character string.
%
%
*/
static int strEQcase(const char *p,const char *q)
{
  char
    c;

  register int
    i;

  for (i=0 ; (c=(*q)) != 0; i++)
  {
    if ((isUPPER(c) ? toLOWER(c) : c) != (isUPPER(*p) ? toLOWER(*p) : *p))
      return(0);
    p++;
    q++;
  }
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e : : M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
MODULE = Image::Magick PACKAGE = Image::Magick

PROTOTYPES: ENABLE

BOOT:
  SetWarningHandler(MagickWarningHandler);
  SetErrorHandler(MagickErrorHandler);

double
constant(name,argument)
  char *name
  int argument

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A n i m a t e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Animate(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    AnimateImage  = 1
    animate       = 2
    animateimage  = 3
  PPCODE:
  {
    jmp_buf
      error_jmp;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    volatile int
      status;

    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    status=0;
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to animate",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"server",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,image,SvPV(ST(i-1),na),ST(i));
    AnimateImages(package_info->image_info,image);
    (void) CatchImageException(image);

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A p p e n d                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Append(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    AppendImage  = 1
    append       = 2
    appendimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next;

    int
      stack;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    attribute=NULL;
    av=NULL;
    reference_vector=NULL;
    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to append",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get options.
    */
    stack=True;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'S':
        case 's':
        {
          if (LocaleCompare(attribute,"stack") == 0)
            {
              stack=LookupStr(BooleanTypes,SvPV(ST(i),na));
              if (stack < 0)
                {
                  MagickWarning(OptionWarning,"Invalid stack type",
                    SvPV(ST(i),na));
                  return;
                }
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    next=AppendImages(image,stack,&image->exception);
    if (!next)
      {
        MagickWarning(image->exception.severity,image->exception.reason,
          image->exception.description);
        goto MethodException;
      }
    for ( ; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A v e r a g e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Average(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    AverageImage   = 1
    average        = 2
    averageimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to average",NULL);
        goto MethodException;
      }
    next=AverageImages(image,&image->exception);
    if (!next)
      {
        MagickWarning(image->exception.severity,image->exception.reason,
          image->exception.description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) next);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    FormatString(info->image_info->filename,"average-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    (void) strncpy(image->filename,info->image_info->filename,MaxTextExtent-1);
    SetImageInfo(info->image_info,False,&image->exception);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   B l o b T o I m a g e                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
BlobToImage(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    BlobToImage  = 1
    blobtoimage  = 2
    blobto       = 3
  PPCODE:
  {
    AV
      *av;

    char
      **keep,
      **list;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *image;

    int
      ac,
      n;

    jmp_buf
      error_jmp;

    register char
      **p;

    register int
      i;

    struct PackageInfo
      *info;

    STRLEN
      *length;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      number_images;

    number_images=0;
    error_list=newSVpv("",0);
    ac=(items < 2) ? 1 : items-1;
    list=(char **) AcquireMemory((ac+1)*sizeof(*list));
    length=(STRLEN *) AcquireMemory((ac+1)*sizeof(length));
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    if (SvTYPE(reference) != SVt_PVAV)
      {
        MagickWarning(OptionWarning,"Unable to read into a single image",NULL);
        goto ReturnIt;
      }
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    n=1;
    if (items <= 1)
      {
        MagickWarning(OptionWarning,"no blobs to convert",NULL);
        goto ReturnIt;
      }
    for (n=0, i=0; i < ac; i++)
    {
      list[n]=(char *) (SvPV(ST(i+1),length[n]));
      if ((items >= 3) && strEQcase((char *) SvPV(ST(i+1),na),"blob"))
        {
          list[n]=(char *) (SvPV(ST(i+2),length[n]));
          continue;
        }
      n++;
    }
    list[n]=(char *) NULL;
    keep=list;
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    GetExceptionInfo(&exception);
    for (i=number_images=0; i < n; i++)
    {
      image=BlobToImage(info->image_info,list[i],length[i],&exception);
      if (image == (Image *) NULL)
        MagickWarning(exception.severity,exception.reason,
          exception.description);
      for ( ; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
        number_images++;
      }
    }
    DestroyExceptionInfo(&exception);
    /*
      Free resources.
    */
    for (i=0; i < n; i++)
      if (list[i])
        for (p=keep; list[i] != *p++; )
          if (*p == NULL)
            {
              LiberateMemory((void **) &list[i]);
              break;
            }

  ReturnIt:
    LiberateMemory((void **) &list);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   C o a l e s c e                                                           #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Coalesce(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    CoalesceImage   = 1
    coalesce        = 2
    coalesceimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    ExceptionInfo
      *exception;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      **reference_vector,
      *sv;

    volatile int
      status;

    reference_vector=NULL;
    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to coalesce",NULL);
        goto MethodException;
      }
    exception=(&image->exception);
    image=CoalesceImages(image,exception);
    if (!image)
      {
        MagickWarning(exception->severity,exception->reason,
          exception->description);
        goto MethodException;
      }
    for (next=image; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   C o p y                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Copy(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    CopyImage   = 1
    copy        = 2
    copyimage   = 3
    CloneImage  = 4
    clone       = 5
    cloneimage  = 6
    Clone  = 7
  PPCODE:
  {
    AV
      *av;

    HV
      *hv;

    Image
      *image,
      *next;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!(image=SetupList(reference,&info,(SV ***) NULL)))
      {
        MagickWarning(OptionWarning,"No images to Copy",NULL);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    for (next=image; next; next=next->next)
    {
      image=CloneImage(next,0,0,True,&next->exception);
      if (!image)
        {
          MagickWarning(next->exception.severity,next->exception.reason,
            next->exception.description);
          continue;
        }
      sv=newSViv((IV) image);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    info=GetPackageInfo((void *) av,info);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   D e s t r o y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
DESTROY(ref)
  Image::Magick ref=NO_INIT
  PPCODE:
  {
    SV
      *reference;

    if (!sv_isobject(ST(0)))
      croak("reference is not my type");
    reference=SvRV(ST(0));
    switch (SvTYPE(reference))
    {
      case SVt_PVAV:
      {
        char
          message[MaxTextExtent];

        struct PackageInfo
          *info;

        SV
          *sv;

        /*
          Array (AV *) reference
        */
        FormatString(message,"%s::A_%lx_Z",PackageName,(long) reference);
        sv=perl_get_sv(message,FALSE);
        if (sv)
          {
            if ((SvREFCNT(sv) == 1) && SvIOK(sv) &&
                (info=(struct PackageInfo *) SvIV(sv)))
              {
                DestroyPackageInfo(info);
                sv_setiv(sv,0);
              }
          }
        break;
      }
      case SVt_PVMG:
      {
        Image
          *image;

        /*
          Blessed scalar = (Image *) SvIV(reference)
        */
        image=(Image *) SvIV(reference);
        if (image)
          {
            DestroyImage(image);
            sv_setiv(reference,0);
          }
        break;
      }
      default:
        break;
    }
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   D i s p l a y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Display(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    DisplayImage  = 1
    display       = 2
    displayimage  = 3
  PPCODE:
  {
    jmp_buf
      error_jmp;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    volatile int
      status;

    status=0;
    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!(image=SetupList(reference,&info,(SV ***) NULL)))
      {
        MagickWarning(OptionWarning,"No images to display",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"server",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,image,SvPV(ST(i-1),na),ST(i));
    DisplayImages(package_info->image_info,image);

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) status);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   F l a t t e n                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Flatten(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    FlattenImage   = 1
    flatten        = 2
    flattenimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    ExceptionInfo
      *exception;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to flatten",NULL);
        goto MethodException;
      }
    exception=(&image->exception);
    image=FlattenImages(image,exception);
    if (!image)
      {
        MagickWarning(exception->severity,exception->reason,
          exception->description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) image);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    FormatString(info->image_info->filename,"average-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    (void) strncpy(image->filename,info->image_info->filename,MaxTextExtent-1);
    SetImageInfo(info->image_info,False,&image->exception);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   G e t                                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Get(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    GetAttributes = 1
    GetAttribute  = 2
    get           = 3
    getattributes = 4
    getattribute  = 5
  PPCODE:
  {
    char
      *attribute,
      color[MaxTextExtent];

    const ImageAttribute
      *image_attribute;

    Image
      *image;

    int
      j;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,
      *s;

    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        XSRETURN_EMPTY;
      }
    reference=SvRV(ST(0));
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image && !info)
      {
        MagickWarning(OptionWarning,"Nothing to get attributes from",NULL);
        XSRETURN_EMPTY;
      }
    EXTEND(sp,items);
    for (i=1; i < items; i++)
    {
      attribute=(char *) SvPV(ST(i),na);
      s=NULL;
      switch (*attribute)
      {
        case 'A':
        case 'a':
        {
          if (LocaleCompare(attribute,"adjoin") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->adjoin);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"antialias") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->antialias);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'B':
        case 'b':
        {
          if (LocaleCompare(attribute,"background") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->background_color.red,
                image->background_color.green,image->background_color.blue,
                image->background_color.opacity);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"base-columns") == 0)
            {
              if (image)
                s=newSViv((long) image->magick_columns);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"base-filename") == 0)
            {
              if (image)
                s=newSVpv(image->magick_filename,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"base-height") == 0)
            {
              if (image)
                s=newSViv((long) image->magick_rows);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"base-rows") == 0)
            {
              if (image)
                s=newSViv((long) image->magick_rows);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"base-width") == 0)
            {
              if (image)
                s=newSViv((long) image->magick_columns);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"blue-primary") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.blue_primary.x,
                image->chromaticity.blue_primary.y);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"bordercolor") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->border_color.red,
                image->border_color.green,image->border_color.blue,
                image->border_color.opacity);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (LocaleCompare(attribute,"comment") == 0)
            {
              const ImageAttribute
                *attribute;

              attribute=GetImageAttribute(image,"comment");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"class") == 0)
            {
              if (!image)
                break;
#if defined(__cplusplus) || defined(c_plusplus)
              j=image->c_class;
#else
              j=image->storage_class;
#endif
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(ClassTypes)-1))
                {
                  (void) sv_setpv(s,ClassTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"compression") == 0)
            {
              j=info ? info->image_info->compression : image->compression;
              if (info)
                if (info->image_info->compression == UndefinedCompression)
                  j=image->compression;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(CompressionTypes)-1))
                {
                  (void) sv_setpv(s,CompressionTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"colorspace") == 0)
            {
              j=image ? image->colorspace : RGBColorspace;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(ColorspaceTypes)-1))
                {
                  (void) sv_setpv(s,ColorspaceTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"colors") == 0)
            {
              if (image)
                s=newSViv((long) GetNumberColors(image,(FILE *) NULL,
                  &image->exception));
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleNCompare(attribute,"colormap",8) == 0)
            {
              if (!image || !image->colormap)
                break;
              j=0;
              (void) sscanf(attribute,"%*[^[][%d",&j);
              if (j > (int) image->colors)
                j%=image->colors;
              FormatString(color,"%u,%u,%u,%u",image->colormap[j].red,
                image->colormap[j].green,image->colormap[j].blue,
                image->colormap[j].opacity);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"columns") == 0)
            {
              if (image)
                s=newSViv((long) image->columns);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'D':
        case 'd':
        {
          if (LocaleCompare(attribute,"density") == 0)
            {
              char
                geometry[MaxTextExtent];

              if (!image)
                break;
              FormatString(geometry,"%gx%g",image->x_resolution,
                image->y_resolution);
              s=newSVpv(geometry,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"dispose") == 0)
            {
              if (image)
                s=newSViv((long) image->dispose);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"delay") == 0)
            {
              if (image)
                s=newSViv((long) image->delay);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"depth") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->depth);
              if (image)
                s=newSViv((long) GetImageDepth(image,&image->exception));
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"dither") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->dither);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"display") == 0)  /* same as server */
            {
              if (info && info->image_info->server_name)
                s=newSVpv(info->image_info->server_name,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"directory") == 0)
            {
              if (image && image->directory)
                s=newSVpv(image->directory,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'E':
        case 'e':
        {
          if (LocaleCompare(attribute,"error") == 0)
            {
              if (image)
                s=newSVnv(image->mean_error_per_pixel);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(attribute,"filesize") == 0)
            {
              if (image)
                s=newSViv((long) GetBlobSize(image));
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"filename") == 0)
            {
              if (image)
                s=newSVpv(image->filename,0);
              else
                if (info && info->image_info->filename &&
                    *info->image_info->filename)
                  s=newSVpv(info->image_info->filename,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"filter") == 0)
            {
              j=image->filter;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(FilterTypess)-1))
                {
                  (void) sv_setpv(s,FilterTypess[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"font") == 0)
            {
              if (info && info->image_info->font)
                s=newSVpv(info->image_info->font,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"format") == 0)
            {
              ExceptionInfo
                exception;

              const MagickInfo
                *magick_info;

              magick_info=(const MagickInfo *) NULL;
              if (info && (*info->image_info->magick != '\0'))
                {
                  GetExceptionInfo(&exception);
                  magick_info=
                    GetMagickInfo(info->image_info->magick,&exception);
                  DestroyExceptionInfo(&exception);
                }
              else
                if (image)
                  magick_info=GetMagickInfo(image->magick,&image->exception);
                if ((magick_info != (const MagickInfo *) NULL) &&
                    (*magick_info->description != '\0'))
                  s=newSVpv((char *) magick_info->description,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"fuzz") == 0)
            {
              if (info)
                s=newSVnv(info->image_info->fuzz);
              else
                if (image)
                  s=newSVnv(image->fuzz);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (LocaleCompare(attribute,"gamma") == 0)
            {
              if (image)
                s=newSVnv(image->gamma);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"geometry") == 0)
            {
              if (image && image->geometry)
                s=newSVpv(image->geometry,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"gravity") == 0)
            {
              j=image->gravity;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(GravityTypes)-1))
                {
                  (void) sv_setpv(s,GravityTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"green-primary") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.green_primary.x,
                image->chromaticity.green_primary.y);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(attribute,"height") == 0)
            {
              if (image)
                s=newSViv((long) image->rows);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'I':
        case 'i':
        {
          if (LocaleCompare(attribute,"icm") == 0)
            {
              if (image)
                s=newSVpv((void *) image->color_profile.info,
                  image->color_profile.length);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"id") == 0)
            {
              if (image)
                s=newSViv(SetMagickRegistry(ImageRegistryType,image,0,
                  &image->exception));
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleNCompare(attribute,"index",5) == 0)
            {
              char
                name[MaxTextExtent];

              IndexPacket
                *indexes;

              long
                x,
                y;

              if (!image)
                break;
              if (image->storage_class != PseudoClass)
                break;
              x=0;
              y=0;
              (void) sscanf(attribute,"%*[^[][%ld%*[, ]%ld",&x,&y);
              (void) AcquireOnePixel(image,(long) (x % image->columns),
                (long) (y % image->rows),&image->exception);
              indexes=GetIndexes(image);
              FormatString(name,"%u",*indexes);
              s=newSVpv(name,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"iptc") == 0)
            {
              if (image)
                s=newSVpv((void *) image->iptc_profile.info,
                  image->iptc_profile.length);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"iterations") == 0)  /* same as loop */
            {
              if (image)
                s=newSViv((long) image->iterations);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"interlace") == 0)
            {
              j=info ? info->image_info->interlace : image->interlace;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(InterlaceTypes)-1))
                {
                  (void) sv_setpv(s,InterlaceTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'L':
        case 'l':
        {
          if (LocaleCompare(attribute,"label") == 0)
            {
              const ImageAttribute
                *attribute;

              if (!image)
                break;
              attribute=GetImageAttribute(image,"label");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"loop") == 0)  /* same as iterations */
            {
              if (image)
                s=newSViv((long) image->iterations);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (LocaleCompare(attribute,"magick") == 0)
            {
              if (info && *info->image_info->magick)
                s=newSVpv(info->image_info->magick,0);
              else
                if (image)
                  s=newSVpv(image->magick,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"maximum-error") == 0)
            {
              if (image)
                s=newSVnv(image->normalized_maximum_error);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"mean-error") == 0)
            {
              if (image)
                s=newSVnv(image->normalized_mean_error);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"monochrome") == 0)
            {
              if (!image)
                continue;
              j=info ? info->image_info->monochrome :
                IsMonochromeImage(image,&image->exception);
              s=newSViv(j);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"mattecolor") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->matte_color.red,
                image->matte_color.green,image->matte_color.blue,
                image->matte_color.opacity);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"matte") == 0)
            {
              if (image)
                s=newSViv((long) image->matte);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"montage") == 0)
            {
              if (image && image->montage)
                s=newSVpv(image->montage,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (LocaleCompare(attribute,"page") == 0)
            {
              if (info && info->image_info->page)
                s=newSVpv(info->image_info->page,0);
              else
                if (image)
                  {
                    char
                      geometry[MaxTextExtent];

                    FormatString(geometry,"%ux%u%+d%+d",image->page.width,
                      image->page.height,image->page.x,image->page.y);
                    s=newSVpv(geometry,0);
                  }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleNCompare(attribute,"pixel",5) == 0)
            {
              char
                name[MaxTextExtent];

              long
                x,
                y;

              PixelPacket
                pixel;

              if (!image)
                break;
              x=0;
              y=0;
              (void) sscanf(attribute,"%*[^[][%ld%*[, ]%ld",&x,&y);
              pixel=AcquireOnePixel(image,(long) (x % image->columns),
                (long) (y % image->rows),&image->exception);
              FormatString(name,"%u,%u,%u,%u",pixel.red,pixel.green,pixel.blue,
                pixel.opacity);
              s=newSVpv(name,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"pointsize") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->pointsize);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"preview") == 0)
            {
              s=newSViv(info->image_info->preview_type);
              if ((info->image_info->preview_type >= 0) &&
                  (info->image_info->preview_type < (int) NumberOf(PreviewTypes)-1))
                {
                  (void) sv_setpv(s,
                    PreviewTypes[info->image_info->preview_type]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'Q':
        case 'q':
        {
          if (LocaleCompare(attribute,"quality") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->quality);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (LocaleCompare(attribute,"rendering-intent") == 0)
            {
              j=image->rendering_intent;
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(IntentTypes)-1))
                {
                  (void) sv_setpv(s,IntentTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"red-primary") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.red_primary.x,
                image->chromaticity.red_primary.y);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"rows") == 0)
            {
              if (image)
                s=newSViv((long) image->rows);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (LocaleCompare(attribute,"subimage") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->subimage);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"subrange") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->subrange);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"server") == 0)  /* same as display */
            {
              if (info && info->image_info->server_name)
                s=newSVpv(info->image_info->server_name,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"size") == 0)
            {
              if (info && info->image_info->size)
                s=newSVpv(info->image_info->size,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"scene") == 0)
            {
              if (image)
                s=newSViv((long) image->scene);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"signature") == 0)
            {
              const ImageAttribute
                *attribute;

              if (!image)
                break;
              (void) SignatureImage(image);
              attribute=GetImageAttribute(image,"signature");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (LocaleCompare(attribute,"taint") == 0)
            {
              if (image)
                s=newSViv((long) IsTaintImage(image));
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"temporary") == 0)
            {
              if (image)
                s=newSViv((long) image->temporary);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"tile") == 0)
            {
              if (info && info->image_info->tile)
                s=newSVpv(info->image_info->tile,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"texture") == 0)
            {
              if (info && info->image_info->texture)
                s=newSVpv(info->image_info->texture,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"type") == 0)
            {
              if (!image)
                break;
              j=(int) GetImageType(image,&image->exception);
              s=newSViv(j);
              if ((j >= 0) && (j < (int) NumberOf(ImageTypes)-1))
                {
                  (void) sv_setpv(s,ImageTypes[j]);
                  SvIOK_on(s);
                }
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'U':
        case 'u':
        {
          if (LocaleCompare(attribute,"units") == 0)
            {
              j=info ? info->image_info->units : image->units;
              if (info)
                if (info->image_info->units == UndefinedResolution)
                  j=image->units;
              if (j == UndefinedResolution)
                s=newSVpv("undefined units",0);
              else
                if (j == PixelsPerInchResolution)
                  s=newSVpv("pixels / inch",0);
                else
                  s=newSVpv("pixels / centimeter",0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'V':
        case 'v':
        {
          if (LocaleCompare(attribute,"verbose") == 0)
            {
              if (info)
                s=newSViv((long) info->image_info->verbose);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"view") == 0)
            {
              if (info && info->image_info->view)
                s=newSVpv(info->image_info->view,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(attribute,"white-point") == 0)
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.white_point.x,
                image->chromaticity.white_point.y);
              s=newSVpv(color,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          if (LocaleCompare(attribute,"width") == 0)
            {
              if (image)
                s=newSViv((long) image->columns);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (LocaleCompare(attribute,"x-resolution") == 0)
            {
              if (image)
                s=newSVnv(image->x_resolution);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (LocaleCompare(attribute,"y-resolution") == 0)
            {
              if (image)
                s=newSVnv(image->y_resolution);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
              continue;
            }
          break;
        }
        default:
          break;
      }
      if (image == (Image *) NULL)
        MagickWarning(OptionWarning,"Invalid attribute",attribute);
      else
        {
          image_attribute=GetImageAttribute(image,attribute);
          if (image_attribute == (ImageAttribute *) NULL)
            MagickWarning(OptionWarning,"Invalid attribute",attribute);
          else
            {
              s=newSVpv(image_attribute->value,0);
              PUSHs(s ? sv_2mortal(s) : &sv_undef);
            }
        }
    }
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   I m a g e T o B l o b                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
ImageToBlob(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    ImageToBlob  = 1
    imagetoblob  = 2
    toblob       = 3
    blob         = 4
  PPCODE:
  {
    char
      filename[MaxTextExtent];

    Image
      *image,
      *next;

    int
      scene;

    register int
      i;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info,
      *package_info;

    size_t
      length;

    SV
      *reference;

    void
      *blob;

    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to blob",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    for (i=2; i < items; i+=2)
      SetAttribute(package_info,image,SvPV(ST(i-1),na),ST(i));
    (void) strncpy(filename,package_info->image_info->filename,MaxTextExtent-1);
    scene=0;
    for (next=image; next; next=next->next)
    {
      (void) strncpy(next->filename,filename,MaxTextExtent-1);
      next->scene=scene++;
    }
    SetImageInfo(package_info->image_info,True,&image->exception);
    EXTEND(sp,(int) GetImageListSize(image));
    for (next=image; next; next=next->next)
    {
      length=0;
      blob=ImageToBlob(package_info->image_info,next,&length,&next->exception);
      if (blob == (void *) NULL)
        MagickWarning(next->exception.severity,next->exception.reason,
          next->exception.description);
      if (blob != (char *) NULL)
        {
          PUSHs(sv_2mortal(newSVpv(blob,length)));
          LiberateMemory((void **) &blob);
        }
      if (package_info->image_info->adjoin)
        break;
    }

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    SvREFCNT_dec(error_list);  /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o g r i f y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Mogrify(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    Comment            =   1
    CommentImage       =   2
    Label              =   3
    LabelImage         =   4
    AddNoise           =   5
    AddNoiseImage      =   6
    Colorize           =   7
    ColorizeImage      =   8
    Border             =   9
    BorderImage        =  10
    Blur               =  11
    BlurImage          =  12
    Chop               =  13
    ChopImage          =  14
    Crop               =  15
    CropImage          =  16
    Despeckle          =  17
    DespeckleImage     =  18
    Edge               =  19
    EdgeImage          =  20
    Emboss             =  21
    EmbossImage        =  22
    Enhance            =  23
    EnhanceImage       =  24
    Flip               =  25
    FlipImage          =  26
    Flop               =  27
    FlopImage          =  28
    Frame              =  29
    FrameImage         =  30
    Implode            =  31
    ImplodeImage       =  32
    Magnify            =  33
    MagnifyImage       =  34
    MedianFilter       =  35
    MedianFilterImage  =  36
    Minify             =  37
    MinifyImage        =  38
    OilPaint           =  39
    OilPaintImage      =  40
    ReduceNoise        =  41
    ReduceNoiseImage   =  42
    Roll               =  43
    RollImage          =  44
    Rotate             =  45
    RotateImage        =  46
    Sample             =  47
    SampleImage        =  48
    Scale              =  49
    ScaleImage         =  50
    Shade              =  51
    ShadeImage         =  52
    Sharpen            =  53
    SharpenImage       =  54
    Shear              =  55
    ShearImage         =  56
    Spread             =  57
    SpreadImage        =  58
    Swirl              =  59
    SwirlImage         =  60
    Resize             =  61
    ResizeImage        =  62
    Zoom               =  63
    ZoomImage          =  64
    Annotate           =  65
    AnnotateImage      =  66
    ColorFloodfill     =  67
    ColorFloodfillImage=  68
    Composite          =  69
    CompositeImage     =  70
    Contrast           =  71
    ContrastImage      =  72
    CycleColormap      =  73
    CycleColormapImage =  74
    Draw               =  75
    DrawImage          =  76
    Equalize           =  77
    EqualizeImage      =  78
    Gamma              =  79
    GammaImage         =  80
    Map                =  81
    MapImage           =  82
    MatteFloodfill     =  83
    MatteFloodfillImage=  84
    Modulate           =  85
    ModulateImage      =  86
    Negate             =  87
    NegateImage        =  88
    Normalize          =  89
    NormalizeImage     =  90
    NumberColors       =  91
    NumberColorsImage  =  92
    Opaque             =  93
    OpaqueImage        =  94
    Quantize           =  95
    QuantizeImage      =  96
    Raise              =  97
    RaiseImage         =  98
    Segment            =  99
    SegmentImage       = 100
    Signature          = 101
    SignatureImage     = 102
    Solarize           = 103
    SolarizeImage      = 104
    Sync               = 105
    SyncImage          = 106
    Texture            = 107
    TextureImage       = 108
    Sans               = 109
    SansImage          = 110
    Transparent        = 111
    TransparentImage   = 112
    Threshold          = 113
    ThresholdImage     = 114
    Charcoal           = 115
    CharcoalImage      = 116
    Trim               = 117
    TrimImage          = 118
    Wave               = 119
    WaveImage          = 120
    Channel            = 121
    ChannelImage       = 122
    Stereo             = 125
    StereoImage        = 126
    Stegano            = 127
    SteganoImage       = 128
    Deconstruct        = 129
    DeconstructImage   = 130
    GaussianBlur       = 131
    GaussianBlurImage  = 132
    Convolve           = 133
    ConvolveImage      = 134
    Profile            = 135
    ProfileImage       = 136
    UnsharpMask        = 137
    UnsharpMaskImage   = 138
    MotionBlur         = 139
    MotionBlurImage    = 140
    OrderedDither      = 141
    OrderedDitherImage = 142
    Shave              = 143
    ShaveImage         = 144
    Level              = 145
    LevelImage         = 146
    Clip               = 147
    ClipImage          = 148
    MogrifyRegion      = 666
  PPCODE:
  {
    AffineMatrix
      affine,
      current;

    char
      *attribute,
      absolute_geometry[MaxTextExtent],
      attribute_flag[MaxArguments],
      message[MaxTextExtent],
      *value;

    double
      angle;

    ExceptionInfo
      *exception;

    FrameInfo
      frame_info;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next,
      *region_image;

    int
      base,
      flags,
      j,
      k,
      y;

    PixelPacket
      fill_color;

    RectangleInfo
      geometry,
      region_info;

    register int
      i,
      x;

    struct PackageInfo
      *info;

    struct Methods
      *rp;

    SV
      **pv,
      *reference,
      **reference_vector;

    union ArgumentList
      argument_list[MaxArguments];

    volatile int
      number_images;

    reference_vector=NULL;
    region_image=NULL;
    number_images=0;
    base=2;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    region_info.width=0;
    region_info.height=0;
    region_info.x=0;
    region_info.y=0;
    region_image=(Image *) NULL;
    image=SetupList(reference,&info,&reference_vector);
    if (ix && (ix != 666))
      {
        /*
          Called as Method(...)
        */
        ix=(ix+1)/2;
        rp=(&Methods[ix-1]);
        attribute=rp->name;
      }
    else
      {
        /*
          Called as Mogrify("Method",...)
        */
        attribute=(char *) SvPV(ST(1),na);
        if (ix)
          {
            flags=GetImageGeometry(image,attribute,False,&region_info);
            attribute=(char *) SvPV(ST(2),na);
            base++;
          }
        for (rp=Methods; ; rp++)
        {
          if (rp >= EndOf(Methods))
            {
              MagickWarning(OptionWarning,"Invalid magick method",attribute);
              goto ReturnIt;
            }
          if (strEQcase(attribute,rp->name))
            break;
        }
        ix=rp-Methods+1;
        base++;
      }
    if (!image)
      {
        MagickWarning(OptionWarning,"no images to mogrify",attribute);
        goto ReturnIt;
      }
    Zero(&argument_list,NumberOf(argument_list),union ArgumentList);
    Zero(&attribute_flag,NumberOf(attribute_flag),char);
    for (i=base; (i < items) || ((i == items) && (base == items)); i+=2)
    {
      int
        longest;

      Arguments
        *pp,
        *qq;

      union ArgumentList
        *al;

      SV
        *sv;

      longest=0;
      pp=rp->arguments,
      qq=rp->arguments;
      if (i == items)
        sv=ST(i-1);
      else
        for (sv=ST(i), attribute=(char *) SvPV(ST(i-1),na); ; qq++)
        {
          if ((qq >= EndOf(rp->arguments)) || (qq->method == NULL))
            {
              if (longest > 0)
                break;
              goto continue_outer_loop;
            }
          if (strEQcase(attribute,qq->method) > longest)
            {
              pp=qq;
              longest=strEQcase(attribute,qq->method);
            }
        }
      al=(&argument_list[pp-rp->arguments]);
      if (pp->type == IntegerReference)
        al->int_reference=SvIV(sv);
      else
        if (pp->type == StringReference)
          al->string_reference=(char *) SvPV(sv,al->length);
        else
          if (pp->type == DoubleReference)
            al->double_reference=SvNV(sv);
          else
            if (pp->type == ImageReference)
              {
                if (!sv_isobject(sv) ||
                    !(al->image_reference=SetupList(SvRV(sv),
                     (struct PackageInfo **) NULL,(SV ***) NULL)))
                  {
                    MagickWarning(OptionWarning,"Reference is not my type",
                      PackageName);
                    goto ReturnIt;
                  }
              }
            else
              if (pp->type == ArrayReference)
                al->array_reference=SvRV(sv);
              else
                if (!SvPOK(sv))  /* not a string; just get number */
                  al->int_reference=SvIV(sv);
                else
                  {
                    /*
                      Is a string; look up name.
                    */
                    al->int_reference=LookupStr(pp->type,SvPV(sv,na));
                    if ((al->int_reference < 0) &&
                        ((al->int_reference=SvIV(sv)) <= 0))
                      {
                        FormatString(message,"invalid %.60s value",pp->method);
                        MagickWarning(OptionWarning,message,attribute);
                        goto continue_outer_loop;
                      }
                  }
      attribute_flag[pp-rp->arguments]++;
      continue_outer_loop: ;
    }
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    (void) memset((char *) &fill_color,0,sizeof(PixelPacket));
    pv=reference_vector;
    for (next=image; next; next=next->next)
    {
      image=next;
      exception=(&image->exception);
      SetGeometry(image,&geometry);
      if ((region_info.width*region_info.height) != 0)
        {
          region_image=image;
          image=CropImage(image,&region_info,exception);
          if (!image)
            {
              MagickWarning(exception->severity,exception->reason,
                exception->description);
              continue;
            }
        }
      switch (ix)
      {
        default:
        {
          FormatString(message,"%d",(int) ix);
          MagickWarning(OptionWarning,"Routine value out of range",message);
          goto ReturnIt;
        }
        case 1:  /* Comment */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=(char *) NULL;
          (void) SetImageAttribute(image,"comment",(char *) NULL);
          (void) SetImageAttribute(image,"comment",
            argument_list[0].string_reference);
          break;
        }
        case 2:  /* Label */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=(char *) NULL;
          (void) SetImageAttribute(image,"label",(char *) NULL);
          (void) SetImageAttribute(image,"label",
            argument_list[0].string_reference);
          break;
        }
        case 3:  /* AddNoise */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=UniformNoise;
          image=AddNoiseImage(image,(NoiseType) argument_list[0].int_reference,
            exception);
          break;
        }
        case 4:  /* Colorize */
        {
          PixelPacket
            target;

          target=AcquireOnePixel(image,0,0,&image->exception);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          if (!attribute_flag[1])
            argument_list[1].string_reference=AllocateString("100");
          image=ColorizeImage(image,argument_list[1].string_reference,target,
            exception);
          break;
        }
        case 5:  /* Border */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            QueryColorDatabase(argument_list[3].string_reference,&fill_color);
          if (attribute_flag[4])
            QueryColorDatabase(argument_list[4].string_reference,&fill_color);
          if (attribute_flag[3] || attribute_flag[4])
            image->border_color=fill_color;
          image=BorderImage(image,&geometry,exception);
          break;
        }
        case 6:  /* Blur */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=BlurImage(image,radius,sigma,exception);
          break;
        }
        case 7:  /* Chop */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            geometry.x=argument_list[3].int_reference;
          if (attribute_flag[4])
            geometry.y=argument_list[4].int_reference;
          image=ChopImage(image,&geometry,exception);
          break;
        }
        case 59:  /* Trim */
        {
          if (attribute_flag[0])
            image->fuzz=argument_list[0].double_reference;
          attribute_flag[0]++;
          argument_list[0].string_reference=AllocateString("0x0");
        }
        case 8:  /* Crop */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            geometry.x=argument_list[3].int_reference;
          if (attribute_flag[4])
            geometry.y=argument_list[4].int_reference;
          image=CropImage(image,&geometry,exception);
          break;
        }
        case 9:  /* Despeckle */
        {
          image=DespeckleImage(image,exception);
          break;
        }
        case 10:  /* Edge */
        {
          double
            radius;

          radius=0.0;
          if (attribute_flag[0])
            radius=argument_list[0].double_reference;
          image=EdgeImage(image,radius,exception);
          break;
        }
        case 11:  /* Emboss */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=EmbossImage(image,radius,sigma,exception);
          break;
        }
        case 12:  /* Enhance */
        {
          image=EnhanceImage(image,exception);
          break;
        }
        case 13:  /* Flip */
        {
          image=FlipImage(image,exception);
          break;
        }
        case 14:  /* Flop */
        {
          image=FlopImage(image,exception);
          break;
        }
        case 15:  /* Frame */
        {
          if (attribute_flag[0])
            {
              flags=GetImageGeometry(image,argument_list[0].string_reference,
                False,&geometry);
              frame_info.width=geometry.width;
              frame_info.height=geometry.height;
              frame_info.outer_bevel=geometry.x;
              frame_info.inner_bevel=geometry.y;
            }
          if (attribute_flag[1])
            frame_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            frame_info.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            frame_info.inner_bevel=argument_list[3].int_reference;
          if (attribute_flag[4])
            frame_info.outer_bevel=argument_list[4].int_reference;
          if (attribute_flag[5])
            QueryColorDatabase(argument_list[5].string_reference,&fill_color);
          if (attribute_flag[6])
            QueryColorDatabase(argument_list[6].string_reference,&fill_color);
          frame_info.x=(long) frame_info.width;
          frame_info.y=(long) frame_info.height;
          frame_info.width=image->columns+2*frame_info.x;
          frame_info.height=image->rows+2*frame_info.y;
          if (attribute_flag[5] || attribute_flag[6])
            image->matte_color=fill_color;
          image=FrameImage(image,&frame_info,exception);
          break;
        }
        case 16:  /* Implode */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.5;
          image=ImplodeImage(image,argument_list[0].double_reference,
            exception);
          break;
        }
        case 17:  /* Magnify */
        {
          image=MagnifyImage(image,exception);
          break;
        }
        case 18:  /* MedianFilter */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=MedianFilterImage(image,argument_list[0].double_reference,
            exception);
          break;
        }
        case 19:  /* Minify */
        {
          image=MinifyImage(image,exception);
          break;
        }
        case 20:  /* OilPaint */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=OilPaintImage(image,argument_list[0].double_reference,
            exception);
          break;
        }
        case 21:  /* ReduceNoise */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=ReduceNoiseImage(image,argument_list[0].double_reference,
            exception);
          break;
        }
        case 22:  /* Roll */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.x=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.y=argument_list[2].int_reference;
          image=RollImage(image,geometry.x,geometry.y,exception);
          break;
        }
        case 23:  /* Rotate */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=90.0;
          if (attribute_flag[1])
             QueryColorDatabase(argument_list[1].string_reference,
               &image->background_color);
          image=RotateImage(image,argument_list[0].double_reference,exception);
          break;
        }
        case 24:  /* Sample */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              True,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          image=SampleImage(image,geometry.width,geometry.height,exception);
          break;
        }
        case 25:  /* Scale */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              True,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          image=ScaleImage(image,geometry.width,geometry.height,exception);
          break;
        }
        case 26:  /* Shade */
        {
          double
            azimuth,
            elevation;

          azimuth=30.0;
          if (attribute_flag[1])
            azimuth=argument_list[1].double_reference;
          elevation=30.0;
          if (attribute_flag[2])
            elevation=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",&azimuth,
              &elevation);
          image=ShadeImage(image,argument_list[3].int_reference,azimuth,
            elevation,exception);
          break;
        }
        case 27:  /* Sharpen */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=SharpenImage(image,radius,sigma,exception);
          break;
        }
        case 28:  /* Shear */
        {
          double
            x_shear,
            y_shear;

          x_shear=45.0;
          y_shear=45.0;
          if (attribute_flag[1])
            x_shear=argument_list[1].double_reference;
          if (attribute_flag[2])
            y_shear=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",&x_shear,
              &y_shear);
          if (attribute_flag[3])
             QueryColorDatabase(argument_list[3].string_reference,
               &image->background_color);
          image=ShearImage(image,x_shear,y_shear,exception);
          break;
        }
        case 29:  /* Spread */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=1;
          image=SpreadImage(image,argument_list[0].int_reference,exception);
          break;
        }
        case 30:  /* Swirl */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=50.0;
          image=SwirlImage(image,argument_list[0].double_reference,exception);
          break;
        }
        case 31:  /* Resize */
        case 32:  /* Zoom */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              True,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          if (!attribute_flag[3])
            argument_list[3].int_reference=(int) LanczosFilter;
          if (!attribute_flag[4])
            argument_list[4].double_reference=1.0;
          image=ResizeImage(image,geometry.width,geometry.height,
            (FilterTypes) argument_list[3].int_reference,
            argument_list[4].double_reference,exception);
          break;
        }
        case 33:  /* Annotate */
        {
          DrawInfo
            *draw_info;

          draw_info=CloneDrawInfo(info ? info->image_info :
            (ImageInfo *) NULL,(DrawInfo *) NULL);
          if (attribute_flag[1])
            (void) CloneString(&draw_info->font,
              argument_list[1].string_reference);
          if (attribute_flag[2])
            draw_info->pointsize=argument_list[2].double_reference;
          if (attribute_flag[3])
            (void) CloneString(&draw_info->density,
              argument_list[3].string_reference);
          if (attribute_flag[0])
            (void) CloneString(&draw_info->text,
              argument_list[0].string_reference);
          if (attribute_flag[4])
            (void) QueryColorDatabase(argument_list[4].string_reference,
              &draw_info->box);
          if (attribute_flag[5])
            (void) QueryColorDatabase(argument_list[5].string_reference,
              &draw_info->stroke);
          if (attribute_flag[6])
            (void) QueryColorDatabase(argument_list[6].string_reference,
              &draw_info->fill);
          if (attribute_flag[7])
            (void) CloneString(&draw_info->geometry,
              argument_list[7].string_reference);
          if (attribute_flag[9] || attribute_flag[10])
            {
              if (!attribute_flag[9])
                argument_list[9].int_reference=0;
              if (!attribute_flag[10])
                argument_list[10].int_reference=0;
              FormatString(message,"%+d%+d",argument_list[9].int_reference,
                argument_list[10].int_reference);
              (void) CloneString(&draw_info->geometry,message);
            }
          if (attribute_flag[11])
            draw_info->gravity=(GravityType) argument_list[11].int_reference;
          for (j=12; j < 17; j++)
          {
            if (!attribute_flag[j])
              continue;
            value=argument_list[j].string_reference;
            angle=argument_list[j].double_reference;
            current=draw_info->affine;
            IdentityAffine(&affine);
            switch (j)
            {
              case 12:
              {
                /*
                  Translate.
                */
                k=sscanf(value,"%lf%*[, ]%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  affine.ty=affine.tx;
                break;
              }
              case 13:
              {
                /*
                  Scale.
                */
                k=sscanf(value,"%lf%*[, ]%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  affine.sy=affine.sx;
                break;
              }
              case 14:
              {
                /*
                  Rotate.
                */
                if (angle == 0.0)
                  break;
                affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 15:
              {
                /*
                  SkewX.
                */
                affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 16:
              {
                /*
                  SkewY.
                */
                affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
            }
            draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
            draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
            draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
            draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
            draw_info->affine.tx=current.sx*affine.tx+current.ry*affine.ty+
              current.tx;
            draw_info->affine.ty=current.rx*affine.tx+current.sy*affine.ty+
              current.ty;
          }
          if (attribute_flag[17])
            draw_info->stroke_width=argument_list[17].int_reference;
          if (attribute_flag[18])
            draw_info->text_antialias=argument_list[18].int_reference != 0;
          if (attribute_flag[19])
            (void) CloneString(&draw_info->family,
              argument_list[19].string_reference);
          if (attribute_flag[20])
            draw_info->style=(StyleType) argument_list[20].int_reference;
          if (attribute_flag[21])
            draw_info->stretch=(StretchType) argument_list[21].int_reference;
          if (attribute_flag[22])
            draw_info->weight=argument_list[22].int_reference;
          AnnotateImage(image,draw_info);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 34:  /* ColorFloodfill */
        {
          DrawInfo
            *draw_info;

          PixelPacket
            target;

          draw_info=CloneDrawInfo(info ? info->image_info : (ImageInfo *) NULL,
            (DrawInfo *) NULL);
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.x=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.y=argument_list[2].int_reference;
          if (attribute_flag[3])
            (void) QueryColorDatabase(argument_list[3].string_reference,
              &draw_info->fill);
          if (attribute_flag[4])
            QueryColorDatabase(argument_list[4].string_reference,&fill_color);
          target=AcquireOnePixel(image,(long) (geometry.x % image->columns),
            (long) (geometry.y % image->rows),&image->exception);
          if (attribute_flag[4])
            target=fill_color;
          if (attribute_flag[5])
            image->fuzz=argument_list[5].double_reference;
          ColorFloodfillImage(image,draw_info,target,geometry.x,geometry.y,
            attribute_flag[4] ? FillToBorderMethod : FloodfillMethod);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 35:  /* Composite */
        {
          char
            composite_geometry[MaxTextExtent];

          CompositeOperator
            compose;

          double
            opacity;

          Image
            *composite_image,
            *rotate_image;

          compose=OverCompositeOp;
          if (attribute_flag[0])
            composite_image=argument_list[0].image_reference;
          else
            {
              MagickWarning(OptionWarning,"Missing image in composite",NULL);
              goto ReturnIt;
            }
          if (attribute_flag[1])
            compose=(CompositeOperator) argument_list[1].int_reference;
          opacity=OpaqueOpacity;
          if (attribute_flag[6])
            opacity=argument_list[6].double_reference;
          if (opacity != OpaqueOpacity)
            SetImageOpacity(composite_image,(unsigned int) opacity);
          if (compose == DissolveCompositeOp)
            {
              register PixelPacket
                *q;

              for (y=0; y < (long) composite_image->rows; y++)
              {
                q=GetImagePixels(composite_image,0,y,
                  composite_image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) composite_image->columns; x++)
                {
                  if (composite_image->matte)
                    q->opacity=(Quantum) ((MaxRGB-q->opacity)*opacity)/100;
                  else
                    q->opacity=(Quantum) (MaxRGB*opacity)/100;
                  q++;
                }
                if (!SyncImagePixels(composite_image))
                  break;
              }
            }
          rotate_image=(Image *) NULL;
          if (attribute_flag[8])
            {
               /*
                 Rotate image.
               */
               rotate_image=RotateImage(composite_image,
                 argument_list[8].double_reference,exception);
               if (rotate_image == (Image *) NULL)
                 break;
            }
          if (attribute_flag[7] && argument_list[7].int_reference)
            {
              /*
                Tile image on background.
              */
              for (y=0; y < (long) image->rows; y+=composite_image->rows)
                for (x=0; x < (long) image->columns; x+=composite_image->columns)
                {
                  if (attribute_flag[8])
                    (void) CompositeImage(image,compose,rotate_image,x,y);
                  else
                    (void) CompositeImage(image,compose,composite_image,x,y);
                  (void) CatchImageException(image);
                }
              if (attribute_flag[8])
                DestroyImage(rotate_image);
              break;
            }
          if (attribute_flag[2])
            flags=GetGeometry(argument_list[2].string_reference,&geometry.x,
              &geometry.y,&geometry.width,&geometry.height);
          if (attribute_flag[3])
            geometry.x=argument_list[3].int_reference;
          if (attribute_flag[4])
            geometry.y=argument_list[4].int_reference;
          if (attribute_flag[5])
            image->gravity=(GravityType) argument_list[5].int_reference;
          /*
            Composite image.
          */
          FormatString(composite_geometry,"%lux%lu%+ld%+ld",
            composite_image->columns,composite_image->rows,geometry.x,
            geometry.y);
          flags=GetImageGeometry(image,composite_geometry,False,&geometry);
          if (!attribute_flag[8])
            CompositeImage(image,compose,composite_image,geometry.x,geometry.y);
          else
            {
              /*
                Rotate image.
              */
              geometry.x-=(long)
                (rotate_image->columns-composite_image->columns)/2;
              geometry.y-=(long) (rotate_image->rows-composite_image->rows)/2;
              CompositeImage(image,compose,rotate_image,geometry.x,geometry.y);
              DestroyImage(rotate_image);
            }
          break;
        }
        case 36:  /* Contrast */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=0;
          ContrastImage(image,argument_list[0].int_reference);
          break;
        }
        case 37:  /* CycleColormap */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=6;
          CycleColormapImage(image,argument_list[0].int_reference);
          break;
        }
        case 38:  /* Draw */
        {
          DrawInfo
            *draw_info;

          draw_info=CloneDrawInfo(info ? info->image_info : (ImageInfo *) NULL,
            (DrawInfo *) NULL);
          (void) CloneString(&draw_info->primitive,"Point");
          if (attribute_flag[0] && (argument_list[0].int_reference > 0))
            (void) CloneString(&draw_info->primitive,
              PrimitiveTypes[argument_list[0].int_reference]);
          if (attribute_flag[1])
            {
              (void) ConcatenateString(&draw_info->primitive," ");
              ConcatenateString(&draw_info->primitive,
                argument_list[1].string_reference);
            }
          if (attribute_flag[2])
            {
              (void) ConcatenateString(&draw_info->primitive," ");
              (void) ConcatenateString(&draw_info->primitive,
                MethodTypes[argument_list[2].int_reference]);
            }
          if (attribute_flag[3])
            (void) QueryColorDatabase(argument_list[3].string_reference,
              &draw_info->stroke);
          if (attribute_flag[4])
            (void) QueryColorDatabase(argument_list[4].string_reference,
              &draw_info->fill);
          if (attribute_flag[5])
            draw_info->stroke_width=argument_list[5].double_reference;
          if (attribute_flag[6])
            (void) CloneString(&draw_info->font,
              argument_list[6].string_reference);
          if (attribute_flag[7])
            (void) QueryColorDatabase(argument_list[7].string_reference,
              &draw_info->border_color);
          for (j=10; j < 15; j++)
          {
            if (!attribute_flag[j])
              continue;
            value=argument_list[j].string_reference;
            angle=argument_list[j].double_reference;
            current=draw_info->affine;
            IdentityAffine(&affine);
            switch (j)
            {
              case 10:
              {
                /*
                  Translate.
                */
                k=sscanf(value,"%lf%*[, ]%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  affine.ty=affine.tx;
                break;
              }
              case 11:
              {
                /*
                  Scale.
                */
                k=sscanf(value,"%lf%*[, ]%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  affine.sy=affine.sx;
                break;
              }
              case 12:
              {
                /*
                  Rotate.
                */
                if (angle == 0.0)
                  break;
                affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 13:
              {
                /*
                  SkewX.
                */
                affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 14:
              {
                /*
                  SkewY.
                */
                affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
            }
            draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
            draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
            draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
            draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
            draw_info->affine.tx=
              current.sx*affine.tx+current.ry*affine.ty+current.tx;
            draw_info->affine.ty=
              current.rx*affine.tx+current.sy*affine.ty+current.ty;
          }
          if (attribute_flag[15])
            draw_info->fill_pattern=
              CloneImage(argument_list[15].image_reference,0,0,True,exception);
          if (attribute_flag[16])
            draw_info->pointsize=argument_list[16].double_reference;
          if (attribute_flag[17])
            {
              draw_info->stroke_antialias=argument_list[17].int_reference != 0;
              draw_info->text_antialias=argument_list[17].int_reference != 0;
            }
          if (attribute_flag[18])
            (void) CloneString(&draw_info->density,
              argument_list[18].string_reference);
          DrawImage(image,draw_info);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 39:  /* Equalize */
        {
          EqualizeImage(image);
          break;
        }
        case 40:  /* Gamma */
        {
          if (!attribute_flag[1])
            argument_list[1].double_reference=1.0;
          if (!attribute_flag[2])
            argument_list[2].double_reference=1.0;
          if (!attribute_flag[3])
            argument_list[3].double_reference=1.0;
          if (!attribute_flag[0])
            {
              FormatString(message,"%g,%g,%g",
                argument_list[1].double_reference,
                argument_list[2].double_reference,
                argument_list[3].double_reference);
              argument_list[0].string_reference=AllocateString(message);
            }
          GammaImage(image,argument_list[0].string_reference);
          break;
        }
        case 41:  /* Map */
        {
          if (!attribute_flag[1])
            argument_list[1].int_reference=1;
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in map",NULL);
              goto ReturnIt;
            }
          (void) MapImages(image,argument_list[0].image_reference,
            argument_list[1].int_reference);
          break;
        }
        case 42:  /* MatteFloodfill */
        {
          PixelPacket
            target;

          unsigned int
            opacity;

          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.x=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.y=argument_list[2].int_reference;
          if (attribute_flag[4])
            QueryColorDatabase(argument_list[4].string_reference,&fill_color);
          opacity=TransparentOpacity;
          if (attribute_flag[3])
            opacity=argument_list[3].int_reference;
          if (!image->matte)
            SetImageOpacity(image,OpaqueOpacity);
          target=AcquireOnePixel(image,(long) (geometry.x % image->columns),
            (long) (geometry.y % image->rows),&image->exception);
          if (attribute_flag[4])
            target=fill_color;
          if (attribute_flag[5])
            image->fuzz=argument_list[5].double_reference;
          MatteFloodfillImage(image,target,opacity,geometry.x,geometry.y,
            attribute_flag[4] ? FillToBorderMethod : FloodfillMethod);
          break;
        }
        case 43:  /* Modulate */
        {
          if (!attribute_flag[1])
            argument_list[1].double_reference=100.0;
          if (!attribute_flag[2])
            argument_list[2].double_reference=100.0;
          if (!attribute_flag[3])
            argument_list[3].double_reference=100.0;
          FormatString(message,"%g,%g,%g",
            argument_list[1].double_reference,
            argument_list[2].double_reference,
            argument_list[3].double_reference);
          if (!attribute_flag[0])
            argument_list[0].string_reference=AllocateString(message);
          ModulateImage(image,argument_list[0].string_reference);
          break;
        }
        case 44:  /* Negate */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=0;
          NegateImage(image,argument_list[0].int_reference);
          break;
        }
        case 45:  /* Normalize */
        {
          NormalizeImage(image);
          break;
        }
        case 46:  /* NumberColors */
          break;
        case 47:  /* Opaque */
        {
          PixelPacket
            fill_color,
            target;

          target=AcquireOnePixel(image,0,0,&image->exception);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          fill_color=AcquireOnePixel(image,0,0,&image->exception);
          if (attribute_flag[1])
            (void) QueryColorDatabase(argument_list[1].string_reference,
              &fill_color);
          if (attribute_flag[2])
            image->fuzz=argument_list[2].double_reference;
          OpaqueImage(image,target,fill_color);
          break;
        }
        case 48:  /* Quantize */
        {
          QuantizeInfo
            quantize_info;

          GetQuantizeInfo(&quantize_info);
          quantize_info.number_colors=
            attribute_flag[0] ? argument_list[0].int_reference : (info ?
            info->quantize_info->number_colors : MaxRGB + 1);
          quantize_info.tree_depth=attribute_flag[1] ?
            argument_list[1].int_reference :
            (info ? info->quantize_info->tree_depth : 8);
          quantize_info.colorspace=(ColorspaceType)
            (attribute_flag[2] ? argument_list[2].int_reference :
            (info? info->quantize_info->colorspace : RGBColorspace));
          quantize_info.dither=attribute_flag[3] ?
            argument_list[3].int_reference :
            (info ? info->quantize_info->dither : False);
          quantize_info.measure_error=attribute_flag[4] ?
            argument_list[4].int_reference :
            (info ? info->quantize_info->measure_error : False);
          if (attribute_flag[5] && argument_list[5].int_reference)
            {
              (void) QuantizeImages(&quantize_info,image);
              goto ReturnIt;
            }
          if ((image->storage_class == DirectClass) ||
              (image->colors > quantize_info.number_colors) ||
              (quantize_info.colorspace == GRAYColorspace))
            (void) QuantizeImage(&quantize_info,image);
          else
            CompressColormap(image);
          break;
        }
        case 49:  /* Raise */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            argument_list[3].int_reference=1;
          RaiseImage(image,&geometry,argument_list[3].int_reference);
          break;
        }
        case 50:  /* Segment */
        {
          ColorspaceType
            colorspace;

          double
            cluster_threshold,
            smoothing_threshold;

          unsigned int
            verbose;

          cluster_threshold=1.0;
          smoothing_threshold=1.5;
          colorspace=RGBColorspace;
          verbose=False;
          if (attribute_flag[1])
            cluster_threshold=argument_list[1].double_reference;
          if (attribute_flag[2])
            smoothing_threshold=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &cluster_threshold,&smoothing_threshold);
          if (attribute_flag[3])
            colorspace=(ColorspaceType) argument_list[3].int_reference;
          if (attribute_flag[4])
            verbose=argument_list[4].int_reference != 0;
          (void) SegmentImage(image,colorspace,verbose,cluster_threshold,
            smoothing_threshold);
          break;
        }
        case 51:  /* Signature */
        {
          (void) SignatureImage(image);
          break;
        }
        case 52:  /* Solarize */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=50.0;
          SolarizeImage(image,argument_list[0].double_reference);
          break;
        }
        case 53:  /* Sync */
        {
          SyncImage(image);
          break;
        }
        case 54:  /* Texture */
        {
          if (!attribute_flag[0])
            break;
          TextureImage(image,argument_list[0].image_reference);
          break;
        }
        case 55:  /* Sans */
          break;
        case 56:  /* Transparent */
        {
          PixelPacket
            target;

          unsigned int
            opacity;

          target=AcquireOnePixel(image,0,0,&image->exception);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          opacity=TransparentOpacity;
          if (attribute_flag[1])
            opacity=argument_list[1].int_reference;
          if (attribute_flag[2])
            image->fuzz=argument_list[2].double_reference;
          TransparentImage(image,target,opacity);
          break;
        }
        case 57:  /* Threshold */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=(MaxRGB+1)/2.0;
          ThresholdImage(image,argument_list[0].double_reference);
          break;
        }
        case 58:  /* Charcoal */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=CharcoalImage(image,radius,sigma,exception);
          break;
        }
        case 60:  /* Wave */
        {
          double
            amplitude,
            wavelength;

          amplitude=25.0;
          wavelength=150.0;
          if (attribute_flag[1])
            amplitude=argument_list[1].double_reference;
          if (attribute_flag[2])
            wavelength=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &amplitude,&wavelength);
          image=WaveImage(image,amplitude,wavelength,exception);
          break;
        }
        case 61:  /* Channel */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=1;
          ChannelImage(image,(ChannelType) argument_list[0].int_reference);
          break;
        }
        case 63:  /* Stereo */
        {
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in stereo",NULL);
              goto ReturnIt;
            }
          image=StereoImage(image,argument_list[0].image_reference,exception);
          break;
        }
        case 64:  /* Stegano */
        {
          if (!attribute_flag[1])
            argument_list[1].int_reference=0;
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in stegano",NULL);
              goto ReturnIt;
            }
          image->offset=argument_list[1].int_reference;
          image=SteganoImage(image,argument_list[0].image_reference,exception);
          break;
        }
        case 65:  /* Deconstruct */
        {
          image=DeconstructImages(image,exception);
          break;
        }
        case 66:  /* GaussianBlur */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=GaussianBlurImage(image,radius,sigma,exception);
          break;
        }
        case 67:  /* Convolve */
        {
          AV
            *av;

          double
            *kernel;

          unsigned int
            radius;

          if (!attribute_flag[0])
            break;
          av=(AV*) argument_list[0].array_reference;
          radius=(unsigned int) sqrt(av_len(av)+1);
          kernel=(double *) AcquireMemory(radius*radius*sizeof(double));
          for (j=0; j < (av_len(av)+1); j++)
            kernel[j]=(double) SvNV(*(av_fetch(av,j,0)));
          for ( ; j < (int) (radius*radius); j++)
            kernel[j]=0.0;
          image=ConvolveImage(image,radius,kernel,exception);
          LiberateMemory((void **) &kernel);
          break;
        }
        case 68:  /* Profile */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=AllocateString("*");
          if (!attribute_flag[1])
            argument_list[1].string_reference=(char *) NULL;
          (void) ProfileImage(image,argument_list[0].string_reference,
            (const unsigned char *) argument_list[1].string_reference,argument_list[1].length);
          break;
        }
        case 69:  /* UnsharpMask */
        {
          double
            amount,
            radius,
            sigma,
            threshold;

          radius=0.0;
          sigma=1.0;
          amount=1.0;
          threshold=0.05;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[3])
            amount=argument_list[3].double_reference;
          if (attribute_flag[4])
            threshold=argument_list[4].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=UnsharpMaskImage(image,radius,sigma,amount,threshold,exception);
          break;
        }
        case 70:  /* MotionBlur */
        {
          double
            angle,
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          angle=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[3])
            angle=argument_list[3].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=MotionBlurImage(image,radius,sigma,angle,exception);
          break;
        }
        case 71:  /* OrderedDither */
        {
          (void) OrderedDitherImage(image);
          break;
        }
        case 72:  /* Shave */
        {
          if (attribute_flag[0])
            flags=GetImageGeometry(image,argument_list[0].string_reference,
              False,&geometry);
          if (attribute_flag[1])
            geometry.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            geometry.height=argument_list[2].int_reference;
          image=ShaveImage(image,&geometry,exception);
          break;
        }
        case 73:  /* Level */
        {
          if (!attribute_flag[1])
            argument_list[1].double_reference=0.0;
          if (!attribute_flag[2])
            argument_list[2].double_reference=1.0;
          if (!attribute_flag[3])
            argument_list[3].double_reference=MaxRGB;
          if (!attribute_flag[0])
            {
              FormatString(message,"%g,%g,%g",
                argument_list[1].double_reference,
                argument_list[2].double_reference,
                argument_list[3].double_reference);
              argument_list[0].string_reference=AllocateString(message);
            }
          LevelImage(image,argument_list[0].string_reference);
          break;
        }
        case 74:  /* Clip */
        {
          (void) ClipImage(image);
          break;
        }
      }
      if (image == (Image *) NULL)
        MagickWarning(exception->severity,exception->reason,
          exception->description);
      else
        if (next != (Image *) NULL)
          (void) CatchImageException(next);
      if (region_image != (Image *) NULL)
        {
          unsigned int
            status;

          /*
            Composite region.
          */
          status=CompositeImage(region_image,CopyCompositeOp,image,
            region_info.x,region_info.y);
          (void) CatchImageException(region_image);
          DestroyImage(image);
          image=region_image;
        }
      if (image)
        {
          number_images++;
          if (next && (next != image))
            {
              image->next=next->next;
              DestroyImage(next);
            }
          sv_setiv(*pv,(IV) image);
          next=image;
        }
      if (*pv)
        pv++;
    }

  ReturnIt:
    if (reference_vector)
      LiberateMemory((void **) &reference_vector);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o n t a g e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Montage(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    MontageImage  = 1
    montage       = 2
    montageimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute;

    ExceptionInfo
      *exception;

    HV
      *hv;

    Image
      *image,
      *next;

    int
      sp;

    jmp_buf
      error_jmp;

    MontageInfo
      *montage_info;

    PixelPacket
      transparent_color;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      **reference_vector,
      *sv;

    volatile int
      status;

    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to montage",NULL);
        goto MethodException;
      }
    /*
      Get options.
    */
    info=GetPackageInfo((void *) av,info);
    montage_info=CloneMontageInfo(info->image_info,(MontageInfo *) NULL);
    (void) QueryColorDatabase("none",&transparent_color);
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'B':
        case 'b':
        {
          if (LocaleCompare(attribute,"background") == 0)
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->background_color);
              break;
            }
          if (LocaleCompare(attribute,"bordercolor") == 0)
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->border_color);
              break;
            }
          if (LocaleCompare(attribute,"borderwidth") == 0)
            {
              montage_info->border_width=SvIV(ST(i));
              break;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (LocaleCompare(attribute,"compose") == 0)
            {
              sp=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(CompositeTypes,SvPV(ST(i),na));
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid composite type",
                    SvPV(ST(i),na));
                  break;
                }
              for (next=image; next; next=next->next)
                next->compose=(CompositeOperator) sp;
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(attribute,"fill") == 0)
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),&montage_info->fill);
              break;
            }
          if (LocaleCompare(attribute,"font") == 0)
            {
              (void) CloneString(&montage_info->font,SvPV(ST(i),na));
              break;
            }
          if (LocaleCompare(attribute,"frame") == 0)
            {
              char
                *p;

              p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on frame",p);
                  break;
                }
              (void) CloneString(&montage_info->frame,p);
              if (*p == '\0')
                montage_info->frame=(char *) NULL;
              break;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (LocaleCompare(attribute,"geometry") == 0)
            {
              char
                *p;

              p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on geometry",p);
                  break;
                }
             (void) CloneString(&montage_info->geometry,p);
             if (*p == '\0')
               montage_info->geometry=(char *) NULL;
             break;
           }
         if (LocaleCompare(attribute,"gravity") == 0)
           {
             int
               in;

             in=!SvPOK(ST(i)) ? SvIV(ST(i)) :
               LookupStr(GravityTypes,SvPV(ST(i),na));
             if (in < 0)
               {
                 MagickWarning(OptionWarning,"Invalid gravity type",
                   SvPV(ST(i),na));
                 return;
               }
             montage_info->gravity=(GravityType) in;
             for (next=image; next; next=next->next)
               next->gravity=(GravityType) in;
             break;
           }
          break;
        }
        case 'L':
        case 'l':
        {
          if (LocaleCompare(attribute,"label") == 0)
            {
              for (next=image; next; next=next->next)
                (void) SetImageAttribute(next,"label",SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (LocaleCompare(attribute,"mattecolor") == 0)
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->matte_color);
              break;
            }
          if (LocaleCompare(attribute,"mode") == 0)
            {
              int
                in;

              in=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(ModeTypes,SvPV(ST(i),na));
              switch (in)
              {
                default:
                {
                  MagickWarning(OptionWarning,"Invalid mode value",
                    SvPV(ST(i),na));
                  break;
                }
                case FrameMode:
                {
                  (void) CloneString(&montage_info->frame,"15x15+3+3");
                  montage_info->shadow=True;
                  break;
                }
                case UnframeMode:
                {
                  montage_info->frame=(char *) NULL;
                  montage_info->shadow=False;
                  montage_info->border_width=0;
                  break;
                }
                case ConcatenateMode:
                {
                  montage_info->frame=(char *) NULL;
                  montage_info->shadow=False;
                  (void) CloneString(&montage_info->geometry,"+0+0");
                  montage_info->border_width=0;
                }
              }
              break;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (LocaleCompare(attribute,"pointsize") == 0)
            {
              montage_info->pointsize=SvIV(ST(i));
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (LocaleCompare(attribute,"shadow") == 0)
            {
              sp=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(BooleanTypes,SvPV(ST(i),na));
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid shadow type",
                    SvPV(ST(i),na));
                  break;
                }
             montage_info->shadow=sp != 0;
             break;
            }
          if (LocaleCompare(attribute,"stroke") == 0)
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),&montage_info->stroke);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (LocaleCompare(attribute,"texture") == 0)
            {
              (void) CloneString(&montage_info->texture,SvPV(ST(i),na));
              break;
            }
          if (LocaleCompare(attribute,"tile") == 0)
            {
              char *p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on tile",p);
                  break;
                }
              (void) CloneString(&montage_info->tile,p);
              if (*p == '\0')
                montage_info->tile=(char *) NULL;
              break;
            }
          if (LocaleCompare(attribute,"title") == 0)
            {
              (void) CloneString(&montage_info->title,SvPV(ST(i),na));
              break;
            }
          if (LocaleCompare(attribute,"transparent") == 0)
            {
              transparent_color=AcquireOnePixel(image,0,0,&image->exception);
              QueryColorDatabase(SvPV(ST(i),na),&transparent_color);
              for (next=image; next; next=next->next)
                TransparentImage(next,transparent_color,TransparentOpacity);
              break;
            }
          break;
        }
        default:
          MagickWarning(OptionWarning,"Invalid attribute",attribute);
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    exception=(&image->exception);
    image=MontageImages(image,montage_info,exception);
    DestroyMontageInfo(montage_info);
    if (!image)
      {
        MagickWarning(exception->severity,exception->reason,
          exception->description);
        goto MethodException;
      }
    if (transparent_color.opacity != TransparentOpacity)
      for (next=image; next; next=next->next)
        TransparentImage(next,transparent_color,TransparentOpacity);
    (void) SetImageInfo(info->image_info,False,&image->exception);
    for (next=image; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o r p h                                                                 #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Morph(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    MorphImage  = 1
    morph       = 2
    morphimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute;

    ExceptionInfo
      *exception;

    HV
      *hv;

    Image
      *next,
      *image;

    jmp_buf
      error_jmp;

    int
      number_frames;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    av=NULL;
    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to morph",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get attribute.
    */
    number_frames=30;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'F':
        case 'f':
        {
          if (LocaleCompare(attribute,"frames") == 0)
            {
              number_frames=SvIV(ST(i));
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    exception=(&image->exception);
    image=MorphImages(image,number_frames,exception);
    if (!image)
      {
        MagickWarning(exception->severity,exception->reason,
          exception->description);
        goto MethodException;
      }
    for (next=image; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o s a i c                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Mosaic(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    MosaicImage   = 1
    mosaic        = 2
    mosaicimage   = 3
  PPCODE:
  {
    AV
      *av;

    ExceptionInfo
      *exception;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to mosaic",NULL);
        goto MethodException;
      }
    exception=(&image->exception);
    image=MosaicImages(image,exception);
    if (!image)
      {
        MagickWarning(exception->severity,exception->reason,
          exception->description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) image);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    (void) strncpy(image->filename,info->image_info->filename,MaxTextExtent-1);
    SetImageInfo(info->image_info,False,&image->exception);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   P i n g                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Ping(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    PingImage  = 1
    ping       = 2
    pingimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      message[MaxTextExtent];

    ExceptionInfo
      exception;

    Image
      *image;

    int
      count;

    register Image
      *p;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference;  /* reference is the SV* of ref=SvIV(reference) */

    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    count=0;
    GetExceptionInfo(&exception);
    for (i=1; i < items; i++)
    {
      (void) strncpy(info->image_info->filename,(char *) SvPV(ST(i),na),
        MaxTextExtent-1);
      if ((items >= 3) && strEQcase(info->image_info->filename,"filename"))
        continue;
      if ((items >= 3) && strEQcase(info->image_info->filename,"file"))
        {
          i++;
          info->image_info->file=(FILE *) IoIFP(sv_2io(ST(i)));
        }
      if ((items >= 3) && strEQcase(info->image_info->filename,"blob"))
        {
          char
            *blob;

          STRLEN
            length;

          i++;
          blob=(char *) (SvPV(ST(i),length));
          AttachBlob(info->image_info->blob,blob,length);
        }
      image=PingImage(info->image_info,&exception);
      if (image == (Image *) NULL)
        {
          MagickWarning(exception.severity,exception.reason,
            exception.description);
          PUSHs(&sv_undef);
          continue;
        }
      count+=GetImageListSize(image);
      EXTEND(sp,4*count);
      for (p=image ; p != (Image *) NULL; p=p->next)
      {
        FormatString(message,"%u",p->columns);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%u",p->rows);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%lu",(unsigned long) GetBlobSize(p));
        PUSHs(sv_2mortal(newSVpv(message,0)));
        PUSHs(sv_2mortal(newSVpv(p->magick,0)));
      }
      DestroyImage(image);
    }
    DestroyExceptionInfo(&exception);
    info->image_info->file=(FILE *) NULL;
    SvREFCNT_dec(error_list);  /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y C o l o r                                                       #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryColor(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    querycolor = 1
  PPCODE:
  {
    char
      *name,
      message[MaxTextExtent];

    PixelPacket
      color;

    register int
      i;

    error_list=newSVpv("",0);
    if (items == 1)
      {
        const ColorInfo
          *color_info;

        ExceptionInfo
          exception;

        register const ColorInfo
          *p;

        GetExceptionInfo(&exception);
        color_info=GetColorInfo("*",&exception);
        DestroyExceptionInfo(&exception);
        if (color_info == (const ColorInfo *) NULL)
          {
            PUSHs(&sv_undef);
            goto MethodException;
          }
        i=0;
        for (p=color_info; p != (ColorInfo *) NULL; p=p->next)
          i++;
        EXTEND(sp,i);
        for (p=color_info; p != (ColorInfo *) NULL; p=p->next)
        {
          if (p->stealth)
            continue;
          if (p->name == (char *) NULL)
            {
              PUSHs(&sv_undef);
              continue;
            }
          PUSHs(sv_2mortal(newSVpv(p->name,0)));
        }
        goto MethodException;
      }
    EXTEND(sp,4*items);
    for (i=1; i < items; i++)
    {
      name=(char *) SvPV(ST(i),na);
      if (!QueryColorDatabase(name,&color))
        {
          PUSHs(&sv_undef);
          continue;
        }
      FormatString(message,"%u",color.red);
      PUSHs(sv_2mortal(newSVpv(message,0)));
      FormatString(message,"%u",color.green);
      PUSHs(sv_2mortal(newSVpv(message,0)));
      FormatString(message,"%u",color.blue);
      PUSHs(sv_2mortal(newSVpv(message,0)));
      FormatString(message,"%u",color.opacity);
      PUSHs(sv_2mortal(newSVpv(message,0)));
    }

  MethodException:
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y C o l o r N a m e                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryColorname(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    querycolorname = 1
  PPCODE:
  {
    AV
      *av;

    char
      message[MaxTextExtent];

    Image
      *image;

    PixelPacket
      target_color;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference;  /* reference is the SV* of ref=SvIV(reference) */

    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    image=SetupList(reference,&info,(SV ***) NULL);
    EXTEND(sp,items);
    for (i=1; i < items; i++)
    {
      (void) QueryColorDatabase(SvPV(ST(i),na),&target_color);
      (void) QueryColorname(image,&target_color,AllCompliance,message,
        &image->exception);
      PUSHs(sv_2mortal(newSVpv(message,0)));
    }
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y F o n t                                                         #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryFont(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    queryfont = 1
  PPCODE:
  {
    char
      *name,
      message[MaxTextExtent];

    const TypeInfo
      *type_info;

    ExceptionInfo
      exception;

    register int
      i;

    GetExceptionInfo(&exception);
    error_list=newSVpv("",0);
    if (items == 1)
      {
        register const TypeInfo
          *p;

        type_info=GetTypeInfo("*",&exception);
        if (type_info == (const TypeInfo *) NULL)
          {
            PUSHs(&sv_undef);
            goto MethodException;
          }
        i=0;
        for (p=type_info; p != (TypeInfo *) NULL; p=p->next)
          i++;
        EXTEND(sp,i);
        for (p=type_info; p != (TypeInfo *) NULL; p=p->next)
        {
          if (p->stealth)
            continue;
          if (p->name == (char *) NULL)
            {
              PUSHs(&sv_undef);
              continue;
            }
          PUSHs(sv_2mortal(newSVpv(p->name,0)));
        }
        goto MethodException;
      }
    EXTEND(sp,10*items);
    for (i=1; i < items; i++)
    {
      name=(char *) SvPV(ST(i),na);
      type_info=GetTypeInfo(name,&exception);
      if (type_info == (TypeInfo *) NULL)
        {
          PUSHs(&sv_undef);
          continue;
        }
      if (type_info->name == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->name,0)));
      if (type_info->description == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->description,0)));
      if (type_info->family == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->family,0)));
      PUSHs(sv_2mortal(newSVpv(StyleTypes[(int) type_info->style],0)));
      PUSHs(sv_2mortal(newSVpv(StretchTypes[(int) type_info->stretch],0)));
      FormatString(message,"%lu",type_info->weight);
      PUSHs(sv_2mortal(newSVpv(message,0)));
      if (type_info->encoding == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->encoding,0)));
      if (type_info->foundry == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->foundry,0)));
      if (type_info->format == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->format,0)));
      if (type_info->metrics == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->metrics,0)));
      if (type_info->glyphs == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(type_info->glyphs,0)));
    }

  MethodException:
    DestroyExceptionInfo(&exception);
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y F o n t M e t r i c s                                           #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryFontMetrics(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    queryfontmetrics = 1
  PPCODE:
  {
    AffineMatrix
      affine,
      current;

    AV
      *av;

    char
      *attribute,
      message[MaxTextExtent];

    double
      x,
      y;

    DrawInfo
      *draw_info;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference;  /* reference is the SV* of ref=SvIV(reference) */

    TypeMetric
      metrics;

    unsigned int
      status;

    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No image to query",NULL);
        goto MethodException;
      }
    draw_info=CloneDrawInfo(info->image_info,(DrawInfo *) NULL);
    CloneString(&draw_info->text,"");
    current=draw_info->affine;
    IdentityAffine(&affine);
    x=0.0;
    y=0.0;
    EXTEND(sp,7*items);
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'd':
        case 'D':
        {
          if (LocaleCompare(attribute,"density") == 0)
            {
              CloneString(&draw_info->density,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'f':
        case 'F':
        {
          if (LocaleCompare(attribute,"font") == 0)
            {
              CloneString(&draw_info->font,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'g':
        case 'G':
        {
          if (LocaleCompare(attribute,"geometry") == 0)
            {
              CloneString(&draw_info->geometry,SvPV(ST(i),na));
              break;
            }
          if (LocaleCompare(attribute,"gravity") == 0)
            {
              draw_info->gravity=(GravityType)
                LookupStr(GravityTypes,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'p':
        case 'P':
        {
          if (LocaleCompare(attribute,"pointsize") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&draw_info->pointsize);
              break;
            }
          break;
        }
        case 'r':
        case 'R':
        {
          if (LocaleCompare(attribute,"rotate") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf%*[, ]%lf",&affine.rx,
                &affine.ry);
              break;
            }
          break;
        }
        case 's':
        case 'S':
        {
          if (LocaleCompare(attribute,"scale") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf%*[, ]%lf",&affine.sx,
                &affine.sy);
              break;
            }
          if (LocaleCompare(attribute,"skew") == 0)
            {
              double
                x_angle,
                y_angle;

              x_angle=0.0;
              y_angle=0.0;
              (void) sscanf(SvPV(ST(i),na),"%lf%*[, ]%lf",&x_angle,&y_angle);
              affine.ry=tan(DegreesToRadians(fmod(x_angle,360.0)));
              affine.rx=tan(DegreesToRadians(fmod(y_angle,360.0)));
              break;
            }
          break;
        }
        case 't':
        case 'T':
        {
          if (LocaleCompare(attribute,"text") == 0)
            {
              CloneString(&draw_info->text,SvPV(ST(i),na));
              break;
            }
          if (LocaleCompare(attribute,"translate") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf%*[, ]%lf",&affine.tx,
                &affine.ty);
              break;
            }
          break;
        }
        case 'x':
        case 'X':
        {
          if (LocaleCompare(attribute,"x") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&x);
              break;
            }
          break;
        }
        case 'y':
        case 'Y':
        {
          if (LocaleCompare(attribute,"y") == 0)
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&y);
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
    draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
    draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
    draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
    draw_info->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
    draw_info->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
    if (draw_info->geometry == (char *) NULL)
      {
        draw_info->geometry=AllocateString((char *) NULL);
        FormatString(draw_info->geometry,"%g,%g",x,y);
      }
    status=GetTypeMetrics(image,draw_info,&metrics);
    if (status == False)
      PUSHs(&sv_undef);
    else
      {
        FormatString(message,"%g",metrics.pixels_per_em.x);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.pixels_per_em.y);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.ascent);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.descent);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.width);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.height);
        PUSHs(sv_2mortal(newSVpv(message,0)));
        FormatString(message,"%g",metrics.max_advance);
        PUSHs(sv_2mortal(newSVpv(message,0)));
      }
    DestroyDrawInfo(draw_info);

  MethodException:
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y F o r m a t                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryFormat(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    queryformat = 1
  PPCODE:
  {
    char
      message[MaxTextExtent],
      *name;

    const MagickInfo
      *magick_info;

    ExceptionInfo
      exception;

    register int
      i;

    error_list=newSVpv("",0);
    GetExceptionInfo(&exception);
    if (items == 1)
      {
        register const MagickInfo
          *p;

        magick_info=GetMagickInfo("*",&exception);
        if (magick_info == (const MagickInfo *) NULL)
          {
            PUSHs(&sv_undef);
            goto MethodException;
          }
        i=0;
        for (p=magick_info; p != (MagickInfo *) NULL; p=p->next)
          i++;
        EXTEND(sp,i);
        for (p=magick_info; p != (MagickInfo *) NULL; p=p->next)
        {
          if (p->stealth)
            continue;
          if (p->name == (char *) NULL)
            {
              PUSHs(&sv_undef);
              continue;
            }
          (void) strncpy(message,p->name,MaxTextExtent-1);
          LocaleLower(message);
          PUSHs(sv_2mortal(newSVpv(message,0)));
        }
        goto MethodException;
      }
    EXTEND(sp,8*items);
    for (i=1; i < items; i++)
    {
      name=(char *) SvPV(ST(i),na);
      magick_info=GetMagickInfo(name,&exception);
      if (magick_info == (const MagickInfo *) NULL)
        {
          PUSHs(&sv_undef);
          continue;
        }
      PUSHs(sv_2mortal(newSVpv(magick_info->adjoin ? "1" : "0",0)));
      PUSHs(sv_2mortal(newSVpv(magick_info->blob_support ? "1" : "0",0)));
      PUSHs(sv_2mortal(newSVpv(magick_info->raw ? "1" : "0",0)));
      PUSHs(sv_2mortal(newSVpv(magick_info->decoder ? "1" : "0",0)));
      PUSHs(sv_2mortal(newSVpv(magick_info->encoder ? "1" : "0",0)));
      if (magick_info->description == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(magick_info->description,0)));
      if (magick_info->module == (char *) NULL)
        PUSHs(&sv_undef);
      else
        PUSHs(sv_2mortal(newSVpv(magick_info->module,0)));
    }

  MethodException:
    DestroyExceptionInfo(&exception);
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   R e a d                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Read(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    ReadImage  = 1
    read       = 2
    readimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      **keep,
      **list;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *image;

    int
      ac,
      n;

    jmp_buf
      error_jmp;

    register char
      **p;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    unsigned int
      status;

    volatile int
      number_images;

    number_images=0;
    error_list=newSVpv("",0);
    ac=(items < 2) ? 1 : items-1;
    list=(char **) AcquireMemory((ac+1)*sizeof(*list));
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    if (SvTYPE(reference) != SVt_PVAV)
      {
        MagickWarning(OptionWarning,"Unable to read into a single image",NULL);
        goto ReturnIt;
      }
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    n=1;
    if (items <= 1)
      *list=(char *)
        (*info->image_info->filename ? info->image_info->filename : "XC:black");
    else
      for (n=0, i=0; i < ac; i++)
      {
        list[n]=(char *) SvPV(ST(i+1),na);
        if ((items >= 3) && strEQcase(list[n],"filename"))
          continue;
        if ((items >= 3) && strEQcase(list[n],"file"))
          {
            info->image_info->file=(FILE *) IoIFP(sv_2io(ST(i+2)));
            continue;
          }
        n++;
      }
    list[n]=(char *) NULL;
    keep=list;
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    status=ExpandFilenames(&n,&list);
    if (status == False)
      {
        MagickWarning(ResourceLimitWarning,"Memory allocation failed",NULL);
        goto ReturnIt;
      }
    GetExceptionInfo(&exception);
    for (i=number_images=0; i < n; i++)
    {
      (void) strncpy(info->image_info->filename,list[i],MaxTextExtent-1);
      image=ReadImage(info->image_info,&exception);
      if (exception.severity != UndefinedException)
        MagickWarning(exception.severity,exception.reason,
          exception.description);
      for ( ; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
        number_images++;
      }
    }
    info->image_info->file=(FILE *) NULL;
    /*
      Free resources.
    */
    GetExceptionInfo(&exception);
    for (i=0; i < n; i++)
      if (list[i])
        for (p=keep; list[i] != *p++; )
          if (*p == NULL)
            {
              LiberateMemory((void **) &list[i]);
              break;
            }

  ReturnIt:
    LiberateMemory((void **) &list);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   R e m o t e                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Remote(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    RemoteCommand  = 1
    remote         = 2
    remoteCommand  = 3
  PPCODE:
  {
    AV
      *av;

    SV
      *reference;

    struct PackageInfo
      *info;

    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
#if defined(XlibSpecificationRelease)
    {
      Display
        *display;

      register int
        i;

      display=XOpenDisplay(info->image_info->server_name);
      for (i=1; i < items; i++)
        XRemoteCommand(display,(char *) NULL,(char *) SvPV(ST(i),na));
    }
#endif
    SvREFCNT_dec(error_list);    /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   S e t                                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Set(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    SetAttributes  = 1
    SetAttribute   = 2
    set            = 3
    setattributes  = 4
    setattribute   = 5
  PPCODE:
  {
    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference;  /* reference is the SV* of ref=SvIV(reference) */

    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    image=SetupList(reference,&info,(SV ***) NULL);
    for (i=2; i < items; i+=2)
      SetAttribute(info,image,SvPV(ST(i-1),na),ST(i));

  MethodException:
    sv_setiv(error_list,(IV) (SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   T r a n s f o r m                                                         #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Transform(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    TransformImage = 1
    transform      = 2
    transformimage = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute,
      *crop_geometry,
      *geometry;

    HV
      *hv;

    Image
      *clone,
      *next,
      *image;

    jmp_buf
      error_jmp;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    av=NULL;
    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to transform",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get attribute.
    */
    crop_geometry=(char *) NULL;
    geometry=(char *) NULL;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'c':
        case 'C':
        {
          if (LocaleCompare(attribute,"crop") == 0)
            {
              crop_geometry=SvPV(ST(i),na);
              break;
            }
          break;
        }
        case 'g':
        case 'G':
        default:
        {
          if (LocaleCompare(attribute,"geometry") == 0)
            {
              geometry=SvPV(ST(i),na);
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    for (next=image; next; next=next->next)
    {
      clone=CloneImage(next,0,0,True,&next->exception);
      if (!clone)
        {
          MagickWarning(next->exception.severity,next->exception.reason,
            next->exception.description);
          goto MethodException;
        }
      TransformImage(&clone,crop_geometry,geometry);
      for (image=clone; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
      }
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   W r i t e                                                                 #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Write(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    WriteImage    = 1
    write         = 2
    writeimage    = 3
  PPCODE:
  {
    char
      filename[MaxTextExtent];

    Image
      *image,
      *next;

    int
      scene;

    register int
      i;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    volatile int
      number_images;

    number_images=0;
    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to write",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"filename",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,image,SvPV(ST(i-1),na),ST(i));
    (void) strncpy(filename,package_info->image_info->filename,MaxTextExtent-1);
    scene=0;
    for (next=image; next; next=next->next)
    {
      (void) strncpy(next->filename,filename,MaxTextExtent-1);
      next->scene=scene++;
    }
    SetImageInfo(package_info->image_info,True,&image->exception);
    for (next=image; next; next=next->next)
    {
      (void) WriteImage(package_info->image_info,next);
      (void) CatchImageException(next);
      number_images++;
      if (package_info->image_info->adjoin)
        break;
    }
    package_info->image_info->file=(FILE *) NULL;

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }
