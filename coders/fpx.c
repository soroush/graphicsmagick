/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            FFFFF  PPPP   X   X                              %
%                            F      P   P   X X                               %
%                            FFF    PPPP     X                                %
%                            F      P       X X                               %
%                            F      P      X   X                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasFPX)
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#include <fpxlib.h>
#else
#include "Fpxlib.h"
#endif
#endif

/*
  Forward declarations.
*/
static unsigned int
  WriteFPXImage(const ImageInfo *,Image *);

#if defined(HasFPX)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F P X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFPXImage reads a FlashPix image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method was contributed by BillR@corbis.com.
%
%  The format of the ReadFPXImage method is:
%
%      Image *ReadFPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadFPXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadFPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  FILE
    *file;

  FPXColorspace
    colorspace;

  FPXImageComponentDesc
    *alpha_component,
    *blue_component,
    *green_component,
    *red_component;

  FPXImageDesc
    fpx_info;

  FPXImageHandle
    *flashpix;

  FPXStatus
    fpx_status;

  FPXSummaryInformation
    summary_info;

  Image
    *image;

  IndexPacket
    index;

  int
    c;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *a,
    *b,
    *g,
    *r;

  unsigned char
    *scanline;

  unsigned int
    status,
    subimage;

  unsigned long
    height,
    memory_limit,
    tile_width,
    tile_height,
    width;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  /*
    Copy image to temporary file.
  */
  TemporaryFilename((char *) image_info->filename);
  file=fopen(image_info->filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenError,"Unable to write file",image);
  c=ReadBlobByte(image);
  while (c != EOF)
  {
    (void) fputc(c,file);
    c=ReadBlobByte(image);
  }
  (void) fclose(file);
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  CloseBlob(image);
  /*
    Initialize FPX toolkit.
  */
  memory_limit=20000000;
  fpx_status=FPX_SetToolkitMemoryLimit(&memory_limit);
  if (fpx_status != FPX_OK)
    ThrowReaderException(DelegateError,"Unable to initialize FPX library",
      image);
  tile_width=64;
  tile_height=64;
  flashpix=(FPXImageHandle *) NULL;
  {
#if defined(macintosh)
    FSSpec
      fsspec;

    FilenameToFSSpec(image->filename,&fsspec);
    fpx_status=FPX_OpenImageByFilename((const FSSpec &) fsspec,(char *) NULL,
#else
    fpx_status=FPX_OpenImageByFilename(image->filename,(char *) NULL,
#endif
      &width,&height,&tile_width,&tile_height,&colorspace,&flashpix);
  }
  if (fpx_status == FPX_LOW_MEMORY_ERROR)
    {
      FPX_ClearSystem();
      ThrowReaderException(ResourceLimitError,"Memory allocation failed",
        image);
    }
  if (fpx_status != FPX_OK)
    {
      FPX_ClearSystem();
      ThrowReaderException(FileOpenError,"Unable to open FPX file",image);
    }
  if (image_info->view == (char *) NULL)
    {
      float
        aspect_ratio;

      /*
        Get the aspect ratio.
      */
      aspect_ratio=(float) width/height;
      fpx_status=FPX_GetImageResultAspectRatio(flashpix,&aspect_ratio);
      if (fpx_status != FPX_OK)
        ThrowReaderException(DelegateError,"Unable to read aspect ratio",
          image);
      if (width != (unsigned long) ((aspect_ratio*height)+0.5))
        Swap(width,height);
    }
  fpx_status=FPX_GetSummaryInformation(flashpix,&summary_info);
  if (fpx_status != FPX_OK)
    {
      FPX_ClearSystem();
      ThrowReaderException(FileOpenError,"Unable to read summary info",image);
    }
  if (summary_info.title_valid)
    if ((summary_info.title.length != 0) &&
        (summary_info.title.ptr != (unsigned char *) NULL))
      {
        char
          *label;

        /*
          Note image label.
        */
        label=(char *) AcquireMemory(summary_info.title.length+1);
        if (label == (char *) NULL)
          {
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,
              "Memory allocation failed",image);
          }
        (void) strncpy(label,(char *) summary_info.title.ptr,
          summary_info.title.length);
        label[summary_info.title.length]='\0';
        (void) SetImageAttribute(image,"label",label);
        LiberateMemory((void **) &label);
      }
  if (summary_info.comments_valid)
    if ((summary_info.comments.length != 0) &&
        (summary_info.comments.ptr != (unsigned char *) NULL))
      {
        char
          *comments;

        /*
          Note image comment.
        */
        comments=(char *) AcquireMemory(summary_info.comments.length+1);
        if (comments == (char *) NULL)
          {
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,
              "Memory allocation failed",image);
          }
        (void) strncpy(comments,(char *) summary_info.comments.ptr,
          summary_info.comments.length);
        comments[summary_info.comments.length]='\0';
        (void) SetImageAttribute(image,"comment",comments);
        LiberateMemory((void **) &comments);
      }
  /*
    Determine resolution by subimage specification.
  */
  for (i=1; ; i++)
    if (((width >> i) < tile_width) || ((height >> i) < tile_height))
      break;
  subimage=i;
  if (image_info->subrange != 0)
    while (subimage > image_info->subimage)
    {
      width>>=1;
      height>>=1;
      subimage--;
    }
  if (image_info->size != (char *) NULL)
    while ((width > image->columns) || (height > image->rows))
    {
      width>>=1;
      height>>=1;
      subimage--;
    }
  image->depth=8;
  image->columns=width;
  image->rows=height;
  if ((colorspace.numberOfComponents % 2) == 0)
    image->matte=True;
  if (colorspace.numberOfComponents == 1)
    {
      /*
        Create linear colormap.
      */
      if (!AllocateImageColormap(image,MaxRGB+1))
        {
          FPX_ClearSystem();
          ThrowReaderException(ResourceLimitError,"Memory allocation failed",
            image);
        }
    }
  if (image_info->ping)
    {
      (void) FPX_CloseImage(flashpix);
      FPX_ClearSystem();
      (void) remove(image->filename);
      return(image);
    }
  /*
    Allocate memory for the image and pixel buffer.
  */
  scanline=(unsigned char *) AcquireMemory(colorspace.numberOfComponents*
    image->columns*(tile_height+1));
  if (scanline == (unsigned char *) NULL)
    {
      FPX_ClearSystem();
      (void) FPX_CloseImage(flashpix);
      ThrowReaderException(ResourceLimitError,"Memory allocation failed",
        image);
    }
  /*
    Initialize FlashPix image description.
  */
  fpx_info.numberOfComponents=colorspace.numberOfComponents;
  for (i=0; i < 4; i++)
  {
    fpx_info.components[i].myColorType.myDataType=DATA_TYPE_UNSIGNED_BYTE;
    fpx_info.components[i].horzSubSampFactor=1;
    fpx_info.components[i].vertSubSampFactor=1;
    fpx_info.components[i].columnStride=fpx_info.numberOfComponents;
    fpx_info.components[i].lineStride=
      image->columns*fpx_info.components[i].columnStride;
    fpx_info.components[i].theData=scanline+i;
  }
  fpx_info.components[0].myColorType.myColor=
    fpx_info.numberOfComponents > 2 ? NIFRGB_R : MONOCHROME;
  red_component=(&fpx_info.components[0]);
  fpx_info.components[1].myColorType.myColor=
    fpx_info.numberOfComponents > 2 ? NIFRGB_G : ALPHA;
  green_component=(&fpx_info.components[1]);
  fpx_info.components[2].myColorType.myColor=NIFRGB_B;
  blue_component=(&fpx_info.components[2]);
  fpx_info.components[3].myColorType.myColor=ALPHA;
  alpha_component=(&fpx_info.components[fpx_info.numberOfComponents-1]);
  FPX_SetResampleMethod(FPX_LINEAR_INTERPOLATION);
  /*
    Initialize image pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if ((y % tile_height) == 0)
      {
        /*
          Read FPX image tile (with or without viewing affine)..
        */
        if (image_info->view != (char *) NULL)
          fpx_status=FPX_ReadImageRectangle(flashpix,0,y,image->columns,y+
            tile_height-1,subimage,&fpx_info);
        else
          fpx_status=FPX_ReadImageTransformRectangle(flashpix,0.0F,
            (float) y/image->rows,(float) image->columns/image->rows,
            (float) (y+tile_height-1)/image->rows,(long) image->columns,
            (long) tile_height,&fpx_info);
        if (fpx_status == FPX_LOW_MEMORY_ERROR)
          {
            LiberateMemory((void **) &scanline);
            (void) FPX_CloseImage(flashpix);
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,
              "Memory allocation failed",image);
          }
      }
    /*
      Transfer a FPX scanline.
    */
    r=red_component->theData+(y % tile_height)*red_component->lineStride;
    g=green_component->theData+(y % tile_height)*green_component->lineStride;
    b=blue_component->theData+(y % tile_height)*blue_component->lineStride;
    a=alpha_component->theData+(y % tile_height)*alpha_component->lineStride;
    for (x=0; x < (long) image->columns; x++)
    {
      if (fpx_info.numberOfComponents > 2)
        {
          q->red=Upscale(*r);
          q->green=Upscale(*g);
          q->blue=Upscale(*b);
        }
      else
        {
          index=Upscale(*r);
          indexes[x]=index;
          q->red=index;
          q->green=index;
          q->blue=index;
        }
      if (image->matte)
        q->opacity=MaxRGB-Upscale(*a);
      q++;
      r+=red_component->columnStride;
      g+=green_component->columnStride;
      b+=blue_component->columnStride;
      a+=alpha_component->columnStride;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  LiberateMemory((void **) &scanline);
  (void) FPX_CloseImage(flashpix);
  FPX_ClearSystem();
  (void) remove(image->filename);
  return(image);
}
#else
static Image *ReadFPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateError,
    "FPX library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r F P X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterFPXImage adds attributes for the FPX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterFPXImage method is:
