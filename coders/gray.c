/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         GGGG  RRRR    AAA   Y   Y                           %
%                        G      R   R  A   A   Y Y                            %
%                        G  GG  RRRR   AAAAA    Y                             %
%                        G   G  R R    A   A    Y                             %
%                         GGG   R  R   A   A    Y                             %
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
  WriteGRAYImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRAYImage reads an image of raw grayscale bytes and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadGRAYImage method is:
%
%      Image *ReadGRAYImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRAYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadGRAYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    count,
    y;

  register int
    i,
    x;

  unsigned char
    *scanline;

  unsigned int
    packet_size,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < image->offset; i++)
    (void) ReadByte(image);
  /*
    Allocate memory for a scanline.
  */
  packet_size=image->depth > 8 ? 2 : 1;
  scanline=(unsigned char *) AllocateMemory(packet_size*image->tile_info.width);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (int) image->rows; y++)
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    }
  x=packet_size*image->tile_info.x;
  do
  {
    /*
      Create linear colormap.
    */
    if (!AllocateImageColormap(image,1 << image->depth))
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    for (i=0; i < (int) image->colors; i++)
    {
      image->colormap[i].red=image->depth == QuantumDepth ? i : UpScale(i);
      image->colormap[i].green=image->colormap[i].red;
      image->colormap[i].blue=image->colormap[i].red;
    }
    /*
      Convert raster image to pixel packets.
    */
    for (y=0; y < image->tile_info.y; y++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    for (y=0; y < (int) image->rows; y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
      if (!SetImagePixels(image,0,y,image->columns,1))
        break;
      (void) PushImagePixels(image,GrayQuantum,scanline+x);
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    count=image->tile_info.height-image->rows-image->tile_info.y;
    for (y=0; y < count; y++)
      (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"not enough pixels",image);
    SyncImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,packet_size*image->tile_info.width,scanline);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,TellBlob(image),image->filesize);
      }
  } while (count > 0);
  FreeMemory((void **) &scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r G R A Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGRAYImage adds attributes for the GRAY image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGRAYImage method is:
%
%      RegisterGRAYImage(void)
%
*/
ModuleExport void RegisterGRAYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GRAY");
  entry->decoder=ReadGRAYImage;
  entry->encoder=WriteGRAYImage;
  entry->raw=True;
  entry->description=AllocateString("Raw gray bytes");
  entry->module=AllocateString("GRAY");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G R A Y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGRAYImage removes format registrations made by the
%  GRAY module from the list of supported formats.
%
%  The format of the UnregisterGRAYImage method is:
%
%      UnregisterGRAYImage(void)
%
*/
MagickExport void UnregisterGRAYImage(void)
{
  UnregisterMagickInfo("GRAY");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGRAYImage writes an image to a file as gray scale intensity
%  values.
%
%  The format of the WriteGRAYImage method is:
%
%      unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGRAYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  /*
    Convert image to gray scale PseudoColor class.
  */
  scene=0;
  do
  {
    /*
      Allocate memory for pixels.
    */
    TransformRGBImage(image,RGBColorspace);
    packet_size=image->depth > 8 ? 2: 1;
    pixels=(unsigned char *) AllocateMemory(packet_size*image->columns);
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    /*
      Convert MIFF to GRAY raster pixels.
    */
    for (y=0; y < (int) image->rows; y++)
    {
      if (!GetImagePixels(image,0,y,image->columns,1))
        break;
      (void) PopImagePixels(image,GrayQuantum,pixels);
      (void) WriteBlob(image,packet_size*image->columns,pixels);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(SaveImageText,y,image->rows);
    }
    FreeMemory((void **) &pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
