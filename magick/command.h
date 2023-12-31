/*
  Copyright (C) 2003, 2007 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Image Command Methods.
*/
#ifndef _MAGICK_COMMAND_H
#define _MAGICK_COMMAND_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport unsigned int
  AnimateImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  BenchmarkImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  CompareImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  CompositeImageCommand(ImageInfo *image_info,int argc,char **argv,
                        char **metadata,ExceptionInfo *exception),
  ConjureImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  ConvertImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  DisplayImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  IdentifyImageCommand(ImageInfo *image_info,int argc,char **argv,
                       char **metadata,ExceptionInfo *exception),
  ImportImageCommand(ImageInfo *image_info,int argc,char **argv,
                     char **metadata,ExceptionInfo *exception),
  MagickCommand(ImageInfo *image_info,int argc,char **argv,
                char **metadata,ExceptionInfo *exception),
  MogrifyImage(const ImageInfo *,int,char **,Image **),
  MogrifyImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  MogrifyImages(const ImageInfo *,int,char **,Image **),
  MontageImageCommand(ImageInfo *image_info,int argc,char **argv,
                      char **metadata,ExceptionInfo *exception),
  TimeImageCommand(ImageInfo *image_info,int argc,char **argv,
		   char **metadata,ExceptionInfo *exception);

extern MagickExport int
  GMCommand(int argc,char **argv);

#if defined(MAGICK_IMPLEMENTATION)

extern void
  MagickDestroyCommandInfo(void);

extern MagickPassFail
  MagickInitializeCommandInfo(void);

#endif /* defined(MAGICK_IMPLEMENTATION) */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
