/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP    CCCC  DDDD                               %
%                            P   P  C      D   D                              %
%                            PPPP   C      D   D                              %
%                            P      C      D   D                              %
%                            P       CCCC  DDDD                               %
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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
  WritePCDImage(const ImageInfo *,Image *);

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
%  Method DecodeImage recovers the Huffman encoded luminance and chrominance
%  deltas.
%
%  The format of the DecodeImage method is:
%
%      static unsigned int DecodeImage(Image *image,unsigned char *luma,
%        unsigned char *chroma1,unsigned char *chroma2)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the deltas are
%      recovered without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o luma: The address of a character buffer that contains the
%      luminance information.
%
%    o chroma1: The address of a character buffer that contains the
%      chrominance information.
%
%    o chroma2: The address of a character buffer that contains the
%      chrominance information.
%
%
%
*/
static unsigned int DecodeImage(Image *image,unsigned char *luma,
  unsigned char *chroma1,unsigned char *chroma2)
{
#define IsSync  ((accumulator & 0xffffff00) == 0xfffffe00)
#define DecodeImageText  "  PCD decode image...  "
#define PCDGetBits(n) \
{  \
  accumulator=(accumulator << n) & 0xffffffff; \
  bits-=n; \
  while (bits <= 24) \
  { \
    if (p >= (buffer+0x800)) \
      { \
        (void) ReadBlob(image,0x800,(char *) buffer); \
        p=buffer; \
      } \
    accumulator|=((unsigned int) (*p) << (24-bits)); \
    bits+=8; \
    p++; \
  } \
  if (EOFBlob(image)) \
    break; \
}

  typedef struct PCDTable
  {
    unsigned int
      length,
      sequence;

    unsigned char
      key;

    unsigned int
      mask;
  } PCDTable;

  int
    count;

  long
    quantum;

  PCDTable
    *pcd_table[3];

  register int
    i,
    j;

  register PCDTable
    *r;

  register unsigned char
    *p,
    *q;

  unsigned char
    *buffer;

  unsigned int
    accumulator,
    bits,
    length,
    pcd_length[3],
    plane,
    row;

  /*
    Initialize Huffman tables.
  */
  assert(image != (const Image *) NULL);
  assert(luma != (unsigned char *) NULL);
  assert(chroma1 != (unsigned char *) NULL);
  assert(chroma2 != (unsigned char *) NULL);
  buffer=(unsigned char *) AllocateMemory(0x800);
  if (buffer == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      (char *) NULL);
  accumulator=0;
  bits=32;
  p=buffer+0x800;
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
  {
    PCDGetBits(8);
    length=(accumulator & 0xff)+1;
    pcd_table[i]=(PCDTable *) AllocateMemory(length*sizeof(PCDTable));
    if (pcd_table[i] == (PCDTable *) NULL)
      {
        FreeMemory(buffer);
        ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
          (char *) NULL);
      }
    r=pcd_table[i];
    for (j=0; j < (int) length; j++)
    {
      PCDGetBits(8);
      r->length=(accumulator & 0xff)+1;
      if (r->length > 16)
        {
          FreeMemory(buffer);
          return(False);
        }
      PCDGetBits(16);
      r->sequence=(accumulator & 0xffff) << 16;
      PCDGetBits(8);
      r->key=accumulator & 0xff;
      r->mask=(~((((unsigned int) 1) << (32-r->length))-1));
      r++;
    }
    pcd_length[i]=length;
  }
  /*
    Search for Sync byte.
  */
  do { PCDGetBits(16) } while (0);
  do { PCDGetBits(16) } while (0);
  while ((accumulator & 0x00fff000) != 0x00fff000)
    PCDGetBits(8);
  while (!IsSync)
    PCDGetBits(1);
  /*
    Recover the Huffman encoded luminance and chrominance deltas.
  */
  count=0;
  length=0;
  plane=0;
  q=luma;
  for ( ; ; )
  {
    if (IsSync)
      {
        /*
          Determine plane and row number.
        */
        PCDGetBits(16);
        row=((accumulator >> 9) & 0x1fff);
        if (row == image->rows)
          break;
        PCDGetBits(8);
        plane=accumulator >> 30;
        PCDGetBits(16);
        switch (plane)
        {
          case 0:
          {
            q=luma+row*image->columns;
            count=image->columns;
            break;
          }
          case 2:
          {
            q=chroma1+(row >> 1)*image->columns;
            count=image->columns >> 1;
            plane--;
            break;
          }
          case 3:
          {
            q=chroma2+(row >> 1)*image->columns;
            count=image->columns >> 1;
            plane--;
            break;
          }
          default:
          {
            ThrowBinaryException(CorruptImageWarning,"Corrupt PCD image",
              image->filename);
          }
        }
        length=pcd_length[plane];
        if (QuantumTick(row,image->rows))
          ProgressMonitor(DecodeImageText,row,image->rows);
        continue;
      }
    /*
      Decode luminance or chrominance deltas.
    */
    r=pcd_table[plane];
    for (i=0; ((i < (int) length) && ((accumulator & r->mask) != r->sequence)); i++)
      r++;
    if (r == (PCDTable *) NULL)
      {
        ThrowException(&image->exception,CorruptImageWarning,
          "Corrupt PCD image, skipping to sync byte",image->filename);
        while ((accumulator & 0x00fff000) != 0x00fff000)
          PCDGetBits(8);
        while (!IsSync)
          PCDGetBits(1);
        continue;
      }
    if (r->key < 128)
      quantum=(long) (*q)+r->key;
    else
      quantum=(long) (*q)+r->key-256;
    *q=(quantum < 0) ? 0 : (quantum > MaxRGB) ? MaxRGB : quantum;
    q++;
    PCDGetBits(r->length);
    count--;
  }
  /*
    Free memory.
  */
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
    FreeMemory(pcd_table[i]);
  FreeMemory(buffer);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P C D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPCD returns True if the image format type, identified by the
