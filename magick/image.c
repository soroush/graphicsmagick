/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     IIIII  M   M   AAA    GGGG  EEEEE                       %
%                       I    MM MM  A   A  G      E                           %
%                       I    M M M  AAAAA  G  GG  EEE                         %
%                       I    M   M  A   A  G   G  E                           %
%                     IIIII  M   M  A   A   GGGG  EEEEE                       %
%                                                                             %
%                                                                             %
%                          ImageMagick Image Methods                          %
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
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "cache.h"
#include "compress.h"
#include "gem.h"
#include "list.h"
#include "magic.h"
#include "magick.h"
#include "module.h"
#include "monitor.h"
#include "quantize.h"
#include "render.h"
#include "utility.h"
#include "xwindow.h"

/*
  Constant declaration.
*/
const char
  *BackgroundColor = "#ffffffffffff",  /* white */
  *BorderColor = "#dfdfdfdfdfdf",  /* gray */
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileGeometry = "120x120+4+3>",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *ForegroundColor = "#000000000000",  /* black */
  *LoadImageText = "  Loading image...  ",
  *LoadImagesText = "  Loading images...  ",
  *MatteColor = "#bdbdbdbdbdbd",  /* gray */
  *PSDensityGeometry = "72.0x72.0",
  *PSPageGeometry = "612x792>",
  *SaveImageText = "  Saving image...  ",
  *SaveImagesText = "  Saving images...  ";

const unsigned long
  DefaultCompressionQuality = 75;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AllocateImage() returns a pointer to an image structure initialized to
