/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            BBBB   M   M  PPPP                               %
%                            B   B  MM MM  P   P                              %
%                            BBBB   M M M  PPPP                               %
%                            B   B  M   M  P                                  %
%                            BBBB   M   M  P                                  %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                            Glenn Randers-Pehrson                            %
%                               December 2001                                 %
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
#include "studio.h"
#include "blob.h"
#include "list.h"
#include "log.h"
#include "magick.h"
#include "monitor.h"
#include "utility.h"

/*
  Macro definitions (from Windows wingdi.h).
*/

#undef BI_JPEG
#define BI_JPEG  4
#undef BI_PNG
#define BI_PNG  5
#if !defined(WIN32) || defined(__MINGW32__)
#define BI_RGB  0
#define BI_RLE8  1
#define BI_RLE4  2
#define BI_BITFIELDS  3

#define LCS_CALIBRATED_RBG  0
#define LCS_sRGB  1
#define LCS_WINDOWS_COLOR_SPACE  2
#define PROFILE_LINKED  3
#define PROFILE_EMBEDDED  4

#define LCS_GM_BUSINESS  1  /* Saturation */
#define LCS_GM_GRAPHICS  2  /* Relative */
#define LCS_GM_IMAGES  4  /* Perceptual */
#define LCS_GM_ABS_COLORIMETRIC  8  /* Absolute */
#endif

/*
  Typedef declarations.
*/
typedef struct _BMPInfo
{
  unsigned long
    file_size,
    ba_offset,
    offset_bits,
    size;

  long
    width,
    height;

  unsigned short
    planes,
    bits_per_pixel;

  unsigned long
    compression,
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    red_mask,
    green_mask,
    blue_mask,
    alpha_mask,
    colors_important;

  long
    colorspace;

  PrimaryInfo
    red_primary,
    green_primary,
    blue_primary,
    gamma_scale;
} BMPInfo;

/*
  Forward declarations.
*/
static unsigned int
  WriteBMPImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,const unsigned long compression,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o compression:  Zero means uncompressed.  A value of 1 means the
