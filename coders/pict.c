/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        PPPP   IIIII   CCCC  TTTTT                           %
%                        P   P    I    C        T                             %
%                        PPPP     I    C        T                             %
%                        P        I    C        T                             %
%                        P      IIIII   CCCC    T                             %
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
  ImageMagick Macintosh PICT Methods.
*/
#define ReadPixmap(pixmap) \
{ \
  pixmap.version=ReadBlobMSBShort(image); \
  pixmap.pack_type=ReadBlobMSBShort(image); \
  pixmap.pack_size=ReadBlobMSBLong(image); \
  pixmap.horizontal_resolution=ReadBlobMSBLong(image); \
  pixmap.vertical_resolution=ReadBlobMSBLong(image); \
  pixmap.pixel_type=ReadBlobMSBShort(image); \
  pixmap.bits_per_pixel=ReadBlobMSBShort(image); \
  pixmap.component_count=ReadBlobMSBShort(image); \
  pixmap.component_size=ReadBlobMSBShort(image); \
  pixmap.plane_bytes=ReadBlobMSBLong(image); \
  pixmap.table=ReadBlobMSBLong(image); \
  pixmap.reserved=ReadBlobMSBLong(image); \
}

#define ReadRectangle(rectangle) \
{ \
  rectangle.top=ReadBlobMSBShort(image); \
  rectangle.left=ReadBlobMSBShort(image); \
  rectangle.bottom=ReadBlobMSBShort(image); \
  rectangle.right=ReadBlobMSBShort(image); \
}

typedef struct _PICTCode
{
  char
    *name;

  int
    length;

  char
    *description;
} PICTCode;

typedef struct _PICTPixmap
{
  short
    version,
    pack_type;

  long int
    pack_size,
    horizontal_resolution,
    vertical_resolution;

  short
    pixel_type,
    bits_per_pixel,
    component_count,
    component_size;

  long int
    plane_bytes,
    table,
    reserved;
} PICTPixmap;

typedef struct _PICTRectangle
{
  short
    top,
    left,
    bottom,
    right;
} PICTRectangle;

