/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     V   V  IIIII   CCCC   AAA   RRRR                        %
%                     V   V    I    C      A   A  R   R                       %
%                     V   V    I    C      AAAAA  RRRR                        %
%                      V V     I    C      A   A  R R                         %
%                       V    IIIII   CCCC  A   A  R  R                        %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteVICARImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s V I C A R                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsVICAR returns True if the image format type, identified by the
%  magick string, is VICAR.
%
%  The format of the IsVICAR method is:
%
%      unsigned int IsVICAR(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsVICAR returns True if the image format type is VICAR.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsVICAR(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 7)
    return(False);
  if (LocaleNCompare((char *) magick,"LBLSIZE",7) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"NJPL1I",6) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I C A R I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVICARImage reads a VICAR image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadVICARImage method is:
%
%      Image *ReadVICARImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Method ReadVICARImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadVICARImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  int
    c,
    y;

  long
    count;

  unsigned char
    *scanline;

  unsigned int
    length,
    status,
    value_expected;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Decode image header.
  */
  c=ReadBlobByte(image);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  length=0;
  image->columns=0;
  image->rows=0;
  while (isgraph(c) && ((image->columns*image->rows) == 0))
  {
    if (!isalnum(c))
      {
        c=ReadBlobByte(image);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=ReadBlobByte(image);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c))
        {
          if ((p-value) < (MaxTextExtent-1))
            *p++=c;
          c=ReadBlobByte(image);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (LocaleCompare(keyword,"Label_RECORDS") == 0)
          length=(unsigned int) atoi(value);
        if (LocaleCompare(keyword,"LBLSIZE") == 0)
          length=(unsigned int) atoi(value);
        if (LocaleCompare(keyword,"RECORD_BYTES") == 0)
          image->columns=(unsigned int) atoi(value);
        if (LocaleCompare(keyword,"NS") == 0)
          image->columns=(unsigned int) atoi(value);
        if (LocaleCompare(keyword,"LINES") == 0)
          image->rows=(unsigned int) atoi(value);
        if (LocaleCompare(keyword,"NL") == 0)
          image->rows=(unsigned int) atoi(value);
      }
    while (isspace(c))
    {
      c=ReadBlobByte(image);
      count++;
    }
  }
  while (count < (int) length)
  {
    c=ReadBlobByte(image);
    count++;
  }
  if ((image->columns*image->rows) == 0)
    ThrowReaderException(CorruptImageWarning,"image size is zero",image);
  image->depth=8;
  if (!AllocateImageColormap(image,256))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Initialize image structure.
  */
  scanline=(unsigned char *) AcquireMemory(image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(CorruptImageWarning,"Unable to read image data",image);
  /*
    Read VICAR pixels.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    if (!SetImagePixels(image,0,y,image->columns,1))
      break;
    (void) ReadBlob(image,image->columns,scanline);
    (void) PushImagePixels(image,GrayQuantum,scanline);
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  LiberateMemory((void **) &scanline);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r V I C A R I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterVICARImage adds attributes for the VICAR image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterVICARImage method is:
%
%      RegisterVICARImage(void)
%
*/
ModuleExport void RegisterVICARImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VICAR");
  entry->decoder=ReadVICARImage;
  entry->encoder=WriteVICARImage;
  entry->magick=IsVICAR;
  entry->adjoin=False;
  entry->description=AllocateString("VICAR rasterfile format");
  entry->module=AllocateString("VICAR");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r V I C A R I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterVICARImage removes format registrations made by the
%  VICAR module from the list of supported formats.
%
%  The format of the UnregisterVICARImage method is:
%
%      UnregisterVICARImage(void)
%
*/
ModuleExport void UnregisterVICARImage(void)
{
  UnregisterMagickInfo("VICAR");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I C A R I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVICARImage writes an image in the VICAR rasterfile format.
%  Vicar files contain a text header, followed by one or more planes of binary
%  grayscale image data.  Vicar files are designed to allow many planes to be
%  stacked together to form image cubes.  This method only writes a single
%  grayscale plane.
%
%  Method WriteVICARImage was written contributed by
%  gorelick@esther.la.asu.edu.
%
%  The format of the WriteVICARImage method is:
%
%      unsigned int WriteVICARImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVICARImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteVICARImage(const ImageInfo *image_info,Image *image)
{
  char
    header[MaxTextExtent];

  int
    y;

  unsigned char
    *scanline;

  unsigned int
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  /*
    Write header.
  */
  memset(header,' ',MaxTextExtent);
  FormatString(header,"LBLSIZE=%u FORMAT='BYTE' TYPE='IMAGE' BUFSIZE=20000 "
    "DIM=2 EOL=0 RECSIZE=%u ORG='BSQ' NL=%u NS=%u NB=1 N1=0 N2=0 N3=0 N4=0 "
    "NBB=0 NLB=0 TASK='ImageMagick'",MaxTextExtent,image->columns,image->rows,
    image->columns);
  (void) WriteBlob(image,MaxTextExtent,header);
  /*
    Allocate memory for scanline.
  */
  scanline=(unsigned char *) AcquireMemory(image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write VICAR scanline.
  */
  image->depth=8;
  for (y=0; y < (int) image->rows; y++)
  {
    if (!GetImagePixels(image,0,y,image->columns,1))
      break;
    (void) PopImagePixels(image,GrayQuantum,scanline);
    (void) WriteBlob(image,image->columns,scanline);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
  }
  LiberateMemory((void **) &scanline);
  CloseBlob(image);
  return(True);
}