%
%      RegisterFPXImage(void)
%
*/
ModuleExport void RegisterFPXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FPX");
  entry->decoder=ReadFPXImage;
  entry->encoder=WriteFPXImage;
  entry->adjoin=False;
  entry->description=AcquireString("FlashPix Format");
  entry->module=AcquireString("FPX");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r F P X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterFPXImage removes format registrations made by the
%  FPX module from the list of supported formats.
%
%  The format of the UnregisterFPXImage method is:
%
%      UnregisterFPXImage(void)
%
*/
ModuleExport void UnregisterFPXImage(void)
{
  (void) UnregisterMagickInfo("FPX");
}

#if defined(HasFPX)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F P X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteFPXImage writes an image in the FlashPix image format.  This
%  method was contributed by BillR@corbis.com.
%
%  The format of the WriteFPXImage method is:
%
%      unsigned int WriteFPXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o fpx_status: Method WriteFPXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static void ColorTwistMultiply(FPXColorTwistMatrix first,
  FPXColorTwistMatrix second,FPXColorTwistMatrix *color_twist)
{
  /*
    Matrix multiply.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  color_twist->byy=(first.byy*second.byy)+(first.byc1*second.bc1y)+
    (first.byc2*second.bc2y)+(first.dummy1_zero*second.dummy4_zero);
  color_twist->byc1=(first.byy*second.byc1)+(first.byc1*second.bc1c1)+
    (first.byc2*second.bc2c1)+(first.dummy1_zero*second.dummy5_zero);
  color_twist->byc2=(first.byy*second.byc2)+(first.byc1*second.bc1c2)+
    (first.byc2*second.bc2c2)+(first.dummy1_zero*second.dummy6_zero);
  color_twist->dummy1_zero=(first.byy*second.dummy1_zero)+
    (first.byc1*second.dummy2_zero)+(first.byc2*second.dummy3_zero)+
    (first.dummy1_zero*second.dummy7_one);
  color_twist->bc1y=(first.bc1y*second.byy)+(first.bc1c1*second.bc1y)+
    (first.bc1c2*second.bc2y)+(first.dummy2_zero*second.dummy4_zero);
  color_twist->bc1c1=(first.bc1y*second.byc1)+(first.bc1c1*second.bc1c1)+
    (first.bc1c2*second.bc2c1)+(first.dummy2_zero*second.dummy5_zero);
  color_twist->bc1c2=(first.bc1y*second.byc2)+(first.bc1c1*second.bc1c2)+
    (first.bc1c2*second.bc2c2)+(first.dummy2_zero*second.dummy6_zero);
  color_twist->dummy2_zero=(first.bc1y*second.dummy1_zero)+
    (first.bc1c1*second.dummy2_zero)+(first.bc1c2*second.dummy3_zero)+
    (first.dummy2_zero*second.dummy7_one);
  color_twist->bc2y=(first.bc2y*second.byy)+(first.bc2c1*second.bc1y)+
    (first.bc2c2*second.bc2y)+(first.dummy3_zero*second.dummy4_zero);
  color_twist->bc2c1=(first.bc2y*second.byc1)+(first.bc2c1*second.bc1c1)+
    (first.bc2c2*second.bc2c1)+(first.dummy3_zero*second.dummy5_zero);
  color_twist->bc2c2=(first.bc2y*second.byc2)+(first.bc2c1*second.bc1c2)+
    (first.bc2c2*second.bc2c2)+(first.dummy3_zero*second.dummy6_zero);
  color_twist->dummy3_zero=(first.bc2y*second.dummy1_zero)+
    (first.bc2c1*second.dummy2_zero)+(first.bc2c2*second.dummy3_zero)+
    (first.dummy3_zero*second.dummy7_one);
  color_twist->dummy4_zero=(first.dummy4_zero*second.byy)+
    (first.dummy5_zero*second.bc1y)+(first.dummy6_zero*second.bc2y)+
    (first.dummy7_one*second.dummy4_zero);
  color_twist->dummy5_zero=(first.dummy4_zero*second.byc1)+
    (first.dummy5_zero*second.bc1c1)+(first.dummy6_zero*second.bc2c1)+
    (first.dummy7_one*second.dummy5_zero);
  color_twist->dummy6_zero=(first.dummy4_zero*second.byc2)+
    (first.dummy5_zero*second.bc1c2)+(first.dummy6_zero*second.bc2c2)+
    (first.dummy7_one*second.dummy6_zero);
  color_twist->dummy7_one=(first.dummy4_zero*second.dummy1_zero)+
    (first.dummy5_zero*second.dummy2_zero)+
    (first.dummy6_zero*second.dummy3_zero)+(first.dummy7_one*second.dummy7_one);
}

static void SetBrightness(double brightness,FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    effect,
    result;

  /*
    Set image brightness in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  brightness=sqrt((double) brightness);
  effect.byy=brightness;
  effect.byc1=0.0;
  effect.byc2=0.0;
  effect.dummy1_zero=0.0;
  effect.bc1y=0.0;
  effect.bc1c1=brightness;
  effect.bc1c2=0.0;
  effect.dummy2_zero=0.0;
  effect.bc2y=0.0;
  effect.bc2c1=0.0;
  effect.bc2c2=brightness;
  effect.dummy3_zero=0.0;
  effect.dummy4_zero=0.0;
  effect.dummy5_zero=0.0;
  effect.dummy6_zero=0.0;
  effect.dummy7_one=1.0;
  ColorTwistMultiply(*color_twist,effect,&result);
  *color_twist=result;
}

static void SetColorBalance(double red,double green,double blue,
  FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    blue_effect,
    green_effect,
    result,
    rgb_effect,
    rg_effect,
    red_effect;

  /*
    Set image color balance in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  red=sqrt((double) red)-1.0;
  green=sqrt((double) green)-1.0;
  blue=sqrt((double) blue)-1.0;
  red_effect.byy=1.0;
  red_effect.byc1=0.0;
  red_effect.byc2=0.299*red;
  red_effect.dummy1_zero=0.0;
  red_effect.bc1y=(-0.299)*red;
  red_effect.bc1c1=1.0-0.299*red;
  red_effect.bc1c2=(-0.299)*red;
  red_effect.dummy2_zero=0.0;
  red_effect.bc2y=0.701*red;
  red_effect.bc2c1=0.0;
  red_effect.bc2c2=1.0+0.402*red;
  red_effect.dummy3_zero=0.0;
  red_effect.dummy4_zero=0.0;
  red_effect.dummy5_zero=0.0;
  red_effect.dummy6_zero=0.0;
  red_effect.dummy7_one=1.0;
  green_effect.byy=1.0;
  green_effect.byc1=(-0.114)*green;
  green_effect.byc2=(-0.299)*green;
  green_effect.dummy1_zero=0.0;
  green_effect.bc1y=(-0.587)*green;
  green_effect.bc1c1=1.0-0.473*green;
  green_effect.bc1c2=0.299*green;
  green_effect.dummy2_zero=0.0;
  green_effect.bc2y=(-0.587)*green;
  green_effect.bc2c1=0.114*green;
  green_effect.bc2c2=1.0-0.288*green;
  green_effect.dummy3_zero=0.0;
  green_effect.dummy4_zero=0.0;
  green_effect.dummy5_zero=0.0;
  green_effect.dummy6_zero=0.0;
  green_effect.dummy7_one=1.0;
  blue_effect.byy=1.0;
  blue_effect.byc1=0.114*blue;
  blue_effect.byc2=0.0;
  blue_effect.dummy1_zero=0.0;
  blue_effect.bc1y=0.886*blue;
  blue_effect.bc1c1=1.0+0.772*blue;
  blue_effect.bc1c2=0.0;
  blue_effect.dummy2_zero=0.0;
  blue_effect.bc2y=(-0.114)*blue;
  blue_effect.bc2c1=(-0.114)*blue;
  blue_effect.bc2c2=1.0-0.114*blue;
  blue_effect.dummy3_zero=0.0;
  blue_effect.dummy4_zero=0.0;
  blue_effect.dummy5_zero=0.0;
  blue_effect.dummy6_zero=0.0;
  blue_effect.dummy7_one=1.0;
  ColorTwistMultiply(red_effect,green_effect,&rg_effect);
  ColorTwistMultiply(rg_effect,blue_effect,&rgb_effect);
  ColorTwistMultiply(*color_twist,rgb_effect,&result);
  *color_twist=result;
}

static void SetSaturation(double saturation,FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    effect,
    result;

  /*
    Set image saturation in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  effect.byy=1.0;
  effect.byc1=0.0;
  effect.byc2=0.0;
  effect.dummy1_zero=0.0;
  effect.bc1y=0.0;
  effect.bc1c1=saturation;
  effect.bc1c2=0.0;
  effect.dummy2_zero=0.0;
  effect.bc2y=0.0;
  effect.bc2c1=0.0;
  effect.bc2c2=saturation;
  effect.dummy3_zero=0.0;
  effect.dummy4_zero=0.0;
  effect.dummy5_zero=0.0;
  effect.dummy6_zero=0.0;
  effect.dummy7_one=1.0;
  ColorTwistMultiply(*color_twist,effect,&result);
  *color_twist=result;
}

static unsigned int WriteFPXImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent];

  FPXBackground
    background_color;

  FPXColorspace
    colorspace =
    {
      TRUE, 4,
      {
        { NIFRGB_R, DATA_TYPE_UNSIGNED_BYTE },
        { NIFRGB_G, DATA_TYPE_UNSIGNED_BYTE },
        { NIFRGB_B, DATA_TYPE_UNSIGNED_BYTE },
        { ALPHA, DATA_TYPE_UNSIGNED_BYTE }
      }
    };

  const ImageAttribute
    *comment,
    *label;

  FPXCompressionOption
    compression;

  FPXImageDesc
    fpx_info;

  FPXImageHandle
    *flashpix;

  FPXStatus
    fpx_status;

  FPXSummaryInformation
    summary_info;

  long
    y;

  register long
    i;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned long
    memory_limit,
    tile_height,
    tile_width;

  /*
    Open input file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  /*
    Initialize FPX toolkit.
  */
  image->depth=8;
  (void) TransformRGBImage(image,RGBColorspace);
  memory_limit=20000000;
  fpx_status=FPX_SetToolkitMemoryLimit(&memory_limit);
  if (fpx_status != FPX_OK)
    ThrowWriterException(ResourceLimitError,
      "Unable to initialize FPX library",image);
  tile_width=64;
  tile_height=64;
  colorspace.numberOfComponents=3;
  if (image->matte)
    colorspace.numberOfComponents=4;
  if ((image_info->type != TrueColorType) &&
      IsGrayImage(image,&image->exception))
    {
      colorspace.numberOfComponents=1;
      colorspace.theComponents[0].myColor=MONOCHROME;
    }
  background_color.color1_value=0;
  background_color.color2_value=0;
  background_color.color3_value=0;
  background_color.color4_value=0;
  compression=NONE;
  if (image_info->compression == JPEGCompression)
    compression=JPEG_UNSPECIFIED;
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  if ((image->blob->file == stdout) || image->pipet ||
      (image->blob->data != (unsigned char *) NULL))
    TemporaryFilename(filename);
  else
    CloseBlob(image);
  {
#if defined(macintosh)
    FSSpec
      fsspec;

    FilenameToFSSpec(filename,&fsspec);
    fpx_status=FPX_CreateImageByFilename((const FSSpec &) fsspec,image->columns,
#else
    fpx_status=FPX_CreateImageByFilename(filename,image->columns,
#endif
      image->rows,tile_width,tile_height,colorspace,background_color,
      compression,&flashpix);
  }
  if (fpx_status != FPX_OK)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  if (image_info->compression == JPEGCompression)
    {
      /*
        Initialize the compression by quality for the entire image.
      */
      fpx_status=
        FPX_SetJPEGCompression(flashpix,(unsigned short) (image_info->quality));
      if (fpx_status != FPX_OK)
        ThrowWriterException(DelegateError,"Unable to set JPEG level",image);
    }
  /*
    Set image summary info.
  */
  summary_info.title_valid=False;
  summary_info.subject_valid=False;
  summary_info.author_valid=False;
  summary_info.comments_valid=False;
  summary_info.keywords_valid=False;
  summary_info.OLEtemplate_valid=False;
  summary_info.last_author_valid=False;
  summary_info.rev_number_valid=False;
  summary_info.edit_time_valid=False;
  summary_info.last_printed_valid=False;
  summary_info.create_dtm_valid=False;
  summary_info.last_save_dtm_valid=False;
  summary_info.page_count_valid=False;
  summary_info.word_count_valid=False;
  summary_info.char_count_valid=False;
  summary_info.thumbnail_valid=False;
  summary_info.appname_valid=False;
  summary_info.security_valid=False;
  label=GetImageAttribute(image,"label");
  if (label != (ImageAttribute *) NULL)
    {
      /*
        Note image label.
      */
      summary_info.title_valid=True;
      summary_info.title.length=strlen(label->value);
      summary_info.title.ptr=(unsigned char *)
        AcquireMemory(strlen(label->value)+1);
      if (summary_info.title.ptr != (unsigned char *) NULL)
        (void) strncpy((char *) summary_info.title.ptr,label->value,
          MaxTextExtent-1);
      else
        ThrowWriterException(DelegateError,"Unable to set image title",image);
    }
  comment=GetImageAttribute(image,"comment");
  if (comment != (ImageAttribute *) NULL)
    {
      /*
        Note image comment.
      */
      summary_info.comments_valid=True;
      summary_info.comments.length=strlen(comment->value);
      summary_info.comments.ptr=(unsigned char *)
        AcquireMemory(strlen(comment->value)+1);
      if (summary_info.comments.ptr != (unsigned char *) NULL)
        (void) strcpy((char *) summary_info.comments.ptr,comment->value);
      else
        ThrowWriterException(DelegateError,"Unable to set image comments",
          image);
    }
  fpx_status=FPX_SetSummaryInformation(flashpix,&summary_info);
  if (fpx_status != FPX_OK)
    ThrowWriterException(DelegateError,"Unable to set summary info",image);
  /*
    Allocate pixels.
  */
  pixels=(unsigned char *)
    AcquireMemory(colorspace.numberOfComponents*image->columns);
  if (pixels == (unsigned char *) NULL)
    {
      (void) FPX_CloseImage(flashpix);
      FPX_ClearSystem();
      ThrowWriterException(ResourceLimitError,"Memory allocation failed",
        image);
    }
  /*
    Initialize FlashPix image description.
  */
  fpx_info.numberOfComponents=colorspace.numberOfComponents;
  for (i=0; i < (long) fpx_info.numberOfComponents; i++)
  {
    fpx_info.components[i].myColorType.myDataType=DATA_TYPE_UNSIGNED_BYTE;
    fpx_info.components[i].horzSubSampFactor=1;
    fpx_info.components[i].vertSubSampFactor=1;
    fpx_info.components[i].columnStride=fpx_info.numberOfComponents;
    fpx_info.components[i].lineStride=
      image->columns*fpx_info.components[i].columnStride;
    fpx_info.components[i].theData=pixels+i;
  }
  fpx_info.components[0].myColorType.myColor=
    fpx_info.numberOfComponents != 1 ? NIFRGB_R : MONOCHROME;
  fpx_info.components[1].myColorType.myColor=NIFRGB_G;
  fpx_info.components[2].myColorType.myColor=NIFRGB_B;
  fpx_info.components[3].myColorType.myColor=ALPHA;
  /*
    Write image scanlines.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
      break;
    if (fpx_info.numberOfComponents == 1)
      (void) PopImagePixels(image,GrayQuantum,pixels);
    else
      if (!image->matte)
        (void) PopImagePixels(image,RGBQuantum,pixels);
      else
        (void) PopImagePixels(image,RGBAQuantum,pixels);
    fpx_status=FPX_WriteImageLine(flashpix,&fpx_info);
    if (fpx_status != FPX_OK)
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  if (image_info->view != (char *) NULL)
    {
      FPXAffineMatrix
        affine;

      FPXColorTwistMatrix
        color_twist;

      FPXContrastAdjustment
        contrast;

      FPXFilteringValue
        sharpen;

      FPXResultAspectRatio
        aspect_ratio;

      FPXROI
        view_rect;

      unsigned int
        affine_valid,
        aspect_ratio_valid,
        color_twist_valid,
        contrast_valid,
        sharpen_valid,
        view_rect_valid;

      /*
        Initialize default viewing parameters.
      */
      contrast=1.0;
      contrast_valid=False;
      color_twist.byy=1.0;
      color_twist.byc1=0.0;
      color_twist.byc2=0.0;
      color_twist.dummy1_zero=0.0;
      color_twist.bc1y=0.0;
      color_twist.bc1c1=1.0;
      color_twist.bc1c2=0.0;
      color_twist.dummy2_zero=0.0;
      color_twist.bc2y=0.0;
      color_twist.bc2c1=0.0;
      color_twist.bc2c2=1.0;
      color_twist.dummy3_zero=0.0;
      color_twist.dummy4_zero=0.0;
      color_twist.dummy5_zero=0.0;
      color_twist.dummy6_zero=0.0;
      color_twist.dummy7_one=1.0;
      color_twist_valid=False;
      sharpen=0.0;
      sharpen_valid=False;
      aspect_ratio=(double) image->columns/image->rows;
      aspect_ratio_valid=False;
      view_rect.left=(float) 0.1;
      view_rect.width=aspect_ratio-0.2;
      view_rect.top=(float) 0.1;
      view_rect.height=(float) 0.8; /* 1.0-0.2 */
      view_rect_valid=False;
      affine.a11=1.0;
      affine.a12=0.0;
      affine.a13=0.0;
      affine.a14=0.0;
      affine.a21=0.0;
      affine.a22=1.0;
      affine.a23=0.0;
      affine.a24=0.0;
      affine.a31=0.0;
      affine.a32=0.0;
      affine.a33=1.0;
      affine.a34=0.0;
      affine.a41=0.0;
      affine.a42=0.0;
      affine.a43=0.0;
      affine.a44=1.0;
      affine_valid=False;
      if (0)
        {
          /*
            Color color twist.
          */
          SetBrightness(0.5,&color_twist);
          SetSaturation(0.5,&color_twist);
          SetColorBalance(0.5,1.0,1.0,&color_twist);
          color_twist_valid=True;
        }
      if (affine_valid)
        {
          fpx_status=FPX_SetImageAffineMatrix(flashpix,&affine);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,"Unable to set affine matrix",
              image);
        }
      if (aspect_ratio_valid)
        {
          fpx_status=FPX_SetImageResultAspectRatio(flashpix,&aspect_ratio);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,"Unable to set aspect ratio",
              image);
        }
      if (color_twist_valid)
        {
          fpx_status=FPX_SetImageColorTwistMatrix(flashpix,&color_twist);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,
              "Unable to set color color twist",image);
        }
      if (contrast_valid)
        {
          fpx_status=FPX_SetImageContrastAdjustment(flashpix,&contrast);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,"Unable to set contrast",
              image);
        }
      if (sharpen_valid)
        {
          fpx_status=FPX_SetImageFilteringValue(flashpix,&sharpen);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,
              "Unable to set filtering value",image);
        }
      if (view_rect_valid)
        {
          fpx_status=FPX_SetImageROI(flashpix,&view_rect);
          if (fpx_status != FPX_OK)
            ThrowWriterException(DelegateError,
              "Unable to set region of interest",image);
        }
    }
  (void) FPX_CloseImage(flashpix);
  FPX_ClearSystem();
  LiberateMemory((void **) &pixels);
  if ((image->blob->file == stdout) || image->pipet ||
      (image->blob->data != (unsigned char *) NULL))
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to image blob->
      */
      file=fopen(filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        ThrowWriterException(FileOpenError,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) WriteBlobByte(image,c);
      (void) fclose(file);
      (void) remove(filename);
      CloseBlob(image);
    }
  return(True);
}
#else
static unsigned int WriteFPXImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateError,"FPX library is not available",
    image->filename)
}
#endif
