/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  BBBB   M   M                              %
%                             X X   B   B  MM MM                              %
%                              X    BBBB   M M M                              %
%                             X X   B   B  M   M                              %
%                            X   X  BBBB   M   M                              %
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
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
^L
/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include "proxy.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X B M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXBMImage reads an X11 bitmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXBMImage routine is:
%
%      image=ReadXBMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXBMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
static int XBMInteger(FILE *file,short int *hex_digits)
{
  int
    c,
    flag,
    value;

  value=0;
  flag=0;
  for ( ; ; )
  {
    c=fgetc(file);
    if (c == EOF)
      {
        value=(-1);
        break;
      }
    c&=0xff;
    if (isxdigit(c))
      {
        value=(value << 4)+hex_digits[c];
        flag++;
        continue;
      }
    if ((hex_digits[c]) < 0 && flag)
      break;
  }
  return(value);
}

Export Image *ReadXBMImage(const ImageInfo *image_info)
{
  char
    buffer[MaxTextExtent],
    name[MaxTextExtent];

  Image
    *image;

  register int
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  register long
    packets;

  short int
    hex_digits[256];

  unsigned char
    bit,
    *data;

  unsigned int
    byte,
    bytes_per_line,
    padding,
    value,
    version;

  unsigned long
    max_packets;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read X bitmap header.
  */
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
    if (sscanf(buffer,"#define %s %u",name,&image->columns) == 2)
      if ((strlen(name) >= 6) &&
          (Latin1Compare(name+strlen(name)-6,"_width") == 0))
          break;
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
    if (sscanf(buffer,"#define %s %u",name,&image->rows) == 2)
      if ((strlen(name) >= 7) &&
          (Latin1Compare(name+strlen(name)-7,"_height") == 0))
          break;
  image->class=PseudoClass;
  image->colors=2;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Scan until hex digits.
  */
  version=11;
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
  {
    if (sscanf(buffer,"static short %s = {",name) == 1)
      version=10;
    else
      if (sscanf(buffer,"static unsigned char %s = {",name) == 1)
        version=11;
      else
        if (sscanf(buffer,"static char %s = {",name) == 1)
          version=11;
        else
          continue;
    p=(unsigned char *) strrchr(name,'_');
    if (p == (unsigned char *) NULL)
      p=(unsigned char *) name;
    else
      p++;
    if (Latin1Compare("bits[]",(char *) p) == 0)
      break;
  }
  if ((image->columns == 0) || (image->rows == 0) || feof(image->file))
    ReaderExit(CorruptImageWarning,"XBM file is not in the correct format",
      image);
  /*
    Initialize image structure.
  */
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  packets=0;
  max_packets=Max((image->columns*image->rows+4) >> 3,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  padding=0;
  if ((image->columns % 16) && ((image->columns % 16) < 9)  && (version == 10))
    padding=1;
  bytes_per_line=(image->columns+7)/8+padding;
  data=(unsigned char *)
    AllocateMemory(bytes_per_line*image->rows*sizeof(unsigned char *));
  if ((image->colormap == (ColorPacket *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL) ||
      (data == (unsigned char *) NULL))
    ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize colormap.
  */
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  /*
    Initialize hex values.
  */
  hex_digits['0']=0;
  hex_digits['1']=1;
  hex_digits['2']=2;
  hex_digits['3']=3;
  hex_digits['4']=4;
  hex_digits['5']=5;
  hex_digits['6']=6;
  hex_digits['7']=7;
  hex_digits['8']=8;
  hex_digits['9']=9;
  hex_digits['A']=10;
  hex_digits['B']=11;
  hex_digits['C']=12;
  hex_digits['D']=13;
  hex_digits['E']=14;
  hex_digits['F']=15;
  hex_digits['a']=10;
  hex_digits['b']=11;
  hex_digits['c']=12;
  hex_digits['d']=13;
  hex_digits['e']=14;
  hex_digits['f']=15;
  hex_digits['x']=0;
  hex_digits[' ']=(-1);
  hex_digits[',']=(-1);
  hex_digits['}']=(-1);
  hex_digits['\n']=(-1);
  hex_digits['\t']=(-1);
  /*
    Read hex image data.
  */
  p=data;
  if (version == 10)
    for (x=0; x < (int) (bytes_per_line*image->rows); (x+=2))
    {
      value=XBMInteger(image->file,hex_digits);
      *p++=value;
      if (!padding || ((x+2) % bytes_per_line))
        *p++=value >> 8;
    }
  else
    for (x=0; x < (int) (bytes_per_line*image->rows); x++)
    {
      value=XBMInteger(image->file,hex_digits);
      *p++=value;
    }
  /*
    Convert X bitmap image to runlength-encoded packets.
  */
  byte=0;
  p=data;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    bit=0;
    for (x=0; x < (int) image->columns; x++)
    {
      if (bit == 0)
        byte=(*p++);
      index=byte & 0x01 ? 0 : 1;
      if ((index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  FreeMemory((char *) data);
                  ReaderExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              q=image->pixels+packets-1;
            }
          q->index=index;
          q->length=0;
        }
      bit++;
      byte>>=1;
      if (bit == 8)
        bit=0;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  FreeMemory((char *) data);
  SetRunlengthPackets(image,packets);
  SyncImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X B M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXBMImage writes an image to a file in the X bitmap format.
%
%  The format of the WriteXBMImage routine is:
%
%      status=WriteXBMImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXBMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
Export unsigned int WriteXBMImage(const ImageInfo *image_info,Image *image)
{
  char
    name[MaxTextExtent];

  int
    x,
    y;

  register int
    i,
    j;

  register char
    *q;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    byte,
    polarity;

  unsigned int
    count;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  /*
    Write X bitmap header.
  */
  (void) strcpy(name,BaseFilename(image->filename));
  q=name;
  while ((*q != '.') && (*q != '\0'))
    q++;
  if (*q == '.')
    *q='\0';
  (void) fprintf(image->file,"#define %.1024s_width %u\n",name,image->columns);
  (void) fprintf(image->file,"#define %.1024s_height %u\n",name,image->rows);
  (void) fprintf(image->file,"static char %.1024s_bits[] = {\n",name);
  (void) fprintf(image->file," ");
  /*
    Convert MIFF to X bitmap pixels.
  */
  if (!IsMonochromeImage(image))
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=2;
      quantize_info.dither=image_info->dither;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
    }
  polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
  if (image->colors == 2)
    polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
  bit=0;
  byte=0;
  count=0;
  x=0;
  y=0;
  p=image->pixels;
  (void) fprintf(image->file," ");
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      byte>>=1;
      if (p->index == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
          (void) fprintf(image->file,"0x%02x, ",(unsigned int) (byte & 0xff));
          count++;
          if (count == 12)
            {
              (void) fprintf(image->file,"\n  ");
              count=0;
            };
          bit=0;
          byte=0;
        }
      x++;
      if (x == (int) image->columns)
        {
          if (bit != 0)
            {
              /*
                Write a bitmap byte to the image file.
              */
              byte>>=(8-bit);
              (void) fprintf(image->file,"0x%02x, ",(unsigned int)
                (byte & 0xff));
              count++;
              if (count == 12)
                {
                  (void) fprintf(image->file,"\n  ");
                  count=0;
                };
              bit=0;
              byte=0;
            };
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
          x=0;
          y++;
        }
    }
    p++;
  }
  (void) fprintf(image->file,"};\n");
  CloseImage(image);
  return(True);
}