% default values.
%
%  The format of the AllocateImage method is:
%
%      Image *AllocateImage(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%
*/
MagickExport Image *AllocateImage(const ImageInfo *image_info)
{
  Image
    *allocate_image;

  /*
    Allocate image structure.
  */
  allocate_image=(Image *) AcquireMemory(sizeof(Image));
  if (allocate_image == (Image *) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to allocate image",
      "Memory allocation failed");
  (void) memset(allocate_image,0,sizeof(Image));
  /*
    Initialize Image structure.
  */
  (void) strcpy(allocate_image->magick,"MIFF");
  allocate_image->storage_class=DirectClass;
  allocate_image->depth=QuantumDepth;
  allocate_image->interlace=NoInterlace;
  allocate_image->colorspace=RGBColorspace;
  allocate_image->compose=OverCompositeOp;
  allocate_image->blur=1.0;
  GetExceptionInfo(&allocate_image->exception);
  (void) QueryColorDatabase(BackgroundColor,&allocate_image->background_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(BorderColor,&allocate_image->border_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(MatteColor,&allocate_image->matte_color,
    &allocate_image->exception);
  GetTimerInfo(&allocate_image->timer);
  GetCacheInfo(&allocate_image->cache);
  allocate_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  allocate_image->reference_count=1;
  allocate_image->signature=MagickSignature;
  if (image_info == (ImageInfo *) NULL)
    return(allocate_image);
  /*
    Transfer image info.
  */
  allocate_image->blob->exempt=image_info->file != (FILE *) NULL;
  (void) strncpy(allocate_image->filename,image_info->filename,MaxTextExtent-1);
  (void) strncpy(allocate_image->magick_filename,image_info->filename,
    MaxTextExtent-1);
  (void) strncpy(allocate_image->magick,image_info->magick,MaxTextExtent-1);
  if (image_info->size != (char *) NULL)
    {
      (void) GetGeometry(image_info->size,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
      allocate_image->offset=allocate_image->tile_info.x;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      (void) GetGeometry(image_info->tile,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
  allocate_image->compression=image_info->compression;
  allocate_image->interlace=image_info->interlace;
  allocate_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%lfx%lf",&allocate_image->x_resolution,
        &allocate_image->y_resolution);
      if (count != 2)
        allocate_image->y_resolution=allocate_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      allocate_image->page=allocate_image->tile_info;
      geometry=GetPageGeometry(image_info->page);
      (void) GetGeometry(geometry,&allocate_image->page.x,
        &allocate_image->page.y,&allocate_image->page.width,
        &allocate_image->page.height);
      LiberateMemory((void **) &geometry);
    }
  allocate_image->depth=image_info->depth;
  allocate_image->background_color=image_info->background_color;
  allocate_image->border_color=image_info->border_color;
  allocate_image->matte_color=image_info->matte_color;
  allocate_image->client_data=image_info->client_data;
  if (image_info->cache != (void *) NULL)
    ClonePixelCacheMethods(allocate_image->cache,image_info->cache);
  return(allocate_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageColormap() allocates an image colormap and initializes
%  it to a linear gray colorspace.  If the image already has a colormap,
%  it is replaced.  AllocateImageColormap() returns True if successful,
%  otherwise False if there is not enough memory.
%
%  The format of the AllocateImageColormap method is:
%
%      unsigned int AllocateImageColormap(Image *image,
%        const unsigned long colors)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o colors: The number of colors in the image colormap.
%
%
*/
MagickExport unsigned int AllocateImageColormap(Image *image,
  const unsigned long colors)
{
  register long
    i;

  size_t
    length;

  unsigned long
    pixel;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->storage_class=PseudoClass;
  image->colors=colors;
  length=image->colors*sizeof(PixelPacket);
  if (image->colormap == (PixelPacket *) NULL)
    image->colormap=(PixelPacket *) AcquireMemory(length);
  else
    ReacquireMemory((void **) &image->colormap,length);
  if (image->colormap == (PixelPacket *) NULL)
    return(False);
  for (i=0; i < (long) image->colors; i++)
  {
    pixel=i*(MaxRGB/Max(colors-1,1));
    image->colormap[i].red=(Quantum) pixel;
    image->colormap[i].green=(Quantum) pixel;
    image->colormap[i].blue=(Quantum) pixel;
    image->colormap[i].opacity=OpaqueOpacity;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e N e x t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use AllocateNextImage() to initialize the next image in a sequence to
%  default values.  The next member of image points to the newly allocated
%  image.  If there is a memory shortage, next is assigned NULL.
%
%  The format of the AllocateNextImage method is:
%
%      void AllocateNextImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%    o image: The image.
%
%
*/
MagickExport void AllocateNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strncpy(image->next->filename,image->filename,MaxTextExtent-1);
  if (image_info != (ImageInfo *) NULL)
    (void) strncpy(image->next->filename,image_info->filename,MaxTextExtent-1);
  DestroyBlobInfo(image->next->blob);
  image->next->blob=ReferenceBlob(image->blob);
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnimateImages() repeatedly displays an image sequence to any X window
%  screen.  It returns a value other than 0 if successful.  Check the
%  exception member of image to determine the reason for any failure.
%
%  The format of the AnimateImages method is:
%
%      unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  char
    *client_name;

  Display
    *display;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  resource.image_info=CloneImageInfo(image_info);
  resource.immutable=True;
  (void) XAnimateImages(display,&resource,&client_name,1,image);
  (void) XCloseDisplay(display);
  DestroyImageInfo(resource.image_info);
  return(image->exception.severity == UndefinedException);
}
#else
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,"X11 library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A p p e n d I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The AppendImages() method takes a set of images and appends them to each
%  other top-to-bottom if the stack parameter is true, otherwise left-to-right.
%
%  The format of the AppendImage method is:
%
%      Image *AppendImages(const Image *image,const unsigned int stack,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o stack: A value other than 0 stacks the images top-to-bottom.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AppendImages(const Image *image,const unsigned int stack,
  ExceptionInfo *exception)
{
#define AppendImageText  "  Append image sequence...  "

  Image
    *append_image;

  register const Image
    *next;

  register long
    x,
    y;

  unsigned int
    status;

  unsigned long
    height,
    scene,
    width;

  /*
    Ensure the image have the same column width.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionError,"Unable to append images",
      "an image sequence is required");
  width=image->columns;
  height=image->rows;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    if (stack)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        continue;
      }
    width+=next->columns;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Initialize append next attributes.
  */
  append_image=CloneImage(image,width,height,True,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  SetImage(append_image,OpaqueOpacity);
  scene=0;
  if (stack)
    {
      /*
        Stack top-to-bottom.
      */
      y=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          SetImageType(append_image,TrueColorType);
        (void) CompositeImage(append_image,CopyCompositeOp,next,0,y);
        y+=next->rows;
        status=MagickMonitor(AppendImageText,scene,GetImageListSize(image),
          exception);
        if (status == False)
          break;
        scene++;
      }
      return(append_image);
    }
  /*
    Stack left-to-right.
  */
  x=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->storage_class == DirectClass)
      SetImageType(append_image,TrueColorType);
    (void) CompositeImage(append_image,CopyCompositeOp,next,x,0);
    x+=next->columns;
    status=MagickMonitor(AppendImageText,scene++,GetImageListSize(image),
      exception);
    if (status == False)
      break;
  }
  return(append_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A v e r a g e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Average() method takes a set of images and averages them together.
%  Each image in the set must have the same width and height.  Average()
%  returns a single image with each corresponding pixel component of
%  each image averaged.   On failure, a NULL image is returned and
%  exception describes the reason for the failure.
%
%  The format of the AverageImage method is:
%
%      Image *AverageImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AverageImages(const Image *image,ExceptionInfo *exception)
{
#define AverageImageText  "  Average image sequence...  "

  DoublePixelPacket
    *pixels;

  Image
    *average_image;

  long
    y;

  register const Image
    *next;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned long
    number_scenes,
    number_pixels;

  /*
    Ensure the image are the same size.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionError,"Unable to average image sequence",
      "image sequence required");
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if ((next->columns != image->columns) || (next->rows != image->rows))
      ThrowImageException(OptionError,"Unable to average image sequence",
        "image widths or heights differ");
  }
  /*
    Allocate sum accumulation buffer.
  */
  number_pixels=image->columns*image->rows;
  pixels=(DoublePixelPacket *)
    AcquireMemory(number_pixels*sizeof(DoublePixelPacket));
  if (pixels == (DoublePixelPacket *) NULL)
    ThrowImageException(ResourceLimitError,"Unable to average image sequence",
      "Memory allocation failed");
  (void) memset(pixels,0,number_pixels*sizeof(DoublePixelPacket));
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      LiberateMemory((void **) &pixels);
      return((Image *) NULL);
    }
  average_image->storage_class=DirectClass;
  /*
    Compute sum over each pixel color component.
  */
  number_scenes=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    i=0;
    for (y=0; y < (long) next->rows; y++)
    {
      p=AcquireImagePixels(next,0,y,next->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (long) next->columns; x++)
      {
        pixels[i].red+=p->red;
        pixels[i].green+=p->green;
        pixels[i].blue+=p->blue;
        pixels[i].opacity+=p->opacity;
        p++;
        i++;
      }
    }
    number_scenes++;
  }
  /*
    Average next pixels.
  */
  i=0;
  for (y=0; y < (long) average_image->rows; y++)
  {
    q=SetImagePixels(average_image,0,y,average_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) average_image->columns; x++)
    {
      q->red=(Quantum) (pixels[i].red/number_scenes+0.5);
      q->green=(Quantum) (pixels[i].green/number_scenes+0.5);
      q->blue=(Quantum) (pixels[i].blue/number_scenes+0.5);
      q->opacity=(Quantum) (pixels[i].opacity/number_scenes+0.5);
      q++;
      i++;
    }
    if (!SyncImagePixels(average_image))
      break;
    if (QuantumTick(y,average_image->rows))
      if (!MagickMonitor(AverageImageText,y,average_image->rows,exception))
        break;
  }
  LiberateMemory((void **) &pixels);
  return(average_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C a t c h I m a g e E x c e p t i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CatchImageException() returns if no exceptions are found in the image
%  sequence, otherwise it determines the most severe exception and reports
%  it as a warning or error depending on the severity.
%
%  The format of the CatchImageException method is:
%
%      ExceptionType CatchImageException(Image *image)
%
%  A description of each parameter follows:
%
%    o image: An image sequence.
%
%
*/
MagickExport ExceptionType CatchImageException(Image *image)
{
  ExceptionInfo
    exception;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  GetExceptionInfo(&exception);
  GetImageException(image,&exception);
  CatchException(&exception);
  DestroyExceptionInfo(&exception);
  return(exception.severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a n n e l I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Extract a channel from the image.  A channel is a particular color
%  component of each pixel in the image.
%
%  The format of the ChannelImage method is:
%
%      unsigned int ChannelImage(Image *image,const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to extract: Red, Cyan, Green, Magenta,
%    Blue, Yellow, or Opacity.
%
%
*/
MagickExport unsigned int ChannelImage(Image *image,const ChannelType channel)
{
#define ChannelImageText  "  Extract a channel from the image...  "

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Channel DirectClass packets.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  SetImageType(image,TrueColorType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      switch (channel)
      {
        case RedChannel:
        case CyanChannel:
        {
          q->green=q->red;
          q->blue=q->red;
          break;
        }
        case GreenChannel:
        case MagentaChannel:
        {
          q->red=q->green;
          q->blue=q->green;
          break;
        }
        case BlueChannel:
        case YellowChannel:
        {
          q->red=q->blue;
          q->green=q->blue;
          break;
        }
        case OpacityChannel:
        {
          if (image->colorspace == CMYKColorspace)
            {
              q->red=indexes[x];
              q->green=indexes[x];
              q->blue=indexes[x];
              break;
            }
        }
        case MatteChannel:
        case BlackChannel:
        default:
        {
          q->red=q->opacity;
          q->green=q->opacity;
          q->blue=q->opacity;
          break;
        }
      }
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ChannelImageText,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClipImage() sets the image clip mask based any clipping path information if
%  it exists.
%
%  The format of the ClipImage method is:
%
%      unsigned int ClipImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int ClipImage(Image *image)
{
  const ImageAttribute
    *attribute;

  Image
    *clip_mask;

  ImageInfo
    *image_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  attribute=GetImageAttribute(image,"8BIM:1999,2998");
  if (attribute == (const ImageAttribute *) NULL)
    return(False);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) QueryColorDatabase("none",&image_info->background_color,
    &image->exception);
  clip_mask=BlobToImage(image_info,attribute->value,strlen(attribute->value),
    &image->exception);
  DestroyImageInfo(image_info);
  if (clip_mask == (Image *) NULL)
    return(False);
  (void) NegateImage(clip_mask,False);
  (void) SetImageClipMask(image,clip_mask);
  DestroyImage(clip_mask);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImage() copies an image and returns the copy as a new image object.
%  If the specified columns and rows is 0, an exact copy of the image is
%  returned, otherwise the pixel data is undefined and must be initialized
%  with the SetImagePixels() and SyncImagePixels() methods.  On failure,
%  a NULL image is returned and exception describes the reason for the
%  failure.
%
%  The format of the CloneImage method is:
%
%      Image *CloneImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,const unsigned int orphan,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the cloned image.
%
%    o rows: The number of rows in the cloned image.
%
%    o orphan:  With a value other than 0, the cloned image is an orphan.  An
%      orphan is a stand-alone image that is not assocated with an image list.
%      In effect, the next and previous members of the cloned image is set to
%      NULL.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImage(const Image *image,const unsigned long columns,
  const unsigned long rows,const unsigned int orphan,ExceptionInfo *exception)
{
  const ImageAttribute
    *attribute;

  Image
    *clone_image;

  register long
    i;

  size_t
    length;

  /*
    Clone the image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_image=(Image *) AcquireMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    ThrowImageException(ResourceLimitError,"Unable to clone image",
      "Memory allocation failed");
  (void) memset(clone_image,0,sizeof(Image));
  clone_image->storage_class=image->storage_class;
  clone_image->colorspace=image->colorspace;
  clone_image->compression=image->compression;
  clone_image->taint=image->taint;
  clone_image->matte=image->matte;
  clone_image->columns=image->columns;
  clone_image->rows=image->rows;
  clone_image->depth=image->depth;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colors=image->colors;
      length=image->colors*sizeof(PixelPacket);
      clone_image->colormap=(PixelPacket *) AcquireMemory(length);
      if (clone_image->colormap == (PixelPacket *) NULL)
        ThrowImageException(ResourceLimitError,"Unable to clone image",
          "Memory allocation failed");
      length=image->colors*sizeof(PixelPacket);
      (void) memcpy(clone_image->colormap,image->colormap,length);
    }
  clone_image->background_color=image->background_color;
  clone_image->border_color=image->border_color;
  clone_image->matte_color=image->matte_color;
  clone_image->gamma=image->gamma;
  clone_image->chromaticity=image->chromaticity;
  if (image->color_profile.length != 0)
    {
      /*
        Allocate and clone any ICM profile.
      */
      clone_image->color_profile.name=AllocateString(image->color_profile.name);
      clone_image->color_profile.length=image->color_profile.length;
      clone_image->color_profile.info=(unsigned char *)
        AcquireMemory(clone_image->color_profile.length);
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitError,"Unable to clone image",
          "Memory allocation failed");
      (void) memcpy(clone_image->color_profile.info,
        image->color_profile.info,clone_image->color_profile.length);
    }
  if (image->iptc_profile.length != 0)
    {
      /*
        Allocate and clone any IPTC profile.
      */
      clone_image->iptc_profile.name=AllocateString(image->iptc_profile.name);
      clone_image->iptc_profile.length=image->iptc_profile.length;
      clone_image->iptc_profile.info=(unsigned char *)
        AcquireMemory(clone_image->iptc_profile.length);
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitError,"Unable to clone image",
          "Memory allocation failed");
      (void) memcpy(clone_image->iptc_profile.info,
        image->iptc_profile.info,clone_image->iptc_profile.length);
    }
  clone_image->generic_profiles=image->generic_profiles;
  if (image->generic_profiles != 0)
    {
      /*
        Allocate and clone any generic profiles.
      */
      clone_image->generic_profile=(ProfileInfo *)
        AcquireMemory(image->generic_profiles*sizeof(ProfileInfo));
      if (clone_image->generic_profile == (ProfileInfo *) NULL)
        ThrowImageException(ResourceLimitError,"Unable to clone image",
          "Memory allocation failed");
      length=image->generic_profiles*sizeof(ProfileInfo);
      (void) memcpy(clone_image->generic_profile,image->generic_profile,
        length);
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        clone_image->generic_profile[i].name=
          AllocateString(image->generic_profile[i].name);
        if (image->generic_profile[i].length == 0)
          continue;
        length=image->generic_profile[i].length;
        clone_image->generic_profile[i].info=
          (unsigned char *) AcquireMemory(length);
        if (clone_image->generic_profile[i].info == (unsigned char *) NULL)
          ThrowImageException(ResourceLimitError,"Unable to clone image",
            "Memory allocation failed");
        (void) memcpy(clone_image->generic_profile[i].info,
          image->generic_profile[i].info,length);
      }
    }
  clone_image->rendering_intent=image->rendering_intent;
  clone_image->units=image->units;
  clone_image->montage=(char *) NULL;
  clone_image->directory=(char *) NULL;
  clone_image->geometry=(char *) NULL;
  clone_image->offset=image->offset;
  clone_image->x_resolution=image->x_resolution;
  clone_image->y_resolution=image->y_resolution;
  clone_image->page=image->page;
  clone_image->tile_info=image->tile_info;
  clone_image->blur=image->blur;
  clone_image->fuzz=image->fuzz;
  clone_image->filter=image->filter;
  clone_image->interlace=image->interlace;
  clone_image->endian=image->endian;
  clone_image->gravity=image->gravity;
  clone_image->compose=image->compose;
  clone_image->signature=MagickSignature;
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
    (void) SetImageAttribute(clone_image,attribute->key,attribute->value);
  clone_image->scene=image->scene;
  clone_image->dispose=image->dispose;
  clone_image->delay=image->delay;
  clone_image->iterations=image->iterations;
  clone_image->total_colors=image->total_colors;
  clone_image->error=image->error;
  clone_image->semaphore=(SemaphoreInfo *) NULL;
  clone_image->timer=image->timer;
  GetExceptionInfo(&clone_image->exception);
  ThrowException(&clone_image->exception,image->exception.severity,
    image->exception.reason,image->exception.description);
  clone_image->client_data=image->client_data;
  clone_image->start_loop=image->start_loop;
  clone_image->blob=ReferenceBlob(image->blob);
  clone_image->ascii85=image->ascii85;
  clone_image->magick_columns=image->magick_columns;
  clone_image->magick_rows=image->magick_rows;
  (void) strncpy(clone_image->magick_filename,image->magick_filename,
    MaxTextExtent-1);
  (void) strncpy(clone_image->magick,image->magick,MaxTextExtent-1);
  (void) strncpy(clone_image->filename,image->filename,MaxTextExtent-1);
  clone_image->reference_count=1;
  clone_image->previous=(Image *) NULL;
  clone_image->list=(Image *) NULL;
  clone_image->next=(Image *) NULL;
  clone_image->clip_mask=(Image *) NULL;
  if (!orphan)
    {
      if (image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  if ((columns == 0) && (rows == 0))
    {
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
      if (image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,True,exception);
      clone_image->cache=ReferenceCache(image->cache);
      return(clone_image);
    }
  clone_image->page.width=columns;
  clone_image->page.height=rows;
  clone_image->page.x=(long) (columns*image->page.x/clone_image->columns);
  clone_image->page.y=(long) (rows*image->page.y/clone_image->rows);
  clone_image->columns=columns;
  clone_image->rows=rows;
  GetCacheInfo(&clone_image->cache);
  return(clone_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageInfo() makes a copy of the given image info structure.  If
%  NULL is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneImageInfo method is:
%
%      ImageInfo *CloneImageInfo(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *clone_info;

  clone_info=(ImageInfo *) AcquireMemory(sizeof(ImageInfo));
  if (clone_info == (ImageInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"Unable to clone image info",
      "Memory allocation failed");
  GetImageInfo(clone_info);
  if (image_info == (ImageInfo *) NULL)
    return(clone_info);
  clone_info->compression=image_info->compression;
  clone_info->temporary=image_info->temporary;
  clone_info->adjoin=image_info->adjoin;
  clone_info->antialias=image_info->antialias;
  clone_info->subimage=image_info->subimage;
  clone_info->subrange=image_info->subrange;
  clone_info->depth=image_info->depth;
  if (image_info->size != (char *) NULL)
    clone_info->size=AllocateString(image_info->size);
  if (image_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(image_info->tile);
  if (image_info->page != (char *) NULL)
    clone_info->page=AllocateString(image_info->page);
  clone_info->interlace=image_info->interlace;
  clone_info->endian=image_info->endian;
  clone_info->units=image_info->units;
  clone_info->quality=image_info->quality;
  if (image_info->sampling_factor != (char *) NULL)
    clone_info->sampling_factor=AllocateString(image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(image_info->server_name);
  if (image_info->font != (char *) NULL)
    clone_info->font=AllocateString(image_info->font);
  if (image_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(image_info->texture);
  if (image_info->density != (char *) NULL)
    clone_info->density=AllocateString(image_info->density);
  clone_info->pointsize=image_info->pointsize;
  clone_info->fuzz=image_info->fuzz;
  clone_info->pen=image_info->pen;
  clone_info->background_color=image_info->background_color;
  clone_info->border_color=image_info->border_color;
  clone_info->matte_color=image_info->matte_color;
  clone_info->dither=image_info->dither;
  clone_info->monochrome=image_info->monochrome;
  clone_info->colorspace=image_info->colorspace;
  clone_info->type=image_info->type;
  clone_info->preview_type=image_info->preview_type;
  clone_info->group=image_info->group;
  clone_info->ping=image_info->ping;
  clone_info->verbose=image_info->verbose;
  if (image_info->view != (char *) NULL)
    clone_info->view=AllocateString(image_info->view);
  if (image_info->authenticate != (char *) NULL)
    clone_info->authenticate=AllocateString(image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    clone_info->attributes=CloneImage(image_info->attributes,0,0,True,
      &image_info->attributes->exception);
  clone_info->client_data=image_info->client_data;
  clone_info->cache=image_info->cache;
  if (image_info->cache != (void *) NULL)
    clone_info->cache=ReferenceCache(image_info->cache);
  clone_info->fifo=image_info->fifo;
  clone_info->file=image_info->file;
  clone_info->blob=image_info->blob;
  clone_info->length=image_info->length;
  (void) strncpy(clone_info->magick,image_info->magick,MaxTextExtent-1);
  (void) strncpy(clone_info->unique,image_info->unique,MaxTextExtent-1);
  (void) strncpy(clone_info->zero,image_info->zero,MaxTextExtent-1);
  (void) strncpy(clone_info->filename,image_info->filename,MaxTextExtent-1);
  clone_info->signature=image_info->signature;
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C y c l e C o l o r m a p I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CycleColormap() displaces an image's colormap by a given number of
%  positions.  If you cycle the colormap a number of times you can produce
%  a psychodelic effect.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(Image *image,const int amount)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amount:  Offset the colormap this much.
%
%
*/
MagickExport void CycleColormapImage(Image *image,const int amount)
{
  long
    index,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    SetImageType(image,PaletteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(long) ((indexes[x]+amount) % image->colors);
      if (index < 0)
        index+=image->colors;
      indexes[x]=(IndexPacket) index;
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DescribeImage() describes an image by printing its attributes to the file.
%  Attributes include the image width, height, size, and others.
%
%  The format of the DescribeImage method is:
%
%      void DescribeImage(Image *image,FILE *file,const unsigned int verbose)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o file: The file, typically stdout.
%
%    o verbose: A value other than zero prints more detailed information
%      about the image.
%
%
*/
MagickExport void DescribeImage(Image *image,FILE *file,
  const unsigned int verbose)
{
  char
    color[MaxTextExtent],
    format[MaxTextExtent];

  const ImageAttribute
    *attribute;

  const MagickInfo
    *magick_info;

  double
    elapsed_time,
    user_time;

  Image
    *p;

  long
    y;

  register long
    i,
    x;

  unsigned long
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(file != (FILE *) NULL);
  elapsed_time=GetElapsedTime(&image->timer);
  user_time=GetUserTime(&image->timer);
  (void) ContinueTimer(&image->timer);
  if (!verbose)
    {
      /*
        Display detailed info about the image.
      */
      if (*image->magick_filename != '\0')
        if (LocaleCompare(image->magick_filename,image->filename) != 0)
          (void) fprintf(file,"%.1024s=>",image->magick_filename);
       if ((image->previous == (Image *) NULL) &&
           (image->next == (Image *) NULL) && (image->scene == 0))
        (void) fprintf(file,"%.1024s ",image->filename);
      else
        (void) fprintf(file,"%.1024s[%lu] ",image->filename,image->scene);
      (void) fprintf(file,"%.1024s ",image->magick);
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          (void) fprintf(file,"%lux%lu=>",image->magick_columns,
            image->magick_rows);
      (void) fprintf(file,"%lux%lu%+ld%+ld ",image->columns,image->rows,
        image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            {
              FormatSize(image->total_colors,format);
              (void) fprintf(file,"%.1024s ",format);
            }
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %luc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%luc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%ld/%.6f/%.6fe ",
              (long) image->error.mean_error_per_pixel,
              image->error.normalized_mean_error,
              image->error.normalized_maximum_error);
          }
      (void) fprintf(file,"%lu-bit ",image->depth);
      if (GetBlobSize(image) != 0)
        {
          FormatSize(GetBlobSize(image),format);
          (void) fprintf(file,"%.1024s ",format);
        }
      (void) fprintf(file,"%.1fu %ld:%02ld\n",user_time,
        (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
      return;
    }
  /*
    Display verbose info about the image.
  */
  (void) SignatureImage(image);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=GetMagickInfo(image->magick,&image->exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  Format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  Format: %.1024s (%.1024s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  Geometry: %lux%lu\n",image->columns,image->rows);
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Class: DirectClass\n");
  else
    (void) fprintf(file,"  Class: PseudoClass\n");
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  Base geometry: %lux%lu\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  Type: ");
  switch (GetImageType(image,&image->exception))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case GrayscaleMatteType:
      (void) fprintf(file,"grayscale with transparency"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case PaletteMatteType:
      (void) fprintf(file,"palette with transparency"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case TrueColorMatteType:
      (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    case ColorSeparationMatteType:
      (void) fprintf(file,"color separated with transparency"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  (void) fprintf(file,"  Depth: %lu bits-per-pixel component\n",
    GetImageDepth(image,&image->exception));
  x=0;
  p=(Image *) NULL;
  if ((image->matte && (strcmp(image->magick,"GIF") != 0)) || image->taint)
    {
      char
        tuple[MaxTextExtent];

      register const PixelPacket
        *p;

      p=(PixelPacket *) NULL;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (p->opacity == TransparentOpacity)
            break;
          p++;
        }
        if (x < (long) image->columns)
          break;
      }
      if ((x < (long) image->columns) || (y < (long) image->rows))
        {
          GetColorTuple(p,image->depth,image->matte,False,tuple);
          (void) fprintf(file,"  Opacity: %.1024s\t",tuple);
          GetColorTuple(p,image->depth,image->matte,True,tuple);
          (void) fprintf(file,"  %.1024s\n",tuple);
        }
    }
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Colors: %lu\n",image->total_colors);
  else
    if (image->total_colors <= image->colors)
      (void) fprintf(file,"  Colors: %lu\n",image->colors);
    else
      (void) fprintf(file,"  Colors: %lu=>%lu\n",image->total_colors,
        image->colors);
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors < 1024)
        (void) GetNumberColors(image,file,&image->exception);
    }
  else
    {
      char
        name[MaxTextExtent];

      register PixelPacket
        *p;

      /*
        Display image colormap.
      */
      p=image->colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        char
          tuple[MaxTextExtent];

        GetColorTuple(p,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"    %lu: %.1024s",i,tuple);
        (void) fprintf(file,"\t");
        (void) QueryColorname(image,p,SVGCompliance,name,&image->exception);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->error.mean_error_per_pixel != 0.0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %ld\n",
      (long) image->error.mean_error_per_pixel);
  if (image->error.normalized_mean_error != 0.0)
    (void) fprintf(file,"  Normalized Mean Exception: %g\n",
      image->error.normalized_mean_error);
  if (image->error.normalized_maximum_error != 0.0)
    (void) fprintf(file,"  Normalized Maximum Exception: %gn",
      image->error.normalized_maximum_error);
  if (image->rendering_intent == SaturationIntent)
    (void) fprintf(file,"  Rendering-Intent: saturation\n");
  else
    if (image->rendering_intent == PerceptualIntent)
      (void) fprintf(file,"  Rendering-Intent: perceptual\n");
    else
      if (image->rendering_intent == AbsoluteIntent)
        (void) fprintf(file,"  Rendering-Intent: absolute\n");
    else
      if (image->rendering_intent == RelativeIntent)
        (void) fprintf(file,"  Rendering-Intent: relative\n");
  if (image->gamma != 0.0)
    (void) fprintf(file,"  Gamma: %g\n",image->gamma);
  if ((image->chromaticity.red_primary.x != 0.0) ||
      (image->chromaticity.green_primary.x != 0.0) ||
      (image->chromaticity.blue_primary.x != 0.0) ||
      (image->chromaticity.white_point.x != 0.0))
    {
      /*
        Display image chromaticity.
      */
      (void) fprintf(file,"  Chromaticity:\n");
      (void) fprintf(file,"    red primary: (%g,%g,%g)\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
        image->chromaticity.red_primary.z);
      (void) fprintf(file,"    green primary: (%g,%g,%g)\n",
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y,
        image->chromaticity.green_primary.z);
      (void) fprintf(file,"    blue primary: (%g,%g,%g)\n",
        image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y,
        image->chromaticity.blue_primary.z);
      (void) fprintf(file,"    white point: (%g,%g,%g)\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y,
        image->chromaticity.white_point.z);
    }
  if (image->color_profile.length != 0)
    (void) fprintf(file,"  Profile-color: %lu bytes\n",(unsigned long)
      image->color_profile.length);
  if (image->iptc_profile.length != 0)
    {
      char
        *tag,
        *text;

      size_t
        length;

      /*
        Describe IPTC data.
      */
      (void) fprintf(file,"  Profile-iptc: %lu bytes\n",(unsigned long)
        image->iptc_profile.length);
      for (i=0; i < (long) image->iptc_profile.length; )
      {
        if (image->iptc_profile.info[i] != 0x1c)
          {
            i++;
            continue;
          }
        i++;  /* skip file separator */
        i++;  /* skip record number */
        switch (image->iptc_profile.info[i])
        {
          case 5: tag=(char *) "Image Name"; break;
          case 7: tag=(char *) "Edit Status"; break;
          case 10: tag=(char *) "Priority"; break;
          case 15: tag=(char *) "Category"; break;
          case 20: tag=(char *) "Supplemental Category"; break;
          case 22: tag=(char *) "Fixture Identifier"; break;
          case 25: tag=(char *) "Keyword"; break;
          case 30: tag=(char *) "Release Date"; break;
          case 35: tag=(char *) "Release Time"; break;
          case 40: tag=(char *) "Special Instructions"; break;
          case 45: tag=(char *) "Reference Service"; break;
          case 47: tag=(char *) "Reference Date"; break;
          case 50: tag=(char *) "Reference Number"; break;
          case 55: tag=(char *) "Created Date"; break;
          case 60: tag=(char *) "Created Time"; break;
          case 65: tag=(char *) "Originating Program"; break;
          case 70: tag=(char *) "Program Version"; break;
          case 75: tag=(char *) "Object Cycle"; break;
          case 80: tag=(char *) "Byline"; break;
          case 85: tag=(char *) "Byline Title"; break;
          case 90: tag=(char *) "City"; break;
          case 95: tag=(char *) "Province State"; break;
          case 100: tag=(char *) "Country Code"; break;
          case 101: tag=(char *) "Country"; break;
          case 103: tag=(char *) "Original Transmission Reference"; break;
          case 105: tag=(char *) "Headline"; break;
          case 110: tag=(char *) "Credit"; break;
          case 115: tag=(char *) "Source"; break;
          case 116: tag=(char *) "Copyright String"; break;
          case 120: tag=(char *) "Caption"; break;
          case 121: tag=(char *) "Local Caption"; break;
          case 122: tag=(char *) "Caption Writer"; break;
          case 200: tag=(char *) "Custom Field 1"; break;
          case 201: tag=(char *) "Custom Field 2"; break;
          case 202: tag=(char *) "Custom Field 3"; break;
          case 203: tag=(char *) "Custom Field 4"; break;
          case 204: tag=(char *) "Custom Field 5"; break;
          case 205: tag=(char *) "Custom Field 6"; break;
          case 206: tag=(char *) "Custom Field 7"; break;
          case 207: tag=(char *) "Custom Field 8"; break;
          case 208: tag=(char *) "Custom Field 9"; break;
          case 209: tag=(char *) "Custom Field 10"; break;
          case 210: tag=(char *) "Custom Field 11"; break;
          case 211: tag=(char *) "Custom Field 12"; break;
          case 212: tag=(char *) "Custom Field 13"; break;
          case 213: tag=(char *) "Custom Field 14"; break;
          case 214: tag=(char *) "Custom Field 15"; break;
          case 215: tag=(char *) "Custom Field 16"; break;
          case 216: tag=(char *) "Custom Field 17"; break;
          case 217: tag=(char *) "Custom Field 18"; break;
          case 218: tag=(char *) "Custom Field 19"; break;
          case 219: tag=(char *) "Custom Field 20"; break;
          default: tag=(char *) "unknown"; break;
        }
        i++;
        (void) fprintf(file,"    %.1024s:\n",tag);
        length=image->iptc_profile.info[i++] << 8;
        length|=image->iptc_profile.info[i++];
        text=(char *) AcquireMemory(length+1);
        if (text != (char *) NULL)
          {
            char
              **textlist;

            register long
              j;

            (void) strncpy(text,(char *) image->iptc_profile.info+i,length);
            text[length]='\0';
            textlist=StringToList(text);
            if (textlist != (char **) NULL)
              {
                for (j=0; textlist[j] != (char *) NULL; j++)
                {
                  (void) fprintf(file,"  %s\n",textlist[j]);
                  LiberateMemory((void **) &textlist[j]);
                }
                LiberateMemory((void **) &textlist);
              }
            LiberateMemory((void **) &text);
          }
        i+=length;
      }
    }
  for (i=0; i < (long) image->generic_profiles; i++)
  {
    if (image->generic_profile[i].length == 0)
      continue;
    (void) fprintf(file,"  Profile-%.1024s: %lu bytes\n",
      image->generic_profile[i].name == (char *) NULL ? "generic" :
      image->generic_profile[i].name,(unsigned long)
      image->generic_profile[i].length);
  }
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile geometry: %lux%lu%+ld%+ld\n",
      image->tile_info.width,image->tile_info.height,image->tile_info.x,
      image->tile_info.y);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    {
      /*
        Display image resolution.
      */
      (void) fprintf(file,"  Resolution: %gx%g",image->x_resolution,
        image->y_resolution);
      if (image->units == UndefinedResolution)
        (void) fprintf(file," pixels\n");
      else
        if (image->units == PixelsPerInchResolution)
          (void) fprintf(file," pixels/inch\n");
        else
          if (image->units == PixelsPerCentimeterResolution)
            (void) fprintf(file," pixels/centimeter\n");
          else
            (void) fprintf(file,"\n");
    }
  FormatSize(GetBlobSize(image),format);
  (void) fprintf(file,"  Filesize: %.1024s\n",format);
  if (image->interlace == NoInterlace)
    (void) fprintf(file,"  Interlace: None\n");
  else
    if (image->interlace == LineInterlace)
      (void) fprintf(file,"  Interlace: Line\n");
    else
      if (image->interlace == PlaneInterlace)
        (void) fprintf(file,"  Interlace: Plane\n");
    else
      if (image->interlace == PartitionInterlace)
        (void) fprintf(file,"  Interlace: Partition\n");
  (void) QueryColorname(image,&image->background_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->border_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->matte_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page geometry: %lux%lu%+ld%+ld\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  (void) fprintf(file,"  Dispose: ");
  switch (image->dispose)
  {
    case UndefinedDispose: (void) fprintf(file,"Undefined\n"); break;
    case NoneDispose: (void) fprintf(file,"None\n"); break;
    case BackgroundDispose: (void) fprintf(file,"Background\n"); break;
    case PreviousDispose: (void) fprintf(file,"Previous\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  if (image->delay != 0)
    (void) fprintf(file,"  Delay: %lu\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  Iterations: %lu\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  Scene: %lu of %lu\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  Scene: %lu\n",image->scene);
  (void) fprintf(file,"  Compression: ");
  switch (image->compression)
  {
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group 4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
    case LosslessJPEGCompression: (void) fprintf(file,"Lossless JPEG\n"); break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RLECompression: (void) fprintf(file,"Runlength Encoded\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  Montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
      Image
        *tile;

      ImageInfo
        *image_info;

      register char
        *p,
        *q;

      WarningHandler
        handler;

      /*
        Display visual image directory.
      */
      image_info=CloneImageInfo((ImageInfo *) NULL);
      (void) CloneString(&image_info->size,"64x64");
      (void) fprintf(file,"  Directory:\n");
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(image_info->filename,p,q-p);
        image_info->filename[q-p]='\0';
        p=q;
        (void) fprintf(file,"    %.1024s",image_info->filename);
        handler=SetWarningHandler((WarningHandler) NULL);
        tile=ReadImage(image_info,&image->exception);
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %lux%lu %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        (void) SignatureImage(tile);
        attribute=GetImageAttribute(tile,(char *) NULL);
        for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
        {
          if (*attribute->key == '[')
            continue;
          (void) fprintf(file,"  %.1024s:\n",attribute->key);
          (void) fprintf(file,"%s\n",attribute->value);
        }
        DestroyImage(tile);
      }
      DestroyImageInfo(image_info);
    }
  /*
    Display image attributes.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
  {
    if (*attribute->key == '[')
      continue;
    (void) fprintf(file,"  %.1024s: ",attribute->key);
    (void) fprintf(file,"%s\n",attribute->value);
  }
  if (image->taint)
    (void) fprintf(file,"  Tainted: True\n");
  else
    (void) fprintf(file,"  Tainted: False\n");
  if (user_time != 0.0)
    (void) fprintf(file,"  User Time: %.1fu\n",user_time);
  if (elapsed_time != 0.0)
    (void) fprintf(file,"  Elapsed Time: %ld:%02ld\n",
      (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
  (void) fflush(file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImage() dereferences an image, deallocating memory associated with
%  the image if the reference count becomes zero.
%
%  The format of the DestroyImage method is:
%
%      void DestroyImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImage(Image *image)
{
  int
    destroy;

  register long
    i;

  /*
    Dereference image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  destroy=False;
  AcquireSemaphoreInfo(&image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=True;
  LiberateSemaphoreInfo(&image->semaphore);
  if (!destroy)
    return;
  /*
    Destroy image.
  */
  DestroyImagePixels(image);
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  if (image->montage != (char *) NULL)
    LiberateMemory((void **) &image->montage);
  if (image->directory != (char *) NULL)
    LiberateMemory((void **) &image->directory);
  if (image->colormap != (PixelPacket *) NULL)
    LiberateMemory((void **) &image->colormap);
  if (image->color_profile.name != (char *) NULL)
    LiberateMemory((void **) &image->color_profile.name);
  if (image->color_profile.length != 0)
    LiberateMemory((void **) &image->color_profile.info);
  if (image->iptc_profile.name != (char *) NULL)
    LiberateMemory((void **) &image->iptc_profile.name);
  if (image->iptc_profile.length != 0)
    LiberateMemory((void **) &image->iptc_profile.info);
  if (image->generic_profiles != 0)
    {
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        if (image->generic_profile[i].name != (char *) NULL)
          LiberateMemory((void **) &image->generic_profile[i].name);
        if (image->generic_profile[i].length != 0)
          LiberateMemory((void **) &image->generic_profile[i].info);
      }
      LiberateMemory((void **) &image->generic_profile);
    }
  DestroyImageAttributes(image);
  DestroyExceptionInfo(&image->exception);
  if (image->ascii85 != (Ascii85Info *) NULL)
    LiberateMemory((void **) &image->ascii85);
  DestroyBlobInfo(image->blob);
  if (image->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(&image->semaphore);
  LiberateMemory((void **) &image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageInfo() deallocates memory associated with an ImageInfo
%  structure.
%
%  The format of the DestroyImageInfo method is:
%
%      void DestroyImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->size != (char *) NULL)
    LiberateMemory((void **) &image_info->size);
  if (image_info->tile != (char *) NULL)
    LiberateMemory((void **) &image_info->tile);
  if (image_info->page != (char *) NULL)
    LiberateMemory((void **) &image_info->page);
  if (image_info->server_name != (char *) NULL)
    LiberateMemory((void **) &image_info->server_name);
  if (image_info->font != (char *) NULL)
    LiberateMemory((void **) &image_info->font);
  if (image_info->texture != (char *) NULL)
    LiberateMemory((void **) &image_info->texture);
  if (image_info->density != (char *) NULL)
    LiberateMemory((void **) &image_info->density);
  if (image_info->view != (char *) NULL)
    LiberateMemory((void **) &image_info->view);
  if (image_info->authenticate != (char *) NULL)
    LiberateMemory((void **) &image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    DestroyImage(image_info->attributes);
  if (image_info->cache != (void *) NULL)
    DestroyCacheInfo(image_info->cache);
  LiberateMemory((void **) &image_info);
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p l a y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DisplayImages() displays an image sequence to any X window screen.  It
%  returns a value other than 0 if successful.  Check the exception member
%  of image to determine the reason for any failure.
%
%  The format of the DisplayImages method is:
%
%      unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  char
    *client_name;

  Display
    *display;

  Image
    *next;

  unsigned long
    state;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) XDisplayImage(display,&resource,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  (void) XCloseDisplay(display);
  return(image->exception.severity != UndefinedException);
}
#else
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,"X11 library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I m a g e B o u n d i n g B o x                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageBoundingBox returns the bounding box of an image canvas.
%
%  The format of the GetImageBoundingBox method is:
%
%      RectangleInfo GetImageBoundingBox(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o bounds: Method GetImageBoundingBox returns the bounding box of an
%      image canvas.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport RectangleInfo GetImageBoundingBox(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  PixelPacket
    corners[3];

  RectangleInfo
    bounds;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  bounds.width=0;
  bounds.height=0;
  bounds.x=(long) image->columns;
  bounds.y=(long) image->rows;
  corners[0]=AcquireOnePixel(image,0,0,exception);
  corners[1]=AcquireOnePixel(image,(long) image->columns-1,0,exception);
  corners[2]=AcquireOnePixel(image,0,(long) image->rows-1,exception);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (image->matte)
      for (x=0; x < (long) image->columns; x++)
      {
        if (p->opacity != corners[0].opacity)
          if (x < bounds.x)
            bounds.x=x;
        if (p->opacity != corners[1].opacity)
          if (x > (long) bounds.width)
            bounds.width=x;
        if (p->opacity != corners[0].opacity)
          if (y < bounds.y)
            bounds.y=y;
        if (p->opacity != corners[2].opacity)
          if (y > (long) bounds.height)
            bounds.height=y;
        p++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
          if (x < bounds.x)
            bounds.x=x;
        if (!FuzzyColorMatch(p,&corners[1],image->fuzz))
          if (x > (long) bounds.width)
            bounds.width=x;
        if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
          if (y < bounds.y)
            bounds.y=y;
        if (!FuzzyColorMatch(p,&corners[2],image->fuzz))
          if (y > (long) bounds.height)
            bounds.height=y;
        p++;
      }
  }
  if ((bounds.width != 0) || (bounds.height != 0))
    {
      bounds.width-=(bounds.x-1);
      bounds.height-=(bounds.y-1);
    }
  if (bounds.x < 0)
    bounds.x=0;
  if (bounds.y < 0)
    bounds.y=0;
  return(bounds);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageDepth() returns the depth of the image, either 8 or 16 bits.  By
%  default, pixels components are stored as 16-bit two byte unsigned short
%  integers that range in value from 0 to 65535L.  However, If all the pixels
%  have lower-order bytes that are identical to their higher-order bytes, the
%  image depth is 8-bit.
%
%  The format of the GetImageDepth method is:
%
%      unsigned long GetImageDepth(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned long GetImageDepth(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
#if (QuantumDepth == 8)
  return(QuantumDepth);
#endif
#if (QuantumDepth == 32)
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (p->red != ScaleShortToQuantum(ScaleQuantumToShort(p->red)))
        break;
      if (p->green != ScaleShortToQuantum(ScaleQuantumToShort(p->green)))
        break;
      if (p->blue != ScaleShortToQuantum(ScaleQuantumToShort(p->blue)))
        break;
      if (image->matte)
        if (p->opacity != ScaleShortToQuantum(ScaleQuantumToShort(p->opacity)))
          break;
      p++;
    }
    if (x < (long) image->columns)
      break;
  }
  if (y < (long) image->rows)
    return(QuantumDepth);
#endif
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (p->red != ScaleCharToQuantum(ScaleQuantumToChar(p->red)))
        return(16);
      if (p->green != ScaleCharToQuantum(ScaleQuantumToChar(p->green)))
        return(16);
      if (p->blue != ScaleCharToQuantum(ScaleQuantumToChar(p->blue)))
        return(16);
      if (image->matte)
        if (p->opacity != ScaleCharToQuantum(ScaleQuantumToChar(p->opacity)))
          return(16);
      p++;
    }
  }
  return(8);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e E x c e p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageException() traverses an image sequence and returns any
%  error more severe than noted by the exception parameter.
%
%  The format of the GetImageException method is:
%
%      void GetImageException(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a list of one or more images.
%
%    o exception: return the highest severity exception.
%
%
*/
MagickExport void GetImageException(Image *image,ExceptionInfo *exception)
{
  register Image
    *next;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->exception.severity == UndefinedException)
      continue;
    if (next->exception.severity > exception->severity)
      ThrowException(exception,next->exception.severity,
        next->exception.reason,next->exception.description);
    next->exception.severity=UndefinedException;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e G e o m e t r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageGeometry() returns a region as defined by the geometry string with
%  respect to the image and its gravity.
%
%  The format of the GetImageGeometry method is:
%
%      int GetImageGeometry(const Image *image,const char *geometry,
%        const unsigned int size_to_fit,RectangeInfo *region_info)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetImageGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o geometry:  The geometry (e.g. 100x100+10+10).
%
%    o size_to_fit:  A value other than 0 means to scale the region so it
%      fits within the specified width and height.
%
%    o region_info: The region as defined by the geometry string with
%      respect to the image and its gravity.
%
%
*/
MagickExport int GetImageGeometry(const Image *image,const char *geometry,
  const unsigned int size_to_fit,RectangleInfo *region_info)
{
  char
    region_geometry[MaxTextExtent];

  int
    flags;

  region_info->width=image->columns;
  region_info->height=image->rows;
  region_info->x=0;
  region_info->y=0;
  (void) strncpy(region_geometry,geometry,MaxTextExtent-2);
  if (!size_to_fit)
    (void) strcat(region_geometry,"!");
  flags=GetMagickGeometry(region_geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  switch (image->gravity)
  {
    case ForgetGravity:
    case NorthWestGravity:
      break;
    case NorthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      break;
    }
    case NorthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      break;
    }
    case WestGravity:
    {
      region_info->y+=(long) (image->rows/2-region_info->width/2);
      break;
    }
    case StaticGravity:
    case CenterGravity:
    default:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case EastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y+=(long) (image->rows/2-region_info->width/2);
      break;
    }
    case SouthWestGravity:
    {
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
  }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageInfo() initializes image_info to default values.
%
%  The format of the GetImageInfo method is:
%
%      void GetImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void GetImageInfo(ImageInfo *image_info)
{
  ExceptionInfo
    exception;

  /*
    File and image dimension members.
  */
  assert(image_info != (ImageInfo *) NULL);
  (void) memset(image_info,0,sizeof(ImageInfo));
  image_info->adjoin=True;
  image_info->depth=QuantumDepth;
  image_info->interlace=NoInterlace;
  image_info->quality=DefaultCompressionQuality;
  image_info->antialias=True;
  image_info->pointsize=12;
  image_info->dither=True;
  GetExceptionInfo(&exception);
  (void) QueryColorDatabase(BackgroundColor,&image_info->background_color,
    &exception);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color,&exception);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color,&exception);
  DestroyExceptionInfo(&exception);
  image_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageType() returns the type of image:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%
%
%  The format of the GetImageType method is:
%
%      ImageType GetImageType(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport ImageType GetImageType(const Image *image,ExceptionInfo *exception)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == CMYKColorspace)
    {
      if (!image->matte)
        return(ColorSeparationType);
      return(ColorSeparationMatteType);
    }
  if (IsGrayImage(image,exception))
    {
      if (IsMonochromeImage(image,exception))
        return(BilevelType);
      if (image->matte)
        return(GrayscaleMatteType);
      return(GrayscaleType);
    }
  if (IsPaletteImage(image,exception))
    {
      if (image->matte)
        return(PaletteMatteType);
      return(PaletteType);
    }
  if (!IsOpaqueImage(image,exception))
    return(TrueColorMatteType);
  return(TrueColorType);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     G r a d i e n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GradientImage() applies a continuously smooth color transitions along a
%  vector from one color to another.
%
%  Note, the interface of this method will change in the future to support
%  more than one transistion.
%
%  The format of the GradientImage method is:
%
%      unsigned int GradientImage(Image *image,const PixelPacket *start_color,
%        const PixelPacket *stop_color)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o start_color: The start color.
%
%    o stop_color: The stop color.
%
%
*/
MagickExport unsigned int GradientImage(Image *image,
  const PixelPacket *start_color,const PixelPacket *stop_color)
{
  double
    blackness,
    blackness_step,
    hue,
    hue_step,
    whiteness,
    whiteness_step;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  unsigned long
    number_pixels;

  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(start_color != (const PixelPacket *) NULL);
  assert(stop_color != (const PixelPacket *) NULL);
  (void) TransformHWB(start_color->red,start_color->green,start_color->blue,
    &hue,&whiteness,&blackness);
  (void) TransformHWB(stop_color->red,stop_color->green,stop_color->blue,
    &hue_step,&whiteness_step,&blackness_step);
  number_pixels=image->columns*image->rows;
  hue_step=(hue_step-hue)/number_pixels;
  whiteness_step=(whiteness_step-whiteness)/number_pixels;
  blackness_step=(blackness_step-blackness)/number_pixels;
  /*
    Initialize image pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      HWBTransform(hue,whiteness,blackness,&q->red,&q->green,&q->blue);
      q->opacity=OpaqueOpacity;
      q++;
      hue+=hue_step;
      whiteness+=whiteness_step;
      blackness+=blackness_step;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(LoadImageText,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s I m a g e s E q u a l                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsImagesEqual() measures the difference between colors at each pixel
%  location of two images.  A value other than 0 means the colors match
%  exactly.  Otherwise an error measure is computed by summing over all
%  pixels in an image the distance squared in RGB space between each image
%  pixel and its corresponding pixel in the reference image.  The error
%  measure is assigned to these image members:
%
%    o mean_error_per_pixel:  The mean error for any single pixel in
%      the image.
%
%    o normalized_mean_error:  The normalized mean quantization error for
%      any single pixel in the image.  This distance measure is normalized to
%      a range between 0 and 1.  It is independent of the range of red, green,
%      and blue values in the image.
%
%    o normalized_maximum_error:  The normalized maximum quantization
%      error for any single pixel in the image.  This distance measure is
%      normalized to a range between 0 and 1.  It is independent of the range
%      of red, green, and blue values in your image.
%
%  A small normalized mean square error, accessed as
%  image->normalized_mean_error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the IsImagesEqual method is:
%
%      unsigned int IsImagesEqual(Image *image,const Image *reference)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o reference: The reference image.
%
*/
MagickExport unsigned int IsImagesEqual(Image *image,const Image *reference)
{
  double
    distance,
    maximum_error_per_pixel,
    normalize;

  DoublePixelPacket
    pixel;

  long
    y;

  long double
    total_error;

  register const PixelPacket
    *p,
    *q;

  register long
    x;

  /*
    Initialize measurement.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(reference != (const Image *) NULL);
  assert(reference->signature == MagickSignature);
  memset(&image->error,0,sizeof(ErrorInfo));
  if ((image->rows != reference->rows) ||
      (image->columns != reference->columns))
    ThrowBinaryException(OptionError,"Unable to compare images",
      "images size differs");
  if (image->colorspace != reference->colorspace)
    ThrowBinaryException(OptionError,"Unable to compare images",
      "images colorspace differs");
  if(image->matte != reference->matte)
    ThrowBinaryException(OptionError,"Unable to compare images",
      "images opacity differs");
  /*
    For each pixel, collect error statistics.
  */
  maximum_error_per_pixel=0;
  total_error=0;
  pixel.opacity=0;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    q=AcquireImagePixels(reference,0,y,reference->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (!image->matte)
      for (x=0; x < (long) image->columns; x++)
      {
        pixel.red=p->red-(double) q->red;
        pixel.green=p->green-(double) q->green;
        pixel.blue=p->blue-(double) q->blue;
        distance=pixel.red*pixel.red+pixel.green*pixel.green+
          pixel.blue*pixel.blue;
        total_error+=distance;
        if (distance > maximum_error_per_pixel)
          maximum_error_per_pixel=distance;
        p++;
        q++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        pixel.red=p->red-(double) q->red;
        pixel.green=p->green-(double) q->green;
        pixel.blue=p->blue-(double) q->blue;
        pixel.opacity=p->opacity-(double) q->opacity;
        distance=pixel.red*pixel.red+pixel.green*pixel.green+
          pixel.blue*pixel.blue+pixel.opacity*pixel.opacity;
        total_error+=distance;
        if (distance > maximum_error_per_pixel)
          maximum_error_per_pixel=distance;
        p++;
        q++;
      }
  }
  /*
    Compute final error statistics.
  */
  normalize=3.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
  if (image->matte)
    normalize=4.0*((double) MaxRGB+1.0)*((double) MaxRGB+1.0);
  image->error.mean_error_per_pixel=total_error/image->columns/image->rows;
  image->error.normalized_mean_error=
    image->error.mean_error_per_pixel/normalize;
  image->error.normalized_maximum_error=maximum_error_per_pixel/normalize;
  return(image->error.normalized_mean_error == 0.0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s S u b i m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSubimage returns True if the geometry is a valid subimage
%  specification (e.g. [1], [1-9], [1,7,4]).
%
%  The format of the IsSubimage method is:
%
%      unsigned int IsSubimage(const char *geometry,const unsigned int pedantic)
%
%  A description of each parameter follows:
%
%    o status: Method IsSubimage returns True if the geometry is a valid
%      subimage specification otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%    o pedantic: A value other than 0 invokes a more restriction set of
%      conditions for a valid specification (e.g. [1], [1-4], [4-1]).
%
%
*/
MagickExport unsigned int IsSubimage(const char *geometry,
  const unsigned int pedantic)
{
  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=GetGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s T a i n t I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsTaintImage() returns a value other than 0 if any pixel in the image
%  has been altered since it was first constituted.
%
%  The format of the IsTaintImage method is:
%
%      unsigned int IsTaintImage(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int IsTaintImage(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) strncpy(magick,image->magick,MaxTextExtent-1);
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    if (p->taint)
      return(True);
    if (LocaleCompare(p->magick,magick) != 0)
      return(True);
    if (LocaleCompare(p->filename,filename) != 0)
      return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModifyImage() ensures that there is only a single reference to the image
%  to be modified, updating the provided image pointer to point to a clone of
%  the original image if necessary.
%
%  The format of the ModifyImage method is:
%
%      ModifyImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void ModifyImage(Image **image,ExceptionInfo *exception)
{
  Image
    *clone_image;

  unsigned int
    clone;

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  assert((*image)->signature == MagickSignature);
  clone=False;
  AcquireSemaphoreInfo(&(*image)->semaphore);
  if ((*image)->reference_count > 1)
    clone=True;
  LiberateSemaphoreInfo(&(*image)->semaphore);
  if (!clone)
    return;
  clone_image=CloneImage(*image,0,0,True,exception);
  AcquireSemaphoreInfo(&(*image)->semaphore);
  (*image)->reference_count--;
  LiberateSemaphoreInfo(&(*image)->semaphore);
  *image=clone_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImage applies image processing options to an image as
%  prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImage(const ImageInfo *image_info,const int argc,
%        char **argv,Image **image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImage(const ImageInfo *image_info,
  const int argc,char **argv,Image **image)
{
  char
    *option;

  DrawInfo
    *draw_info;

  Image
    *map_image,
    *region_image;

  ImageInfo
    *clone_info;

  int
    count;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    geometry,
    region_geometry;

  register long
    i;

  unsigned int
    matte;

  /*
    Verify option length.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  if (argc <= 0)
    return(True);
  for (i=0; i < argc; i++)
    if (strlen(argv[i]) > (MaxTextExtent/2-1))
      MagickFatalError(OptionError,"Option length exceeds limit",argv[i]);
  /*
    Initialize method variables.
  */
  clone_info=CloneImageInfo(image_info);
  draw_info=CloneDrawInfo(clone_info,(DrawInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  map_image=(Image *) NULL;
  quantize_info.number_colors=0;
  quantize_info.tree_depth=0;
  quantize_info.dither=True;
  if (clone_info->monochrome)
    if (!IsMonochromeImage(*image,&(*image)->exception))
      {
        quantize_info.number_colors=2;
        quantize_info.tree_depth=8;
        quantize_info.colorspace=GRAYColorspace;
      }
  SetGeometry(*image,&region_geometry);
  region_image=(Image *) NULL;
  /*
    Transmogrify the image.
  */
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            char
              *p;

            /*
              Draw affine matrix.
            */
            if (*option == '+')
              {
                IdentityAffine(&draw_info->affine);
                continue;
              }
            p=argv[++i];
            draw_info->affine.sx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.rx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ry=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.sy=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.tx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ty=strtod(p,&p);
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            clone_info->antialias=(*option == '-');
            draw_info->stroke_antialias=(*option == '-');
            draw_info->text_antialias=(*option == '-');
            break;
          }
        break;
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->background_color,
              &(*image)->exception);
            (*image)->background_color=clone_info->background_color;
            continue;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            /*
              Blue chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.blue_primary.x=0.0;
                (*image)->chromaticity.blue_primary.y=0.0;
                (*image)->chromaticity.blue_primary.z=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.blue_primary.x,
              &(*image)->chromaticity.blue_primary.y);
            (*image)->chromaticity.blue_primary.z=1.0-
              ((*image)->chromaticity.blue_primary.x+
               (*image)->chromaticity.blue_primary.y);
            continue;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            blur_image=BlurImage(*image,radius,sigma,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            Image
              *border_image;

            /*
              Surround image with a border of solid color.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            border_image=BorderImage(*image,&geometry,&(*image)->exception);
            if (border_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=border_image;
            continue;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->border_color,
              &(*image)->exception);
            draw_info->border_color=clone_info->border_color;
            (*image)->border_color=clone_info->border_color;
            continue;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->undercolor,
              &(*image)->exception);
            continue;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("channel",option+1) == 0)
          {
            ChannelType
              channel;

            option=argv[++i];
            channel=UndefinedChannel;
            if (LocaleCompare("Red",option) == 0)
              channel=RedChannel;
            if (LocaleCompare("Cyan",option) == 0)
              channel=CyanChannel;
            if (LocaleCompare("Green",option) == 0)
              channel=GreenChannel;
            if (LocaleCompare("Magenta",option) == 0)
              channel=MagentaChannel;
            if (LocaleCompare("Blue",option) == 0)
              channel=BlueChannel;
            if (LocaleCompare("Yellow",option) == 0)
              channel=YellowChannel;
            if (LocaleCompare("Opacity",option) == 0)
              channel=OpacityChannel;
            if (LocaleCompare("Black",option) == 0)
              channel=BlackChannel;
            if (LocaleCompare("Matte",option) == 0)
              channel=MatteChannel;
            if ((*image)->colorspace != clone_info->colorspace)
              (void) RGBTransformImage(*image,clone_info->colorspace);
            (void) ChannelImage(*image,channel);
            continue;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *charcoal_image;

            /*
              Charcoal image.
            */
            radius=0.0;
            sigma=1.0;
            (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            charcoal_image=
              CharcoalImage(*image,radius,sigma,&(*image)->exception);
            if (charcoal_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=charcoal_image;
            continue;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            Image
              *chop_image;

            /*
              Chop the image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            chop_image=ChopImage(*image,&geometry,&(*image)->exception);
            if (chop_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=chop_image;
            continue;
          }
        if (LocaleCompare("clip",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageClipMask(*image,(Image *) NULL);
                continue;
              }
            (void) ClipImage(*image);
            continue;
          }
        if (LocaleCompare("colorize",option+1) == 0)
          {
            Image
              *colorize_image;

            /*
              Colorize the image.
            */
            colorize_image=ColorizeImage(*image,argv[++i],draw_info->fill,
              &(*image)->exception);
            if (colorize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=colorize_image;
            continue;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info.number_colors=atol(argv[++i]);
            continue;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            char
              type;

            type=(*option);
            option=argv[++i];
            if (LocaleCompare("cmyk",option) == 0)
              {
                (void) RGBTransformImage(*image,CMYKColorspace);
                quantize_info.colorspace=CMYKColorspace;
              }
            if (LocaleCompare("gray",option) == 0)
              {
                (void) TransformRGBImage(*image,GRAYColorspace);
                quantize_info.colorspace=GRAYColorspace;
              }
            if (LocaleCompare("ohta",option) == 0)
              quantize_info.colorspace=OHTAColorspace;
            if (LocaleCompare("rgb",option) == 0)
              {
                (void) TransformRGBImage(*image,RGBColorspace);
                quantize_info.colorspace=RGBColorspace;
              }
            if (LocaleCompare("srgb",option) == 0)
              quantize_info.colorspace=sRGBColorspace;
            if (LocaleCompare("transparent",option) == 0)
              quantize_info.colorspace=TransparentColorspace;
            if (LocaleCompare("xyz",option) == 0)
              quantize_info.colorspace=XYZColorspace;
            if (LocaleCompare("ycbcr",option) == 0)
              quantize_info.colorspace=YCbCrColorspace;
            if (LocaleCompare("ycc",option) == 0)
              quantize_info.colorspace=YCCColorspace;
            if (LocaleCompare("yiq",option) == 0)
              quantize_info.colorspace=YIQColorspace;
            if (LocaleCompare("ypbpr",option) == 0)
              quantize_info.colorspace=YPbPrColorspace;
            if (LocaleCompare("yuv",option) == 0)
              quantize_info.colorspace=YUVColorspace;
            clone_info->colorspace=quantize_info.colorspace;
            if (type == '+')
              (*image)->colorspace=clone_info->colorspace;
            continue;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              (void) SetImageAttribute(*image,"comment",argv[++i]);
            else
              (void) SetImageAttribute(*image,"comment",(char *) NULL);
            continue;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
            CompositeOperator
              compose;

            option=argv[++i];
            compose=OverCompositeOp;
            if (LocaleCompare("Over",option) == 0)
              compose=OverCompositeOp;
            if (LocaleCompare("In",option) == 0)
              compose=InCompositeOp;
            if (LocaleCompare("Out",option) == 0)
              compose=OutCompositeOp;
            if (LocaleCompare("Atop",option) == 0)
              compose=AtopCompositeOp;
            if (LocaleCompare("Xor",option) == 0)
              compose=XorCompositeOp;
            if (LocaleCompare("Plus",option) == 0)
              compose=PlusCompositeOp;
            if (LocaleCompare("Minus",option) == 0)
              compose=MinusCompositeOp;
            if (LocaleCompare("Add",option) == 0)
              compose=AddCompositeOp;
            if (LocaleCompare("Subtract",option) == 0)
              compose=SubtractCompositeOp;
            if (LocaleCompare("Difference",option) == 0)
              compose=DifferenceCompositeOp;
            if (LocaleCompare("Multiply",option) == 0)
              compose=MultiplyCompositeOp;
            if (LocaleCompare("Bumpmap",option) == 0)
              compose=BumpmapCompositeOp;
            if (LocaleCompare("Copy",option) == 0)
              compose=CopyCompositeOp;
            if (LocaleCompare("CopyRed",option) == 0)
              compose=CopyRedCompositeOp;
            if (LocaleCompare("CopyGreen",option) == 0)
              compose=CopyGreenCompositeOp;
            if (LocaleCompare("CopyBlue",option) == 0)
              compose=CopyBlueCompositeOp;
            if (LocaleCompare("CopyOpacity",option) == 0)
              compose=CopyOpacityCompositeOp;
            if (LocaleCompare("Clear",option) == 0)
              compose=ClearCompositeOp;
            (*image)->compose=compose;
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            CompressionType
              compression;

            if (*option == '+')
              {
                (*image)->compression=UndefinedCompression;
                continue;
              }
            option=argv[++i];
            compression=UndefinedCompression;
            if (LocaleCompare("None",option) == 0)
              compression=NoCompression;
            if (LocaleCompare("BZip",option) == 0)
              compression=BZipCompression;
            if (LocaleCompare("Fax",option) == 0)
              compression=FaxCompression;
            if (LocaleCompare("Group4",option) == 0)
              compression=Group4Compression;
            if (LocaleCompare("JPEG",option) == 0)
              compression=JPEGCompression;
            if (LocaleCompare("Lossless",option) == 0)
              compression=LosslessJPEGCompression;
            if (LocaleCompare("LZW",option) == 0)
              compression=LZWCompression;
            if (LocaleCompare("RLE",option) == 0)
              compression=RLECompression;
            if (LocaleCompare("Zip",option) == 0)
              compression=ZipCompression;
            (*image)->compression=compression;
            continue;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          {
            (void) ContrastImage(*image,*option == '-');
            continue;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            TransformImage(image,argv[++i],(char *) NULL);
            continue;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            /*
              Cycle an image colormap.
            */
            CycleColormapImage(*image,atoi(argv[++i]));
            continue;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("delay",option+1) == 0)
          {
            double
              maximum_delay,
              minimum_delay;

            /*
              Set image delay.
            */
            if (*option == '+')
              {
                (*image)->delay=0;
                continue;
              }
            count=sscanf(argv[++i],"%lf-%lf",&minimum_delay,&maximum_delay);
            if (count == 1)
              (*image)->delay=(unsigned long) minimum_delay;
            else
              {
                if ((*image)->delay < minimum_delay)
                  (*image)->delay=(unsigned long) minimum_delay;
                if ((*image)->delay > maximum_delay)
                  (*image)->delay=(unsigned long) maximum_delay;
              }
            continue;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            /*
              Set image density.
            */
            (void) CloneString(&clone_info->density,argv[++i]);
            (void) CloneString(&draw_info->density,clone_info->density);
            count=sscanf(clone_info->density,"%lfx%lf",
              &(*image)->x_resolution,&(*image)->y_resolution);
            if (count != 2)
              (*image)->y_resolution=(*image)->x_resolution;
            continue;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            (void) SetImageDepth(*image,atol(argv[++i]));
            continue;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          {
            Image
              *despeckle_image;

            /*
              Reduce the speckles within an image.
            */
            despeckle_image=DespeckleImage(*image,&(*image)->exception);
            if (despeckle_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=despeckle_image;
            continue;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&clone_info->server_name,argv[++i]);
            (void) CloneString(&draw_info->server_name,clone_info->server_name);
            continue;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            DisposeType
              dispose;;

            if (*option == '+')
              {
                (*image)->dispose=UndefinedDispose;
                continue;
              }
            option=argv[++i];
            dispose=UndefinedDispose;
            if (LocaleCompare("0",option) == 0)
              dispose=UndefinedDispose;
            if (LocaleCompare("1",option) == 0)
              dispose=NoneDispose;
            if (LocaleCompare("2",option) == 0)
              dispose=BackgroundDispose;
            if (LocaleCompare("3",option) == 0)
              dispose=PreviousDispose;
            if (LocaleCompare("Background",option) == 0)
              dispose=BackgroundDispose;
            if (LocaleCompare("None",option) == 0)
              dispose=NoneDispose;
            if (LocaleCompare("Previous",option) == 0)
              dispose=PreviousDispose;
            if (LocaleCompare("Undefined",option) == 0)
              dispose=UndefinedDispose;
            (*image)->dispose=dispose;
            continue;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            clone_info->dither=(*option == '-');
            quantize_info.dither=(*option == '-');
            continue;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            /*
              Draw image.
            */
            (void) CloneString(&draw_info->primitive,argv[++i]);
            (void) DrawImage(*image,draw_info);
            continue;
          }
        break;
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            double
              radius;

            Image
              *edge_image;

            /*
              Enhance edges in the image.
            */
            radius=atof(argv[++i]);
            edge_image=EdgeImage(*image,radius,&(*image)->exception);
            if (edge_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=edge_image;
            continue;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *emboss_image;

            /*
              Emboss the image.
            */
            radius=0.0;
            sigma=1.0;
            (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            emboss_image=EmbossImage(*image,radius,sigma,&(*image)->exception);
            if (emboss_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=emboss_image;
            continue;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            (void) CloneString(&draw_info->encoding,argv[++i]);
            continue;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            EndianType
              endian;

            if (*option == '+')
              {
                clone_info->endian=LSBEndian;
                continue;
              }
            option=argv[++i];
            endian=UndefinedEndian;
            if (LocaleCompare("LSB",option) == 0)
              endian=LSBEndian;
            if (LocaleCompare("MSB",option) == 0)
              endian=MSBEndian;
            clone_info->endian=endian;
            continue;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          {
            Image
              *enhance_image;

            /*
              Enhance image.
            */
            enhance_image=EnhanceImage(*image,&(*image)->exception);
            if (enhance_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=enhance_image;
            continue;
          }
        if (LocaleCompare("equalize",option+1) == 0)
          {
            /*
              Equalize image.
            */
            (void) EqualizeImage(*image);
            continue;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->fill,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            FilterTypes
              filter;

            if (*option == '+')
              {
                (*image)->filter=LanczosFilter;
                continue;
              }
            option=argv[++i];
            filter=LanczosFilter;
            if (LocaleCompare("Point",option) == 0)
              filter=PointFilter;
            if (LocaleCompare("Box",option) == 0)
              filter=BoxFilter;
            if (LocaleCompare("Triangle",option) == 0)
              filter=TriangleFilter;
            if (LocaleCompare("Hermite",option) == 0)
              filter=HermiteFilter;
            if (LocaleCompare("Hanning",option) == 0)
              filter=HanningFilter;
            if (LocaleCompare("Hamming",option) == 0)
              filter=HammingFilter;
            if (LocaleCompare("Blackman",option) == 0)
              filter=BlackmanFilter;
            if (LocaleCompare("Gaussian",option) == 0)
              filter=GaussianFilter;
            if (LocaleCompare("Quadratic",option) == 0)
              filter=QuadraticFilter;
            if (LocaleCompare("Cubic",option) == 0)
              filter=CubicFilter;
            if (LocaleCompare("Catrom",option) == 0)
              filter=CatromFilter;
            if (LocaleCompare("Mitchell",option) == 0)
              filter=MitchellFilter;
            if (LocaleCompare("Lanczos",option) == 0)
              filter=LanczosFilter;
            if (LocaleCompare("Bessel",option) == 0)
              filter=BesselFilter;
            if (LocaleCompare("Sinc",option) == 0)
              filter=SincFilter;
            (*image)->filter=filter;
            continue;
          }
        if (LocaleCompare("flip",option+1) == 0)
          {
            Image
              *flip_image;

            /*
              Flip image scanlines.
            */
            flip_image=FlipImage(*image,&(*image)->exception);
            if (flip_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flip_image;
            continue;
          }
        if (LocaleCompare("flop",option+1) == 0)
          {
            Image
              *flop_image;

            /*
              Flop image scanlines.
            */
            flop_image=FlopImage(*image,&(*image)->exception);
            if (flop_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flop_image;
            continue;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            Image
              *frame_image;

            FrameInfo
              frame_info;

            /*
              Surround image with an ornamental border.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            frame_info.width=geometry.width;
            frame_info.height=geometry.height;
            frame_info.outer_bevel=geometry.x;
            frame_info.inner_bevel=geometry.y;
            frame_info.x=(long) frame_info.width;
            frame_info.y=(long) frame_info.height;
            frame_info.width=(*image)->columns+2*frame_info.width;
            frame_info.height=(*image)->rows+2*frame_info.height;
            frame_image=FrameImage(*image,&frame_info,&(*image)->exception);
            if (frame_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=frame_image;
            continue;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            (*image)->fuzz=StringToDouble(argv[++i],MaxRGB);
            continue;
          }
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&clone_info->font,argv[++i]);
            (void) CloneString(&draw_info->font,clone_info->font);
            continue;
          }
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            if (*option == '+')
              (*image)->gamma=atof(argv[++i]);
            else
              (void) GammaImage(*image,argv[++i]);
            continue;
          }
        if (LocaleCompare("gaussian",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            blur_image=
              GaussianBlurImage(*image,radius,sigma,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            Image
              *zoom_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            zoom_image=ZoomImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (zoom_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=zoom_image;
            continue;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            if (*option == '+')
              {
                draw_info->gravity=(GravityType) ForgetGravity;
                (*image)->gravity=(GravityType) ForgetGravity;
                continue;
              }
            option=argv[++i];
            gravity=(GravityType) ForgetGravity;
            if (LocaleCompare("NorthWest",option) == 0)
              gravity=(GravityType) NorthWestGravity;
            if (LocaleCompare("North",option) == 0)
              gravity=(GravityType) NorthGravity;
            if (LocaleCompare("NorthEast",option) == 0)
              gravity=(GravityType) NorthEastGravity;
            if (LocaleCompare("West",option) == 0)
              gravity=(GravityType) WestGravity;
            if (LocaleCompare("Center",option) == 0)
              gravity=(GravityType) CenterGravity;
            if (LocaleCompare("East",option) == 0)
              gravity=(GravityType) EastGravity;
            if (LocaleCompare("SouthWest",option) == 0)
              gravity=(GravityType) SouthWestGravity;
            if (LocaleCompare("South",option) == 0)
              gravity=(GravityType) SouthGravity;
            if (LocaleCompare("SouthEast",option) == 0)
              gravity=(GravityType) SouthEastGravity;
            draw_info->gravity=gravity;
            (*image)->gravity=gravity;
            continue;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            /*
              Green chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.green_primary.x=0.0;
                (*image)->chromaticity.green_primary.y=0.0;
                (*image)->chromaticity.green_primary.z=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.green_primary.x,
              &(*image)->chromaticity.green_primary.y);
            (*image)->chromaticity.green_primary.z=1.0-
              ((*image)->chromaticity.green_primary.x+
               (*image)->chromaticity.green_primary.y);
            continue;
          }
        break;
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            double
              amount;

            Image
              *implode_image;

            /*
              Implode image.
            */
            amount=atof(argv[++i]);
            implode_image=ImplodeImage(*image,amount,&(*image)->exception);
            if (implode_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=implode_image;
            continue;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            InterlaceType
              interlace;

            if (*option == '+')
              {
                clone_info->interlace=NoInterlace;
                continue;
              }
            option=argv[++i];
            interlace=UndefinedInterlace;
            if (LocaleCompare("None",option) == 0)
              interlace=NoInterlace;
            if (LocaleCompare("Line",option) == 0)
              interlace=LineInterlace;
            if (LocaleCompare("Plane",option) == 0)
              interlace=PlaneInterlace;
            if (LocaleCompare("Partition",option) == 0)
              interlace=PartitionInterlace;
            clone_info->interlace=interlace;
            continue;
          }
        if (LocaleCompare("intent",option+1) == 0)
          {
            RenderingIntent
              rendering_intent;

            if (*option == '+')
              {
                (*image)->rendering_intent=UndefinedIntent;
                continue;
              }
            option=argv[++i];
            rendering_intent=UndefinedIntent;
            if (LocaleCompare("Absolute",option) == 0)
              rendering_intent=AbsoluteIntent;
            if (LocaleCompare("Perceptual",option) == 0)
              rendering_intent=PerceptualIntent;
            if (LocaleCompare("Relative",option) == 0)
              rendering_intent=RelativeIntent;
            if (LocaleCompare("Saturation",option) == 0)
              rendering_intent=SaturationIntent;
            (*image)->rendering_intent=rendering_intent;
            continue;
          }
        break;
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            (void) SetImageAttribute(*image,"label",(char *) NULL);
            if (*option == '-')
              (void) SetImageAttribute(*image,"label",argv[++i]);
            continue;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            (void) LevelImage(*image,argv[++i]);
            continue;
          }
        if (LocaleCompare("linewidth",option+1) == 0)
          {
            draw_info->stroke_width=atof(argv[++i]);
            continue;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            /*
              Set image iterations.
            */
            (*image)->iterations=atol(argv[++i]);
            continue;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            /*
              Transform image colors to match this set of colors.
            */
            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            map_image=ReadImage(clone_info,&(*image)->exception);
            continue;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            Image
              *mask;

            long
              y;

            register long
              x;

            register PixelPacket
              *q;

            if (*option == '+')
              {
                /*
                  Remove a clip mask.
                */
                (void) SetImageClipMask(*image,(Image *) NULL);
                continue;
              }
            /*
              Set the image clip mask.
            */
            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            mask=ReadImage(clone_info,&(*image)->exception);
            if (mask == (Image *) NULL)
              continue;
            for (y=0; y < (long) mask->rows; y++)
            {
              q=GetImagePixels(mask,0,y,mask->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) mask->columns; x++)
              {
                if (!mask->matte)
                  q->opacity=PixelIntensityToQuantum(q);
                q->red=q->opacity;
                q->green=q->opacity;
                q->blue=q->opacity;
                q++;
              }
              if (!SyncImagePixels(mask))
                break;
            }
            SetImageType(mask,TrueColorMatteType);
            (void) SetImageClipMask(*image,mask);
          }
        if (LocaleCompare("matte",option+1) == 0)
          {
            if (*option == '-')
              if (!(*image)->matte)
                SetImageOpacity(*image,OpaqueOpacity);
            (*image)->matte=(*option == '-');
            continue;
          }
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->matte_color,
              &(*image)->exception);
            (*image)->matte_color=clone_info->matte_color;
            continue;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            double
              radius;

            Image
              *median_image;

            /*
              Median filter image.
            */
            radius=atof(argv[++i]);
            median_image=MedianFilterImage(*image,radius,&(*image)->exception);
            if (median_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=median_image;
            continue;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            (void) ModulateImage(*image,argv[++i]);
            continue;
          }
        if ((LocaleCompare("mono",option+1) == 0) ||
            (LocaleCompare("monochrome",option+1) == 0))
          {
            clone_info->monochrome=True;
            quantize_info.number_colors=2;
            quantize_info.tree_depth=8;
            quantize_info.colorspace=GRAYColorspace;
            continue;
          }
        break;
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          {
            (void) NegateImage(*image,*option == '+');
            continue;
          }
        if (LocaleCompare("noise",option+1) == 0)
          {
            Image
              *noisy_image;

            if (*option == '-')
              noisy_image=
                ReduceNoiseImage(*image,atol(argv[++i]),&(*image)->exception);
            else
              {
                NoiseType
                  noise_type;

                /*
                  Add noise to image.
                */
                option=argv[++i];
                noise_type=UniformNoise;
                if (LocaleCompare("Gaussian",option) == 0)
                  noise_type=GaussianNoise;
                if (LocaleCompare("multiplicative",option) == 0)
                  noise_type=MultiplicativeGaussianNoise;
                if (LocaleCompare("impulse",option) == 0)
                  noise_type=ImpulseNoise;
                if (LocaleCompare("laplacian",option) == 0)
                  noise_type=LaplacianNoise;
                if (LocaleCompare("Poisson",option) == 0)
                  noise_type=PoissonNoise;
                noisy_image=
                  AddNoiseImage(*image,noise_type,&(*image)->exception);
              }
            if (noisy_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=noisy_image;
            continue;
          }
        if (LocaleCompare("normalize",option+1) == 0)
          {
            (void) NormalizeImage(*image);
            continue;
          }
        break;
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            PixelPacket
              target;

            target=AcquireOnePixel(*image,0,0,&(*image)->exception);
            (void) QueryColorDatabase(argv[++i],&target,&(*image)->exception);
            (void) OpaqueImage(*image,target,draw_info->fill);
            continue;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            char
              *geometry;

            geometry=GetPageGeometry(argv[++i]);
            (void) GetGeometry(geometry,&(*image)->page.x,&(*image)->page.y,
              &(*image)->page.width,&(*image)->page.height);
            LiberateMemory((void **) &geometry);
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            double
              radius;

            Image
              *paint_image;

            /*
              Oil paint image.
            */
            radius=atof(argv[++i]);
            paint_image=OilPaintImage(*image,radius,&(*image)->exception);
            if (paint_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=paint_image;
            continue;
          }
        if (LocaleCompare("pen",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->fill,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            clone_info->pointsize=atof(argv[++i]);
            draw_info->pointsize=clone_info->pointsize;
            continue;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            Image
              *profile;

            register long
              j;

            void
              *client_data;

            if (*option == '+')
              {
                /*
                  Remove a ICM, IPTC, or generic profile from the image.
                */
                (void) ProfileImage(*image,argv[++i],
                  (const unsigned char *) NULL,0,True);
                continue;
              }
            /*
              Add a ICM, IPTC, or generic profile to the image.
            */
            client_data=clone_info->client_data;
            clone_info->client_data=(void *) &(*image)->iptc_profile;
            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            profile=ReadImage(clone_info,&(*image)->exception);
            if (profile == (Image *) NULL)
              continue;
            if (profile->iptc_profile.length != 0)
              {
                (void) ProfileImage(*image,"IPTC",profile->iptc_profile.info,
                  profile->iptc_profile.length,False);
                profile->iptc_profile.info=(unsigned char *) NULL,
                profile->iptc_profile.length=0;
              }
            if (profile->color_profile.length != 0)
              {
                (void) ProfileImage(*image,"ICM",profile->color_profile.info,
                  profile->color_profile.length,False);
                profile->color_profile.info=(unsigned char *) NULL;
                profile->color_profile.length=0;
              }
            for (j=0; j < (long) profile->generic_profiles; j++)
            {
              ProfileInfo
                *generic;

              generic=profile->generic_profile+j;
              (void) ProfileImage(*image,generic->name,generic->info,
                generic->length,False);
              generic->info=(unsigned char *) NULL;
              generic->length=0;
            }
            DestroyImage(profile);
            clone_info->client_data=client_data;
            continue;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            /*
              Surround image with a raise of solid color.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            (void) RaiseImage(*image,&geometry,*option == '-');
            continue;
          }
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            /*
              Red chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.red_primary.x=0.0;
                (*image)->chromaticity.red_primary.y=0.0;
                (*image)->chromaticity.red_primary.z=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.red_primary.x,
              &(*image)->chromaticity.red_primary.y);
            (*image)->chromaticity.red_primary.z=1.0-
              ((*image)->chromaticity.red_primary.x+
               (*image)->chromaticity.red_primary.y);
            continue;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            Image
              *crop_image;

            if (region_image != (Image *) NULL)
              {
                /*
                  Composite region.
                */
                (void) CompositeImage(region_image,(*image)->matte ?
                  OverCompositeOp : CopyCompositeOp,*image,region_geometry.x,
                  region_geometry.y);
                DestroyImage(*image);
                *image=region_image;
              }
            if (*option == '+')
              continue;
            /*
              Apply transformations to a selected region of the image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&region_geometry);
            crop_image=CropImage(*image,&region_geometry,&(*image)->exception);
            if (crop_image == (Image *) NULL)
              break;
            region_image=(*image);
            *image=crop_image;
            continue;
          }
        if (LocaleCompare("render",option+1) == 0)
          {
            draw_info->render=(*option == '+');
            continue;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            Image
              *resize_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            resize_image=ResizeImage(*image,geometry.width,geometry.height,
              (*image)->filter,(*image)->blur,&(*image)->exception);
            if (resize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=resize_image;
            continue;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            Image
              *roll_image;

            /*
              Roll image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            roll_image=RollImage(*image,geometry.x,geometry.y,
              &(*image)->exception);
            if (roll_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=roll_image;
            continue;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            double
              degrees;

            Image
              *rotate_image;

            /*
              Check for conditional image rotation.
            */
            i++;
            if (strchr(argv[i],'>') != (char *) NULL)
              if ((*image)->columns <= (*image)->rows)
                break;
            if (strchr(argv[i],'<') != (char *) NULL)
              if ((*image)->columns >= (*image)->rows)
                break;
            /*
              Rotate image.
            */
            degrees=atof(argv[i]);
            rotate_image=RotateImage(*image,degrees,&(*image)->exception);
            if (rotate_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=rotate_image;
            continue;
          }
        break;
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            Image
              *sample_image;

            /*
              Sample image with pixel replication.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            sample_image=SampleImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (sample_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sample_image;
            continue;
          }
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            /*
              Set image sampling factor.
            */
            (void) CloneString(&clone_info->sampling_factor,argv[++i]);
            continue;
          }
        if (LocaleCompare("sans",option+1) == 0)
          if (*option == '-')
            i++;
        if (LocaleCompare("scale",option+1) == 0)
          {
            Image
              *scale_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            scale_image=ScaleImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (scale_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=scale_image;
            continue;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            (*image)->scene=atol(argv[++i]);
            continue;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            double
              cluster_threshold,
              smoothing_threshold;

            /*
              Segment image.
            */
            cluster_threshold=1.0;
            smoothing_threshold=1.5;
            (void) sscanf(argv[++i],"%lfx%lf",&cluster_threshold,
              &smoothing_threshold);
            (void) SegmentImage(*image,quantize_info.colorspace,
              clone_info->verbose,cluster_threshold,smoothing_threshold);
            continue;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            double
              azimuth,
              elevation;

            Image
              *shade_image;

            /*
              Shade image.
            */
            azimuth=30.0;
            elevation=30.0;
            (void) sscanf(argv[++i],"%lfx%lf",&azimuth,&elevation);
            shade_image=ShadeImage(*image,*option == '-',azimuth,elevation,
              &(*image)->exception);
            if (shade_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shade_image;
            continue;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *sharp_image;

            /*
              Gaussian sharpen image.
            */
            radius=0.0;
            sigma=1.0;
            (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            sharp_image=SharpenImage(*image,radius,sigma,&(*image)->exception);
            if (sharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sharp_image;
            continue;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            Image
              *shave_image;

            /*
              Shave the image edges.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            shave_image=ShaveImage(*image,&geometry,&(*image)->exception);
            if (shave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shave_image;
            continue;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            double
              x_shear,
              y_shear;

            Image
              *shear_image;

            /*
              Shear image.
            */
            x_shear=0.0;
            y_shear=0.0;
            (void) sscanf(argv[++i],"%lfx%lf",&x_shear,&y_shear);
            shear_image=ShearImage(*image,x_shear,y_shear,&(*image)->exception);
            if (shear_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shear_image;
            continue;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            double
              threshold;

            threshold=StringToDouble(argv[++i],MaxRGB);
            SolarizeImage(*image,threshold);
            continue;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            unsigned int
              amount;

            Image
              *spread_image;

            /*
              Spread an image.
            */
            amount=atoi(argv[++i]);
            spread_image=SpreadImage(*image,amount,&(*image)->exception);
            if (spread_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=spread_image;
            continue;
          }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->stroke,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            draw_info->stroke_width=atof(argv[++i]);
            continue;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            double
              degrees;

            Image
              *swirl_image;

            /*
              Swirl image.
            */
            degrees=atof(argv[++i]);
            swirl_image=SwirlImage(*image,degrees,&(*image)->exception);
            if (swirl_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=swirl_image;
            continue;
          }
        break;
      }
      case 't':
      {
        if (LocaleCompare("threshold",option+1) == 0)
          {
            /*
              Threshold image.
            */
            (void) ThresholdImage(*image,argv[++i]);
            continue;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            Image
              *fill_pattern;

            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            fill_pattern=ReadImage(clone_info,&(*image)->exception);
            if (fill_pattern == (Image *) NULL)
              continue;
            draw_info->fill_pattern=
              CloneImage(fill_pattern,0,0,True,&(*image)->exception);
            DestroyImage(fill_pattern);
            continue;
          }
        if (LocaleCompare("transform",option+1) == 0)
          {
            Image
              *transform_image;

            /*
              Affine transform image.
            */
            transform_image=AffineTransformImage(*image,&draw_info->affine,
              &(*image)->exception);
            if (transform_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=transform_image;
            continue;
          }
        if (LocaleCompare("transparent",option+1) == 0)
          {
            PixelPacket
              target;

            target=AcquireOnePixel(*image,0,0,&(*image)->exception);
            (void) QueryColorDatabase(argv[++i],&target,&(*image)->exception);
            (void) TransparentImage(*image,target,TransparentOpacity);
            continue;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info.tree_depth=atoi(argv[++i]);
            continue;
          }
        if (LocaleCompare("trim",option+1) == 0)
          {
            TransformImage(image,"0x0",(char *) NULL);
            continue;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            ImageType
              image_type;

            option=argv[++i];
            image_type=GetImageType(*image,&(*image)->exception);
            if (LocaleCompare("Bilevel",option) == 0)
              image_type=BilevelType;
            if (LocaleCompare("Grayscale",option) == 0)
              image_type=GrayscaleType;
            if (LocaleCompare("GrayscaleMatte",option) == 0)
              image_type=GrayscaleMatteType;
            if (LocaleCompare("Palette",option) == 0)
              image_type=PaletteType;
            if (LocaleCompare("PaletteMatte",option) == 0)
              image_type=PaletteMatteType;
            if (LocaleCompare("TrueColor",option) == 0)
              image_type=TrueColorType;
            if (LocaleCompare("TrueColorMatte",option) == 0)
              image_type=TrueColorMatteType;
            if (LocaleCompare("ColorSeparation",option) == 0)
              image_type=ColorSeparationType;
            if (LocaleCompare("ColorSeparationMatte",option) == 0)
              image_type=ColorSeparationMatteType;
            if (LocaleCompare("Optimize",option) == 0)
              image_type=OptimizeType;
            SetImageType(*image,image_type);
            continue;
          }
        break;
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->undercolor,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            ResolutionType
              resolution;

            if (*option == '+')
              {
                (*image)->units=UndefinedResolution;
                continue;
              }
            option=argv[++i];
            resolution=UndefinedResolution;
            if (LocaleCompare("PixelsPerInch",option) == 0)
              resolution=PixelsPerInchResolution;
            if (LocaleCompare("PixelsPerCentimeter",option) == 0)
              resolution=PixelsPerCentimeterResolution;
            (*image)->units=resolution;
            continue;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            double
              amount,
              radius,
              sigma,
              threshold;

            Image
              *unsharp_image;

            /*
              Gaussian unsharpen image.
            */
            amount=1.0;
            radius=0.0;
            sigma=1.0;
            threshold=0.05;
            (void) sscanf(argv[++i],"%lfx%lf%lf%lf",&radius,&sigma,&amount,
              &threshold);
            unsharp_image=UnsharpMaskImage(*image,radius,sigma,amount,threshold,
              &(*image)->exception);
            if (unsharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=unsharp_image;
            continue;
          }
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            clone_info->verbose=(*option == '-');
            quantize_info.measure_error=(*option == '-');
            continue;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            VirtualPixelMethod
              virtual_pixel_method;

            if (*option == '+')
              {
                SetImageVirtualPixelMethod(*image,UndefinedVirtualPixelMethod);
                continue;
              }
            option=argv[++i];
            virtual_pixel_method=UndefinedVirtualPixelMethod;
            if (LocaleCompare("Constant",option) == 0)
              virtual_pixel_method=ConstantVirtualPixelMethod;
            if (LocaleCompare("Edge",option) == 0)
              virtual_pixel_method=EdgeVirtualPixelMethod;
            if (LocaleCompare("Mirror",option) == 0)
              virtual_pixel_method=MirrorVirtualPixelMethod;
            if (LocaleCompare("Tile",option) == 0)
              virtual_pixel_method=TileVirtualPixelMethod;
            SetImageVirtualPixelMethod(*image,virtual_pixel_method);
            continue;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            double
              amplitude,
              wavelength;

            Image
              *wave_image;

            /*
              Wave image.
            */
            amplitude=25.0;
            wavelength=150.0;
            (void) sscanf(argv[++i],"%lfx%lf",&amplitude,&wavelength);
            wave_image=WaveImage(*image,amplitude,wavelength,
              &(*image)->exception);
            if (wave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=wave_image;
            continue;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            /*
              White chromaticity point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.white_point.x=0.0;
                (*image)->chromaticity.white_point.y=0.0;
                (*image)->chromaticity.white_point.z=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.white_point.x,
              &(*image)->chromaticity.white_point.y);
            (*image)->chromaticity.white_point.z=1.0-
              ((*image)->chromaticity.white_point.x+
               (*image)->chromaticity.white_point.y);
            continue;
          }
        break;
      }
      default:
        break;
    }
  }
  if (region_image != (Image *) NULL)
    {
      /*
        Composite transformed region onto image.
      */
      matte=region_image->matte;
      (void) CompositeImage(region_image,
        (*image)->matte ? OverCompositeOp : CopyCompositeOp,*image,
        region_geometry.x,region_geometry.y);
      DestroyImage(*image);
      *image=region_image;
      (*image)->matte=matte;
    }
  if ((quantize_info.number_colors != 0) ||
      (quantize_info.colorspace == GRAYColorspace))
    {
      /*
        Reduce the number of colors in the image.
      */
      if (((*image)->storage_class == DirectClass) ||
          ((*image)->colors > quantize_info.number_colors) ||
          (quantize_info.colorspace == GRAYColorspace))
        (void) QuantizeImage(&quantize_info,*image);
      else
        CompressImageColormap(*image);
    }
  if (map_image != (Image *) NULL)
    {
      (void) MapImage(*image,map_image,quantize_info.dither);
      DestroyImage(map_image);
    }
  /*
    Free resources.
  */
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(clone_info);
  return((*image)->exception.severity == UndefinedException);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImages applies next processing options to a sequence of
%  images as prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImages(const ImageInfo *image_info,const int argc,
%        char **argv,Image **images)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o images: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImages(const ImageInfo *image_info,
  const int argc,char **argv,Image **images)
{
#define MogrifyImageText  "  Transform image...  "

  char
    *option;

  Image
    *clone_images,
    *image,
    *mogrify_images;

  ImageInfo
    *clone_info;

  MonitorHandler
    handler;

  register long
    i;

  unsigned int
    status;

  unsigned long
    number_images,
    scene;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(images != (Image **) NULL);
  assert((*images)->signature == MagickSignature);
  if ((argc <= 0) || (*argv == (char *) NULL))
    return(True);
  clone_images=(Image *) NULL;
  scene=False;
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    switch (*(option+1))
    {
      case 's':
      {
        if (LocaleCompare("scene",option+1) == 0)
          scene=True;
        break;
      }
      case 'w':
      {
        if (LocaleCompare("+write",option) == 0)
          clone_images=CloneImageList(*images,&(*images)->exception);
        break;
      }
      default:
        break;
    }
  }
  /*
    Apply options to individual each image in the list.
  */
  status=True;
  mogrify_images=NewImageList();
  number_images=GetImageListSize(*images);
  for (i=0; i < (long) number_images; i++)
  {
    image=ShiftImageList(images);
    handler=SetMonitorHandler((MonitorHandler) NULL);
    status&=MogrifyImage(image_info,argc,argv,&image);
    (void) SetMonitorHandler(handler);
    if (scene)
      image->scene+=i;
    if (image_info->verbose)
      DescribeImage(image,stdout,False);
    PushImageList(&mogrify_images,image,&image->exception);
    DestroyImage(image);
    if (!MagickMonitor(MogrifyImageText,i,number_images,&image->exception))
      break;
  }
  /*
    Apply options to the entire image list.
  */
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("append",option+1) == 0)
          {
            Image
              *append_image;

            append_image=AppendImages(mogrify_images,*option == '-',
              &mogrify_images->exception);
            if (append_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=append_image;
              }
            break;
          }
        if (LocaleCompare("average",option+1) == 0)
          {
            Image
              *average_image;

            average_image=AverageImages(mogrify_images,
              &mogrify_images->exception);
            if (average_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=average_image;
              }
            break;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("coalesce",option+1) == 0)
          {
            Image
              *coalesce_image;

            coalesce_image=CoalesceImages(mogrify_images,
              &mogrify_images->exception);
            if (coalesce_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=coalesce_image;
              }
            break;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("deconstruct",option+1) == 0)
          {
            Image
              *deconstruct_image;

            deconstruct_image=DeconstructImages(mogrify_images,
              &mogrify_images->exception);
            if (deconstruct_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=deconstruct_image;
              }
            break;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("flatten",option+1) == 0)
          {
            Image
              *flatten_image;

            flatten_image=FlattenImages(mogrify_images,
              &mogrify_images->exception);
            if (flatten_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=flatten_image;
              }
            break;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
             if (*option == '+')
               {
                 (void) MapImages(mogrify_images,(Image *) NULL,
                   image_info->dither);
                 break;
               }
             i++;
             break;
          }
        if (LocaleCompare("morph",option+1) == 0)
          {
            Image
              *morph_image;

            morph_image=MorphImages(mogrify_images,atol(argv[++i]),
              &mogrify_images->exception);
            if (morph_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=morph_image;
              }
            break;
          }
        if (LocaleCompare("mosaic",option+1) == 0)
          {
            Image
              *mosaic_image;

            mosaic_image=MosaicImages(mogrify_images,
              &mogrify_images->exception);
            if (mosaic_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=mosaic_image;
              }
            break;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("process",option+1) == 0)
          {
            char
              *arguments,
              breaker,
              quote,
              *token;

            int
              next,
              status;

            size_t
              length;

            TokenInfo
              token_info;

            length=strlen(argv[++i]);
            token=(char *) AcquireMemory(length+1);
            if (token == (char *) NULL)
              continue;
            next=0;
            arguments=argv[i];
            status=Tokenizer(&token_info,0,token,length,arguments,"","=","\"",0,
              &breaker,&next,&quote);
            if (status == 0)
              {
                char
                  *argv;

                argv=&(arguments[next]);
                (void) ExecuteModuleProcess(token,&mogrify_images,1,&argv);
              }
            LiberateMemory((void **) &token);
            continue;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("write",option+1) == 0)
          {
            clone_info=CloneImageInfo(image_info);
            status&=WriteImages(clone_info,mogrify_images,argv[++i],
              &mogrify_images->exception);
            DestroyImageInfo(clone_info);
            if (*option == '+')
              {
                DestroyImageList(mogrify_images);
                mogrify_images=clone_images;
              }
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  *images=mogrify_images;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     P l a s m a I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PlasmaImage() initializes an image with plasma fractal values.  The image
%  must be initialized with a base color and the random number generator
%  seeded before this method is called.
%
%  The format of the PlasmaImage method is:
%
%      unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
%        unsigned long attenuate,unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o segment:   Define the region to apply plasma fractals values.
%
%    o attenuate: Define the plasma attenuation factor.
%
%    o depth: Limit the plasma recursion depth.
%
%
*/

static inline Quantum PlasmaPixel(const double pixel,const double noise)
{
  double
    value;

  value=pixel+noise*rand()/RAND_MAX-noise/2;
  if (value <= 0.0)
    return(0);
  if (value >= MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

MagickExport unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
  unsigned long attenuate,unsigned long depth)
{
  double
    plasma;

  long
    x,
    x_mid,
    y,
    y_mid;

  PixelPacket
    u,
    v;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(segment != (SegmentInfo *) NULL);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=(long) (segment->x1+segment->x2+0.5)/2;
      y_mid=(long) (segment->y1+segment->y2+0.5)/2;
      local_info=(*segment);
      local_info.x2=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.y1=y_mid;
      local_info.x2=x_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y1=y_mid;
      return(PlasmaImage(image,&local_info,attenuate,depth));
    }
  SetImageType(image,TrueColorType);
  x_mid=(long) (segment->x1+segment->x2+0.5)/2;
  y_mid=(long) (segment->y1+segment->y2+0.5)/2;
  if ((segment->x1 == x_mid) && (segment->x2 == x_mid) &&
      (segment->y1 == y_mid) && (segment->y2 == y_mid))
    return(False);
  /*
    Average pixels and apply plasma.
  */
  plasma=MaxRGB/(2.0*attenuate);
  if ((segment->x1 != x_mid) || (segment->x2 != x_mid))
    {
      /*
        Left pixel.
      */
      x=(long) (segment->x1+0.5);
      u=GetOnePixel(image,x,(long) (segment->y1+0.5));
      v=GetOnePixel(image,x,(long) (segment->y2+0.5));
      q=SetImagePixels(image,x,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
      if (segment->x1 != segment->x2)
        {
          /*
            Right pixel.
          */
          x=(long) (segment->x2+0.5);
          u=GetOnePixel(image,x,(long) (segment->y1+0.5));
          v=GetOnePixel(image,x,(long) (segment->y2+0.5));
          q=SetImagePixels(image,x,y_mid,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->y1 != y_mid) || (segment->y2 != y_mid))
    {
      if ((segment->x1 != x_mid) || (segment->y2 != y_mid))
        {
          /*
            Bottom pixel.
          */
          y=(long) (segment->y2+0.5);
          u=GetOnePixel(image,(long) (segment->x1+0.5),y);
          v=GetOnePixel(image,(long) (segment->x2+0.5),y);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
      if (segment->y1 != segment->y2)
        {
          /*
            Top pixel.
          */
          y=(long) (segment->y1+0.5);
          u=GetOnePixel(image,(long) (segment->x1+0.5),y);
          v=GetOnePixel(image,(long) (segment->x2+0.5),y);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->x1 != segment->x2) || (segment->y1 != segment->y2))
    {
      /*
        Middle pixel.
      */
      x=(long) (segment->x1+0.5);
      y=(long) (segment->y1+0.5);
      u=GetOnePixel(image,x,y);
      x=(long) (segment->x2+0.5);
      y=(long) (segment->y2+0.5);
      v=GetOnePixel(image,x,y);
      q=SetImagePixels(image,x_mid,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
    }
  if (((segment->x2-segment->x1) < 3.0) && ((segment->y2-segment->y1) < 3.0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceImage() increments the reference count associated with an image
%  returning a pointer to the image.
%
%  The format of the ReferenceImage method is:
%
%      Image *ReferenceImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport Image *ReferenceImage(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  AcquireSemaphoreInfo(&image->semaphore);
  image->reference_count++;
  LiberateSemaphoreInfo(&image->semaphore);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalized the range of the transformed
%  values to be [0..MaxRGB].
%
%  The format of the RGBTransformImage method is:
%
%      unsigned int RGBTransformImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates which colorspace
%      to transform the image.
%
%
*/
MagickExport unsigned int RGBTransformImage(Image *image,
  const ColorspaceType colorspace)
{
#define RGBTransformImageText  "  Transform image colors...  "
#define X  0
#define Y  65536L
#define Z  (2*65536L)

  double
    *x_map,
    *y_map,
    *z_map;

  DoublePixelPacket
    pixel;

  long
    y;

  PrimaryInfo
    primary_info;

  register long
    i,
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == colorspace)
    return(True);
  if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace))
    return(True);
  if (colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      Quantum
        black,
        cyan,
        magenta,
        yellow;

      /*
        Convert RGB to CMYK colorspace.
      */
      image->storage_class=DirectClass;
      image->colorspace=CMYKColorspace;
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          cyan=(Quantum) (MaxRGB-q->red);
          magenta=(Quantum) (MaxRGB-q->green);
          yellow=(Quantum) (MaxRGB-q->blue);
          black=(Quantum)
            (cyan < magenta ? Min(cyan,yellow) : Min(magenta,yellow));
          q->red=cyan;
          q->green=magenta;
          q->blue=yellow;
          indexes[x]=q->opacity;
          q->opacity=(Quantum) black;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      return(True);
    }
  x=0;
  if ((colorspace == GRAYColorspace) && IsGrayImage(image,&image->exception))
    return(True);
  /*
    Allocate the tables.
  */
  x_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  y_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  z_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  if ((x_map == (double *) NULL) || (y_map == (double *) NULL) ||
      (z_map == (double *) NULL))
    ThrowBinaryException(ResourceLimitError,
      "Unable to transform color space","Memory allocation failed");
  memset(&primary_info,0,sizeof(primary_info));
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=0.299*i;
        y_map[i+Y]=0.587*i;
        z_map[i+Y]=0.114*i;
        x_map[i+Z]=0.299*i;
        y_map[i+Z]=0.587*i;
        z_map[i+Z]=0.114*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=65536L/2;
      primary_info.z=65536L/2;
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.33333*i;
        y_map[i+X]=0.33334*i;
        z_map[i+X]=0.33333*i;
        x_map[i+Y]=0.5*i;
        y_map[i+Y]=0.0;
        z_map[i+Y]=(-0.5)*i;
        x_map[i+Z]=(-0.25)*i;
        y_map[i+Z]=0.5*i;
        z_map[i+Z]=(-0.25)*i;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      primary_info.y=ScaleQuantumToShort(ScaleCharToQuantum(156));
      primary_info.z=ScaleQuantumToShort(ScaleCharToQuantum(137));
      for (i=0; i <= (long) (0.018*65535L); i++)
      {
        x_map[i+X]=0.003962014134275617*i;
        y_map[i+X]=0.007778268551236748*i;
        z_map[i+X]=0.001510600706713781*i;
        x_map[i+Y]=(-0.002426619775463276)*i;
        y_map[i+Y]=(-0.004763965913702149)*i;
        z_map[i+Y]=0.007190585689165425*i;
        x_map[i+Z]=0.006927257754597858*i;
        y_map[i+Z]=(-0.005800713697502058)*i;
        z_map[i+Z]=(-0.0011265440570958)*i;
      }
      for ( ; i <= 65535L; i++)
      {
        x_map[i+X]=0.2201118963486454*(1.099*i-0.099);
        y_map[i+X]=0.4321260306242638*(1.099*i-0.099);
        z_map[i+X]=0.08392226148409894*(1.099*i-0.099);
        x_map[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y_map[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z_map[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x_map[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y_map[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z_map[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          X = 0.412453*X+0.357580*Y+0.180423*Z
          Y = 0.212671*X+0.715160*Y+0.072169*Z
          Z = 0.019334*X+0.119193*Y+0.950227*Z
      */
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.412453*i;
        y_map[i+X]=0.35758*i;
        z_map[i+X]=0.180423*i;
        x_map[i+Y]=0.212671*i;
        y_map[i+Y]=0.71516*i;
        z_map[i+Y]=0.072169*i;
        x_map[i+Z]=0.019334*i;
        y_map[i+Z]=0.119193*i;
        z_map[i+Z]=0.950227*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Cb= -0.168736*R-0.331264*G+0.500000*B
          Cr=  0.500000*R-0.418688*G-0.081316*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=65536L/2;
      primary_info.z=65536L/2;
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.16873)*i;
        y_map[i+Y]=(-0.331264)*i;
        z_map[i+Y]=0.500000*i;
        x_map[i+Z]=0.500000*i;
        y_map[i+Z]=(-0.418688)*i;
        z_map[i+Z]=(-0.081316)*i;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      primary_info.y=ScaleQuantumToShort(ScaleCharToQuantum(156));
      primary_info.z=ScaleQuantumToShort(ScaleCharToQuantum(137));
      for (i=0; i <= (long) (0.018*65535L); i++)
      {
        x_map[i+X]=0.003962014134275617*i;
        y_map[i+X]=0.007778268551236748*i;
        z_map[i+X]=0.001510600706713781*i;
        x_map[i+Y]=(-0.002426619775463276)*i;
        y_map[i+Y]=(-0.004763965913702149)*i;
        z_map[i+Y]=0.007190585689165425*i;
        x_map[i+Z]=0.006927257754597858*i;
        y_map[i+Z]=(-0.005800713697502058)*i;
        z_map[i+Z]=(-0.0011265440570958)*i;
      }
      for ( ; i <= 65535L; i++)
      {
        x_map[i+X]=0.2201118963486454*(1.099*i-0.099);
        y_map[i+X]=0.4321260306242638*(1.099*i-0.099);
        z_map[i+X]=0.08392226148409894*(1.099*i-0.099);
        x_map[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y_map[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z_map[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x_map[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y_map[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z_map[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=65536L/2;
      primary_info.z=65536L/2;
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=0.596*i;
        y_map[i+Y]=(-0.274)*i;
        z_map[i+Y]=(-0.322)*i;
        x_map[i+Z]=0.211*i;
        y_map[i+Z]=(-0.523)*i;
        z_map[i+Z]=0.312*i;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Pb= -0.168736*R-0.331264*G+0.500000*B
          Pr=  0.500000*R-0.418688*G-0.081312*B

        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      primary_info.y=65536L/2;
      primary_info.z=65536L/2;
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.168736)*i;
        y_map[i+Y]=(-0.331264)*i;
        z_map[i+Y]=0.5*i;
        x_map[i+Z]=0.5*i;
        y_map[i+Z]=(-0.418688)*i;
        z_map[i+Z]=(-0.081312)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      primary_info.y=65536L/2;
      primary_info.z=65536L/2;
      for (i=0; i <= 65535L; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.1474)*i;
        y_map[i+Y]=(-0.2895)*i;
        z_map[i+Y]=0.4369*i;
        x_map[i+Z]=0.615*i;
        y_map[i+Z]=(-0.515)*i;
        z_map[i+Z]=(-0.1)*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          pixel.red=x_map[ScaleQuantumToShort(q->red)+X]+
            y_map[ScaleQuantumToShort(q->green)+X]+
            z_map[ScaleQuantumToShort(q->blue)+X]+primary_info.x;
          pixel.red=(unsigned long) ((pixel.red < 0) ? 0 :
            (pixel.red > 65535L) ? 65535L : pixel.red+0.5);
          pixel.green=x_map[ScaleQuantumToShort(q->red)+Y]+
            y_map[ScaleQuantumToShort(q->green)+Y]+
            z_map[ScaleQuantumToShort(q->blue)+Y]+primary_info.y;
          pixel.green=(unsigned long) ((pixel.green < 0) ? 0 :
            (pixel.green > 65535L) ? 65535L : pixel.green+0.5);
          pixel.blue=x_map[ScaleQuantumToShort(q->red)+Z]+
            y_map[ScaleQuantumToShort(q->green)+Z]+
            z_map[ScaleQuantumToShort(q->blue)+Z]+primary_info.z;
          pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0 :
            (pixel.blue > 65535L) ? 65535L : pixel.blue+0.5);
          q->red=ScaleShortToQuantum(pixel.red);
          q->green=ScaleShortToQuantum(pixel.green);
          q->blue=ScaleShortToQuantum(pixel.blue);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(RGBTransformImageText,y,image->rows,exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        pixel.red=x_map[ScaleQuantumToShort(image->colormap[i].red)+X]+
          y_map[ScaleQuantumToShort(image->colormap[i].green)+X]+
          z_map[ScaleQuantumToShort(image->colormap[i].blue)+X]+primary_info.x;
        pixel.red=(unsigned long) ((pixel.red < 0) ? 0.0 :
          (pixel.red > 65535L) ? 65535L : pixel.red+0.5);
        pixel.green=x_map[ScaleQuantumToShort(image->colormap[i].red)+Y]+
          y_map[ScaleQuantumToShort(image->colormap[i].green)+Y]+
          z_map[ScaleQuantumToShort(image->colormap[i].blue)+Y]+primary_info.y;
        pixel.green=(unsigned long) ((pixel.green < 0) ? 0.0 :
          (pixel.green > 65535L) ? 65535L : pixel.green+0.5);
        pixel.blue=x_map[ScaleQuantumToShort(image->colormap[i].red)+Z]+
          y_map[ScaleQuantumToShort(image->colormap[i].green)+Z]+
          z_map[ScaleQuantumToShort(image->colormap[i].blue)+Z]+primary_info.z;
        pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0.0 :
          (pixel.blue > 65535L) ? 65535L : pixel.blue+0.5);
        image->colormap[i].red=ScaleShortToQuantum(pixel.red);
        image->colormap[i].green=ScaleShortToQuantum(pixel.green);
        image->colormap[i].blue=ScaleShortToQuantum(pixel.blue);
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocate memory.
  */
  LiberateMemory((void **) &z_map);
  LiberateMemory((void **) &y_map);
  LiberateMemory((void **) &x_map);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   S e t I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImage() sets the red, green, and blue components of each pixel to
%  the image background color and the opacity component to the specified
%  level of transparency.  The background color is defined by the
%  background_color member of the image.
%
%  The format of the SetImage method is:
%
%      void SetImage(Image *image,const Quantum opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: Set each pixel to this level of transparency.
%
%
*/
MagickExport void SetImage(Image *image,const Quantum opacity)
{
  long
    y;

  PixelPacket
    background_color;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  background_color=image->background_color;
  if (opacity != OpaqueOpacity)
    background_color.opacity=opacity;
  if (background_color.opacity != OpaqueOpacity)
    SetImageType(image,TrueColorMatteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if (indexes == (IndexPacket *) NULL)
      for (x=0; x < (long) image->columns; x++)
      {
        *q=background_color;
        q++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        indexes[x]=0;
        *q=background_color;
        q++;
      }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C l i p M a s k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageClipMask() associates a clip mask with the image.  The clip mask
%  must be the same dimensions as the image.  Set any pixel component of
%  the clip mask to TransparentOpacity to prevent that corresponding image
%  pixel component from being updated when SyncImagePixels() is applied.
%
%  The format of the SetImageClipMask method is:
%
%      unsigned int SetImageClipMask(Image *image,Image *clip_mask)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o clip_mask: The image clip mask.
%
%
*/
MagickExport unsigned int SetImageClipMask(Image *image,Image *clip_mask)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (clip_mask != (const Image *) NULL)
    if ((clip_mask->columns != image->columns) ||
        (clip_mask->rows != image->rows))
      ThrowBinaryException(OptionError,"Unable to set clip mask",
        "image widths or heights differ");
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  if (clip_mask == (Image *) NULL)
    {
      image->clip_mask=(Image *) NULL;
      return(True);
    }
  image->clip_mask=CloneImage(clip_mask,0,0,True,&clip_mask->exception);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageDepth() sets the depth of the image, either 8 or 16.  Some image
%  formats support both 8 and 16-bits per color component (e.g. PNG).  Use
%  SetImageDepth() to specify your preference.  A value other than 0 is
%  returned if the depth is set.  Check the exception member of image to
%  determine the cause for any failure.
%
%  The format of the SetImageDepth method is:
%
%      unsigned int SetImageDepth(Image *image,const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o depth: The image depth.
%
%
*/
MagickExport unsigned int SetImageDepth(Image *image,const unsigned long depth)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->depth=GetImageDepth(image,&image->exception);
  if (image->depth == depth)
    return(True);
  if (depth <= 8)
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=ScaleCharToQuantum(ScaleQuantumToChar(q->red));
          q->green=ScaleCharToQuantum(ScaleQuantumToChar(q->green));
          q->blue=ScaleCharToQuantum(ScaleQuantumToChar(q->blue));
          q->opacity=ScaleCharToQuantum(ScaleQuantumToChar(q->opacity));
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      if (image->storage_class == PseudoClass)
        {
          register long
            i;

          q=image->colormap;
          for (i=0; i < (long) image->colors; i++)
          {
            q->red=ScaleCharToQuantum(ScaleQuantumToChar(q->red));
            q->green=ScaleCharToQuantum(ScaleQuantumToChar(q->green));
            q->blue=ScaleCharToQuantum(ScaleQuantumToChar(q->blue));
            q->opacity=ScaleCharToQuantum(ScaleQuantumToChar(q->opacity));
            q++;
          }
        }
      image->depth=8;
      return(True);
    }
  if (depth <= 16)
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=ScaleShortToQuantum(ScaleQuantumToShort(q->red));
          q->green=ScaleShortToQuantum(ScaleQuantumToShort(q->green));
          q->blue=ScaleShortToQuantum(ScaleQuantumToShort(q->blue));
          q->opacity=ScaleShortToQuantum(ScaleQuantumToShort(q->opacity));
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      if (image->storage_class == PseudoClass)
        {
          register long
            i;

          q=image->colormap;
          for (i=0; i < (long) image->colors; i++)
          {
            q->red=ScaleShortToQuantum(ScaleQuantumToShort(q->red));
            q->green=ScaleShortToQuantum(ScaleQuantumToShort(q->green));
            q->blue=ScaleShortToQuantum(ScaleQuantumToShort(q->blue));
            q->opacity=ScaleShortToQuantum(ScaleQuantumToShort(q->opacity));
            q++;
          }
        }
      image->depth=16;
      return(True);
    }
  image->depth=32;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageInfo() initializes the `magick' field of the ImageInfo
%  structure.  It is set to a type of image format based on the prefix or
%  suffix of the filename.  For example, `ps:image' returns PS indicating
%  a Postscript image.  JPEG is returned for this filename: `image.jpg'.
%  The filename prefix has precendence over the suffix.  Use an optional
%  index enclosed in brackets after a file name to specify a desired subimage
%  of a multi-resolution image format like Photo CD (e.g. img0001.pcd[4]).
%  A True (non-zero) return value indicates success.
%
%  The format of the SetImageInfo method is:
%
%      unsigned int SetImageInfo(ImageInfo *image_info,
%        const unsigned int rectify,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static inline unsigned int IsFrame(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

MagickExport unsigned int SetImageInfo(ImageInfo *image_info,
  const unsigned int rectify,ExceptionInfo *exception)
{
  char
    magic[MaxTextExtent],
    *q;

  const char
    *r;

  const MagicInfo
    *magic_info;

  Image
    *image;

  register char
    *p;

  unsigned char
    magick[2*MaxTextExtent];

  unsigned int
    status;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  *magic='\0';
  p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
  if (*p == ']')
    for (q=p-1; q > image_info->filename; q--)
    {
      unsigned long
        first,
        last;

      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      if (*q != '[')
        continue;
      if (!IsFrame(q+1))
        break;
      (void) CloneString(&image_info->tile,q+1);
      image_info->tile[p-q-1]='\0';
      *q='\0';
      image_info->subimage=atol(image_info->tile);
      image_info->subrange=image_info->subimage;
      p=image_info->tile;
      for (q=p; *q != '\0'; p++)
      {
        while (isspace((int) *p) || (*p == ','))
          p++;
        first=strtol(p,&q,10);
        last=first;
        while (isspace((int) *q))
          q++;
        if (*q == '-')
          last=strtol(q+1,&q,10);
        if (first > last)
          Swap(first,last);
        if (first < image_info->subimage)
          image_info->subimage=first;
        if (last > image_info->subrange)
          image_info->subrange=last;
        p=q;
      }
      image_info->subrange-=image_info->subimage-1;
      break;
    }
  while ((*p != '.') && (p > (image_info->filename+1)))
    p--;
  if ((LocaleCompare(p,".gz") == 0) || (LocaleCompare(p,".Z") == 0) ||
      (LocaleCompare(p,".bz2") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > (image_info->filename+1)));
  if ((*p == '.') && (strlen(p) < (long) sizeof(magic)))
    {
      /*
        User specified image format.
      */
      (void) strncpy(magic,p+1,MaxTextExtent-1);
      for (q=magic; *q != '\0'; q++)
        if (*q == '.')
          {
            *q='\0';
            break;
          }
      LocaleUpper(magic);
      /*
        SGI and RGB are ambiguous;  TMP must be set explicitly.
      */
      if (((LocaleNCompare(image_info->magick,"SGI",3) != 0) ||
          (LocaleCompare(magic,"RGB") != 0)) &&
          (LocaleCompare(magic,"TMP") != 0))
        (void) strncpy(image_info->magick,magic,MaxTextExtent-1);
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  image_info->affirm=False;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
#if defined(vms)
  if (*(p+1) == ':')
    p+=2;  /* skip DECnet node spec */
#endif
  if ((*p == ':') && ((p-image_info->filename) < (long) sizeof(magic)))
    {
      char
        format[MaxTextExtent];

      /*
        User specified image format.
      */
      (void) strncpy(format,image_info->filename,p-image_info->filename);
      format[p-image_info->filename]='\0';
      if (LocaleCompare(format,"GRADATION") == 0)
        (void) strcpy(format,"GRADIENT");
      LocaleUpper(format);
      if (!IsMagickConflict(format))
        {
          /*
            Strip off image format prefix.
          */
          p++;
          (void) strncpy(image_info->filename,p,MaxTextExtent-1);
          (void) strncpy(magic,format,MaxTextExtent-1);
          if (LocaleCompare(magic,"IMPLICIT") != 0)
            {
              (void) strncpy(image_info->magick,magic,MaxTextExtent-1);
              if (LocaleCompare(magic,"TMP") != 0)
                image_info->affirm=True;
              else
                image_info->temporary=True;
            }
        }
    }
  if (rectify)
    {
      char
        filename[MaxTextExtent];

      const MagickInfo
        *magick_info;

      /*
        Rectify multi-image file support.
      */
      FormatString(filename,image_info->filename,0);
      if ((LocaleCompare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=False;
      magick_info=GetMagickInfo(magic,exception);
      if (magick_info != (const MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return(True);
    }
  if (image_info->affirm)
    return(True);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return(False);
  /*
    Determine the image format from the first few bytes of the file.
  */
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      CloseBlob(image);
      DestroyImage(image);
      return(False);
    }
  *magick='\0';
  if ((image->blob->data != (unsigned char *) NULL) || !image->blob->exempt)
    (void) ReadBlob(image,2*MaxTextExtent,magick);
  else
    {
      FILE
        *file;

      int
        c;

      register long
        i;

      /*
        Copy standard input or pipe to temporary file.
      */
      image_info->file=(FILE *) NULL;
      TemporaryFilename(image->filename);
      image_info->temporary=True;
      (void) strncpy(image_info->filename,image->filename,MaxTextExtent-1);
      file=fopen(image->filename,"wb");
      if (file == (FILE *) NULL)
        ThrowBinaryException(MissingDelegateError,"Unable to write file",
          image->filename);
      i=0;
      for (c=fgetc(image->blob->file); c != EOF; c=fgetc(image->blob->file))
      {
        if (i < MaxTextExtent)
          magick[i++]=(unsigned char) c;
        (void) fputc(c,file);
      }
      (void) fclose(file);
    }
  CloseBlob(image);
  DestroyImage(image);
  /*
    Check magic.mgk configuration file.
  */
  magic_info=GetMagicInfo(magick,2*MaxTextExtent,exception);
  if ((magic_info != (const MagicInfo *) NULL) &&
      (magic_info->name != (char *) NULL))
    {
      (void) strncpy(image_info->magick,magic_info->name,MaxTextExtent-1);
      return(True);
    }
  /*
    Check module IsImage() methods.
  */
  r=GetImageMagick(magick,2*MaxTextExtent);
  if (r != (const char *) NULL)
    {
      (void) strncpy(image_info->magick,r,MaxTextExtent-1);
      return(True);
    }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S e t I m a g e O p a c i t y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageOpacity() attenuates the opacity channel of an image.  If the
%  image pixels are opaque, they are set to the specified opacity level.
%  Otherwise, the pixel oapcity values are blended with the supplied
%  transparency value.
%
%  The format of the SetImageOpacity method is:
%
%      void SetImageOpacity(Image *image,const unsigned int opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: The level of transparency: 0 is fully opaque and MaxRGB is
%      fully transparent.
%
%
*/
MagickExport void SetImageOpacity(Image *image,const unsigned int opacity)
{
  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->matte)
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          if (image->colorspace == CMYKColorspace)
            {
              indexes[x]=(IndexPacket)
                ((unsigned long) (opacity*indexes[x])/MaxRGB);
              continue;
            }
          q->opacity=(Quantum)
            ((unsigned long) (opacity*q->opacity)/MaxRGB);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      return;
    }
  image->matte=True;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (image->colorspace == CMYKColorspace)
        {
          indexes[x]=opacity;
          continue;
        }
      q->opacity=opacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageType() sets the type of image.  Choose from these types:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%        OptimizeType
%
%  The format of the SetImageType method is:
%
%      SetImageType(Image *image,const ImageType image_type)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o image_type: Image type.
%
%
*/
MagickExport void SetImageType(Image *image,const ImageType image_type)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image_type)
  {
    case BilevelType:
    {
      if ((image->colorspace == RGBColorspace) &&
          (image->storage_class == PseudoClass) &&
          IsMonochromeImage(image,&image->exception))
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      GetQuantizeInfo(&quantize_info);
      quantize_info.colorspace=GRAYColorspace;
      quantize_info.tree_depth=8;
      quantize_info.number_colors=2;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case GrayscaleType:
    {
      if ((image->colorspace == RGBColorspace) &&
          IsGrayImage(image,&image->exception))
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      (void) RGBTransformImage(image,GRAYColorspace);
      break;
    }
    case GrayscaleMatteType:
    {
      if ((image->colorspace == RGBColorspace) &&
          IsGrayImage(image,&image->exception) && image->matte)
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      (void) RGBTransformImage(image,GRAYColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      break;
    }
    case PaletteType:
    {
      if ((image->colorspace == RGBColorspace) &&
          (image->storage_class == PseudoClass))
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      GetQuantizeInfo(&quantize_info);
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case PaletteMatteType:
    {
      if ((image->colorspace == RGBColorspace) &&
          (image->storage_class == PseudoClass) && image->matte)
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      GetQuantizeInfo(&quantize_info);
      quantize_info.colorspace=TransparentColorspace;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case TrueColorType:
    {
      if ((image->colorspace == RGBColorspace) &&
          (image->storage_class == DirectClass))
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      image->storage_class=DirectClass;
      break;
    }
    case TrueColorMatteType:
    {
      if ((image->colorspace == RGBColorspace) &&
          (image->storage_class == DirectClass) && image->matte)
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      image->storage_class=DirectClass;
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      break;
    }
    case ColorSeparationType:
    {
      if (image->colorspace == CMYKColorspace)
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      (void) RGBTransformImage(image,CMYKColorspace);
      break;
    }
    case ColorSeparationMatteType:
    {
      if ((image->colorspace == CMYKColorspace) && image->matte)
        break;
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      if (image->colorspace != CMYKColorspace)
        (void) RGBTransformImage(image,CMYKColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      break;
    }
    case OptimizeType:
    default:
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SortColormapByIntensity() sorts the colormap of a PseudoClass image by
%  decreasing color intensity.
%
%  The format of the SortColormapByIntensity method is:
%
%      unsigned int SortColormapByIntensity(Image *image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to a Image structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int IntensityCompare(const void *x,const void *y)
{
  long
    intensity;

  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  intensity=PixelIntensityToQuantum(color_2)-
    (long) PixelIntensityToQuantum(color_1);
  return(intensity);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport unsigned int SortColormapByIntensity(Image *image)
{
  IndexPacket
    index;

  long
    y;

  register long
    x;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register long
    i;

  unsigned short
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(True);
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned short *) AcquireMemory(image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    ThrowBinaryException(MissingDelegateError,"Unable to sort colormap",
      "Memory allocation failed");
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (long) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing color popularity.
  */
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
    IntensityCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < (long) image->colors; i++)
    pixels[image->colormap[i].opacity]=(unsigned short) i;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=pixels[indexes[x]];
      indexes[x]=index;
      *q++=image->colormap[index];
    }
  }
  LiberateMemory((void **) &pixels);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage method is:
%
%      void SyncImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void SyncImage(Image *image)
{
  IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    return;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (indexes[x] >= image->colors)
        {
          ThrowException(&image->exception,CorruptImageError,
            "invalid colormap index",image->filename);
          indexes[x]=0;
        }
      index=indexes[x];
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T e x t u r e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TextureImage() repeatedly tiles the texture image across and down the image
%  canvas.
%
%  The format of the TextureImage method is:
%
%      void TextureImage(Image *image,const Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o texture: This image is the texture to layer on the background.
%
%
*/
MagickExport void TextureImage(Image *image,const Image *texture)
{
#define TextureImageText  "  Apply image texture...  "

  long
    x,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (texture == (const Image *) NULL)
    return;
  /*
    Tile texture onto the image background.
  */
  for (y=0; y < (long) image->rows; y+=texture->rows)
  {
    for (x=0; x < (long) image->columns; x+=texture->columns)
      (void) CompositeImage(image,CopyCompositeOp,texture,x,y);
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(TextureImageText,y,image->rows,&image->exception))
        break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformRGBImage converts the reference image from an alternate
%  colorspace.  The transformation matrices are not the standard ones:  the
%  weights are rescaled to normalize the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the TransformRGBImage method is:
%
%      unsigned int TransformRGBImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value defines which colorspace to
%      transform the image to.
%
%
*/
MagickExport unsigned int TransformRGBImage(Image *image,
  const ColorspaceType colorspace)
{
#define B  (2*65536L)
#define G  65536L
#define R  0
#define TransformRGBImageText  "  Transform image colors...  "

  static const unsigned char
    sRGBMap[351] =
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66,
      67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
      83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99,
      100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
      127, 128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140,
      141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
      154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
      167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178,
      179, 180, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190,
      191, 192, 193, 194, 194, 195, 196, 197, 198, 199, 199, 200, 201,
      202, 203, 203, 204, 205, 206, 207, 207, 208, 209, 210, 210, 211,
      212, 213, 213, 214, 215, 215, 216, 217, 218, 218, 219, 220, 220,
      221, 222, 222, 223, 223, 224, 225, 225, 226, 227, 227, 228, 228,
      229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235, 235,
      235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 240,
      241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244, 245,
      245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250,
      250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255,
      255, 255, 255, 255, 255
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 50, 51, 52,
      53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70,
      71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88,
      89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118,
      119, 120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132,
      133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
      146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
      159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
      172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195,
      196, 197, 198, 199, 200, 201, 201, 202, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 221, 221, 222, 223, 224, 224, 225, 226, 226,
      227, 228, 228, 229, 230, 230, 231, 232, 232, 233, 234, 234, 235,
      236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242,
      243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247, 248,
      248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255
    };

  double
    *blue_map,
    *green_map,
    *red_map;

  DoublePixelPacket
    pixel;

  long
    y;

  register long
    x;

  register long
    i;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == colorspace)
    return(True);
  if ((image->colorspace == CMYKColorspace) && (colorspace == RGBColorspace))
    {
      IndexPacket
        *indexes;

      /*
        Transform image from CMYK to RGB.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=(Quantum) (((MaxRGB-q->red)*(MaxRGB-q->opacity))/MaxRGB);
          q->green=(Quantum) (((MaxRGB-q->green)*(MaxRGB-q->opacity))/MaxRGB);
          q->blue=(Quantum) (((MaxRGB-q->blue)*(MaxRGB-q->opacity))/MaxRGB);
          q->opacity=image->matte ? indexes[x] : OpaqueOpacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      image->colorspace=RGBColorspace;
      return(True);
    }
  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace) ||
      (colorspace == TransparentColorspace))
    return(True);
  /*
    Allocate the tables.
  */
  red_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  green_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  blue_map=(double *) AcquireMemory(3*65536L*sizeof(double));
  if ((red_map == (double *) NULL) || (green_map == (double *) NULL) ||
      (blue_map == (double *) NULL))
    ThrowBinaryException(MissingDelegateError,
      "Unable to transform colorspace","Memory allocation failed");
  switch (colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.5*(2.0*i-65535L);
        blue_map[i+R]=(-0.33334)*(2.0*i-65535L);
        red_map[i+G]=i;
        green_map[i+G]=0.0;
        blue_map[i+G]=0.666665*(2.0*i-65535L);
        red_map[i+B]=i;
        green_map[i+B]=(-0.5)*(2.0*i-65535L);
        blue_map[i+B]=(-0.33334)*(2.0*i-65535L);
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          R = Y            +1.032096*C2
          G = Y-0.326904*C1-0.704445*C2
          B = Y+1.685070*C1

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=1.40200*i;
        green_map[i+R]=0.0;
        blue_map[i+R]=1.88000*(i-(long)
          ScaleQuantumToShort(ScaleCharToQuantum(137)));
        red_map[i+G]=1.40200*i;
        green_map[i+G]=(-0.444066)*(i-(long)
          ScaleQuantumToShort(ScaleCharToQuantum(156)));
        blue_map[i+G]=(-0.95692)*(i-(long)
          ScaleQuantumToShort(ScaleCharToQuantum(137)));
        red_map[i+B]=1.40200*i;
        green_map[i+B]=2.28900*(i-(long)
          ScaleQuantumToShort(ScaleCharToQuantum(156)));
        blue_map[i+B]=0.0;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          R =  3.240479*R-1.537150*G-0.498535*B
          G = -0.969256*R+1.875992*G+0.041556*B
          B =  0.055648*R-0.204043*G+1.057311*B
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=3.240479*i;
        green_map[i+R]=(-1.537150)*i;
        blue_map[i+R]=(-0.498535)*i;
        red_map[i+G]=(-0.969256)*i;
        green_map[i+G]=1.875992*i;
        blue_map[i+G]=0.041556*i;
        red_map[i+B]=0.055648*i;
        green_map[i+B]=(-0.204043)*i;
        blue_map[i+B]=1.057311*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y            +1.402000*Cr
          G = Y-0.344136*Cb-0.714136*Cr
          B = Y+1.772000*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=(1.402000*0.5)*(2.0*i-65535L);
        red_map[i+G]=i;
        green_map[i+G]=(-0.344136*0.5)*(2.0*i-65535L);
        blue_map[i+G]=(-0.714136*0.5)*(2.0*i-65535L);
        red_map[i+B]=i;
        green_map[i+B]=(1.772000*0.5)*(2.0*i-65535L);
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=1.3584*i;
        green_map[i+R]=0.0;
        blue_map[i+R]=1.8215*(i-(double)
          ScaleQuantumToShort(ScaleCharToQuantum(137)));
        red_map[i+G]=1.3584*i;
        green_map[i+G]=(-0.4302726)*(i-(double)
          ScaleQuantumToShort(ScaleCharToQuantum(156)));
        blue_map[i+G]=(-0.9271435)*(i-(double)
          ScaleQuantumToShort(ScaleCharToQuantum(137)));
        red_map[i+B]=1.3584*i;
        green_map[i+B]=2.2179*(i-(double)
          ScaleQuantumToShort(ScaleCharToQuantum(156)));
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95620*I+0.62140*Q
          G = Y-0.27270*I-0.64680*Q
          B = Y-1.10370*I+1.70060*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.4781*(2.0*i-65535L);
        blue_map[i+R]=0.3107*(2.0*i-65535L);
        red_map[i+G]=i;
        green_map[i+G]=(-0.13635)*(2.0*i-65535L);
        blue_map[i+G]=(-0.3234)*(2.0*i-65535L);
        red_map[i+B]=i;
        green_map[i+B]=(-0.55185)*(2.0*i-65535L);
        blue_map[i+B]=0.8503*(2.0*i-65535L);
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          R = Y            +1.402000*C2
          G = Y-0.344136*C1+0.714136*C2
          B = Y+1.772000*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=0.701*(2.0*i-65535L);
        red_map[i+G]=i;
        green_map[i+G]=(-0.172068)*(2.0*i-65535L);
        blue_map[i+G]=0.357068*(2.0*i-65535L);
        red_map[i+B]=i;
        green_map[i+B]=0.886*(2.0*i-65535L);
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= 65535L; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=0.5699*(2.0*i-65535L);
        red_map[i+G]=i;
        green_map[i+G]=(-0.1969)*(2.0*i-65535L);
        blue_map[i+G]=(-0.29025)*(2.0*i-65535L);
        red_map[i+B]=i;
        green_map[i+B]=1.01395*(2.0*i-65535L);
        blue_map[i+B]=0;
      }
      break;
    }
  }
  /*
    Convert to RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          pixel.red=red_map[ScaleQuantumToShort(q->red)+R]+
            green_map[ScaleQuantumToShort(q->green)+R]+
            blue_map[ScaleQuantumToShort(q->blue)+R];
          pixel.green=red_map[ScaleQuantumToShort(q->red)+G]+
            green_map[ScaleQuantumToShort(q->green)+G]+
            blue_map[ScaleQuantumToShort(q->blue)+G];
          pixel.blue=red_map[ScaleQuantumToShort(q->red)+B]+
            green_map[ScaleQuantumToShort(q->green)+B]+
            blue_map[ScaleQuantumToShort(q->blue)+B];
          switch (colorspace)
          {
            case sRGBColorspace:
            case YCCColorspace:
            {
              pixel.red=(unsigned long) ((pixel.red < 0) ? 0 :
                (pixel.red > 89950) ? 89950 : pixel.red+0.5);
              pixel.green=(unsigned long) ((pixel.green < 0) ? 0 :
                (pixel.green > 89950) ? 89950 : pixel.green+0.5);
              pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0 :
                (pixel.blue > 89950) ? 89950 : pixel.blue+0.5);
              if (colorspace == sRGBColorspace)
                {
                  q->red=(Quantum)
                    ScaleToQuantum(sRGBMap[(unsigned long) (pixel.red/257)]);
                  q->green=(Quantum)
                    ScaleToQuantum(sRGBMap[(unsigned long) (pixel.green/257)]);
                  q->blue=(Quantum)
                    ScaleToQuantum(sRGBMap[(unsigned long) (pixel.blue/257)]);
                  break;
                }
              q->red=(Quantum)
                ScaleToQuantum(YCCMap[(unsigned long) (pixel.red/257)]);
              q->green=(Quantum)
                ScaleToQuantum(YCCMap[(unsigned long) (pixel.green/257)]);
              q->blue=(Quantum)
                ScaleToQuantum(YCCMap[(unsigned long) (pixel.blue/257)]);
              break;
            }
            default:
            {
              pixel.red=(unsigned long) ((pixel.red < 0) ? 0 :
                (pixel.red > 65535L) ? 65535L : pixel.red+0.5);
              pixel.green=(unsigned long) ((pixel.green < 0) ? 0 :
                (pixel.green > 65535L) ? 65535L : pixel.green+0.5);
              pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0 :
                (pixel.blue > 65535L) ? 65535L : pixel.blue+0.5);
              q->red=ScaleShortToQuantum(pixel.red);
              q->green=ScaleShortToQuantum(pixel.green);
              q->blue=ScaleShortToQuantum(pixel.blue);
              break;
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(TransformRGBImageText,y,image->rows,exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        pixel.red=red_map[ScaleQuantumToShort(image->colormap[i].red)+R]+
          green_map[ScaleQuantumToShort(image->colormap[i].green)+R]+
          blue_map[ScaleQuantumToShort(image->colormap[i].blue)+R];
        pixel.green=red_map[ScaleQuantumToShort(image->colormap[i].red)+G]+
          green_map[ScaleQuantumToShort(image->colormap[i].green)+G]+
          blue_map[ScaleQuantumToShort(image->colormap[i].blue)+G];
        pixel.blue=red_map[ScaleQuantumToShort(image->colormap[i].red)+B]+
          green_map[ScaleQuantumToShort(image->colormap[i].green)+B]+
          blue_map[ScaleQuantumToShort(image->colormap[i].blue)+B];
        switch (colorspace)
        {
          case sRGBColorspace:
          case YCCColorspace:
          {
            pixel.red=(unsigned long) ((pixel.red < 0) ? 0 :
              (pixel.red > 89950) ? 89950 : pixel.red+0.5);
            pixel.green=(unsigned long) ((pixel.green < 0) ? 0 :
              (pixel.green > 89950) ? 89950 : pixel.green+0.5);
            pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0 :
              (pixel.blue > 89950) ? 89950 : pixel.blue+0.5);
            if (colorspace == sRGBColorspace)
              {
                image->colormap[i].red=(Quantum)
                  ScaleToQuantum(sRGBMap[(unsigned long) (pixel.red/257)]);
                image->colormap[i].green=(Quantum)
                  ScaleToQuantum(sRGBMap[(unsigned long) (pixel.green/257)]);
                image->colormap[i].blue=(Quantum)
                  ScaleToQuantum(sRGBMap[(unsigned long) (pixel.blue/257)]);
                break;
              }
            image->colormap[i].red=(Quantum)
              ScaleToQuantum(YCCMap[(unsigned long) (pixel.red/257)]);
            image->colormap[i].green=(Quantum)
              ScaleToQuantum(YCCMap[(unsigned long) (pixel.green/257)]);
            image->colormap[i].blue=(Quantum)
              ScaleToQuantum(YCCMap[(unsigned long) (pixel.blue/257)]);
            break;
          }
          default:
          {
            pixel.red=(unsigned long) ((pixel.red < 0) ? 0 :
              (pixel.red > 65535L) ? 65535L : pixel.red+0.5);
            pixel.green=(unsigned long) ((pixel.green < 0) ? 0 :
              (pixel.green > 65535L) ? 65535L : pixel.green+0.5);
            pixel.blue=(unsigned long) ((pixel.blue < 0) ? 0 :
              (pixel.blue > 65535L) ? 65535L : pixel.blue+0.5);
            image->colormap[i].red=ScaleShortToQuantum(pixel.red);
            image->colormap[i].green=ScaleShortToQuantum(pixel.green);
            image->colormap[i].blue=ScaleShortToQuantum(pixel.blue);
            break;
          }
        }
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocate memory.
  */
  LiberateMemory((void **) &blue_map);
  LiberateMemory((void **) &green_map);
  LiberateMemory((void **) &red_map);
  return(True);
}
