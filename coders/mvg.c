/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            M   M  V   V   GGGG                              %
%                            MM MM  V   V  G                                  %
%                            M M M  V   V  G GG                               %
%                            M   M   V V   G   G                              %
%                            M   M    V     GGG                               %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 April 2000                                  %
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
%  Licensor ("ImageMagick Studio LLC") warrants that the copyright in and to  %
%  the Software ("ImageMagick") is owned by ImageMagick Studio LLC or is      %
%  distributed by ImageMagick Studio LLC under a valid current license.       %
%  Except as expressly stated in the immediately preceding sentence,          %
%  ImageMagick is provided by ImageMagick Studio LLC, distributors, and       %
%  copyright owners "AS IS", without warranty of any kind, express or         %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and non-infringement.  In no event shall  %
%  ImageMagick Studio LLC, contributors or copyright owners be liable for     %
%  any claim, damages, or other liability, whether in an action of contract,  %
%  tort or otherwise, arising from, out of or in connection with              %
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
#include "studio.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMVG returns True if the image format type, identified by the
%  magick string, is MVG.
%
%  The format of the IsMVG method is:
%
%      unsigned int IsMVG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMVG returns True if the image format type is MVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMVG(const unsigned char *magick,const size_t length)
{
  if (length < 7)
    return(False);
  if (LocaleNCompare((char *) magick,"viewbox",7) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMVGImage creates a gradient image and initializes it to
%  the X server color range as specified by the filename.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadMVGImage method is:
%
%      Image *ReadMVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMVGImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BoundingBox  "ViewBox"

  char
    filename[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *image;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  if ((image->columns == 0) || (image->rows == 0))
    {
      char
        primitive[MaxTextExtent];

      register char
        *p;

      SegmentInfo
        bounds;

      unsigned int
        status;
 
      /*
        Determine size of image canvas.
      */
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        ThrowReaderException(FileOpenError,"Unable to open file",image);
      while (ReadBlobString(image,primitive) != (char *) NULL)
      {
        for (p=primitive; (*p == ' ') || (*p == '\t'); p++);
        if (LocaleNCompare(BoundingBox,p,strlen(BoundingBox)) != 0)
          continue;
        (void) sscanf(p,"viewbox %lf %lf %lf %lf",&bounds.x1,&bounds.y1,
          &bounds.x2,&bounds.y2);
        image->columns=(unsigned long) (bounds.x2-bounds.x1+0.5);
        image->rows=(unsigned long) (bounds.y2-bounds.y1+0.5);
        break;
      }
    }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"Must specify image size",image);
  SetImage(image,OpaqueOpacity);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->fill=image_info->pen;
  (void) strcpy(filename,"@");
  (void) strcat(filename,image_info->filename);
  (void) CloneString(&draw_info->primitive,filename);
  (void) DrawImage(image,draw_info);
  DestroyDrawInfo(draw_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMVGImage adds attributes for the MVG image format
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMVGImage method is:
%
%      RegisterMVGImage(void)
%
*/
ModuleExport void RegisterMVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MVG");
  entry->decoder=ReadMVGImage;
  entry->encoder=WriteMVGImage;
  entry->magick=IsMVG;
  entry->adjoin=False;
  entry->description=AcquireString("Magick Vector Graphics");
  entry->module=AcquireString("MVG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M V G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMVGImage removes format registrations made by the
%  MVG module from the list of supported formats.
%
%  The format of the UnregisterMVGImage method is:
%
%      UnregisterMVGImage(void)
%
*/
ModuleExport void UnregisterMVGImage(void)
{
  (void) UnregisterMagickInfo("MVG");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M V G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMVGImage writes an image to a file in MVG image format.
%
%  The format of the WriteMVGImage method is:
%
%      unsigned int WriteMVGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMVGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMVGImage(const ImageInfo *image_info,Image *image)
{
  const ImageAttribute
    *attribute;

  unsigned int
    status;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  attribute=GetImageAttribute(image,"[MVG]");
  if (attribute == (ImageAttribute *) NULL)
    ThrowWriterException(DelegateError,"no image vector graphics",image);
  status=OpenBlob(image_info,image,WriteBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  (void) WriteBlob(image,strlen(attribute->value),attribute->value);
  CloseBlob(image);
  return(True);
}