const PICTCode
  codes[] =
  {
    /* 0x00 */ { (char *) "NOP", 0, (char *) "nop" },
    /* 0x01 */ { (char *) "Clip", 0, (char *) "clip" },
    /* 0x02 */ { (char *) "BkPat", 8, (char *) "background pattern" },
    /* 0x03 */ { (char *) "TxFont", 2, (char *) "text font (word)" },
    /* 0x04 */ { (char *) "TxFace", 1, (char *) "text face (byte)" },
    /* 0x05 */ { (char *) "TxMode", 2, (char *) "text mode (word)" },
    /* 0x06 */ { (char *) "SpExtra", 4, (char *) "space extra (fixed point)" },
    /* 0x07 */ { (char *) "PnSize", 4, (char *) "pen size (point)" },
    /* 0x08 */ { (char *) "PnMode", 2, (char *) "pen mode (word)" },
    /* 0x09 */ { (char *) "PnPat", 8, (char *) "pen pattern" },
    /* 0x0a */ { (char *) "FillPat", 8, (char *) "fill pattern" },
    /* 0x0b */ { (char *) "OvSize", 4, (char *) "oval size (point)" },
    /* 0x0c */ { (char *) "Origin", 4, (char *) "dh, dv (word)" },
    /* 0x0d */ { (char *) "TxSize", 2, (char *) "text size (word)" },
    /* 0x0e */ { (char *) "FgColor", 4, (char *) "foreground color (longword)" },
    /* 0x0f */ { (char *) "BkColor", 4, (char *) "background color (longword)" },
    /* 0x10 */ { (char *) "TxRatio", 8, (char *) "numerator (point), denominator (point)" },
    /* 0x11 */ { (char *) "Version", 1, (char *) "version (byte)" },
    /* 0x12 */ { (char *) "BkPixPat", 0, (char *) "color background pattern" },
    /* 0x13 */ { (char *) "PnPixPat", 0, (char *) "color pen pattern" },
    /* 0x14 */ { (char *) "FillPixPat", 0, (char *) "color fill pattern" },
    /* 0x15 */ { (char *) "PnLocHFrac", 2, (char *) "fractional pen position" },
    /* 0x16 */ { (char *) "ChExtra", 2, (char *) "extra for each character" },
    /* 0x17 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x18 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x19 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x1a */ { (char *) "RGBFgCol", 6, (char *) "RGB foreColor" },
    /* 0x1b */ { (char *) "RGBBkCol", 6, (char *) "RGB backColor" },
    /* 0x1c */ { (char *) "HiliteMode", 0, (char *) "hilite mode flag" },
    /* 0x1d */ { (char *) "HiliteColor", 6, (char *) "RGB hilite color" },
    /* 0x1e */ { (char *) "DefHilite", 0, (char *) "Use default hilite color" },
    /* 0x1f */ { (char *) "OpColor", 6, (char *) "RGB OpColor for arithmetic modes" },
    /* 0x20 */ { (char *) "Line", 8, (char *) "pnLoc (point), newPt (point)" },
    /* 0x21 */ { (char *) "LineFrom", 4, (char *) "newPt (point)" },
    /* 0x22 */ { (char *) "ShortLine", 6, (char *) "pnLoc (point, dh, dv (-128 .. 127))" },
    /* 0x23 */ { (char *) "ShortLineFrom", 2, (char *) "dh, dv (-128 .. 127)" },
    /* 0x24 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x25 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x26 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x27 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x28 */ { (char *) "LongText", 0, (char *) "txLoc (point), count (0..255), text" },
    /* 0x29 */ { (char *) "DHText", 0, (char *) "dh (0..255), count (0..255), text" },
    /* 0x2a */ { (char *) "DVText", 0, (char *) "dv (0..255), count (0..255), text" },
    /* 0x2b */ { (char *) "DHDVText", 0, (char *) "dh, dv (0..255), count (0..255), text" },
    /* 0x2c */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2d */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2e */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x2f */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x30 */ { (char *) "frameRect", 8, (char *) "rect" },
    /* 0x31 */ { (char *) "paintRect", 8, (char *) "rect" },
    /* 0x32 */ { (char *) "eraseRect", 8, (char *) "rect" },
    /* 0x33 */ { (char *) "invertRect", 8, (char *) "rect" },
    /* 0x34 */ { (char *) "fillRect", 8, (char *) "rect" },
    /* 0x35 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x36 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x37 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x38 */ { (char *) "frameSameRect", 0, (char *) "rect" },
    /* 0x39 */ { (char *) "paintSameRect", 0, (char *) "rect" },
    /* 0x3a */ { (char *) "eraseSameRect", 0, (char *) "rect" },
    /* 0x3b */ { (char *) "invertSameRect", 0, (char *) "rect" },
    /* 0x3c */ { (char *) "fillSameRect", 0, (char *) "rect" },
    /* 0x3d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x3e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x3f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x40 */ { (char *) "frameRRect", 8, (char *) "rect" },
    /* 0x41 */ { (char *) "paintRRect", 8, (char *) "rect" },
    /* 0x42 */ { (char *) "eraseRRect", 8, (char *) "rect" },
    /* 0x43 */ { (char *) "invertRRect", 8, (char *) "rect" },
    /* 0x44 */ { (char *) "fillRRrect", 8, (char *) "rect" },
    /* 0x45 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x46 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x47 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x48 */ { (char *) "frameSameRRect", 0, (char *) "rect" },
    /* 0x49 */ { (char *) "paintSameRRect", 0, (char *) "rect" },
    /* 0x4a */ { (char *) "eraseSameRRect", 0, (char *) "rect" },
    /* 0x4b */ { (char *) "invertSameRRect", 0, (char *) "rect" },
    /* 0x4c */ { (char *) "fillSameRRect", 0, (char *) "rect" },
    /* 0x4d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x4e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x4f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x50 */ { (char *) "frameOval", 8, (char *) "rect" },
    /* 0x51 */ { (char *) "paintOval", 8, (char *) "rect" },
    /* 0x52 */ { (char *) "eraseOval", 8, (char *) "rect" },
    /* 0x53 */ { (char *) "invertOval", 8, (char *) "rect" },
    /* 0x54 */ { (char *) "fillOval", 8, (char *) "rect" },
    /* 0x55 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x56 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x57 */ { (char *) "reserved", 8, (char *) "reserved for Apple use" },
    /* 0x58 */ { (char *) "frameSameOval", 0, (char *) "rect" },
    /* 0x59 */ { (char *) "paintSameOval", 0, (char *) "rect" },
    /* 0x5a */ { (char *) "eraseSameOval", 0, (char *) "rect" },
    /* 0x5b */ { (char *) "invertSameOval", 0, (char *) "rect" },
    /* 0x5c */ { (char *) "fillSameOval", 0, (char *) "rect" },
    /* 0x5d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x5e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x5f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x60 */ { (char *) "frameArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x61 */ { (char *) "paintArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x62 */ { (char *) "eraseArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x63 */ { (char *) "invertArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x64 */ { (char *) "fillArc", 12, (char *) "rect, startAngle, arcAngle" },
    /* 0x65 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x66 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x67 */ { (char *) "reserved", 12, (char *) "reserved for Apple use" },
    /* 0x68 */ { (char *) "frameSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x69 */ { (char *) "paintSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6a */ { (char *) "eraseSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6b */ { (char *) "invertSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6c */ { (char *) "fillSameArc", 4, (char *) "rect, startAngle, arcAngle" },
    /* 0x6d */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x6e */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x6f */ { (char *) "reserved", 4, (char *) "reserved for Apple use" },
    /* 0x70 */ { (char *) "framePoly", 0, (char *) "poly" },
    /* 0x71 */ { (char *) "paintPoly", 0, (char *) "poly" },
    /* 0x72 */ { (char *) "erasePoly", 0, (char *) "poly" },
    /* 0x73 */ { (char *) "invertPoly", 0, (char *) "poly" },
    /* 0x74 */ { (char *) "fillPoly", 0, (char *) "poly" },
    /* 0x75 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x76 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x77 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x78 */ { (char *) "frameSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x79 */ { (char *) "paintSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7a */ { (char *) "eraseSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7b */ { (char *) "invertSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7c */ { (char *) "fillSamePoly", 0, (char *) "poly (NYI)" },
    /* 0x7d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x7e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x7f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x80 */ { (char *) "frameRgn", 0, (char *) "region" },
    /* 0x81 */ { (char *) "paintRgn", 0, (char *) "region" },
    /* 0x82 */ { (char *) "eraseRgn", 0, (char *) "region" },
    /* 0x83 */ { (char *) "invertRgn", 0, (char *) "region" },
    /* 0x84 */ { (char *) "fillRgn", 0, (char *) "region" },
    /* 0x85 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x86 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x87 */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x88 */ { (char *) "frameSameRgn", 0, (char *) "region (NYI)" },
    /* 0x89 */ { (char *) "paintSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8a */ { (char *) "eraseSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8b */ { (char *) "invertSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8c */ { (char *) "fillSameRgn", 0, (char *) "region (NYI)" },
    /* 0x8d */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x8e */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x8f */ { (char *) "reserved", 0, (char *) "reserved for Apple use" },
    /* 0x90 */ { (char *) "BitsRect", 0, (char *) "copybits, rect clipped" },
    /* 0x91 */ { (char *) "BitsRgn", 0, (char *) "copybits, rgn clipped" },
    /* 0x92 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x93 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x94 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x95 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x96 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x97 */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x98 */ { (char *) "PackBitsRect", 0, (char *) "packed copybits, rect clipped" },
    /* 0x99 */ { (char *) "PackBitsRgn", 0, (char *) "packed copybits, rgn clipped" },
    /* 0x9a */ { (char *) "DirectBitsRect", 0, (char *) "PixMap, srcRect, dstRect, mode, PixData" },
    /* 0x9b */ { (char *) "DirectBitsRgn", 0, (char *) "PixMap, srcRect, dstRect, mode, maskRgn, PixData" },
    /* 0x9c */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9d */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9e */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0x9f */ { (char *) "reserved", -1, (char *) "reserved for Apple use" },
    /* 0xa0 */ { (char *) "ShortComment", 2, (char *) "kind (word)" },
    /* 0xa1 */ { (char *) "LongComment", 0, (char *) "kind (word), size (word), data" }
  };

/*
  Forward declarations.
*/
static unsigned int
  WritePICTImage(const ImageInfo *,Image *);

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
%  Method DecodeImage decompresses an image via Macintosh pack bits
%  decoding for Macintosh PICT images.
%
%  The format of the DecodeImage method is:
%
%      unsigned char* DecodeImage(const ImageInfo *image_info,Image *blob,
%        Image *image,int bytes_per_line,const int bits_per_pixel)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o blob,image: The address of a structure of type Image.
%
%    o bytes_per_line: This integer identifies the number of bytes in a
%      scanline.
%
%    o bits_per_pixel: The number of bits in a pixel.
%
%
*/

static unsigned char *ExpandBuffer(unsigned char *pixels,int *bytes_per_line,
  const int bits_per_pixel)
{
  register int
    i;

  register unsigned char
    *p,
    *q;

  static unsigned char
    scanline[8*256];

  p=pixels;
  q=scanline;
  switch (bits_per_pixel)
  {
    case 8:
    case 16:
    case 32:
      return(pixels);
    case 4:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 4) & 0xff;
        *q++=(*p & 15);
        p++;
      }
      *bytes_per_line*=2;
      break;
    }
    case 2:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 6) & 0x03;
        *q++=(*p >> 4) & 0x03;
        *q++=(*p >> 2) & 0x03;
        *q++=(*p & 3);
        p++;
      }
      *bytes_per_line*=4;
      break;
    }
    case 1:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 7) & 0x01;
        *q++=(*p >> 6) & 0x01;
        *q++=(*p >> 5) & 0x01;
        *q++=(*p >> 4) & 0x01;
        *q++=(*p >> 3) & 0x01;
        *q++=(*p >> 2) & 0x01;
        *q++=(*p >> 1) & 0x01;
        *q++=(*p & 0x01);
        p++;
      }
      *bytes_per_line*=8;
      break;
    }
    default:
      break;
  }
  return(scanline);
}

