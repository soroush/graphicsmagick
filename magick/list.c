/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         L      IIIII  SSSSS  TTTTT                          %
%                         L        I    SS       T                            %
%                         L        I     SSS     T                            %
%                         L        I       SS    T                            %
%                         LLLLL  IIIII  SSSSS    T                            %
%                                                                             %
%                                                                             %
%                       ImageMagick Image List Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                January 2002                                 %
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
#include "blob.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageList() returns a duplicate of the image list.
%
%  The format of the CloneImageList method is:
%
%      Image *CloneImageList(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImageList(const Image *images,ExceptionInfo *exception)
{
  Image
    *clone_images,
    *image;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  clone_images=(Image *) NULL;
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for ( ; images != (Image *) NULL; images=images->next)
  {
    image=CloneImage(images,0,0,True,exception);
    if (image == (Image *) NULL)
      {
        if (clone_images != (Image *) NULL)
          DestroyImageList(clone_images);
        return((Image *) NULL);
      }
    if (clone_images == (Image *) NULL)
      {
        clone_images=image;
        continue;
      }
    image->previous=clone_images;
    clone_images->next=image;
    clone_images=clone_images->next;
  }
  while (clone_images->previous != (Image *) NULL)
    clone_images=clone_images->previous;
  return(clone_images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e I m a g e L i s t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageList() deletes an image at the specified position in the list.
%
%  The format of the DeleteImageList method is:
%
%      unsigned int DeleteImageList(Image *images,const long offset)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%
*/
MagickExport unsigned int DeleteImageList(Image *images,const long offset)
{
  Image
    *image;

  register long
    i;

  if (images == (Image *) NULL)
    return(False);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    if (i++ == offset)
      break;
  if (images == (Image *) NULL)
    return(False);
  image=images;
  if (images->previous != (Image *) NULL)
    images->previous->next=images->next;
  if (images->next != (Image *) NULL)
    images->next->previous=images->previous;
  DestroyImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageList() destroys an image list.
%
%  The format of the DestroyImageList method is:
%
%      void DestroyImageList(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%
*/
MagickExport void DestroyImageList(Image *images)
{
  Image
    *image;

  if (images == (Image *) NULL)
    return;
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (image=images; images != (Image *) NULL; image=images)
  {
    images=images->next;
    DestroyImage(image);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageList() returns an image at the specified position in the list.
%
%  The format of the GetImageList method is:
%
%      Image *GetImageList(const Image *images,const long offset,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
%
*/
MagickExport Image *GetImageList(const Image *images,const long offset,
  ExceptionInfo *exception)
{
  register long
    i;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    if (i++ == offset)
      break;
  if (images == (Image *) NULL)
    return((Image *) NULL);
  return(CloneImage(images,0,0,True,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t I n d e x                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListIndex() returns the position in the list of the specified
%  image.
%
%  The format of the GetImageListIndex method is:
%
%      long GetImageListIndex(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport long GetImageListIndex(const Image *images)
{
  register long
    i;

  if (images == (const Image *) NULL)
    return(-1);
  assert(images->signature == MagickSignature);
  for (i=0; images->previous != (Image *) NULL; i++)
    images=images->previous;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t S i z e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListSize() returns the number of images in the list.
%
%  The format of the GetImageListSize method is:
%
%      unsigned long GetImageListSize(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport unsigned long GetImageListSize(const Image *images)
{
  register long
    i;

  if (images == (Image *) NULL)
    return(0);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    i++;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N e x t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNextImage() returns the next image in a list.
%
%  The format of the GetNextImage method is:
%
%      Image *GetNextImage(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetNextImage(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->next == (Image *) NULL)
    return((Image *) NULL);
  if (images->blob != images->next->blob)
    {
      DestroyBlobInfo(images->next->blob);
      images->next->blob=ReferenceBlob(images->blob);
    }
  return(images->next);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I m a g e L i s t T o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageListToArray() is a convenience method that converts an image list to
%  a sequential array.  For example,
%
%    group = ImageListToArray(images, exception);
%    for (i = 0; i < n; i++)
%      puts(group[i]->filename);
%
%  The format of the ImageListToArray method is:
%
%      Image **ImageListToArray(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image **ImageListToArray(const Image *images,
  ExceptionInfo *exception)
{
  Image
    **group;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image **) NULL);
  assert(images->signature == MagickSignature);
  group=(Image **) AcquireMemory(GetImageListSize(images)*sizeof(Image *));
  if (group == (Image **) NULL)
    {
      ThrowException(exception,ResourceLimitError,"MemoryAllocationFailed",
        "unable to create image group");
      return((Image **) NULL);
    }
  for (i=0; i < (long) GetImageListSize(images); i++)
    group[i]=GetImageList(images,i,exception);
  return(group);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewImageList() creates an empty image list.
%
%  The format of the NewImageList method is:
%
%      Image *NewImageList(void)
%
*/
MagickExport Image *NewImageList(void)
{
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P o p I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PopImageList() removes the last image in the list.
%
%  The format of the PopImageList method is:
%
%      Image *PopImageList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *PopImageList(Image **images)
{
  Image
    *image;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  while ((*images)->next != (Image *) NULL)
    (*images)=(*images)->next;
  image=(*images);
  *images=(*images)->previous;
  if ((*images) != (Image *) NULL)
    (*images)->next=(Image *) NULL;
  image->previous=(Image *) NULL;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P u s h I m a g e L i s t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PushImageList() adds an image to the end of the list.
%
%  The format of the PushImageList method is:
%
%      unsigned int PushImageList(Image *images,const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int PushImageList(Image **images,const Image *image,
  ExceptionInfo *exception)
{
  Image
    *next;

  assert(images != (Image **) NULL);
  if (image == (Image *) NULL)
    return(False);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      *images=CloneImageList(image,exception);
      return(*images != (Image *) NULL);
    }
  assert((*images)->signature == MagickSignature);
  for (next=(*images); next->next != (Image *) NULL; next=next->next);
  next->next=CloneImageList(image,exception);
  if (next->next == (Image *) NULL)
    return(False);
  next->next->previous=next;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e v e r s e I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReverseImageList() reverses the image list.
%
%  The format of the ReverseImageList method is:
%
%      Image *ReverseImageList(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReverseImageList(const Image *images,
  ExceptionInfo *exception)
{
  Image
    *reverse_images,
    *image;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  reverse_images=(Image *) NULL;
  for (i=0; i < (long) GetImageListSize(images); i++)
  {
    image=GetImageList(images,(long) GetImageListSize(images)-i-1,exception);
    if (image == (Image *) NULL)
      {
        if (reverse_images != (Image *) NULL)
          DestroyImageList(reverse_images);
        break;
      }
    if (reverse_images == (Image *) NULL)
      {
        reverse_images=image;
        continue;
      }
    image->previous=reverse_images;
    reverse_images->next=image;
    reverse_images=reverse_images->next;
  }
  while (reverse_images->previous != (Image *) NULL)
    reverse_images=reverse_images->previous;
  return(reverse_images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageList() inserts an image into the list at the specified position.
%
%  The format of the SetImageList method is:
%
%      unsigned int SetImageList(Image *images,const Image *image,
%        const long offset,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o offset: The position within the list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int SetImageList(Image **images,const Image *image,
  const long offset,ExceptionInfo *exception)
{
  Image
    *next;

  register long
    i;

  assert(images != (Image **) NULL);
  if (image == (Image *) NULL)
    return(False);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      if (offset > 0)
        return(False);
      *images=CloneImageList(image,exception);
      return(*images != (Image *) NULL);
    }
  assert((*images)->signature == MagickSignature);
  for (next=(*images); next->previous != (Image *) NULL; next=next->previous);
  for (i=0; next != (Image *) NULL; next=next->next)
    if (i++ == offset)
      break;
  if (next == (Image *) NULL)
    return(False);
  next->next=CloneImageList(image,exception);
  if (next->next == (Image *) NULL)
    return(False);
  next->next->previous=next;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S h i f t I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ShiftImageList() removes an image from the beginning of the list.
%
%  The format of the ShiftImageList method is:
%
%      Image *ShiftImageList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *ShiftImageList(Image **images)
{
  Image
    *image;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  image=(*images);
  *images=(*images)->next;
  if ((*images) != (Image *) NULL)
    (*images)->previous=(Image *) NULL;
  image->next=(Image *) NULL;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i c e I m a g e L i s t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpliceImageList() removes the images designated by offset and length from
%  the list and replaces them with the specified list.
%
%  The format of the SpliceImageList method is:
%
%      Image *SpliceImageList(Image *images,const long offset,
%        const unsigned long length,const Image *splices,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%    o length: The length of the image list to remove.
%
%    o splice: Replace the removed image list with this list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
%
*/
MagickExport Image *SpliceImageList(Image *images,const long offset,
  const unsigned long length,const Image *splices,ExceptionInfo *exception)
{
  Image
    *splice;

  register long
    i;

  register Image
    *p,
    *q;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    if (i++ == offset)
      break;
  if (images == (Image *) NULL)
    return((Image *) NULL);
  p=images;
  for (i=1; images != (Image *) NULL; images=images->next)
    if (i++ == (long) length)
      break;
  if (images == (Image *) NULL)
    return((Image *) NULL);
  q=images;
  splice=CloneImageList(splices,exception);
  if (splice == (Image *) NULL)
    return(q);
  if (p->previous != (Image *) NULL)
    {
      p->previous->next=splice;
      if (splice != (Image *) NULL)
        splice->previous=p->previous;
    }
  p->previous=(Image *) NULL;
  if (q->next != (Image *) NULL)
    {
      q->next->previous=splice;
      if (splice != (Image *) NULL)
        {
          while (splice->next != (Image *) NULL)
            splice=splice->next;
          splice->next=q->next;
        }
    }
  q->next=(Image *) NULL;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n s h i f t I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnshiftImageList() adds the image to the beginning of the list.
%
%  The format of the UnshiftImageList method is:
%
%      unsigned int UnshiftImageList(Image *images,const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int UnshiftImageList(Image **images,const Image *image,
  ExceptionInfo *exception)
{
  Image
    *next;

  assert(images != (Image **) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      *images=CloneImageList(image,exception);
      return(*images != (Image *) NULL);
    }
  assert((*images)->signature == MagickSignature);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  next=CloneImageList(image,exception);
  if (next == (Image *) NULL)
    return(False);
  while (next->next != (Image *) NULL)
    next=next->next;
  (*images)->previous=next;
  (*images)->previous->next=(*images);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  return(True);
}
