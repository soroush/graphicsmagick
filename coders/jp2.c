/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                              JJJ  PPPP    222                               %
%                               J   P   P  2   2                              %
%                               J   PPPP     22                               %
%                            J  J   P       2                                 %
%                             JJ    P      22222                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                                John Cristy                                  %
%                                Nathan Brown                                 %
%                                 June 2001                                   %
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
#include "magick.h"
#include "monitor.h"
#include "utility.h"
#if defined(HasJP2)
#if !defined(uchar)
#define uchar  unsigned char
#endif
#if !defined(ushort)
#define ushort  unsigned short
#endif
#if !defined(uint)
#define uint  unsigned int
#endif
#if !defined(longlong)
#define longlong  long long
#endif
#if !defined(ulonglong)
#define ulonglong  unsigned long long
#endif
#if !defined(JAS_IMAGE_CS_GRAY)
#define JAS_IMAGE_CS_GRAY  1
#endif
#if !defined(JAS_IMAGE_CS_RGB)
#define JAS_IMAGE_CS_RGB  2
#endif
#ifdef __VMS
#define JAS_VERSION 1.600.0
#define PACKAGE jasper
#define VERSION 1.600.0
#endif
#include "jasper/jasper.h"
#endif

/*
  Forward declarations.
*/
#if defined(HasJP2)
static unsigned int
  WriteJP2Image(const ImageInfo *,Image *);
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P 2                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJP2 returns True if the image format type, identified by the
%  magick string, is JP2.
%
%  The format of the IsJP2 method is:
%
%      unsigned int IsJP2(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJP2 returns True if the image format type is JP2.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJP2(const unsigned char *magick,const size_t length)
{
  if (length < 9)
    return(False);
  if (memcmp(magick+4,"\152\120\040\040\015",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P C                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJPC returns True if the image format type, identified by the
%  magick string, is JPC.
%
%  The format of the IsJPC method is:
%
%      unsigned int IsJPC(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJPC returns True if the image format type is JPC.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJPC(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\377\117",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P 2 I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJP2Image reads a JPEG 2000 Image file (JP2) or JPEG 2000
%  codestream (JPC) image file and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image or set of images.
%
%  JP2 support written by Nathan Brown, nathanbrown@letu.edu.
%
%  The format of the ReadJP2Image method is:
%
%      Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJP2Image returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
#if defined(HasJP2)

typedef struct _StreamManager
{
  jas_stream_t
    *stream;

  Image
    *image;
} StreamManager;

static int BlobRead(jas_stream_obj_t *object,char *buffer,const int length)
{
  size_t
    count;

  count=ReadBlob(((StreamManager *) object)->image,(size_t) length,
    (void *) buffer);
  return((int) count);
}

static int BlobWrite(jas_stream_obj_t *object,char *buffer,const int length)
{
  size_t
    count;

  count=WriteBlob(((StreamManager *) object)->image,(size_t) length,
    (void *) buffer);
  return((int) count);
}

static long BlobSeek(jas_stream_obj_t *object,long offset,int origin)
{
  return(SeekBlob(((StreamManager *) object)->image,offset,origin));
}

static int BlobClose(jas_stream_obj_t *object)
{
  CloseBlob(((StreamManager *) object)->image);
  return(0);
}

static jas_stream_ops_t
  StreamOperators =
  {
    BlobRead,
    BlobWrite,
    BlobSeek,
    BlobClose
  };

static jas_stream_t *JP2StreamManager(Image *image)
{
  jas_stream_t 
    *stream;

  StreamManager
    *source;

  stream=(jas_stream_t *) AcquireMemory(sizeof(jas_stream_t));
  if (stream == (jas_stream_t *) NULL)
    return((jas_stream_t *) NULL);
  (void) memset(stream,0,sizeof(jas_stream_t));
  stream->rwlimit_=(-1);
  stream->obj_=(jas_stream_obj_t *) AcquireMemory(sizeof(StreamManager));
  if (stream->obj_ == (jas_stream_obj_t *) NULL)
    return((jas_stream_t *) NULL);
  stream->ops_=(&StreamOperators);
  stream->openmode_=JAS_STREAM_READ | JAS_STREAM_WRITE | JAS_STREAM_BINARY;
  stream->bufbase_=stream->tinybuf_;
  stream->bufsize_=1;
  stream->bufstart_=(&stream->bufbase_[JAS_STREAM_MAXPUTBACK]);
  stream->ptr_=stream->bufstart_;
  stream->bufmode_|=JAS_STREAM_UNBUF & JAS_STREAM_BUFMODEMASK;
  source=(StreamManager *) stream->obj_;
  source->image=image;
  return(stream);
}

static Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  unsigned long
    number_components;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  /*
    Initialize JPEG 2000 API.
  */
  jas_init();
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowReaderException(DelegateError,"UnableToManageJP2Stream",image);
  jp2_image=jas_image_decode(jp2_stream,-1,0);
  if (jp2_image == (jas_image_t *) NULL)
    ThrowReaderException(DelegateError,"UnableToDecodeImageFile",image);
  /*
    Convert JPEG 2000 pixels.
  */
  image->columns=jas_image_width(jp2_image);
  image->rows=jas_image_height(jp2_image);
  if (image_info->ping)
    {
      (void) jas_stream_close(jp2_stream);
      jas_image_destroy(jp2_image);
      CloseBlob(image);
      return(image);
    }
  number_components=Min(jas_image_numcmpts(jp2_image),4);
  image->matte=number_components > 3;
  for (i=0; i < (long) number_components; i++)
  {
    if (jas_image_cmptprec(jp2_image,i) <= 8)
      image->depth=jas_image_cmptprec(jp2_image,i);
    pixels[i]=jas_matrix_create(1,(unsigned int) image->columns);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        jas_image_destroy(jp2_image);
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image)
      }
  }
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (i=0; i < (long) number_components; i++)
      (void) jas_image_readcmpt(jp2_image,(short) i,0,(unsigned int) y,
        (unsigned int) image->columns,1,pixels[i]);
    if (image->depth <= 8)
      for (x=0; x < (long) image->columns; x++)
      {
        q->red=ScaleCharToQuantum(jas_matrix_getv(pixels[0],x));
        q->green=q->red;
        q->blue=q->red;
        if (number_components > 1)
          q->green=ScaleCharToQuantum(jas_matrix_getv(pixels[1],x));
        if (number_components > 2)
          q->blue=ScaleCharToQuantum(jas_matrix_getv(pixels[2],x));
        if (number_components > 3)
          q->opacity=ScaleCharToQuantum(jas_matrix_getv(pixels[3],x));
        q++;
      }
    else
      if (image->depth <= 16)
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=ScaleShortToQuantum(jas_matrix_getv(pixels[0],x));
          q->green=q->red;
          q->blue=q->red;
          if (number_components > 1)
            q->green=ScaleShortToQuantum(jas_matrix_getv(pixels[1],x));
          if (number_components > 2)
            q->blue=ScaleShortToQuantum(jas_matrix_getv(pixels[2],x));
          if (number_components > 3)
            q->opacity=ScaleShortToQuantum(jas_matrix_getv(pixels[3],x));
          q++;
        }
      else
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=ScaleLongToQuantum(jas_matrix_getv(pixels[0],x));
          q->green=q->red;
          q->blue=q->red;
          if (number_components > 1)
            q->green=ScaleLongToQuantum(jas_matrix_getv(pixels[1],x));
          if (number_components > 2)
            q->blue=ScaleLongToQuantum(jas_matrix_getv(pixels[2],x));
          if (number_components > 3)
            q->opacity=ScaleLongToQuantum(jas_matrix_getv(pixels[3],x));
          q++;
        }
      if (!SyncImagePixels(image))
      break;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitor(LoadImageText,y,image->rows,exception))
          break;
  }
  for (i=0; i < (long) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  (void) jas_stream_close(jp2_stream);
  jas_image_destroy(jp2_image);
  CloseBlob(image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J P 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJP2Image adds attributes for the JP2 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJP2Image method is:
%
%      RegisterJP2Image(void)
%
*/
ModuleExport void RegisterJP2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("JP2");
  entry->description=AcquireString("JPEG-2000 JP2 File Format Syntax");
  entry->module=AcquireString("JP2");
  entry->magick=(MagickHandler) IsJP2;
  entry->adjoin=False;
  entry->thread_support=False;
#if defined(HasJP2)
  entry->decoder=(DecoderHandler) ReadJP2Image;
  entry->encoder=(EncoderHandler) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPC");
  entry->description=AcquireString("JPEG-2000 Code Stream Syntax");
  entry->module=AcquireString("JPC");
  entry->magick=(MagickHandler) IsJPC;
  entry->adjoin=False;
  entry->thread_support=False;
#if defined(HasJP2)
  entry->decoder=(DecoderHandler) ReadJP2Image;
  entry->encoder=(EncoderHandler) WriteJP2Image;
#endif
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J P 2 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJP2Image removes format registrations made by the
%  PNG module from the list of supported formats.
%
%  The format of the UnregisterJP2Image method is:
%
%      UnregisterJP2Image(void)
%
*/
ModuleExport void UnregisterJP2Image(void)
{
  (void) UnregisterMagickInfo("JP2");
  (void) UnregisterMagickInfo("JPC");
}

#if defined(HasJP2)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J P 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJP2Image writes an image in the JPEG 2000 image format.
%
%  JP2 support written by Nathan Brown, nathanbrown@letu.edu
%
%  The format of the WriteJP2Image method is:
%
%      unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteJP2Image return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
{
  char
    magick[MaxTextExtent],
    options[MaxTextExtent];

  int
    format;

  long
    y;

  jas_image_cmptparm_t
    component_info[4];

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  unsigned int
    status;

  unsigned long
    number_components;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  /*
    Intialize JPEG 2000 API.
  */
  (void) TransformRGBImage(image,RGBColorspace);
  jas_init();
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowWriterException(DelegateError,"UnableToManageJP2Stream",image);
  number_components=image->matte ? 4 : 3;
  if ((image_info->type != TrueColorType) &&
      IsGrayImage(image,&image->exception))
    number_components=1;
  for (i=0; i < (long) number_components; i++)
  {
    (void) memset(component_info+i,0,sizeof(jas_image_cmptparm_t));
    component_info[i].hstep=1;
    component_info[i].vstep=1;
    component_info[i].width=(unsigned int) image->columns;
    component_info[i].height=(unsigned int) image->rows;
    component_info[i].prec=(unsigned int) image->depth <= 8 ? 8 : 16;
  }
  jp2_image=jas_image_create((short) number_components,component_info,
    number_components == 1 ? JAS_IMAGE_CS_GRAY : JAS_IMAGE_CS_RGB);
  if (jp2_image == (jas_image_t *) NULL)
    ThrowWriterException(DelegateError,"UnableToCreateImage",image);
  /*
    Convert to JPEG 2000 pixels.
  */
  for (i=0; i < (long) number_components; i++)
  {
    pixels[i]=jas_matrix_create(1,(unsigned int) image->columns);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        for (x=0; x < i; x++)
          jas_matrix_destroy(pixels[x]);
        jas_image_destroy(jp2_image);
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image)
      }
  }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (image->depth <= 8)
      for (x=0; x < (long) image->columns; x++)
      {
        if (number_components == 1)
          jas_matrix_setv(pixels[0],x,
            ScaleQuantumToChar(PixelIntensityToQuantum(p)));
        else
          {
            jas_matrix_setv(pixels[0],x,ScaleQuantumToChar(p->red));
            jas_matrix_setv(pixels[1],x,ScaleQuantumToChar(p->green));
            jas_matrix_setv(pixels[2],x,ScaleQuantumToChar(p->blue));
            if (number_components > 3)
              jas_matrix_setv(pixels[3],x,ScaleQuantumToChar(p->opacity));
          }
        p++;
      }
    else
      if (image->depth <= 16)
        for (x=0; x < (long) image->columns; x++)
        {
          if (number_components == 1)
            jas_matrix_setv(pixels[0],x,
              ScaleQuantumToShort(PixelIntensityToQuantum(p)));
          else
            {
              jas_matrix_setv(pixels[0],x,ScaleQuantumToShort(p->red));
              jas_matrix_setv(pixels[1],x,ScaleQuantumToShort(p->green));
              jas_matrix_setv(pixels[2],x,ScaleQuantumToShort(p->blue));
              if (number_components > 3)
                jas_matrix_setv(pixels[3],x,ScaleQuantumToShort(p->opacity));
            }
          p++;
        }
      else
        for (x=0; x < (long) image->columns; x++)
        {
          if (number_components == 1)
            jas_matrix_setv(pixels[0],x,
              ScaleQuantumToLong(PixelIntensityToQuantum(p)));
          else
            {
              jas_matrix_setv(pixels[0],x,ScaleQuantumToLong(p->red));
              jas_matrix_setv(pixels[1],x,ScaleQuantumToLong(p->green));
              jas_matrix_setv(pixels[2],x,ScaleQuantumToLong(p->blue));
              if (number_components > 3)
                jas_matrix_setv(pixels[3],x,ScaleQuantumToLong(p->opacity));
            }
          p++;
        }
    for (i=0; i < (long) number_components; i++)
      (void) jas_image_writecmpt(jp2_image,(short) i,0,(unsigned int) y,
        (unsigned int) image->columns,1,pixels[i]);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
          break;
  }
  for (i=0; i < (long) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  (void) strncpy(magick,image_info->magick,MaxTextExtent-1);
  LocaleLower(magick);
  format=jas_image_strtofmt(magick);
  FormatString(options,"rate=%lf",(double) image_info->quality/100.0);
  status=jas_image_encode(jp2_image,jp2_stream,format,options);
  if (status)
    ThrowWriterException(DelegateError,"UnableToEncodeImageFile",image);
  (void) jas_stream_close(jp2_stream);
  jas_image_destroy(jp2_image);
  CloseBlob(image);
  return(True);
}
#endif