%      compressed pixels are runlength encoded for a 256-color bitmap.
%      A value of 2 means a 16-color bitmap.  A value of 3 means bitfields
%      encoding.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,const unsigned long compression,
  unsigned char *pixels)
{
  long
    byte,
    count,
    y;

  register long
    i,
    x;

  register unsigned char
    *q;

  assert(image != (Image *) NULL);
  assert(pixels != (unsigned char *) NULL);
  LogMagickEvent(CoderEvent,"   Decoding RLE pixels");
  (void) memset(pixels,0,image->columns*image->rows);
  byte=0;
  x=0;
  q=pixels;
  for (y=0; y < (long) image->rows; )
  {
    count=ReadBlobByte(image);
    if (count == EOF)
      break;
    if (count != 0)
      {
        /*
          Encoded mode.
        */
        byte=ReadBlobByte(image);
        for (i=0; i < count; i++)
        {
          if (compression == BI_RLE8)
            *q++=(unsigned char) byte;
          else
            *q++=(unsigned char)
              ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
          x++;
        }
      }
    else
      {
        /*
          Escape mode.
        */
        count=ReadBlobByte(image);
        if (count == 0x01)
          return(True);
        switch ((int) count)
        {
          case 0x00:
          {
            /*
              End of line.
            */
            x=0;
            y++;
            q=pixels+y*image->columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=ReadBlobByte(image);
            y+=ReadBlobByte(image);
            q=pixels+y*image->columns+x;
            break;
          }
          default:
          {
            /*
              Absolute mode.
            */
            for (i=0; i < count; i++)
            {
              if (compression == BI_RLE8)
                *q++=ReadBlobByte(image);
              else
                {
                  if ((i & 0x01) == 0)
                    byte=ReadBlobByte(image);
                  *q++=(unsigned char)
                    ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
                }
              x++;
            }
            /*
              Read pad byte.
            */
            if (compression == BI_RLE8)
              {
                if (count & 0x01)
                  (void) ReadBlobByte(image);
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                (void) ReadBlobByte(image);
            break;
          }
        }
      }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(LoadImageText,y,image->rows,&image->exception))
        break;
  }
  (void) ReadBlobByte(image);  /* end of line */
  (void) ReadBlobByte(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E n c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EncodeImage compresses pixels using a runlength encoded format.
%
%  The format of the EncodeImage method is:
%
%    static unsigned int EncodeImage(Image *image,
%      const unsigned long bytes_per_line,const unsigned char *pixels,
%      unsigned char *compressed_pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns the number of bytes in the
%      runlength encoded compress_pixels array.
%
%    o image:  A pointer to a Image structure.
%
%    o bytes_per_line: The number of bytes in a scanline of compressed pixels
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the compression process.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%
*/
static size_t EncodeImage(Image *image,const unsigned long bytes_per_line,
  const unsigned char *pixels,unsigned char *compressed_pixels)
{
  long
    y;

  register const unsigned char
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  /*
    Runlength encode pixels.
  */
  assert(image != (Image *) NULL);
  assert(pixels != (const unsigned char *) NULL);
  assert(compressed_pixels != (unsigned char *) NULL);
  p=pixels;
  q=compressed_pixels;
  i=0;
  for (y=0; y < (long) image->rows; y++)
  {
    for (x=0; x < (long) bytes_per_line; x+=i)
    {
      /*
        Determine runlength.
      */
      for (i=1; ((x+i) < (long) bytes_per_line); i++)
        if ((*(p+i) != *p) || (i == 255))
          break;
      *q++=(unsigned char) i;
      *q++=(*p);
      p+=i;
    }
    /*
      End of line.
    */
    *q++=0x00;
    *q++=0x00;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
        break;
  }
  /*
    End of bitmap.
  */
  *q++=0;
  *q++=0x01;
  return(q-compressed_pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s B M P                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsBMP returns True if the image format type, identified by the
%  magick string, is BMP.
%
%  The format of the IsBMP method is:
%
%      unsigned int IsBMP(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsBMP returns True if the image format type is BMP.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsBMP(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if ((LocaleNCompare((char *) magick,"BA",2) == 0) ||
      (LocaleNCompare((char *) magick,"BM",2) == 0) ||
      (LocaleNCompare((char *) magick,"IC",2) == 0) ||
      (LocaleNCompare((char *) magick,"PI",2) == 0) ||
      (LocaleNCompare((char *) magick,"CI",2) == 0) ||
      (LocaleNCompare((char *) magick,"CP",2) == 0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d B M P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBMPImage reads a Microsoft Windows bitmap image file, Version
%  2, 3 (for Windows or NT), or 4, and  returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadBMPImage method is:
%
%      image=ReadBMPImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadBMPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadBMPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  BMPInfo
    bmp_info;

  Image
    *image;

  IndexPacket
    index;

  int
    logging;

  long
    bit,
    y;

  unsigned long
    blue,
    green,
    opacity,
    red;

  off_t
    start_position;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count,
    length;

  unsigned char
    magick[12],
    *pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent," Begin ReadBMPImage()");
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  /*
    Determine if this is a BMP file.
  */
  (void) memset(&bmp_info,0,sizeof(BMPInfo));
  bmp_info.ba_offset=0;
  start_position=0;
  count=ReadBlob(image,2,(char *) magick);
  do
  {
    PixelPacket
      quantum_bits,
      shift;

    unsigned long
      profile_data,
      profile_size;

    /*
      Verify BMP identifier.
    */
    if (bmp_info.ba_offset == 0)
      start_position=TellBlob(image)-2;
    bmp_info.ba_offset=0;
    while (LocaleNCompare((char *) magick,"BA",2) == 0)
    {
      bmp_info.file_size=ReadBlobLSBLong(image);
      bmp_info.ba_offset=ReadBlobLSBLong(image);
      bmp_info.offset_bits=ReadBlobLSBLong(image);
      count=ReadBlob(image,2,(char *) magick);
    }
    if (logging)
      LogMagickEvent(CoderEvent,"   Magick=%c%c",magick[0],magick[1]);
    if ((count == 0) || ((LocaleNCompare((char *) magick,"BM",2) != 0) &&
        (LocaleNCompare((char *) magick,"CI",2) != 0)))
      ThrowReaderException(CorruptImageError,"Not a BMP image file",image);
    bmp_info.file_size=ReadBlobLSBLong(image);
    (void) ReadBlobLSBLong(image);
    bmp_info.offset_bits=ReadBlobLSBLong(image);
    bmp_info.size=ReadBlobLSBLong(image);
    if (logging)
      LogMagickEvent(CoderEvent,"   BMP size=%lu",bmp_info.size);
    if (bmp_info.file_size != GetBlobSize(image))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: length and filesize do not match", image);
    if (bmp_info.size == 12)
      {
        /*
          OS/2 BMP image file.
        */
        bmp_info.width=(short) ReadBlobLSBShort(image);
        bmp_info.height=(short) ReadBlobLSBShort(image);
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.x_pixels=0;
        bmp_info.y_pixels=0;
        bmp_info.number_colors=0;
        bmp_info.compression=BI_RGB;
        bmp_info.image_size=0;
        bmp_info.alpha_mask=0;
        if (logging)
          {
            LogMagickEvent(CoderEvent,"   Decoding OS/2 BMP datastream");
            LogMagickEvent(CoderEvent,"   width=%ld, height=%ld",bmp_info.width,
              bmp_info.height);
          }
      }
    else
      {
        /*
          Microsoft Windows BMP image file.
        */
        if (bmp_info.size < 40)
          ThrowReaderException(CorruptImageWarning,
            "Corrupt BMP image: Non OS/2 BMP header size < 40",image);
        bmp_info.width=(short) ReadBlobLSBLong(image);
        bmp_info.height=(short) ReadBlobLSBLong(image);
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.compression=ReadBlobLSBLong(image);
        bmp_info.image_size=ReadBlobLSBLong(image);
        bmp_info.x_pixels=ReadBlobLSBLong(image);
        bmp_info.y_pixels=ReadBlobLSBLong(image);
        bmp_info.number_colors=ReadBlobLSBLong(image);
        bmp_info.colors_important=ReadBlobLSBLong(image);
        profile_data=0;
        profile_size=0;

        if (logging)
          {
            LogMagickEvent(CoderEvent,"   Decoding MS Windows BMP datastream");
            LogMagickEvent(CoderEvent,"   width=%ld, height=%ld",bmp_info.width,
              bmp_info.height);
            LogMagickEvent(CoderEvent,"   bits per pixel=%d",
              bmp_info.bits_per_pixel);
            switch ((int) bmp_info.compression)
            {
              case BI_RGB:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_RGB");
                break;
              }
              case BI_RLE4:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_RLE4");
                break;
              }
              case BI_RLE8:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_RLE8");
                break;
              }
              case BI_BITFIELDS:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_BITFIELDS");
                break;
              }
              case BI_PNG:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_PNG");
                break;
              }
              case BI_JPEG:
              {
                LogMagickEvent(CoderEvent,"   compression=BI_JPEG");
                break;
              }
              default:
              {
                LogMagickEvent(CoderEvent,"   compression=UNKNOWN (%lu)",
                  bmp_info.compression);
              }
            }
            LogMagickEvent(CoderEvent,"   number_colors=%lu",
              bmp_info.number_colors);
          }

        if ((bmp_info.compression == BI_BITFIELDS) &&
            ((bmp_info.bits_per_pixel == 16) ||
             (bmp_info.bits_per_pixel == 32)))
          {
            bmp_info.red_mask=ReadBlobLSBLong(image);
            bmp_info.green_mask=ReadBlobLSBLong(image);
            bmp_info.blue_mask=ReadBlobLSBLong(image);
          }
        if (bmp_info.size > 40)
          {
            double
              sum;

            /*
              Read color management information.
            */
            bmp_info.alpha_mask=ReadBlobLSBLong(image);
            bmp_info.colorspace=(long) ReadBlobLSBLong(image);
            /*
              Decode 2^30 fixed point formatted CIE primaries.
            */
            bmp_info.red_primary.x=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.red_primary.y=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.red_primary.z=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.green_primary.x=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.green_primary.y=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.green_primary.z=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.blue_primary.x=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.blue_primary.y=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            bmp_info.blue_primary.z=(double)
              ReadBlobLSBLong(image)/0x3ffffff;
            sum=bmp_info.red_primary.x+bmp_info.red_primary.x+
              bmp_info.red_primary.z;
            image->chromaticity.red_primary.x/=sum;
            image->chromaticity.red_primary.y/=sum;
            sum=bmp_info.green_primary.x+bmp_info.green_primary.x+
              bmp_info.green_primary.z;
            image->chromaticity.green_primary.x/=sum;
            image->chromaticity.green_primary.y/=sum;
            sum=bmp_info.blue_primary.x+bmp_info.blue_primary.x+
              bmp_info.blue_primary.z;
            image->chromaticity.blue_primary.x/=sum;
            image->chromaticity.blue_primary.y/=sum;
            /*
              Decode 16^16 fixed point formatted gamma_scales.
            */
            bmp_info.gamma_scale.x=(double) ReadBlobLSBLong(image)/0xffff;
            bmp_info.gamma_scale.y=(double) ReadBlobLSBLong(image)/0xffff;
            bmp_info.gamma_scale.z=(double) ReadBlobLSBLong(image)/0xffff;
            /*
              Compute a single gamma from the BMP 3-channel gamma.
            */
            image->gamma=(bmp_info.gamma_scale.x+bmp_info.gamma_scale.y+
              bmp_info.gamma_scale.z)/3.0;
          }
        if (bmp_info.size > 108)
          {
            unsigned long
              intent;

            /*
              Read BMP Version 5 color management information.
            */
            intent=ReadBlobLSBLong(image);
            switch ((int) intent)
            {
              case LCS_GM_BUSINESS:
              {
                image->rendering_intent=SaturationIntent;
                break;
              }
              case LCS_GM_GRAPHICS:
              {
                image->rendering_intent=RelativeIntent;
                break;
              }
              case LCS_GM_IMAGES:
              {
                image->rendering_intent=PerceptualIntent;
                break;
              }
              case LCS_GM_ABS_COLORIMETRIC:
              {
                image->rendering_intent=AbsoluteIntent;
                break;
              }
            }
            profile_data=ReadBlobLSBLong(image);
            profile_data=profile_data;
            profile_size=ReadBlobLSBLong(image);
            profile_size=profile_size;
            ReadBlobLSBLong(image);  /* Reserved byte */
          }
      }

    if (bmp_info.width <= 0)
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Negative or zero width",image);
    if (bmp_info.height == 0)
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: zero height",image);
    if ((bmp_info.height < 0) && (bmp_info.compression !=0))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Negative height with compression not valid",image);
    if (bmp_info.planes != 1)
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Static planes value not equal 1",image);
    if ((bmp_info.bits_per_pixel != 1) && (bmp_info.bits_per_pixel != 4) &&
        (bmp_info.bits_per_pixel != 8) && (bmp_info.bits_per_pixel != 16) &&
        (bmp_info.bits_per_pixel != 24) && (bmp_info.bits_per_pixel != 32))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Invalid bits per pixel",image);
    if (bmp_info.number_colors > (1L << bmp_info.bits_per_pixel))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Invalid number of colors",image);
    if (bmp_info.compression > 3)
     ThrowReaderException(CorruptImageWarning,
       "Corrupt BMP image: Invalid Compression",image);
    if ((bmp_info.compression == 1) && (bmp_info.bits_per_pixel != 8))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Invalid bits per pixel for RLE8 compression",image);
    if ((bmp_info.compression == 2) && (bmp_info.bits_per_pixel != 4))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Invalid bits per pixel for RLE4 compression",image);
    if ((bmp_info.compression == 3) && (bmp_info.bits_per_pixel < 16))
      ThrowReaderException(CorruptImageWarning,
        "Corrupt BMP image: Invalid bits per pixel for compression",image);
    switch (bmp_info.compression)
    {
      case BI_RGB:
      case BI_RLE8:
      case BI_RLE4:
      case BI_BITFIELDS:
        break;
      case BI_JPEG:
        ThrowReaderException(CorruptImageError,"JPEG compression not supported",
          image)
      case BI_PNG:
        ThrowReaderException(CorruptImageError,"PNG compression not supported",
          image)
      default:
        ThrowReaderException(CorruptImageError,
          "Unrecognized compression method",image)
    }
    image->columns=bmp_info.width;
    image->rows=AbsoluteValue(bmp_info.height);
    image->depth=8;
    image->matte=bmp_info.alpha_mask != 0;
    if ((bmp_info.number_colors != 0) || (bmp_info.bits_per_pixel < 16))
      {
        image->storage_class=PseudoClass;
        image->colors=bmp_info.number_colors;
        if (image->colors == 0)
          image->colors=1L << bmp_info.bits_per_pixel;
      }
    if (image->storage_class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        unsigned int
          packet_size;

        /*
          Read BMP raster colormap.
        */
        if (logging)
          LogMagickEvent(CoderEvent,"   Reading colormap of %ld colors",
            image->colors);
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitError,"Memory allocation failed",
            image);
        bmp_colormap=(unsigned char *) AcquireMemory(4*image->colors);
        if (bmp_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"Memory allocation failed",
            image);
        if ((bmp_info.size == 12) || (bmp_info.size == 64))
          packet_size=3;
        else
          packet_size=4;
        (void) SeekBlob(image,start_position+14+bmp_info.size,SEEK_SET);
        (void) ReadBlob(image,packet_size*image->colors,(char *) bmp_colormap);
        p=bmp_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          image->colormap[i].blue=ScaleCharToQuantum(*p++);
          image->colormap[i].green=ScaleCharToQuantum(*p++);
          image->colormap[i].red=ScaleCharToQuantum(*p++);
          if (packet_size == 4)
            p++;
        }
        LiberateMemory((void **) &bmp_colormap);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Read image data.
    */
    (void) SeekBlob(image,start_position+bmp_info.offset_bits,SEEK_SET);
    if (bmp_info.compression == BI_RLE4)
      bmp_info.bits_per_pixel<<=1;
    bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
    length=bytes_per_line*image->rows;
    pixels=(unsigned char *)
      AcquireMemory(Max(bytes_per_line,image->columns+1)*image->rows);
    if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitError,"Memory allocation failed",
        image);
    if ((bmp_info.compression == BI_RGB) ||
        (bmp_info.compression == BI_BITFIELDS))
      {
        if (logging)
          LogMagickEvent(CoderEvent,"  Reading pixels (%ld bytes)",
            (long) length);
        (void) ReadBlob(image,length,(char *) pixels);
      }
    else
      {
        /*
          Convert run-length encoded raster pixels.
        */
        status=DecodeImage(image,bmp_info.compression,pixels);
        if (status == False)
          ThrowReaderException(CorruptImageError,"runlength decoding failed",
            image);
      }
    /*
      Initialize image structure.
    */
    image->units=PixelsPerCentimeterResolution;
    image->x_resolution=bmp_info.x_pixels/100.0;
    image->y_resolution=bmp_info.y_pixels/100.0;
    /*
      Convert BMP raster image to pixel packets.
    */
    if (bmp_info.compression == BI_RGB)
      {
        bmp_info.alpha_mask=0;
        bmp_info.red_mask=0x00ff0000L;
        bmp_info.green_mask=0x0000ff00L;
        bmp_info.blue_mask=0x000000ffL;
        if (bmp_info.bits_per_pixel == 16)
          {
            /*
              RGB555.
            */
            bmp_info.red_mask=0x00007c00L;
            bmp_info.green_mask=0x000003e0L;
            bmp_info.blue_mask=0x0000001fL;
          }
      }
    if ((bmp_info.bits_per_pixel == 16) || (bmp_info.bits_per_pixel == 32))
      {
        register unsigned long
          sample;

        /*
          Get shift and quantum bits info from bitfield masks.
        */
        memset(&shift,0,sizeof(PixelPacket));
        memset(&quantum_bits,0,sizeof(PixelPacket));
        if (bmp_info.red_mask != 0)
          while (((bmp_info.red_mask << shift.red) & 0x80000000UL) == 0)
            shift.red++;
        if (bmp_info.green_mask != 0)
          while (((bmp_info.green_mask << shift.green) & 0x80000000UL) == 0)
            shift.green++;
        if (bmp_info.blue_mask != 0)
          while (((bmp_info.blue_mask << shift.blue) & 0x80000000UL) == 0)
            shift.blue++;
        if (bmp_info.alpha_mask != 0)
          while (((bmp_info.alpha_mask << shift.opacity) & 0x80000000UL) == 0)
            shift.opacity++;
        sample=shift.red;
        while (((bmp_info.red_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.red=(Quantum) (sample-shift.red);
        sample=shift.green;
        while (((bmp_info.green_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.green=(Quantum) (sample-shift.green);
        sample=shift.blue;
        while (((bmp_info.blue_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.blue=(Quantum) (sample-shift.blue);
        sample=shift.opacity;
        while (((bmp_info.alpha_mask << sample) & 0x80000000UL) != 0)
          sample++;
        quantum_bits.opacity=(Quantum) (sample-shift.opacity);
      }
    switch (bmp_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < ((long) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
            {
              index=((*p) & (0x80 >> bit)) ? 0x01 : 0x00;
              indexes[x+bit]=index;
              *q++=image->colormap[index];
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (long) (image->columns % 8); bit++)
              {
                index=((*p) & (0x80 >> bit)) ? 0x01 : 0x00;
                indexes[x+bit]=index;
                *q++=image->colormap[index];
              }
              p++;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      case 4:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < ((long) image->columns-1); x+=2)
          {
            index=ConstrainColormapIndex(image,(*p >> 4) & 0xf);
            indexes[x]=index;
            *q++=image->colormap[index];
            index=ConstrainColormapIndex(image,*p & 0xf);
            indexes[x+1]=index;
            *q++=image->colormap[index];
            p++;
          }
          if ((image->columns % 2) != 0)
            {
              index=ConstrainColormapIndex(image,(*p >> 4) & 0xf);
              indexes[x]=index;
              *q++=image->colormap[index];
              p++;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        if ((bmp_info.compression == BI_RLE8) ||
            (bmp_info.compression == BI_RLE4))
          bytes_per_line=image->columns;
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            index=ConstrainColormapIndex(image,*p);
            indexes[x]=index;
            *q=image->colormap[index];
            p++;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      case 16:
      {
        unsigned long
          pixel;

        /*
          Convert bitfield encoded 16-bit PseudoColor scanline.
        */
        if (bmp_info.compression != BI_RGB &&
            bmp_info.compression != BI_BITFIELDS)
          ThrowReaderException(CorruptImageError,
            "Compression mode != 0 or 3 in 16-bit BMP image file",image)
        bytes_per_line=2*(image->columns+image->columns%2);
        image->storage_class=DirectClass;
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=(*p++);
            pixel|=(*p++) << 8;
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            opacity=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            q->red=ScaleShortToQuantum(red);
            if (quantum_bits.red == 5)
              q->red|=(q->red >> 5);
            q->green=ScaleShortToQuantum(green);
            if (quantum_bits.green == 5)
              q->green|=(q->green >> 5);
            if (quantum_bits.green == 6)
              q->green|=(q->green >> 6);
            q->blue=ScaleShortToQuantum(blue);
            if (quantum_bits.blue == 5)
              q->blue|=(q->blue >> 5);
            if (image->matte)
              q->opacity=ScaleShortToQuantum(opacity);
#if (QuantumDepth > 8)
            if (quantum_bits.blue <= 8)
              q->blue|=((q->blue & 0xff00) >> 8);
            if (quantum_bits.green <= 8)
              q->green|=((q->green & 0xff00) >> 8);
            if (quantum_bits.red <= 8)
              q->red|=((q->red & 0xff00) >> 8);
            if (quantum_bits.opacity <= 8)
              q->opacity|=((q->opacity & 0xff00) >> 8);
#endif
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      case 24:
      {
        /*
          Convert DirectColor scanline.
        */
        bytes_per_line=4*((image->columns*24+31)/32);
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->blue=ScaleCharToQuantum(*p++);
            q->green=ScaleCharToQuantum(*p++);
            q->red=ScaleCharToQuantum(*p++);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      case 32:
      {
        /*
          Convert bitfield encoded DirectColor scanline.
        */
        if ((bmp_info.compression != BI_RGB) &&
            (bmp_info.compression != BI_BITFIELDS))
          ThrowReaderException(CorruptImageError,
            "Compression mode != 0 or 3 in 32-bit BMP image file",image)
        bytes_per_line=4*(image->columns);
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          unsigned long
            pixel;

          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=(*p++);
            pixel|=(*p++ << 8);
            pixel|=(*p++ << 16);
            pixel|=(*p++ << 24);
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            opacity=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            q->red=ScaleShortToQuantum(red);
            q->green=ScaleShortToQuantum(green);
            q->blue=ScaleShortToQuantum(blue);
            if (image->matte)
              q->opacity=ScaleShortToQuantum(opacity);
#if (QuantumDepth > 8)
            if (quantum_bits.red == 8)
              q->red|=(q->red >> 8);
            if (quantum_bits.green == 8)
              q->green|=(q->green >> 8);
            if (quantum_bits.blue == 8)
              q->blue|=(q->blue >> 8);
            if (quantum_bits.opacity == 8)
              q->opacity|=(q->opacity >> 8);
#endif
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              {
                status=MagickMonitor(LoadImageText,image->rows-y-1,image->rows,
                  exception);
                if (status == False)
                  break;
              }
        }
        break;
      }
      default:
        ThrowReaderException(CorruptImageError,"Not a BMP image file",image)
    }
    LiberateMemory((void **) &pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageError,"Unexpected end-of-file",image);
    if (bmp_info.height < 0)
      {
        Image
          *flipped_image;

        /*
          Correct image orientation.
        */
        flipped_image=FlipImage(image,exception);
        if (flipped_image == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        DestroyImage(image);
        image=flipped_image;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    *magick='\0';
    if (bmp_info.ba_offset != 0)
      (void) SeekBlob(image,bmp_info.ba_offset,SEEK_SET);
    (void) ReadBlob(image,2,(char *) magick);
    if (IsBMP(magick,2))
      {
        /*
          Acquire next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=image->next;
        status=MagickMonitor(LoadImagesText,TellBlob(image),GetBlobSize(image),
          exception);
        if (status == False)
          break;
      }
  } while (IsBMP(magick,2));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  if (logging)
    LogMagickEvent(CoderEvent," End ReadBMPImage()");
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r B M P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterBMPImage adds attributes for the BMP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterBMPImage method is:
%
%      RegisterBMPImage(void)
%
*/
ModuleExport void RegisterBMPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("BMP");
  entry->decoder=ReadBMPImage;
  entry->encoder=WriteBMPImage;
  entry->magick=IsBMP;
  entry->description=AcquireString("Microsoft Windows bitmap image");
  entry->module=AcquireString("BMP");
  entry->adjoin=False;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("BMP2");
  entry->encoder=WriteBMPImage;
  entry->magick=IsBMP;
  entry->description=AcquireString("Microsoft Windows bitmap image v2");
  entry->module=AcquireString("BMP2");
  entry->adjoin=False;
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("BMP3");
  entry->encoder=WriteBMPImage;
  entry->magick=IsBMP;
  entry->description=AcquireString("Microsoft Windows bitmap image v3");
  entry->module=AcquireString("BMP3");
  entry->adjoin=False;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r B M P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterBMPImage removes format registrations made by the
%  BMP module from the list of supported formats.
%
%  The format of the UnregisterBMPImage method is:
%
%      UnregisterBMPImage(void)
%
*/
ModuleExport void UnregisterBMPImage(void)
{
  (void) UnregisterMagickInfo("BMP");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e B M P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBMPImage writes an image in Microsoft Windows bitmap encoded
%  image format, version 3 for Windows or (if the image has a matte channel)
%  version 4.
%
%  The format of the WriteBMPImage method is:
%
%      unsigned int WriteBMPImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteBMPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteBMPImage(const ImageInfo *image_info,Image *image)
{
  BMPInfo
    bmp_info;

  int
    logging;

  long
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *bmp_data,
    *pixels;

  unsigned int
    have_color_info,
    status;

  unsigned long
    bytes_per_line,
    scene,
    type;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent," Begin WriteBMPImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  type=4;
  if (LocaleCompare(image_info->magick,"BMP2") == 0)
    type=2;
  else
    if (LocaleCompare(image_info->magick,"BMP3") == 0)
      type=3;
  scene=0;
  do
  {
    /*
      Initialize BMP raster file header.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    memset(&bmp_info,0,sizeof(BMPInfo));
    bmp_info.file_size=14+12;
    if (type > 2)
      bmp_info.file_size+=28;
    bmp_info.offset_bits=bmp_info.file_size;
    bmp_info.compression=BI_RGB;
    if (image->storage_class != DirectClass)
      {
        /*
          Colormapped BMP raster.
        */
        bmp_info.bits_per_pixel=8;
        if (IsMonochromeImage(image,&image->exception))
          bmp_info.bits_per_pixel=1;
        bmp_info.number_colors=1 << bmp_info.bits_per_pixel;
        if (image->matte)
          SetImageType(image,TrueColorMatteType);
        else
          if (bmp_info.number_colors < image->colors)
            SetImageType(image,TrueColorType);
          else
            {
              bmp_info.file_size+=4*(1 << bmp_info.bits_per_pixel);
              bmp_info.offset_bits+=4*(1 << bmp_info.bits_per_pixel);
            }
      }
    if (image->storage_class == DirectClass)
      {
        /*
          Full color BMP raster.
        */
        bmp_info.number_colors=0;
        bmp_info.bits_per_pixel=((type > 3) && image->matte) ? 32 : 24;
        bmp_info.compression=
          (type > 3) && image->matte ?  BI_BITFIELDS : BI_RGB;
      }
    bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
    bmp_info.ba_offset=0;
    have_color_info=(int) ((image->rendering_intent != UndefinedIntent) ||
      (image->color_profile.length != 0) || (image->gamma != 0.0));
    if (type == 2)
      bmp_info.size=12;
    else
      if ((type == 3) || (!image->matte && !have_color_info))
        {
          type=3;
          bmp_info.size=40;
        }
      else
        {
          int
            extra_size;

          bmp_info.size=108;
          extra_size=68;
          if ((image->rendering_intent != UndefinedIntent) ||
              (image->color_profile.length != 0))
            {
              bmp_info.size=124;
              extra_size+=16;
            }
          bmp_info.file_size+=extra_size;
          bmp_info.offset_bits+=extra_size;
        }
    bmp_info.width=(long) image->columns;
    bmp_info.height=(long) image->rows;
    bmp_info.planes=1;
    bmp_info.image_size=bytes_per_line*image->rows;
    bmp_info.file_size+=bmp_info.image_size;
    bmp_info.x_pixels=75*39;
    bmp_info.y_pixels=75*39;
    if (image->units == PixelsPerInchResolution)
      {
        bmp_info.x_pixels=(unsigned long) (100.0*image->x_resolution/2.54);
        bmp_info.y_pixels=(unsigned long) (100.0*image->y_resolution/2.54);
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        bmp_info.x_pixels=(unsigned long) (100.0*image->x_resolution);
        bmp_info.y_pixels=(unsigned long) (100.0*image->y_resolution);
      }
    bmp_info.colors_important=bmp_info.number_colors;
    /*
      Convert MIFF to BMP raster pixels.
    */
    pixels=(unsigned char *) AcquireMemory(bmp_info.image_size);
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"Memory allocation failed",image);
    switch (bmp_info.bits_per_pixel)
    {
      case 1:
      {
        register unsigned char
          bit,
          byte;

        /*
          Convert PseudoClass image to a BMP monochrome image.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          q=pixels+(image->rows-y-1)*bytes_per_line;
          bit=0;
          byte=0;
          for (x=0; x < (long) image->columns; x++)
          {
            byte<<=1;
            byte|=indexes[x] ? 0x01 : 0x00;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
             p++;
           }
         if (bit != 0)
           *q++=byte << (8-bit);
         if (image->previous == (Image *) NULL)
           if (QuantumTick(y,image->rows))
             if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
               break;
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoClass packet to BMP pixel.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          q=pixels+(image->rows-y-1)*bytes_per_line;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=indexes[x];
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
      case 24:
      case 32:
      {
        /*
          Convert DirectClass packet to BMP BGR888 or BGRA8888 pixel.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels+(image->rows-y-1)*bytes_per_line;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=ScaleQuantumToChar(p->blue);
            *q++=ScaleQuantumToChar(p->green);
            *q++=ScaleQuantumToChar(p->red);
            if (bmp_info.bits_per_pixel == 32)
              *q++=ScaleQuantumToChar(p->opacity);
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
    }
    if ((type > 2) && (bmp_info.bits_per_pixel == 8))
      if (image_info->compression != NoCompression)
        {
          size_t
            length;

          /*
            Convert run-length encoded raster pixels.
          */
          length=2*(bytes_per_line+2)*(image->rows+2)+2;
          bmp_data=(unsigned char *) AcquireMemory(length);
          if (pixels == (unsigned char *) NULL)
            {
              LiberateMemory((void **) &pixels);
              ThrowWriterException(ResourceLimitError,
                "Memory allocation failed",image)
            }
          bmp_info.file_size-=bmp_info.image_size;
          bmp_info.image_size=EncodeImage(image,bytes_per_line,pixels,bmp_data);
          bmp_info.file_size+=bmp_info.image_size;
          LiberateMemory((void **) &pixels);
          pixels=bmp_data;
          bmp_info.compression=BI_RLE8;
        }
    /*
      Write BMP for Windows, all versions, 14-byte header.
    */
    if (logging)
      {
        LogMagickEvent(CoderEvent,"   Writing BMP version %ld datastream",type);
        LogMagickEvent(CoderEvent,"   bits_per_pixel=%d",
          bmp_info.bits_per_pixel);
        switch ((int) bmp_info.compression)
        {
           case BI_RGB:
           {
             LogMagickEvent(CoderEvent,"   compression=BI_RGB");
             break;
           }
           case BI_RLE8:
           {
             LogMagickEvent(CoderEvent,"   compression=BI_RLE8");
             break;
           }
           case BI_BITFIELDS:
           {
             LogMagickEvent(CoderEvent,"   compression=BI_BITFIELDS");
             break;
           }
           default:
           {
             LogMagickEvent(CoderEvent,"   compression=UNKNOWN (%lu)",
               bmp_info.compression);
           }
        }
        LogMagickEvent(CoderEvent,"   number_colors=%lu",
          bmp_info.number_colors);
      }
    (void) WriteBlob(image,2,"BM");
    (void) WriteBlobLSBLong(image,bmp_info.file_size);
    (void) WriteBlobLSBLong(image,bmp_info.ba_offset);  /* always 0 */
    (void) WriteBlobLSBLong(image,bmp_info.offset_bits);
    if (type == 2)
      {
        /*
          Write 12-byte version 2 bitmap header.
        */
        (void) WriteBlobLSBLong(image,bmp_info.size);
        (void) WriteBlobLSBShort(image,bmp_info.width);
        (void) WriteBlobLSBShort(image,bmp_info.height);
        (void) WriteBlobLSBShort(image,bmp_info.planes);
        (void) WriteBlobLSBShort(image,bmp_info.bits_per_pixel);
      }
    else
      {
        /*
          Write 40-byte version 3+ bitmap header.
        */
        (void) WriteBlobLSBLong(image,bmp_info.size);
        (void) WriteBlobLSBLong(image,bmp_info.width);
        (void) WriteBlobLSBLong(image,bmp_info.height);
        (void) WriteBlobLSBShort(image,bmp_info.planes);
        (void) WriteBlobLSBShort(image,bmp_info.bits_per_pixel);
        (void) WriteBlobLSBLong(image,bmp_info.compression);
        (void) WriteBlobLSBLong(image,bmp_info.image_size);
        (void) WriteBlobLSBLong(image,bmp_info.x_pixels);
        (void) WriteBlobLSBLong(image,bmp_info.y_pixels);
        (void) WriteBlobLSBLong(image,bmp_info.number_colors);
        (void) WriteBlobLSBLong(image,bmp_info.colors_important);
      }
    if ((type > 3) && (image->matte || have_color_info))
      {
        /*
          Write the rest of the 108-byte BMP Version 4 header.
        */
        (void) WriteBlobLSBLong(image,0x00ff0000L);  /* Red mask */
        (void) WriteBlobLSBLong(image,0x0000ff00L);  /* Green mask */
        (void) WriteBlobLSBLong(image,0x000000ffL);  /* Blue mask */
        (void) WriteBlobLSBLong(image,0xff000000UL);  /* Alpha mask */
        (void) WriteBlobLSBLong(image,0x00000001L);  /* CSType==Calib. RGB */
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.red_primary.x*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.red_primary.y*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) (1.000f-(image->chromaticity.red_primary.x
          +image->chromaticity.red_primary.y)*0x3ffffff));
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.green_primary.x*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.green_primary.y*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) (1.000f-(image->chromaticity.green_primary.x
          +image->chromaticity.green_primary.y)*0x3ffffff));
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.blue_primary.x*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) image->chromaticity.blue_primary.y*0x3ffffff);
        (void) WriteBlobLSBLong(image,
          (long) (1.000f-(image->chromaticity.blue_primary.x
          +image->chromaticity.blue_primary.y)*0x3ffffff));
        (void) WriteBlobLSBLong(image,(long) bmp_info.gamma_scale.x*0xffff);
        (void) WriteBlobLSBLong(image,(long) bmp_info.gamma_scale.y*0xffff);
        (void) WriteBlobLSBLong(image,(long) bmp_info.gamma_scale.z*0xffff);
        if ((image->rendering_intent != UndefinedIntent) ||
           (image->color_profile.length != 0))
          {
            long
              intent;

            switch ((int) image->rendering_intent)
            {
              case SaturationIntent:
              {
                intent=LCS_GM_BUSINESS;
                break;
              }
              case RelativeIntent:
              {
                intent=LCS_GM_GRAPHICS;
                break;
              }
              case PerceptualIntent:
              {
                intent=LCS_GM_IMAGES;
                break;
              }
              case AbsoluteIntent:
              {
                intent=LCS_GM_ABS_COLORIMETRIC;
                break;
              }
              default:
              {
                intent=0;
                break;
              }
            }
            (void) WriteBlobLSBLong(image,intent);
            (void) WriteBlobLSBLong(image,0x0);  /* dummy profile data */
            (void) WriteBlobLSBLong(image,0x0);  /* dummy profile length */
            (void) WriteBlobLSBLong(image,0x0);  /* reserved */
          }
      }
    if (image->storage_class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        /*
          Dump colormap to file.
        */
        if (logging)
          LogMagickEvent(CoderEvent,"   Writing colormap with %ld entries",
            image->colors);
        bmp_colormap=(unsigned char *)
          AcquireMemory(4*(1 << bmp_info.bits_per_pixel));
        if (bmp_colormap == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,"Memory allocation failed",
            image);
        q=bmp_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          *q++=ScaleQuantumToChar(image->colormap[i].blue);
          *q++=ScaleQuantumToChar(image->colormap[i].green);
          *q++=ScaleQuantumToChar(image->colormap[i].red);
          *q++=(Quantum) 0x0;
        }
        for ( ; i < (1L << bmp_info.bits_per_pixel); i++)
        {
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
        }
        (void) WriteBlob(image,4*(1 << bmp_info.bits_per_pixel),
          (char *) bmp_colormap);
        LiberateMemory((void **) &bmp_colormap);
      }
    if (logging)
      LogMagickEvent(CoderEvent,"   Writing pixels (%lu bytes)",
        bmp_info.image_size);
    (void) WriteBlob(image,bmp_info.image_size,(char *) pixels);
    LiberateMemory((void **) &pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    status=MagickMonitor(SaveImagesText,scene++,GetImageListSize(image),
      &image->exception);
    if (status == False)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  if (logging)
    LogMagickEvent(CoderEvent," End WriteBMPImage()");
  return(True);
}