static unsigned char *DecodeImage(const ImageInfo *image_info,Image *blob,
  Image *image,int bytes_per_line,const int bits_per_pixel)
{
  int
    bytes_per_pixel,
    length,
    number_pixels,
    scanline_length,
    width,
    y;

  register int
    i,
    x;

  register unsigned char
    *p,
    *q;

  unsigned char
    *pixels,
    *scanline;

  unsigned short
    row_bytes;

  /*
    Determine pixel buffer size.
  */
  if (bits_per_pixel <= 8)
    bytes_per_line&=0x7fff;
  width=image->columns;
  bytes_per_pixel=1;
  if (bits_per_pixel == 16)
    {
      bytes_per_pixel=2;
      width*=2;
    }
  else
    if (bits_per_pixel == 32)
      width*=image->matte ? 4 : 3;
  if (bytes_per_line == 0)
    bytes_per_line=width;
  row_bytes=image->columns | 0x8000;
  if (image->storage_class == DirectClass)
    row_bytes=(4*image->columns) | 0x8000;
  /*
    Allocate pixel and scanline buffer.
  */
  pixels=(unsigned char *) AcquireMemory(row_bytes*image->rows);
  if (pixels == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  scanline=(unsigned char *) AcquireMemory(row_bytes);
  if (scanline == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  if (bytes_per_line < 8)
    {
      /*
        Pixels are already uncompressed.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=pixels+y*width;
        number_pixels=bytes_per_line;
        (void) ReadBlob(blob,number_pixels,(char *) scanline);
        p=ExpandBuffer(scanline,&number_pixels,bits_per_pixel);
        memcpy(q,p,number_pixels);
      }
      LiberateMemory((void **) &scanline);
      return(pixels);
    }
  /*
    Uncompress RLE pixels into uncompressed pixel buffer.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    q=pixels+y*width;
    if ((bytes_per_line > 250) || (bits_per_pixel > 8))
      scanline_length=ReadBlobMSBShort(blob);
    else
      scanline_length=ReadBlobByte(blob);
    (void) ReadBlob(blob,scanline_length,(char *) scanline);
    for (x=0; x < scanline_length; )
      if ((scanline[x] & 0x80) == 0)
        {
          length=(scanline[x] & 0xff)+1;
          number_pixels=length*bytes_per_pixel;
          p=ExpandBuffer(scanline+x+1,&number_pixels,bits_per_pixel);
          memcpy(q,p,number_pixels);
          q+=number_pixels;
          x+=length*bytes_per_pixel+1;
        }
      else
        {
          length=((scanline[x] ^ 0xff) & 0xff)+2;
          number_pixels=bytes_per_pixel;
          p=ExpandBuffer(scanline+x+1,&number_pixels,bits_per_pixel);
          for (i=0; i < length; i++)
          {
            memcpy(q,p,number_pixels);
            q+=number_pixels;
          }
          x+=bytes_per_pixel+1;
        }
  }
  LiberateMemory((void **) &scanline);
  return(pixels);
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
%  Method EncodeImage compresses an image via Macintosh pack bits encoding
%  for Macintosh PICT images.
%
%  The format of the EncodeImage method is:
%
%      count=EncodeImage(image,scanline,bytes_per_line,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns the number of encoded pixels.
%
%    o image: The address of a structure of type Image.
%
%    o scanline: A pointer to an array of characters to pack.
%
%    o bytes_per_line: The number of bytes in a scanline.
%
%    o pixels: A pointer to an array of characters where the packed
%      characters are stored.
%
%
*/
static size_t EncodeImage(Image *image,const unsigned char *scanline,
  const int bytes_per_line,unsigned char *pixels)
{
#define MaxCount  128
#define MaxPackbitsRunlength  128

  int
    count,
    repeat_count,
    runlength;

  register const unsigned char
    *p;

  register int
    i;

  register long
    packets;

  register unsigned char
    *q;

  unsigned char
    index;

  /*
    Pack scanline.
  */
  assert(image != (Image *) NULL);
  assert(scanline != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  count=0;
  runlength=0;
  p=scanline+(bytes_per_line-1);
  q=pixels;
  index=(*p);
  for (i=bytes_per_line-1; i >= 0; i--)
  {
    if (index == *p)
      runlength++;
    else
      {
        if (runlength < 3)
          while (runlength > 0)
          {
            *q++=index;
            runlength--;
            count++;
            if (count == MaxCount)
              {
                *q++=MaxCount-1;
                count-=MaxCount;
              }
          }
        else
          {
            if (count > 0)
              *q++=count-1;
            count=0;
            while (runlength > 0)
            {
              repeat_count=runlength;
              if (repeat_count > MaxPackbitsRunlength)
                repeat_count=MaxPackbitsRunlength;
              *q++=index;
              *q++=257-repeat_count;
              runlength-=repeat_count;
            }
          }
        runlength=1;
      }
    index=(*p);
    p--;
  }
  if (runlength < 3)
    while (runlength > 0)
    {
      *q++=index;
      runlength--;
      count++;
      if (count == MaxCount)
        {
          *q++=MaxCount-1;
          count-=MaxCount;
        }
    }
  else
    {
      if (count > 0)
        *q++=count-1;
      count=0;
      while (runlength > 0)
      {
        repeat_count=runlength;
        if (repeat_count > MaxPackbitsRunlength)
          repeat_count=MaxPackbitsRunlength;
        *q++=index;
        *q++=257-repeat_count;
        runlength-=repeat_count;
      }
    }
  if (count > 0)
    *q++=count-1;
  /*
    Write the number of and the packed packets.
  */
  packets=(int) (q-pixels);
  if (bytes_per_line > 250)
    {
      WriteBlobMSBShort(image,(unsigned short) packets);
      packets+=2;
    }
  else
    {
      (void) WriteBlobByte(image,packets);
      packets++;
    }
  while (q != pixels)
  {
    q--;
    (void) WriteBlobByte(image,*q);
  }
  return(packets);
}

#if !defined(macintosh)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P I C T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPICTImage reads an Apple Macintosh QuickDraw/PICT image file
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadPICTImage method is:
%
%      Image *ReadPICTImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPICTImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPICTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    geometry[MaxTextExtent];

  Image
    *image;

  IndexPacket
    index;

  int
    c,
    code,
    flags,
    version,
    y;

  long
    length;

  PICTRectangle
    frame;

  PICTPixmap
    pixmap;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    jpeg,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read PICT header.
  */
  for (i=0; i < 512; i++)
    (void) ReadBlobByte(image);  /* skip header */
  (void) ReadBlobMSBShort(image);  /* skip picture size */
  ReadRectangle(frame);
  while ((c=ReadBlobByte(image)) == 0);
  if (c != 0x11)
    ThrowReaderException(CorruptImageWarning,"Not a PICT image file",image);
  version=ReadBlobByte(image);
  if (version == 2)
    {
      c=ReadBlobByte(image);
      if (c != 0xff)
        ThrowReaderException(CorruptImageWarning,"Not a PICT image file",image);
    }
  else
    if (version != 1)
      ThrowReaderException(CorruptImageWarning,"Not a PICT image file",image);
  /*
    Create black canvas.
  */
  flags=0;
  image->columns=frame.right-frame.left;
  image->rows=frame.bottom-frame.top;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Interpret PICT opcodes.
  */
  code=0;
  jpeg=False;
  for ( ; ; )
  {
    if ((version == 1) || ((TellBlob(image) % 2) != 0))
      code=ReadBlobByte(image);
    if (version == 2)
      code=ReadBlobMSBShort(image);
    if (code > 0xa1)
      {
        if (image_info->debug)
          (void) fprintf(stderr,"%04x:\n",code);
      }
    else
      {
        if (image_info->debug)
          (void) fprintf(stderr,"  %04x %.1024s: %.1024s\n",code,
            codes[code].name,codes[code].description);
        switch (code)
        {
          case 0x01:
          {
            /*
              Clipping rectangle.
            */
            length=ReadBlobMSBShort(image);
            if (length != 0x000a)
              {
                for (i=0; i < (length-2); i++)
                  (void) ReadBlobByte(image);
                break;
              }
            ReadRectangle(frame);
            if ((frame.left & 0x8000) || (frame.top & 0x8000))
              break;
            image->columns=frame.right-frame.left;
            image->rows=frame.bottom-frame.top;
            SetImage(image,OpaqueOpacity);
            break;
          }
          case 0x12:
          case 0x13:
          case 0x14:
          {
            int
              pattern;

            unsigned int
              height,
              width;

            /*
              Skip pattern definition.
            */
            pattern=ReadBlobMSBShort(image);
            for (i=0; i < 8; i++)
              (void) ReadBlobByte(image);
            if (pattern == 2)
              {
                for (i=0; i < 5; i++)
                  (void) ReadBlobByte(image);
                break;
              }
            if (pattern != 1)
              ThrowReaderException(CorruptImageWarning,"Unknown pattern type",
                image);
            length=ReadBlobMSBShort(image);
            ReadRectangle(frame);
            ReadPixmap(pixmap);
            (void) ReadBlobMSBLong(image);
            flags=ReadBlobMSBShort(image);
            length=ReadBlobMSBShort(image);
            for (i=0; i <= length; i++)
              (void) ReadBlobMSBLong(image);
            width=frame.bottom-frame.top;
            height=frame.right-frame.left;
            image->depth=pixmap.bits_per_pixel <= 8 ? 8 : QuantumDepth;
            if (pixmap.bits_per_pixel < 8)
              image->depth=8;
            if (pixmap.bits_per_pixel <= 8)
              length&=0x7fff;
            if (pixmap.bits_per_pixel == 16)
              width<<=1;
            if (length == 0)
              length=width;
            if (length < 8)
              {
                for (i=0; i < (int) (length*height); i++)
                  (void) ReadBlobByte(image);
              }
            else
              for (i=0; i < (int) height; i++)
                if (length > 250)
                  for (i=0; i < (int) ReadBlobMSBShort(image); i++)
                    (void) ReadBlobByte(image);
                else
                  for (i=0; i < ReadBlobByte(image); i++)
                    (void) ReadBlobByte(image);
            break;
          }
          case 0x1b:
          {
            /*
              Initialize image background color.
            */
            image->background_color.red=XDownScale(ReadBlobMSBShort(image));
            image->background_color.green=XDownScale(ReadBlobMSBShort(image));
            image->background_color.blue=XDownScale(ReadBlobMSBShort(image));
            break;
          }
          case 0x70:
          case 0x71:
          case 0x72:
          case 0x73:
          case 0x74:
          case 0x75:
          case 0x76:
          case 0x77:
          {
            /*
              Skip polygon or region.
            */
            length=ReadBlobMSBShort(image);
            for (i=0; i < (length-2); i++)
              (void) ReadBlobByte(image);
            break;
          }
          case 0x90:
          case 0x91:
          case 0x98:
          case 0x99:
          case 0x9a:
          case 0x9b:
          {
            int
              bytes_per_line,
              j;

            PICTRectangle
              source,
              destination;

            register unsigned char
              *p;

            unsigned char
              *pixels;

            Image
              *tile_image;

            /*
              Pixmap clipped by a rectangle.
            */
            bytes_per_line=0;
            if ((code != 0x9a) && (code != 0x9b))
              bytes_per_line=ReadBlobMSBShort(image);
            else
              {
                (void) ReadBlobMSBShort(image);
                (void) ReadBlobMSBShort(image);
                (void) ReadBlobMSBShort(image);
              }
            ReadRectangle(frame);
            /*
              Initialize tile image.
            */
            tile_image=image;
            if ((frame.left != 0) || (frame.top != 0) ||
                (frame.right != image->columns) ||
                (frame.bottom != image->rows) || jpeg)
              tile_image=CloneImage(image,frame.right-frame.left,
                frame.bottom-frame.top,True,exception);
            if (tile_image == (Image *) NULL)
              return((Image *) NULL);
            if ((code == 0x9a) || (code == 0x9b) || (bytes_per_line & 0x8000))
              {
                ReadPixmap(pixmap);
                tile_image->matte=pixmap.component_count == 4;
              }
            if ((code != 0x9a) && (code != 0x9b))
              {
                /*
                  Initialize colormap.
                */
                tile_image->colors=2;
                if (bytes_per_line & 0x8000)
                  {
                    (void) ReadBlobMSBLong(image);
                    flags=ReadBlobMSBShort(image);
                    tile_image->colors=ReadBlobMSBShort(image)+1;
                  }
                if (!AllocateImageColormap(tile_image,tile_image->colors))
                  {
                    DestroyImage(tile_image);
                    ThrowReaderException(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  }
                if (bytes_per_line & 0x8000)
                  {
                    for (i=0; i < (int) tile_image->colors; i++)
                    {
                      j=ReadBlobMSBShort(image) % tile_image->colors;
                      if (flags & 0x8000)
                        j=i;
                      tile_image->colormap[j].red=
                        XDownScale(ReadBlobMSBShort(image));
                      tile_image->colormap[j].green=
                        XDownScale(ReadBlobMSBShort(image));
                      tile_image->colormap[j].blue=
                        XDownScale(ReadBlobMSBShort(image));
                    }
                  }
                else
                  {
                    for (i=0; i < (int) tile_image->colors; i++)
                    {
                      tile_image->colormap[i].red=MaxRGB-
                        tile_image->colormap[i].red;
                      tile_image->colormap[i].green=MaxRGB-
                        tile_image->colormap[i].green;
                      tile_image->colormap[i].blue=MaxRGB-
                        tile_image->colormap[i].blue;
                    }
                  }
              }
            ReadRectangle(source);
            ReadRectangle(destination);
            (void) ReadBlobMSBShort(image);
            if ((code == 0x91) || (code == 0x99) || (code == 0x9b))
              {
                /*
                  Skip region.
                */
                length=ReadBlobMSBShort(image);
                for (i=0; i <= (length-2); i++)
                  (void) ReadBlobByte(image);
              }
            if ((code != 0x9a) && (code != 0x9b) &&
                (bytes_per_line & 0x8000) == 0)
              pixels=DecodeImage(image_info,image,tile_image,bytes_per_line,1);
            else
              pixels=DecodeImage(image_info,image,tile_image,bytes_per_line,
                pixmap.bits_per_pixel);
            if (pixels == (unsigned char *) NULL)
              {
                DestroyImage(tile_image);
                ThrowReaderException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              }
            /*
              Convert PICT tile image to pixel packets.
            */
            p=pixels;
            for (y=0; y < (int) tile_image->rows; y++)
            {
              q=SetImagePixels(tile_image,0,y,tile_image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(tile_image);
              for (x=0; x < (int) tile_image->columns; x++)
              {
                if (tile_image->storage_class == PseudoClass)
                  {
                    index=ValidateColormapIndex(image,*p);
                    indexes[x]=index;
                    q->red=tile_image->colormap[index].red;
                    q->green=tile_image->colormap[index].green;
                    q->blue=tile_image->colormap[index].blue;
                  }
                else
                  {
                    if (pixmap.bits_per_pixel == 16)
                      {
                        i=(*p++);
                        j=(*p);
                        q->red=UpScale((i & 0x7c) << 1);
                        q->green=UpScale(((i & 0x03) << 6) | ((j & 0xe0) >> 2));
                        q->blue=UpScale((j & 0x1f) << 3);
                      }
                    else
                      if (!tile_image->matte)
                        {
                          q->red=UpScale(*p);
                          q->green=UpScale(*(p+tile_image->columns));
                          q->blue=UpScale(*(p+2*tile_image->columns));
                        }
                      else
                        {
                          q->opacity=MaxRGB-UpScale(*p);
                          q->red=UpScale(*(p+tile_image->columns));
                          q->green=UpScale(*(p+2*tile_image->columns));
                          q->blue=UpScale(*(p+3*tile_image->columns));
                        }
                  }
                p++;
                q++;
              }
              if (!SyncImagePixels(tile_image))
                break;
              if ((tile_image->storage_class == DirectClass) &&
                  (pixmap.bits_per_pixel != 16))
                p+=(pixmap.component_count-1)*tile_image->columns;
              if (destination.bottom == (int) image->rows)
                if (QuantumTick(y,tile_image->rows))
                  MagickMonitor(LoadImageText,y,tile_image->rows);
            }
            (void) LiberateMemory((void **) &pixels);
            if (tile_image != image)
              {
                if (!jpeg)
                  if ((code == 0x9a) || (code == 0x9b) ||
                      (bytes_per_line & 0x8000))
                    CompositeImage(image,CopyCompositeOp,tile_image,
                      destination.left,destination.top);
                DestroyImage(tile_image);
              }
            if (destination.bottom != (int) image->rows)
              MagickMonitor(LoadImageText,destination.bottom,image->rows);
            break;
          }
          case 0xa1:
          {
            unsigned char
              *info;

            unsigned short
              type;

            /*
              Comment.
            */
            type=ReadBlobMSBShort(image);
            length=ReadBlobMSBShort(image);
            if (length == 0)
              break;
            (void) ReadBlobMSBLong(image);
            length-=4;
            if (length == 0)
              break;
            info=(unsigned char *) AcquireMemory(length);
            if (info == (unsigned char *) NULL)
              break;
            ReadBlob(image,length,info);
            switch (type)
            {
              case 0xe0:
              {
                image->color_profile.info=(unsigned char *)
                  AcquireMemory(length);
                if (image->color_profile.info == (unsigned char *) NULL)
                  ThrowReaderException(ResourceLimitWarning,
                    "Memory allocation failed",image);
                image->color_profile.length=length;
                memcpy(image->color_profile.info,info,length);
                break;
              }
              case 0x1f2:
              {
                image->iptc_profile.info=(unsigned char *)
                  AcquireMemory(length);
                if (image->iptc_profile.info == (unsigned char *) NULL)
                  ThrowReaderException(ResourceLimitWarning,
                    "Memory allocation failed",image);
                image->iptc_profile.length=length;
                memcpy(image->iptc_profile.info,info,length);
                break;
              }
              default:
                break;
            }
            LiberateMemory((void **) &info);
            break;
          }
          default:
          {
            /*
              Skip to next op code.
            */
            if (codes[code].length == -1)
              (void) ReadBlobMSBShort(image);
            else
              for (i=0; i < codes[code].length; i++)
                (void) ReadBlobByte(image);
          }
        }
      }
    if (code == 0xc00)
      {
        /*
          Skip header.
        */
        for (i=0; i < 24; i++)
          (void) ReadBlobByte(image);
        continue;
      }
    if (((code >= 0xb0) && (code <= 0xcf)) ||
        ((code >= 0x8000) && (code <= 0x80ff)))
      continue;
    if (code == 0x8200)
      {
        FILE
          *file;

        Image
          *tile_image;

        ImageInfo
          *clone_info;

        /*
          Embedded JPEG.
        */
        jpeg=True;
        clone_info=CloneImageInfo(image_info);
        if (clone_info == (ImageInfo *) NULL)
          ThrowReaderException(FileOpenWarning,"Unable to write file",image);
        GetBlobInfo(&(clone_info->blob));
        TemporaryFilename(clone_info->filename);
        file=fopen(clone_info->filename,WriteBinaryType);
        if (file == (FILE *) NULL)
          ThrowReaderException(FileOpenWarning,"Unable to write file",image);
        length=ReadBlobMSBLong(image);
        for (i=0; i < 6; i++)
          (void) ReadBlobMSBLong(image);
        ReadRectangle(frame);
        for (i=0; i < 122; i++)
          (void) ReadBlobByte(image);
        for (i=0; i < (length-154); i++)
        {
          c=ReadBlobByte(image);
          (void) fputc(c,file);
        }
        (void) fclose(file);
        tile_image=ReadImage(clone_info,exception);
        DestroyImageInfo(clone_info);
        (void) remove(clone_info->filename);
        if (tile_image == (Image *) NULL)
          continue;
        FormatString(geometry,"%ux%u",Max(image->columns,tile_image->columns),
          Max(image->rows,tile_image->rows));
        TransformImage(&image,(char *) NULL,geometry);
        CompositeImage(image,CopyCompositeOp,tile_image,frame.left,frame.right);
        DestroyImage(tile_image);
        continue;
      }
    if ((code == 0xff) || (code == 0xffff))
      break;
    if (((code >= 0xd0) && (code <= 0xfe)) ||
        ((code >= 0x8100) && (code <= 0xffff)))
      {
        /*
          Skip reserved.
        */
        length=ReadBlobMSBShort(image);
        for (i=0; i < length; i++)
          (void) ReadBlobByte(image);
        continue;
      }
    if ((code >= 0x100) && (code <= 0x7fff))
      {
        /*
          Skip reserved.
        */
        length=(code >> 7) & 0xff;
        for (i=0; i < length; i++)
          (void) ReadBlobByte(image);
        continue;
      }
  }
  CloseBlob(image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P I C T I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPICTImage adds attributes for the PICT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPICTImage method is:
%
%      RegisterPICTImage(void)
%
*/
ModuleExport void RegisterPICTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCT");
  entry->decoder=ReadPICTImage;
  entry->encoder=WritePICTImage;
  entry->adjoin=False;
  entry->description=AllocateString("Apple Macintosh QuickDraw/PICT");
  entry->module=AllocateString("PICT");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("PICT");
  entry->decoder=ReadPICTImage;
  entry->encoder=WritePICTImage;
  entry->adjoin=False;
  entry->description=AllocateString("Apple Macintosh QuickDraw/PICT");
  entry->module=AllocateString("PICT");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P I C T I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPICTImage removes format registrations made by the
%  PICT module from the list of supported formats.
%
%  The format of the UnregisterPICTImage method is:
%
%      UnregisterPICTImage(void)
%
*/
ModuleExport void UnregisterPICTImage(void)
{
  UnregisterMagickInfo("PCT");
  UnregisterMagickInfo("PICT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P I C T I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePICTImage writes an image to a file in the Apple Macintosh
%  QuickDraw/PICT image format.
%
%  The format of the WritePICTImage method is:
%
%      unsigned int WritePICTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePICTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePICTImage(const ImageInfo *image_info,Image *image)
{
#define MaxCount  128
#define PictCropRegionOp  0x01
#define PictEndOfPictureOp  0xff
#define PictJPEGOp  0x8200
#define PictInfoOp  0x0C00
#define PictInfoSize  512
#define PictPixmapOp  0x9A
#define PictPICTOp  0x98
#define PictVersion  0x11

  int
    y;

  PICTPixmap
    pixmap;

  PICTRectangle
    bounds,
    crop_rectangle,
    destination_rectangle,
    frame_rectangle,
    size_rectangle,
    source_rectangle;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p;

  size_t
    count,
    offset;

  unsigned char
    *buffer,
    *packed_scanline,
    *scanline;

  unsigned int
    bytes_per_line,
    status,
    storage_class;

  unsigned short
    base_address,
    row_bytes,
    transfer_mode,
    x_resolution,
    y_resolution;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  /*
    Initialize image info.
  */
  size_rectangle.top=0;
  size_rectangle.left=0;
  size_rectangle.bottom=image->rows;
  size_rectangle.right=image->columns;
  frame_rectangle=size_rectangle;
  crop_rectangle=size_rectangle;
  source_rectangle=size_rectangle;
  destination_rectangle=size_rectangle;
  base_address=0xff;
  row_bytes=image->columns | 0x8000;
  bounds.top=0;
  bounds.left=0;
  bounds.bottom=image->rows;
  bounds.right=image->columns;
  pixmap.version=0;
  pixmap.pack_type=0;
  pixmap.pack_size=0;
  pixmap.pixel_type=0;
  pixmap.bits_per_pixel=8;
  pixmap.component_count=1;
  pixmap.component_size=8;
  pixmap.plane_bytes=0;
  pixmap.table=0;
  pixmap.reserved=0;
  transfer_mode=0;
  x_resolution=image->x_resolution ? image->x_resolution : 72;
  y_resolution=image->y_resolution ? image->y_resolution : 72;
  storage_class=image->storage_class;
  if (image->compression == JPEGCompression)
    storage_class=DirectClass;
  if (storage_class == DirectClass)
    {
      pixmap.component_count=image->matte ? 4 : 3;
      pixmap.pixel_type=16;
      pixmap.bits_per_pixel=32;
      pixmap.pack_type=0x04;
      transfer_mode=0x40;
      row_bytes=(4*image->columns) | 0x8000;
    }
  /*
    Allocate memory.
  */
  bytes_per_line=image->columns;
  if (storage_class == DirectClass)
    bytes_per_line*=image->matte ? 4 : 3;
  buffer=(unsigned char *) AcquireMemory(PictInfoSize);
  packed_scanline=(unsigned char *) AcquireMemory(row_bytes+MaxCount);
  scanline=(unsigned char *) AcquireMemory(row_bytes);
  if ((buffer == (unsigned char *) NULL) ||
      (packed_scanline == (unsigned char *) NULL) ||
      (scanline == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write header, header size, size bounding box, version, and reserved.
  */
  for (i=0; i < PictInfoSize; i++)
    buffer[i]=0;
  (void) WriteBlob(image,PictInfoSize,buffer);
  WriteBlobMSBShort(image,0);
  WriteBlobMSBShort(image,size_rectangle.top);
  WriteBlobMSBShort(image,size_rectangle.left);
  WriteBlobMSBShort(image,size_rectangle.bottom);
  WriteBlobMSBShort(image,size_rectangle.right);
  WriteBlobMSBShort(image,PictVersion);
  WriteBlobMSBShort(image,0x02ff);  /* version #2 */
  WriteBlobMSBShort(image,PictInfoOp);
  WriteBlobMSBLong(image,0xFFFE0000UL);
  /*
    Write full size of the file, resolution, frame bounding box, and reserved.
  */
  WriteBlobMSBShort(image,x_resolution);
  WriteBlobMSBShort(image,0x0000);
  WriteBlobMSBShort(image,y_resolution);
  WriteBlobMSBShort(image,0x0000);
  WriteBlobMSBShort(image,frame_rectangle.top);
  WriteBlobMSBShort(image,frame_rectangle.left);
  WriteBlobMSBShort(image,frame_rectangle.bottom);
  WriteBlobMSBShort(image,frame_rectangle.right);
  WriteBlobMSBLong(image,0x00000000L);
  if (image->iptc_profile.info != (unsigned char *) NULL)
    {
      WriteBlobMSBShort(image,0xa1);
      WriteBlobMSBShort(image,0x1f2);
      WriteBlobMSBShort(image,image->iptc_profile.length+4);
      WriteBlobString(image,"8BIM");
      WriteBlob(image,image->iptc_profile.length,image->iptc_profile.info);
    }
  if (image->color_profile.info != (unsigned char *) NULL)
    {
      WriteBlobMSBShort(image,0xa1);
      WriteBlobMSBShort(image,0xe0);
      WriteBlobMSBShort(image,image->color_profile.length+4);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlob(image,image->color_profile.length,image->color_profile.info);
      WriteBlobMSBShort(image,0xa1);
      WriteBlobMSBShort(image,0xe0);
      WriteBlobMSBShort(image,4);
      WriteBlobMSBLong(image,0x00000002UL);
    }
  /*
    Write crop region opcode and crop bounding box.
  */
  WriteBlobMSBShort(image,PictCropRegionOp);
  WriteBlobMSBShort(image,0xa);
  WriteBlobMSBShort(image,crop_rectangle.top);
  WriteBlobMSBShort(image,crop_rectangle.left);
  WriteBlobMSBShort(image,crop_rectangle.bottom);
  WriteBlobMSBShort(image,crop_rectangle.right);
  if (image->compression == JPEGCompression)
    {
      size_t
        length;

      void
        *blob;

      (void) strcpy(image->magick,"JPEG");
      length=image->columns*image->rows/4;
      blob=ImageToBlob(image_info,image,&length,&image->exception);
      if (blob == (void *) NULL)
        return(False);
      WriteBlobMSBShort(image,PictJPEGOp);
      WriteBlobMSBLong(image,length+154);
      WriteBlobMSBShort(image,0x0000);
      WriteBlobMSBLong(image,0x00010000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00010000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x40000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00400000UL);
      WriteBlobMSBShort(image,0x0000);
      WriteBlobMSBShort(image,image->rows);
      WriteBlobMSBShort(image,image->columns);
      WriteBlobMSBShort(image,0x0000);
      WriteBlobMSBShort(image,768);
      WriteBlobMSBShort(image,0x0000);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00566A70UL);
      WriteBlobMSBLong(image,0x65670000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000001UL);
      WriteBlobMSBLong(image,0x00016170UL);
      WriteBlobMSBLong(image,0x706C0000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBShort(image,768);
      WriteBlobMSBShort(image,image->columns);
      WriteBlobMSBShort(image,image->rows);
      WriteBlobMSBShort(image,x_resolution);
      WriteBlobMSBShort(image,0x0000);
      WriteBlobMSBShort(image,y_resolution);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x87AC0001UL);
      WriteBlobMSBLong(image,0x0B466F74UL);
      WriteBlobMSBLong(image,0x6F202D20UL);
      WriteBlobMSBLong(image,0x4A504547UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x00000000UL);
      WriteBlobMSBLong(image,0x0018FFFFUL);
      WriteBlob(image,length,blob);
      if (length & 0x01)
        (void) WriteBlobByte(image,'\0');
    }
  /*
    Write picture opcode, row bytes, and picture bounding box, and version.
  */
  if (storage_class == PseudoClass)
    WriteBlobMSBShort(image,PictPICTOp);
  else
    {
      WriteBlobMSBShort(image,PictPixmapOp);
      WriteBlobMSBLong(image,(unsigned long) base_address);
    }
  WriteBlobMSBShort(image,row_bytes | 0x8000);
  WriteBlobMSBShort(image,bounds.top);
  WriteBlobMSBShort(image,bounds.left);
  WriteBlobMSBShort(image,bounds.bottom);
  WriteBlobMSBShort(image,bounds.right);
  /*
    Write pack type, pack size, resolution, pixel type, and pixel size.
  */
  WriteBlobMSBShort(image,pixmap.version);
  WriteBlobMSBShort(image,pixmap.pack_type);
  WriteBlobMSBLong(image,pixmap.pack_size);
  WriteBlobMSBShort(image,x_resolution);
  WriteBlobMSBShort(image,0x0000);
  WriteBlobMSBShort(image,y_resolution);
  WriteBlobMSBShort(image,0x0000);
  WriteBlobMSBShort(image,pixmap.pixel_type);
  WriteBlobMSBShort(image,pixmap.bits_per_pixel);
  /*
    Write component count, size, plane bytes, table size, and reserved.
  */
  WriteBlobMSBShort(image,pixmap.component_count);
  WriteBlobMSBShort(image,pixmap.component_size);
  WriteBlobMSBLong(image,(unsigned long) pixmap.plane_bytes);
  WriteBlobMSBLong(image,(unsigned long) pixmap.table);
  WriteBlobMSBLong(image,(unsigned long) pixmap.reserved);
  if (storage_class == PseudoClass)
    {
      unsigned short
        red,
        green,
        blue;

      /*
        Write image colormap.
      */
      WriteBlobMSBLong(image,0x00000000L);  /* color seed */
      WriteBlobMSBShort(image,0L);  /* color flags */
      WriteBlobMSBShort(image,(unsigned short) Max(image->colors-1,1));
      for (i=0; i < (int) image->colors; i++)
      {
        red=((unsigned long) (image->colormap[i].red*65535L)/MaxRGB);
        green=((unsigned long) (image->colormap[i].green*65535L)/MaxRGB);
        blue=((unsigned long) (image->colormap[i].blue*65535L)/MaxRGB);
        WriteBlobMSBShort(image,(unsigned int) i);
        WriteBlobMSBShort(image,red);
        WriteBlobMSBShort(image,green);
        WriteBlobMSBShort(image,blue);
      }
    }
  /*
    Write source and destination rectangle.
  */
  WriteBlobMSBShort(image,source_rectangle.top);
  WriteBlobMSBShort(image,source_rectangle.left);
  WriteBlobMSBShort(image,source_rectangle.bottom);
  WriteBlobMSBShort(image,source_rectangle.right);
  WriteBlobMSBShort(image,destination_rectangle.top);
  WriteBlobMSBShort(image,destination_rectangle.left);
  WriteBlobMSBShort(image,destination_rectangle.bottom);
  WriteBlobMSBShort(image,destination_rectangle.right);
  WriteBlobMSBShort(image,transfer_mode);
  /*
    Write picture data.
  */
  count=0;
  if (storage_class == PseudoClass)
    for (y=0; y < (int) image->rows; y++)
    {
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (int) image->columns; x++)
        scanline[x]=indexes[x];
      count+=EncodeImage(image,scanline,row_bytes & 0x7FFF,packed_scanline);
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
    }
  else
    if (image->compression == JPEGCompression)
      {
        memset(scanline,0,row_bytes);
        for (y=0; y < (int) image->rows; y++)
          count+=EncodeImage(image,scanline,row_bytes & 0x7FFF,packed_scanline);
      }
    else
      {
        register unsigned char
          *blue,
          *green,
          *opacity,
          *red;

        red=scanline;
        green=scanline+image->columns;
        blue=scanline+2*image->columns;
        opacity=scanline+3*image->columns;
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          red=scanline;
          green=scanline+image->columns;
          blue=scanline+2*image->columns;
          if (image->matte)
            {
              opacity=scanline;
              red=scanline+image->columns;
              green=scanline+2*image->columns;
              blue=scanline+3*image->columns;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *red++=DownScale(p->red);
            *green++=DownScale(p->green);
            *blue++=DownScale(p->blue);
            if (image->matte)
              *opacity++=MaxRGB-DownScale(p->opacity);
            p++;
          }
          count+=EncodeImage(image,scanline,row_bytes & 0x7FFF,packed_scanline);
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
        }
      }
  if (count & 0x1)
    (void) WriteBlobByte(image,'\0');
  WriteBlobMSBShort(image,PictEndOfPictureOp);
  offset=TellBlob(image);
  (void) SeekBlob(image,512,SEEK_SET);
  WriteBlobMSBShort(image,offset);
  LiberateMemory((void **) &scanline);
  LiberateMemory((void **) &packed_scanline);
  LiberateMemory((void **) &buffer);
  CloseBlob(image);
  return(True);
}
