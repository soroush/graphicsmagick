/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            TTTTT  TTTTT  FFFFF                              %
%                              T      T    F                                  %
%                              T      T    FFF                                %
%                              T      T    F                                  %
%                              T      T    F                                  %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T T F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTTF returns True if the image format type, identified by the
%  magick string can be handled by this module.
%
%  The format of the IsTTF method is:
%
%      unsigned int IsTTF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTTF returns True if the image format type can be
%               handled by this module.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/

static unsigned int IsPFA(const unsigned char *magick,const size_t length)
{
  if (length < 14)
    return(False);
  if (LocaleNCompare((char *) magick,"%!PS-AdobeFont-1.0",14) == 0)
    return(True);
  return(False);
}

static unsigned int IsTTF(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if ((magick[0] == 0x00) && (magick[1] == 0x01) && (magick[2] == 0x00) &&
      (magick[3] == 0x00) && (magick[4] == 0x0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T T F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTTFImage reads a TrueType font file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTTFImage method is:
%
%      Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTTFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#if defined(HasTTF)
static Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent];

  const char
    *Text =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\n0123456789 "
      "\342\352\356\373\364\344\353\357\366\374\377\340\371\351\350\347\n"
      "&#~\\\"\'(-`_^@)=+\260 $\243^\250*\265\371%!\247:/;.,?<>";

  DrawContext
    context;

  DrawInfo
    *draw_info;

  Image
    *image;

  long
    y;

  PixelPacket
    background_color;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    ThrowReaderException(ResourceLimitError,"Unable to allocate image",image);
  image->columns=800;
  image->rows=480;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  /*
    Color canvas with background color
  */
  background_color=image_info->background_color;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
      *q++=background_color;
    if (!SyncImagePixels(image))
      break;
  }
  (void) strncpy(image->magick,image_info->magick,MaxTextExtent-1);
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  /*
    Prepare drawing commands
  */
  y=20;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->font=AllocateString(image->filename);
  draw_info->fill=image_info->pen;
  context=DrawAllocateContext(draw_info,image);
  (void) DrawPushGraphicContext(context);
  (void) DrawSetViewbox(context,0,0,image->columns,image->rows);
  (void) DrawSetFont(context,image_info->filename);
  (void) DrawSetFontSize(context,18);
  (void) DrawAnnotation(context,10,y,(const unsigned char *) Text);
  y+=20*MultilineCensus(Text)+20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+12;
    (void) DrawSetFontSize(context,18);
    (void) FormatString(buffer,"%ld",i);
    (void) DrawAnnotation(context,10,y,(const unsigned char *) buffer);
    (void) DrawSetFontSize(context,i);
    (void) DrawAnnotation(context,50,y,(const unsigned char *)
      "That which does not destroy me, only makes me stronger.");
    if (i >= 24)
      i+=6;
  }
  (void) DrawPopGraphicContext(context);
  (void) DrawRender(context);
  /*
    Free resources.
  */
  DestroyDrawInfo(draw_info);
  DrawDestroyContext(context);
  CloseBlob(image);
  return(image);
}
#endif /* HasTTF */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T T F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTTFImage adds attributes for the TTF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTTFImage method is:
%
%      RegisterTTFImage(void)
%
*/
ModuleExport void RegisterTTFImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(FREETYPE_MAJOR) && defined(FREETYPE_MINOR)
  FormatString(version,"%d.%d",FREETYPE_MAJOR,FREETYPE_MINOR);
#endif
  entry=SetMagickInfo("TTF");
#if defined(HasTTF)
  entry->decoder=ReadTTFImage;
#endif
  entry->magick=IsTTF;
  entry->adjoin=False;
  entry->description=AcquireString("TrueType font");
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("TTF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PFA");
#if defined(HasTTF)
  entry->decoder=ReadTTFImage;
#endif
  entry->magick=IsPFA;
  entry->adjoin=False;
  entry->description=AcquireString("Postscript Type 1 font (ASCII)");
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("TTF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PFB");
#if defined(HasTTF)
  entry->decoder=ReadTTFImage;
#endif
  entry->magick=IsPFA;
  entry->adjoin=False;
  entry->description=AcquireString("Postscript Type 1 font (binary)");
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("TTF");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T T F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTTFImage removes format registrations made by the
%  TTF module from the list of supported formats.
%
%  The format of the UnregisterTTFImage method is:
%
%      UnregisterTTFImage(void)
%
*/
ModuleExport void UnregisterTTFImage(void)
{
  (void) UnregisterMagickInfo("TTF");
  (void) UnregisterMagickInfo("PFA");
  (void) UnregisterMagickInfo("PFB");
}
