/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            V   V  IIIII  DDDD                               %
%                            V   V    I    D   D                              %
%                            V   V    I    D   D                              %
%                             V V     I    D   D                              %
%                              V    IIIII  DDDD                               %
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
#include "attribute.h"
#include "blob.h"
#include "log.h"
#include "magick.h"
#include "monitor.h"
#include "utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteVIDImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVIDImage reads one of more images and creates a Visual Image
%  Directory file.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadVIDImage method is:
%
%      Image *ReadVIDImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadVIDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadVIDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define ClientName  "montage"

  char
    **filelist,
    **list;

  Image
    *image,
    *montage_image,
    *next_image;

  ImageInfo
    *clone_info;

  int
    number_files;

  MonitorHandler
    handler;

  MontageInfo
    *montage_info;

  register long
    i;

  unsigned int
    status;

  /*
    Expand the filename.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  image=AllocateImage(image_info);
  list=(char **) AcquireMemory(sizeof(char *));
  if (list == (char **) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
    }
  list[0]=(char *) AllocateString((char *) NULL);
  (void) strncpy(list[0],image_info->filename,MaxTextExtent-1);
  number_files=1;
  filelist=list;
  status=ExpandFilenames(&number_files,&filelist);
  if ((status == False) || (number_files == 0))
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
    }
  DestroyImage(image);
  /*
    Read each image and convert them to a tile.
  */
  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  for (i=0; i < number_files; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (clone_info->size == (char *) NULL)
      CloneString(&clone_info->size,DefaultTileGeometry);
    (void) strncpy(clone_info->filename,filelist[i],MaxTextExtent-1);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"name: %.1024s",
      clone_info->filename);
    next_image=ReadImage(clone_info,exception);
    LiberateMemory((void **) &filelist[i]);
    if (next_image != (Image *) NULL)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"geometry: %ldx%ld",
          next_image->columns,next_image->rows);
        (void) SetImageAttribute(next_image,"label",DefaultTileLabel);
        (void) TransformImage(&next_image,(char *) NULL,clone_info->size);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"thumbnail geometry: %ldx%ld",
          next_image->columns,next_image->rows);
        if (image == (Image *) NULL)
          image=next_image;
        else
          {
            image->next=next_image;
            image->next->previous=image;
            image=image->next;
          }
      }
    (void) SetMonitorHandler(handler);
    if (!MagickMonitor(LoadImageText,i,number_files,&image->exception))
      break;
  }
  DestroyImageInfo(clone_info);
  LiberateMemory((void **) &filelist);
  if (image == (Image *) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(CorruptImageError,"UnableToReadVIDImage",image);
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Create the visual image directory.
  */
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"creating montage");
  montage_image=MontageImages(image,montage_info,exception);
  DestroyMontageInfo(montage_info);
  if (montage_image == (Image *) NULL)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
      ThrowReaderException(CorruptImageError,"UnableToReadVIDImage",image);
    }
  DestroyImageList(image);
  LiberateMemory((void **) &list[0]);
  LiberateMemory((void **) &list);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r V I D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterVIDImage adds attributes for the VID image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterVIDImage method is:
%
%      RegisterVIDImage(void)
%
*/
ModuleExport void RegisterVIDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("VID");
  entry->decoder=(DecoderHandler) ReadVIDImage;
  entry->encoder=(EncoderHandler) WriteVIDImage;
  entry->description=AcquireString("Visual Image Directory");
  entry->module=AcquireString("VID");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r V I D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterVIDImage removes format registrations made by the
%  VID module from the list of supported formats.
%
%  The format of the UnregisterVIDImage method is:
%
%      UnregisterVIDImage(void)
%
*/
ModuleExport void UnregisterVIDImage(void)
{
  (void) UnregisterMagickInfo("VID");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVIDImage writes an image to a file in VID X image format.
%
%  The format of the WriteVIDImage method is:
%
%      unsigned int WriteVIDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVIDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteVIDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *montage_image;

  MontageInfo
    *montage_info;

  register Image
    *p;

  unsigned int
    status;

  /*
    Create the visual image directory.
  */
  for (p=image; p != (Image *) NULL; p=p->next)
    (void) SetImageAttribute(p,"label",DefaultTileLabel);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  montage_image=MontageImages(image,montage_info,&image->exception);
  if (montage_image == (Image *) NULL)
    ThrowWriterException(CorruptImageError,image->exception.reason,image);
  FormatString(montage_image->filename,"miff:%.1024s",image->filename);
  status=WriteImage(image_info,montage_image);
  DestroyImageList(montage_image);
  return(status);
}
