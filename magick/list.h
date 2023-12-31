/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  ImageMagick Image List Methods.
*/
#ifndef _MAGICK_LIST_H
#define _MAGICK_LIST_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport Image
  *CloneImageList(const Image *,ExceptionInfo *),
  *GetFirstImageInList(const Image *),
  *GetImageFromList(const Image *,const long),
  *GetLastImageInList(const Image *),
  *GetNextImageInList(const Image *),
  *GetPreviousImageInList(const Image *),
  **ImageListToArray(const Image *,ExceptionInfo *),
  *NewImageList(void),
  *RemoveLastImageFromList(Image **),
  *RemoveFirstImageFromList(Image **),
  *SplitImageList(Image *),
  *SyncNextImageInList(const Image *);

extern MagickExport long
  GetImageIndexInList(const Image *);

extern MagickExport unsigned long
  GetImageListLength(const Image *);

extern MagickExport void
  AppendImageToList(Image **,Image *),
  DeleteImageFromList(Image **),
  DestroyImageList(Image *),
  InsertImageInList(Image **,Image *),
  PrependImageToList(Image **,Image *),
  ReplaceImageInList(Image **images,Image *image),
  ReverseImageList(Image **),
  SpliceImageIntoList(Image **,const unsigned long,Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
