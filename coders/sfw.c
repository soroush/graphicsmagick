/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  FFFFF  W   W                              %
%                            SS     F      W   W                              %
%                             SSS   FFF    W   W                              %
%                               SS  F      W W W                              %
%                            SSSSS  F       W W                               %
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
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S F W                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSFW returns True if the image format type, identified by the
%  magick string, is SFW.
%
%  The format of the IsSFW method is:
%
%      unsigned int IsSFW(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSFW returns True if the image format type is SFW.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSFW(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"SFW94",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S F W I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSFWImage reads a Seattle Film Works image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSFWImage method is:
%
%      Image *ReadSFWImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSFWImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned char *SFWScan(const unsigned char *p,const unsigned char *q,
  const unsigned char *target,const int length)
{
  register int
    i;

  for ( ; p < q; p++)
  {
    if (*p != *target)
      continue;
    if (length == 1)
      return((unsigned char *) p);
    for (i=1; i < length; i++)
      if (*(p+i) != *(target+i))
        break;
    if (i == length)
      return((unsigned char *) p);
  }
  return((unsigned char *) NULL);
}

static void TranslateSFWMarker(unsigned char *marker)
{
  switch (marker[1])
  {
    case 0xc8: marker[1]=0xd8; break;  /* soi */
    case 0xd0: marker[1]=0xe0; break;  /* app */
    case 0xcb: marker[1]=0xdb; break;  /* dqt */
    case 0xa0: marker[1]=0xc0; break;  /* sof */
    case 0xa4: marker[1]=0xc4; break;  /* sof */
    case 0xca: marker[1]=0xda; break;  /* sos */
    case 0xc9: marker[1]=0xd9; break;  /* eoi */
    default: break;
  }
}

static Image *ReadSFWImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  static unsigned char
    HuffmanTable[] =
    {
      0xFF, 0xC4, 0x01, 0xA2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
      0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04,
      0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x10, 0x00, 0x02, 0x01,
      0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00,
      0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21,
      0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32,
      0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1,
      0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18,
      0x19, 0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36,
      0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
      0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64,
      0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77,
      0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
      0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3,
      0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5,
      0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
      0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9,
      0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
      0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0x11,
      0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04,
      0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
      0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13,
      0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09,
      0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24,
      0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28,
      0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45,
      0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73,
      0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85,
      0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
      0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
      0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2,
      0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4,
      0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
      0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
      0xF9, 0xFA
    };

  FILE
    *file;

  Image
    *flipped_image,
    *image;

  ImageInfo
    *clone_info;

  register unsigned char
    *header,
    *data;

  size_t
    count;

  unsigned char
    *buffer,
    *offset;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read image into a buffer.
  */
  buffer=(unsigned char *) AcquireMemory(SizeBlob(image));
  if (buffer == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  count=ReadBlob(image,SizeBlob(image),(char *) buffer);
  if ((count == 0) || (LocaleNCompare((char *) buffer,"SFW",3) != 0))
    ThrowReaderException(CorruptImageWarning,"Not a SFW image file",image);
  CloseBlob(image);
  DestroyImage(image);
  /*
    Find the start of the JFIF data
  */
  header=SFWScan(buffer,buffer+SizeBlob(image)-1,(unsigned char *)
    "\377\310\377\320",4);
  if (header == (unsigned char *) NULL)
    {
      LiberateMemory((void **) &buffer);
      ThrowReaderException(CorruptImageWarning,"Not a SFW image file",image)
    }
  TranslateSFWMarker(header);  /* translate soi and app tags */
  TranslateSFWMarker(header+2);
  (void) memcpy(header+6,"JFIF\0\001\0",7);  /* JFIF magic */
  /*
    Translate remaining markers.
  */
  offset=header+2;
  offset+=(offset[2] << 8)+offset[3]+2;
  for ( ; ; )
  {
    TranslateSFWMarker(offset);
    if (offset[1] == 0xda)
      break;
    offset+=(offset[2] << 8)+offset[3]+2;
  }
  offset--;
  data=SFWScan(offset,buffer+SizeBlob(image)-1,
    (unsigned char *) "\377\311",2);
  if (data == (unsigned char *) NULL)
    {
      LiberateMemory((void **) &buffer);
      ThrowReaderException(CorruptImageWarning,"Not a SFW image file",image)
    }
  TranslateSFWMarker(data++);  /* translate eoi marker */
  /*
    Write JFIF file.
  */
  clone_info=CloneImageInfo(image_info);
  DetachBlob(clone_info->blob);
  TemporaryFilename(clone_info->filename);
  file=fopen(clone_info->filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    {
      LiberateMemory((void **) &buffer);
      DestroyImageInfo(clone_info);
      ThrowReaderException(FileOpenWarning,"Unable to write file",image)
    }
  (void) fwrite(header,offset-header+1,1,file);
  (void) fwrite(HuffmanTable,1,sizeof(HuffmanTable)/sizeof(*HuffmanTable),file);
  (void) fwrite(offset+1,data-offset,1,file);
  status=ferror(file);
  (void) fclose(file);
  LiberateMemory((void **) &buffer);
  if (status)
    {
      (void) remove(clone_info->filename);
      DestroyImageInfo(clone_info);
      ThrowReaderException(FileOpenWarning,"Unable to write file",image)
    }
  /*
    Read JPEG image.
  */
  image=ReadImage(clone_info,exception);
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  if (image == (Image *) NULL)
    return(image);
  /*
    Correct image orientation.
  */
  flipped_image=FlipImage(image,exception);
  if (flipped_image == (Image *) NULL)
    return(image);
  DestroyImage(image);
  return(flipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S F W I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSFWImage adds attributes for the SFW image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSFWImage method is:
%
%      RegisterSFWImage(void)
%
*/
ModuleExport void RegisterSFWImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SFW");
  entry->decoder=ReadSFWImage;
  entry->magick=IsSFW;
  entry->adjoin=False;
  entry->description=AllocateString("Seattle Film Works");
  entry->module=AllocateString("SFW");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S F W I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSFWImage removes format registrations made by the
%  SFW module from the list of supported formats.
%
%  The format of the UnregisterSFWImage method is:
%
%      UnregisterSFWImage(void)
%
*/
ModuleExport void UnregisterSFWImage(void)
{
  (void) UnregisterMagickInfo("SFW");
}