%  magick string, is PCD.
%
%  The format of the IsPCD method is:
%
%      unsigned int IsPCD(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPCD returns True if the image format type is PCD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPCD(const unsigned char *magick,const unsigned int length)
{
  if (length < 4)
    return(False);
  if (strncmp((char *) magick,"PCD_",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P C D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCDImage reads a Photo CD image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  Much of the PCD decoder was derived from
%  the program hpcdtoppm(1) by Hadmut Danisch.
%
%  The format of the ReadPCDImage method is:
%
%      image=ReadPCDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *OverviewImage(const ImageInfo *image_info,ExceptionInfo *exception,
  Image *image)
{
  char
    *commands[3];

  Image
    *montage_image;

  ImageInfo
    *clone_info;

  MontageInfo
    montage_info;

  /*
    Create image tiles.
  */
  clone_info=CloneImageInfo(image_info);
  if (clone_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  commands[0]=SetClientName((char *) NULL);
  commands[1]="-label";
  commands[2]=(char *) DefaultTileLabel;
  MogrifyImages(clone_info,3,commands,&image);
  DestroyImageInfo(clone_info);
  /*
    Create the PCD Overview image.
  */
  GetMontageInfo(&montage_info);
  (void) strcpy(montage_info.filename,image_info->filename);
  (void) CloneString(&montage_info.font,image_info->font);
  montage_info.pointsize=image_info->pointsize;
  montage_image=MontageImages(image,&montage_info);
  DestroyMontageInfo(&montage_info);
  if (montage_image == (Image *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  DestroyImage(image);
  return(montage_image);
}

static Image *ReadPCDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    x;

  long int
    offset;

  register int
    i,
    y;

  register PixelPacket
    *q;

  register unsigned char
    *c1,
    *c2,
    *yy;

  unsigned char
    *chroma1,
    *chroma2,
    *header,
    *luma;

  unsigned int
    height,
    number_images,
    overview,
    rotate,
    status,
    subimage,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a PCD file.
  */
  header=(unsigned char *) AllocateMemory(3*0x800);
  if (header == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  status=ReadBlob(image,3*0x800,(char *) header);
  overview=strncmp((char *) header,"PCD_OPA",7) == 0;
  if ((status == False) ||
      ((strncmp((char *) header+0x800,"PCD",3) != 0) && !overview))
    ThrowReaderException(CorruptImageWarning,"Not a PCD image file",image);
  rotate=header[0x0e02] & 0x03;
  number_images=(header[10] << 8) | header[11];
  FreeMemory(header);
  /*
    Determine resolution by subimage specification.
  */
  if ((image->columns*image->rows) == 0)
    subimage=3;
  else
    {
      width=192;
      height=128;
      for (subimage=1; subimage < 6; subimage++)
      {
        if ((width >= image->columns) && (height >= image->rows))
          break;
        width<<=1;
        height<<=1;
      }
    }
  if (image_info->subrange != 0)
    subimage=Min(image_info->subimage,6);
  if (overview)
    subimage=1;
  /*
    Initialize image structure.
  */
  width=192;
  height=128;
  for (i=1; i < Min((int) subimage,3); i++)
  {
    width<<=1;
    height<<=1;
  }
  image->columns=width;
  image->rows=height;
  image->depth=8;
  for ( ; i < (int) subimage; i++)
  {
    image->columns<<=1;
    image->rows<<=1;
  }
  if (image_info->ping)
    {
      if ((rotate == 1) || (rotate == 3))
        Swap(image->columns,image->rows);
      CloseBlob(image);
      return(image);
    }
  /*
    Allocate luma and chroma memory.
  */
  chroma1=(unsigned char *) AllocateMemory(image->columns*image->rows+1);
  chroma2=(unsigned char *) AllocateMemory(image->columns*image->rows+1);
  luma=(unsigned char *) AllocateMemory(image->columns*image->rows+1);
  if ((chroma1 == (unsigned char *) NULL) ||
      (chroma2 == (unsigned char *) NULL) || (luma == (unsigned char *) NULL))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Advance to image data.
  */
  offset=93;
  if (overview)
    offset=2;
  else
    if (subimage == 2)
      offset=20;
    else
      if (subimage <= 1)
        offset=1;
  for (i=0; i < (offset*0x800); i++)
    (void) ReadByte(image);
  if (overview)
    {
      Image
        *overview_image;

      MonitorHandler
        handler;

      register int
        j;

      /*
        Read thumbnails from overview image.
      */
      for (j=1; j <= (int) number_images; j++)
      {
        handler=SetMonitorHandler((MonitorHandler) NULL);
        FormatString(image->filename,"images/img%04d.pcd",j);
        FormatString(image->magick_filename,"images/img%04d.pcd",j);
        image->scene=j;
        image->columns=width;
        image->rows=height;
        image->depth=8;
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (i=0; i < (int) height; i+=2)
        {
          (void) ReadBlob(image,width,(char *) yy);
          yy+=image->columns;
          (void) ReadBlob(image,width,(char *) yy);
          yy+=image->columns;
          (void) ReadBlob(image,width >> 1,(char *) c1);
          c1+=image->columns;
          (void) ReadBlob(image,width >> 1,(char *) c2);
          c2+=image->columns;
        }
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
        /*
          Transfer luminance and chrominance channels.
        */
        yy=luma;
        c1=chroma1;
        c2=chroma2;
        for (y=0; y < (int) image->rows; y++)
        {
          q=SetPixelCache(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*yy++);
            q->green=UpScale(*c1++);
            q->blue=UpScale(*c2++);
            q++;
          }
          if (!SyncPixelCache(image))
            break;
        }
        if (Latin1Compare(image_info->magick,"PCDS") == 0)
          TransformRGBImage(image,sRGBColorspace);
        else
          TransformRGBImage(image,YCCColorspace);
        if (j < (int) number_images)
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImages(image);
                return((Image *) NULL);
              }
            image=image->next;
          }
        (void) SetMonitorHandler(handler);
        ProgressMonitor(LoadImageText,j-1,number_images);
      }
      FreeMemory(chroma2);
      FreeMemory(chroma1);
      FreeMemory(luma);
      while (image->previous != (Image *) NULL)
        image=image->previous;
      overview_image=OverviewImage(image_info,exception,image);
      return(overview_image);
    }
  /*
    Read interleaved image.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (i=0; i < (int) height; i+=2)
  {
    (void) ReadBlob(image,width,(char *) yy);
    yy+=image->columns;
    (void) ReadBlob(image,width,(char *) yy);
    yy+=image->columns;
    (void) ReadBlob(image,width >> 1,(char *) c1);
    c1+=image->columns;
    (void) ReadBlob(image,width >> 1,(char *) c2);
    c2+=image->columns;
  }
  if (subimage >= 4)
    {
      /*
        Recover luminance deltas for 1536x1024 image.
      */
      Upsample(768,512,image->columns,luma);
      Upsample(384,256,image->columns,chroma1);
      Upsample(384,256,image->columns,chroma2);
      image->rows=1024;
      for (i=0; i < (4*0x800); i++)
        (void) ReadByte(image);
      status=DecodeImage(image,luma,chroma1,chroma2);
      if ((subimage >= 5) && status)
        {
          /*
            Recover luminance deltas for 3072x2048 image.
          */
          Upsample(1536,1024,image->columns,luma);
          Upsample(768,512,image->columns,chroma1);
          Upsample(768,512,image->columns,chroma2);
          image->rows=2048;
          offset=TellBlob(image)/0x800+12;
          (void) SeekBlob(image,offset*0x800,SEEK_SET);
          status=DecodeImage(image,luma,chroma1,chroma2);
          if ((subimage >= 6) && status)
            {
              /*
                Recover luminance deltas for 6144x4096 image (vaporware).
              */
              Upsample(3072,2048,image->columns,luma);
              Upsample(1536,1024,image->columns,chroma1);
              Upsample(1536,1024,image->columns,chroma2);
              image->rows=4096;
            }
        }
    }
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
  /*
    Transfer luminance and chrominance channels.
  */
  yy=luma;
  c1=chroma1;
  c2=chroma2;
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetPixelCache(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      q->red=UpScale(*yy++);
      q->green=UpScale(*c1++);
      q->blue=UpScale(*c2++);
      q++;
    }
    if (!SyncPixelCache(image))
      break;
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  FreeMemory(chroma2);
  FreeMemory(chroma1);
  FreeMemory(luma);
  if (Latin1Compare(image_info->magick,"PCDS") == 0)
    TransformRGBImage(image,sRGBColorspace);
  else
    TransformRGBImage(image,YCCColorspace);
  if ((rotate == 1) || (rotate == 3))
    {
      double
        degrees;

      Image
        *rotated_image;

      /*
        Rotate image.
      */
      degrees=rotate == 1 ? -90.0 : 90.0;
      image->orphan=True;
      rotated_image=RotateImage(image,degrees);
      if (rotated_image != (Image *) NULL)
        {
          DestroyImage(image);
          image=rotated_image;
        }
    }
  /*
    Set CCIR 709 primaries with a D65 white point.
  */
  image->chromaticity.red_primary.x=0.64f;
  image->chromaticity.red_primary.y=0.33f;
  image->chromaticity.green_primary.x=0.30f;
  image->chromaticity.green_primary.y=0.60f;
  image->chromaticity.blue_primary.x=0.15f;
  image->chromaticity.blue_primary.y=0.06f;
  image->chromaticity.white_point.x=0.3127f;
  image->chromaticity.white_point.y=0.3290f;
  image->gamma=0.5;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P C D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPCDImage adds attributes for the PCD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPCDImage method is:
%
%      RegisterPCDImage(void)
%
*/
Export void RegisterPCDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCD");
  entry->decoder=ReadPCDImage;
  entry->encoder=WritePCDImage;
  entry->magick=IsPCD;
  entry->adjoin=False;
  entry->description=AllocateString("Photo CD");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("PCDS");
  entry->decoder=ReadPCDImage;
  entry->encoder=WritePCDImage;
  entry->adjoin=False;
  entry->description=AllocateString("Photo CD");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCDImage writes an image in the Photo CD encoded image
%  format.
%
%  The format of the WritePCDImage method is:
%
%      unsigned int WritePCDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static unsigned int WritePCDTile(const ImageInfo *image_info,Image *image,
  char *geometry,char *tile_geometry)
{
  Image
    *downsampled_image,
    *tile_image;

  int
    x,
    y;

  register int
    i;

  register PixelPacket
    *p,
    *q;

  unsigned int
    height,
    width;

  /*
    Scale image to tile size.
  */
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
  if ((width % 2) != 0)
    width--;
  if ((height % 2) != 0)
    height--;
  image->orphan=True;
  tile_image=ZoomImage(image,width,height);
  if (tile_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Unable to scale image",image);
  (void) sscanf(geometry,"%ux%u",&width,&height);
  if ((tile_image->columns != width) || (tile_image->rows != height))
    {
      Image
        *bordered_image;

      RectangleInfo
        border_info;

      /*
        Put a border around the image.
      */
      border_info.width=(width-tile_image->columns+1) >> 1;
      border_info.height=(height-tile_image->rows+1) >> 1;
      bordered_image=BorderImage(tile_image,&border_info);
      if (bordered_image == (Image *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Unable to border image",image);
      DestroyImage(tile_image);
      tile_image=bordered_image;
    }
  TransformImage(&tile_image,(char *) NULL,tile_geometry);
  RGBTransformImage(tile_image,YCCColorspace);
  downsampled_image=
    ZoomImage(tile_image,tile_image->columns >> 1,tile_image->rows >> 1);
  if (downsampled_image == (Image *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Unable to down sample image",image);
  /*
    Write tile to PCD file.
  */
  for (y=0; y < (int) tile_image->rows; y+=2)
  {
    p=GetPixelCache(tile_image,0,y,tile_image->columns,2);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) (tile_image->columns << 1); x++)
    {
      (void) WriteByte(image,DownScale(p->red));
      p++;
    }
    q=GetPixelCache(downsampled_image,0,y >> 1,downsampled_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) downsampled_image->columns; x++)
    {
      (void) WriteByte(image,DownScale(q->green));
      q++;
    }
    q=GetPixelCache(downsampled_image,0,y >> 1,downsampled_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) downsampled_image->columns; x++)
    {
      (void) WriteByte(image,DownScale(q->blue));
      q++;
    }
    if (QuantumTick(y,tile_image->rows))
      ProgressMonitor(SaveImageText,y,tile_image->rows);
  }
  for (i=0; i < 0x800; i++)
    (void) WriteByte(image,'\0');
  DestroyImage(downsampled_image);
  DestroyImage(tile_image);
  return(True);
}

static unsigned int WritePCDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pcd_image;

  register int
    i;

  unsigned int
    status;

  pcd_image=image;
  if (image->columns < image->rows)
    {
      Image
        *rotated_image;

      /*
        Rotate portrait to landscape.
      */
      image->orphan=True;
      rotated_image=RotateImage(image,90.0);
      if (rotated_image == (Image *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Unable to rotate image",image);
      pcd_image=rotated_image;
    }
  /*
    Open output image file.
  */
  status=OpenBlob(image_info,pcd_image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",pcd_image);
  TransformRGBImage(pcd_image,RGBColorspace);
  /*
    Write PCD image header.
  */
  for (i=0; i < 32; i++)
    (void) WriteByte(pcd_image,0xff);
  for (i=0; i < 4; i++)
    (void) WriteByte(pcd_image,0x0e);
  for (i=0; i < 8; i++)
    (void) WriteByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteByte(pcd_image,0x01);
  for (i=0; i < 4; i++)
    (void) WriteByte(pcd_image,0x05);
  for (i=0; i < 8; i++)
    (void) WriteByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteByte(pcd_image,0x0A);
  for (i=0; i < 36; i++)
    (void) WriteByte(pcd_image,'\0');
  for (i=0; i < 4; i++)
    (void) WriteByte(pcd_image,0x01);
  for (i=0; i < 1944; i++)
    (void) WriteByte(pcd_image,'\0');
  (void) WriteBlob(pcd_image,7,"PCD_IPI");
  (void) WriteByte(pcd_image,0x06);
  for (i=0; i < 1530; i++)
    (void) WriteByte(pcd_image,'\0');
  if (image->columns < image->rows)
    (void) WriteByte(pcd_image,'\1');
  else
    (void) WriteByte(pcd_image,'\0');
  for (i=0; i < 3*0x800-1539; i++)
    (void) WriteByte(pcd_image,'\0');
  /*
    Write PCD tiles.
  */
  status=WritePCDTile(image_info,pcd_image,"768x512>","192x128");
  status|=WritePCDTile(image_info,pcd_image,"768x512>","384x256");
  status|=WritePCDTile(image_info,pcd_image,"768x512>","768x512");
  CloseBlob(pcd_image);
  if (image->columns < image->rows)
    DestroyImage(pcd_image);
  return(status);
}
