/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         BBBB   L       OOO   BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   L      O   O  BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   LLLLL   OOO   BBBB                           %
%                                                                             %
%                                                                             %
%                    ImageMagick Binary Large OBjectS Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include <signal.h>

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BlobToImage implements direct to memory image formats.  It returns
%  the blob as an image.
%
%  The format of the BlobToImage method is:
%
%      Image *BlobToImage(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method BlobToImage returns an image from the supplied blob.
%      If an error occurs NULL is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by ImageMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *BlobToImage(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  ExceptionInfo
    error;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    file;

  MagickInfo
    *magick_info;

  off_t
    count;

  assert(image_info != (ImageInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  GetExceptionInfo(exception);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob.data=(char *) blob;
  clone_info->blob.offset=0;
  clone_info->blob.length=length;
  clone_info->blob.extent=length;
  SetImageInfo(clone_info,False);
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick);
  if (magick_info == (MagickInfo *) NULL)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unrecognized image format",
        clone_info->magick);
      return((Image *) NULL);
    }
  GetBlobInfo(&(clone_info->blob));
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      *clone_info->filename='\0';
      clone_info->blob.data=(char *) blob;
      clone_info->blob.length=length;
      clone_info->blob.extent=length;
      image=ReadImage(clone_info,&error);
      DestroyImageInfo(clone_info);
      if (image != (Image *) NULL)
        GetBlobInfo(&(image->blob));
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  TemporaryFilename(clone_info->filename);
  file=open(clone_info->filename,O_WRONLY | O_CREAT | O_EXCL | O_BINARY,0777);
  if (file == -1)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unable to convert blob to an image",
        clone_info->filename);
      return((Image *) NULL);
    }
  count=write(file,(const char *) blob,length);
  (void) close(file);
  if (count != length)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unable to convert blob to an image",
        clone_info->filename);
      return((Image *) NULL);
    }
  image=ReadImage(clone_info,&error);
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseBlob closes a file associated with the image.  If the
%  filename prefix is '|', the file is a pipe and is closed with PipeClose.
%
%  The format of the CloseBlob method is:
%
%      void CloseBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport void CloseBlob(Image *image)
{
  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  CloseImagePixels(image);
  image->taint=False;
  image->filesize=SizeBlob(image);
  if (image->blob.data != (char *) NULL)
    {
      image->blob.extent=image->blob.length;
      return;
    }
  if (image->file == (FILE *) NULL)
    return;
  image->status=ferror(image->file);
  errno=0;
  if (image->exempt)
    return;
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  if (image->pipe)
    (void) pclose(image->file);
  else
#endif
    (void) fclose(image->file);
  image->file=(FILE *) NULL;
  if (!image->orphan)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      for ( ; image != (Image *) NULL; image=image->next)
        image->file=(FILE *) NULL;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y B l o b I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyBlobInfo deallocates memory associated with an BlobInfo
%  structure.
%
%  The format of the DestroyBlobInfo method is:
%
%      void DestroyBlobInfo(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void DestroyBlobInfo(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  if (blob->mapped)
    (void) UnmapBlob(blob->data,blob->length);
  GetBlobInfo(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  E O F B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EOFBlob returns a non-zero value when EOF has been detected reading
%  from a blob or file.
%
%  The format of the EOFBlob method is:
%
%      int EOFBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method EOFBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport int EOFBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(feof(image->file));
  return(image->blob.offset > image->blob.length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetBlobInfo initializes the BlobInfo structure.
%
%  The format of the GetBlobInfo method is:
%
%      void GetBlobInfo(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void GetBlobInfo(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  blob->mapped=False;
  blob->data=(char *) NULL;
  blob->offset=0;
  blob->length=0;
  blob->extent=0;
  blob->quantum=BlobQuantum;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t S t r i n g B l o b                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetStringBlob reads characters from a blob or file until a newline
%  character is read or an end-of-file  condition is encountered.
%  from a blob or file.
%
%  The format of the GetStringBlob method is:
%
%      char *GetStringBlob(Image *image,char *string)
%
%  A description of each parameter follows:
%
%    o status:  Method GetStringBlob returns the string on success, otherwise,
%      a null is returned.
%
%    o image: The address of a structure of type Image.
%
%    o string: The address of a character buffer.
%
%
*/
MagickExport char *GetStringBlob(Image *image,char *string)
{
  int
    c;

  register int
    i;

  assert(image != (Image *) NULL);
  for (i=0; i < (MaxTextExtent-1); i++)
  {
    c=ReadByte(image);
    if (c == EOF)
      return((char *) NULL);
    string[i]=c;
    if ((string[i] == '\n') || (string[i] == '\r'))
      break;
  }
  string[i]='\0';
  return(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o B l o b                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageToBlob implements direct to memory image formats.  It returns
%  the image as a blob and its length.  The magick member of the Image
%  structure determines the format of the returned blob (GIG, JPEG, PNG, etc.).
%
%  The format of the ImageToBlob method is:
%
%      void *ImageToBlob(const ImageInfo *image_info,Image *image,
%        size_t *length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  Method ImageToBlob returns a chunk of memory written in the
%      desired image format (e.g. JPEG, GIF, etc.).  If an error occurs
%      NULL is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport void *ImageToBlob(const ImageInfo *image_info,Image *image,
  size_t *length,ExceptionInfo *exception)
{
  char
    *blob,
    filename[MaxTextExtent];

  ImageInfo
    *clone_info;

  int
    file;

  MagickInfo
    *magick_info;

  off_t
    count;

  struct stat
    attributes;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image != (Image *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  GetExceptionInfo(exception);
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->magick,image->magick);
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick);
  if (magick_info == (MagickInfo *) NULL)
     {
       ThrowException(exception,BlobWarning,"No delegate for this image format",
         clone_info->magick);
       return((void *) NULL);
     }
  if (magick_info->blob_support)
    {
      register Image
        *p;

      /*
        Native blob support for this image format.
      */
      *image->filename='\0';
      clone_info->blob.extent=Max((int) *length,image->blob.quantum);
      clone_info->blob.data=(char *) AllocateMemory(clone_info->blob.extent);
      if (clone_info->blob.data == (char *) NULL)
        {
          ThrowException(exception,BlobWarning,"Unable to create blob",
            "Memory allocation failed");
          return((void *) NULL);
        }
      clone_info->blob.offset=0;
      clone_info->blob.length=0;
      status=WriteImage(clone_info,image);
      if (status == False)
        {
          ThrowException(exception,BlobWarning,"Unable to create blob",
            clone_info->magick);
          DestroyImageInfo(clone_info);
          return((void *) NULL);
        }
      DestroyImageInfo(clone_info);
      for (p=image; p->next != (Image *) NULL; p++)
      {
        *length=p->blob.length;
        blob=p->blob.data;
        GetBlobInfo(&(p->blob));
      }
      return(blob);
    }
  /*
    Write file to disk in blob image format.
  */
  *length=0;
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(filename,image->filename);
  FormatString(image->filename,"%.1024s:%.1024s",image->magick,
    clone_info->unique);
  status=WriteImage(clone_info,image);
  if (status == False)
    {
      ThrowException(exception,BlobWarning,"Unable to create blob",
        image->filename);
      return((void *) NULL);
    }
  /*
    Read image from disk as blob.
  */
  file=open(image->filename,O_RDONLY | O_BINARY,0777);
  DestroyImageInfo(clone_info);
  if (file == -1)
    {
      (void) remove(image->filename);
      (void) strcpy(image->filename,filename);
      ThrowException(exception,BlobWarning,"Unable to create blob",
        image->filename);
      return((void *) NULL);
    }
  *length=fstat(file,&attributes) < 0 ? 0 : attributes.st_size;
  blob=(char *) AllocateMemory(*length);
  if (blob == (char *) NULL)
    {
      (void) remove(image->filename);
      (void) strcpy(image->filename,filename);
      ThrowException(exception,BlobWarning,"Unable to create blob",
        "Memory allocation failed");
      return((void *) NULL);
    }
  count=read(file,blob,*length);
  (void) close(file);
  (void) remove(image->filename);
  (void) strcpy(image->filename,filename);
  if (count != *length)
    {
      ThrowException(exception,BlobWarning,"Unable to read blob",(char *) NULL);
      return((void *) NULL);
    }
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  L S B F i r s t R e a d L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstReadLong reads a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstReadLong method is:
%
%      unsigned long LSBFirstReadLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method LSBFirstReadLong returns an unsigned long read from
%      the file.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned long LSBFirstReadLong(Image *image)
{
  unsigned char
    buffer[4];

  unsigned long
    value;

  assert(image != (Image *) NULL);
  value=ReadBlob(image,4,(char *) buffer);
  if (value == 0)
    return((unsigned long) ~0);
  value=buffer[3] << 24;
  value|=buffer[2] << 16;
  value|=buffer[1] << 8;
  value|=buffer[0];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  L S B F i r s t R e a d S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstReadShort reads a short value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstReadShort method is:
%
%      unsigned short LSBFirstReadShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method LSBFirstReadShort returns an unsigned short read from
%      the file.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned short LSBFirstReadShort(Image *image)
{
  unsigned char
    buffer[2];

  unsigned short
    value;

  assert(image != (Image *) NULL);
  value=ReadBlob(image,2,(char *) buffer);
  if (value == 0)
    return((unsigned short) ~0);
  value=buffer[1] << 8;
  value|=buffer[0];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  L S B F i r s t W r i t e L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstWriteLong writes a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstWriteLong method is:
%
%      size_t LSBFirstWriteLong(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count: Method LSBFirstWriteLong returns the number of unsigned longs
%      written.
%
%    o image: The address of a structure of type Image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t LSBFirstWriteLong(Image *image,const unsigned long value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  buffer[0]=value;
  buffer[1]=value >> 8;
  buffer[2]=value >> 16;
  buffer[3]=value >> 24;
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  L S B F i r s t W r i t e S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstWriteShort writes a long value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstWriteShort method is:
%
%      size_t LSBFirstWriteShort(Image *image,const unsigned int value)
%
%  A description of each parameter follows.
%
%    o count: Method LSBFirstWriteShort returns the number of unsigned longs
%      written.
%
%    o image: The address of a structure of type Image.
%
%    o value:  Specifies the value to write.
%
%
*/
MagickExport size_t LSBFirstWriteShort(Image *image,const unsigned int value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  buffer[0]=value;
  buffer[1]=(value) >> 8;
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M a p B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MapBlob creates a mapping from a file to a binary large object.
%
%  The format of the MapBlob method is:
%
%      void *MapBlob(int file,const MapMode mode,size_t *length)
%
%  A description of each parameter follows:
%
%    o status:  Method MapBlob returns the address of the blob as well as
%      its length in bytes.
%
%    o file: map this file descriptor.
%
%    o mode: ReadMode, WriteMode, or IOMode.
%
%    o length: the length of the mapping is returned in this pointer.
%
%
*/
MagickExport void *MapBlob(int file,const MapMode mode,size_t *length)
{
#if defined(HAVE_MMAP)
  struct stat
    attributes;

  void
    *map;

  /*
    Map file.
  */
  assert(length != (size_t *) NULL);
  if (file == -1)
    return((void *) NULL);
  if (fstat(file,&attributes) == -1)
    return((void *) NULL);
  *length=attributes.st_size;
  if (*length != attributes.st_size)
    return((void *) NULL);
  switch (mode)
  {
    case ReadMode:
    default:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_READ,MAP_PRIVATE,file,0);
      break;
    }
    case WriteMode:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_WRITE,MAP_SHARED,file,0);
      break;
    }
    case IOMode:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_READ | PROT_WRITE,
        MAP_SHARED,file,0);
      break;
    }
  }
  if (map == (void *) MAP_FAILED)
    return((void *) NULL);
  return((void *) map);
#else
  return((void *) NULL);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t O r d e r L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstOrderLong converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBFirstOrderLong method is:
%
%      void MSBFirstOrderLong(register char *p,const unsigned int length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBFirstOrderLong(register char *p,const size_t length)
{
  register char
    c,
    *q,
    *sp;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    sp=p+3;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    sp=p+1;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    p+=2;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t O r d e r S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstOrderShort converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBFirstOrderShort method is:
%
%      void MSBFirstOrderShort(register char *p,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBFirstOrderShort(register char *p,const size_t length)
{
  register char
    c,
    *q;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t R e a d S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstReadShort reads a short value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstReadShort method is:
%
%      unsigned short MSBFirstReadShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method MSBFirstReadShort returns an unsigned short read from
%      the file.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned short MSBFirstReadShort(Image *image)
{
  unsigned char
    buffer[2];

  unsigned short
    value;

  assert(image != (Image *) NULL);
  value=ReadBlob(image,2,(char *) buffer);
  if (value == 0)
    return((unsigned short) ~0);
  value=buffer[0] << 8;
  value|=buffer[1];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t R e a d L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstReadLong reads a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstReadLong method is:
%
%      unsigned long MSBFirstReadLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method MSBFirstReadLong returns an unsigned long read from
%      the file.
%
%    o image: The address of a structure of type Image.
%
%
%
*/
MagickExport unsigned long MSBFirstReadLong(Image *image)
{
  unsigned char
    buffer[4];

  unsigned long
    value;

  assert(image != (Image *) NULL);
  value=ReadBlob(image,4,(char *) buffer);
  if (value == 0)
    return((unsigned long) ~0);
  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t W r i t e L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstWriteLong writes a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstWriteLong method is:
%
%      size_t MSBFirstWriteLong(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count: Method MSBFirstWriteLong returns the number of unsigned longs
%      written.
%
%    o value:  Specifies the value to write.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport size_t MSBFirstWriteLong(Image *image,const unsigned long value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  buffer[0]=value >> 24;
  buffer[1]=value >> 16;
  buffer[2]=value >> 8;
  buffer[3]=value;
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B F i r s t W r i t e S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstWriteShort writes a long value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstWriteShort method is:
%
%      size_t MSBFirstWriteShort(Image *image,const unsigned short value)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
MagickExport size_t MSBFirstWriteShort(Image *image,const unsigned int value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  buffer[0]=(value) >> 8;
  buffer[1]=value;
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenBlob opens a file associated with the image.  A file name of
%  '-' sets the file to stdin for type 'r' and stdout for type 'w'.  If the
%  filename suffix is '.gz' or '.Z', the image is decompressed for type 'r'
%  and compressed for type 'w'.  If the filename prefix is '|', it is piped
%  to or from a system command.
%
%  The format of the OpenBlob method is:
%
%      unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
%        const char *type)
%
%  A description of each parameter follows:
%
%    o status:  Method OpenBlob returns True if the file is successfully
%      opened otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o type: 'r' for reading; 'w' for writing.
%
*/
MagickExport unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
  const char *type)
{
  char
    filename[MaxTextExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(image != (Image *) NULL);
  assert(type != (char *) NULL);
  if (image_info->blob.data != (char *) NULL)
    {
      image->blob=image_info->blob;
      return(True);
    }
  GetBlobInfo(&image->blob);
  image->exempt=False;
  if (image_info->file != (FILE *) NULL)
    {
      /*
        Use previously opened filehandle.
      */
      image->file=image_info->file;
      image->exempt=True;
      return(True);
    }
  (void) strcpy(filename,image->filename);
  p=(char *) NULL;
  if (*filename != '|')
    {
      if ((Extent(filename) > 4) &&
          (LocaleCompare(filename+Extent(filename)-4,".pgp") == 0))
        {
          /*
            Decrypt image file with PGP encryption utilities.
          */
          if (*type == 'r')
            p=GetDelegateCommand(image_info,image,"pgp",(char *) NULL);
        }
      else
        if ((Extent(filename) > 4) &&
            (LocaleCompare(filename+Extent(filename)-4,".bz2") == 0))
          {
            /*
              Uncompress/compress image file with BZIP compress utilities.
            */
            if (*type == 'r')
              p=GetDelegateCommand(image_info,image,"bzip",(char *) NULL);
            else
              p=GetDelegateCommand(image_info,image,(char *) NULL,"bzip");
          }
        else
          if ((Extent(filename) > 3) &&
              (LocaleCompare(filename+Extent(filename)-3,".gz") == 0))
            {
              /*
                Uncompress/compress image file with GNU compress utilities.
              */
              if (*type == 'r')
                p=GetDelegateCommand(image_info,image,"zip",(char *) NULL);
              else
                p=GetDelegateCommand(image_info,image,(char *) NULL,"zip");
            }
          else
            if ((Extent(filename) > 2) &&
                (LocaleCompare(filename+Extent(filename)-2,".Z") == 0))
              {
                /*
                  Uncompress/compress image file with UNIX compress utilities.
                */
                if (*type == 'r')
                  p=GetDelegateCommand(image_info,image,"compress",
                    (char *) NULL);
                else
                  p=GetDelegateCommand(image_info,image,(char *) NULL,
                    "compress");
              }
    }
  if (p != (char *) NULL)
    {
      char
        *command;

      (void) strcpy(filename,p);
      command=p;
      FreeMemory((void **) &command);
    }
  /*
    Open image file.
  */
  image->pipe=False;
  if (LocaleCompare(filename,"-") == 0)
    {
      image->file=(*type == 'r') ? stdin : stdout;
#if defined(_VISUALC_)
      if (strchr(type,'b') != (char *) NULL)
        _setmode(_fileno(image->file),_O_BINARY);
#endif
      image->exempt=True;
    }
  else
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
    if (*filename == '|')
      {
        char
          mode[MaxTextExtent];

        /*
          Pipe image to or from a system command.
        */
        if (*type == 'w')
          (void) signal(SIGPIPE,SIG_IGN);
        (void) strncpy(mode,type,1);
        mode[1]='\0';
        image->file=(FILE *) popen(filename+1,mode);
        image->pipe=True;
        image->exempt=True;
      }
    else
#endif
      {
        if (*type == 'w')
          {
            /*
              Form filename for multi-part images.
            */
            FormatString(filename,image->filename,image->scene);
            if (!image_info->adjoin)
              if ((image->previous != (Image *) NULL) ||
                  (image->next != (Image *) NULL))
                {
                  if ((LocaleCompare(filename,image->filename) == 0) ||
                      (strchr(filename,'%') != (char *) NULL))
                    FormatString(filename,"%.1024s.%u",image->filename,
                      image->scene);
                  if (image->next != (Image *) NULL)
                    (void) strcpy(image->next->magick,image->magick);
                }
            (void) strcpy(image->filename,filename);
          }
#if defined(macintosh)
        if (*type == 'w')
          SetApplicationType(filename,image_info->magick,'8BIM');
#endif
        image->file=(FILE *) fopen(filename,type);
        if ((image->file != (FILE *) NULL) && (*type == 'r'))
          {
            MagickInfo
              *magick_info;

            magick_info=(MagickInfo *) GetMagickInfo(image_info->magick);
            if (magick_info != (MagickInfo *) NULL)
              {
                if (magick_info->blob_support)
                  {
                    /*
                      Format supports blobs-- try memory-mapped I/O.
                    */
                    image->blob.length=0;
                    image->blob.data=(char *) MapBlob(fileno(image->file),
                      ReadMode,&image->blob.length);
                    image->blob.mapped=
                      image->blob.data != (void *) NULL;
                    if (image->blob.mapped)
                      {
                        (void) fclose(image->file);
                        image->file=(FILE *) NULL;
                      }
                  }
              }
          }
        image->filesize=SizeBlob(image);
      }
  image->status=False;
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return((image->file != (FILE *) NULL) || (image->blob.data != NULL));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlob reads data from the blob or image file and returns it.  It
%  returns the number of bytes read.
%
%  The format of the ReadBlob method is:
%
%      size_t ReadBlob(Image *image,const size_t length,char *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlob returns the number of bytes read.
%
%    o image: The address of a structure of type Image.
%
%    o length:  Specifies an integer representing the number of bytes
%      to read from the file.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
MagickExport size_t ReadBlob(Image *image,const size_t length,void *data)
{
  off_t
    count;

  assert(image != (Image *) NULL);
  assert(data != (char *) NULL);
  if (image->blob.data != (char *) NULL)
    {
      /*
        Read bytes from blob.
      */
      count=Min(length,image->blob.length-image->blob.offset);
      if (count > 0)
        memcpy(data,image->blob.data+image->blob.offset,(size_t) count);
      image->blob.offset+=count;
      return(count);
    }
  /*
    Read bytes from a file.
  */
  count=fread(data,1,length,image->file);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B y t e                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadByte reads a single byte from the image file and returns it.
%
%  The format of the ReadByte method is:
%
%      int ReadByte(Image *image)
%
%  A description of each parameter follows.
%
%    o value: Method ReadByte returns an integer read from the file.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport int ReadByte(Image *image)
{
  int
    count;

  unsigned char
    value;

  assert(image != (Image *) NULL);
  count=ReadBlob(image,1,(char *) &value);
  if (count == 0)
    return(EOF);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d D a t a B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  The number
%  or bytes read is returned.
%
%  The format of the ReadBlobBlock method is:
%
%      size_t ReadBlobBlock(Image *image,char *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlobBlock returns the number of bytes read.
%
%    o image: The address of a structure of type Image.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
MagickExport size_t ReadBlobBlock(Image *image,char *data)
{
  size_t
    count;

  unsigned char
    block_count;

  assert(image != (Image *) NULL);
  assert(data != (char *) NULL);
  count=ReadBlob(image,1,(char *) &block_count);
  if (count == 0)
    return(0);
  count=ReadBlob(image,(size_t) block_count,data);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S e e k B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SeekBlob sets the offset in bytes from the beginning of a blob or
%  file and returns the resulting offset.
%
%  The format of the SeekBlob method is:
%
%      off_t SeekBlob(Image *image,const off_t offset,const int whence)
%
%  A description of each parameter follows:
%
%    o offset:  Method SeekBlob returns the offset from the beginning
%      of the file or blob.
%
%    o image: The address of a structure of type Image.
%
%    o offset:  Specifies an integer representing the offset in bytes.
%
%    o whence:  Specifies an integer representing how the offset is
%      treated relative to the beginning of the blob as follows:
%
%        SEEK_SET  Set position equal to offset bytes.
%        SEEK_CUR  Set position to current location plus offset.
%        SEEK_END  Set position to EOF plus offset.
%
%
*/
MagickExport off_t SeekBlob(Image *image,const off_t offset,const int whence)
{
  int
    status;

  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    {
      status=fseek(image->file,offset,whence);
      if (status == -1)
        return(status);
      return(TellBlob(image));
    }
  switch (whence)
  {
    case SEEK_SET:
    default:
    {
      if (offset < 0)
        return(-1);
      image->blob.offset=offset;
      break;
    }
    case SEEK_CUR:
    {
      if ((image->blob.offset+offset) < 0)
        return(-1);
      image->blob.offset+=offset;
      break;
    }
    case SEEK_END:
    {
      if ((off_t) (image->blob.offset+image->blob.length+offset) < 0)
        return(-1);
      image->blob.offset=image->blob.length+offset;
      break;
    }
  }
  if (image->blob.offset > image->blob.length)
    {
      image->blob.length=image->blob.offset;
      image->blob.data=(char *)
        ReallocateMemory(image->blob.data,image->blob.length);
      if (image->blob.data == (char *) NULL)
        {
          image->blob.length=0;
          return(-1);
        }
      image->blob.extent=image->blob.length;
    }
  return(image->blob.offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S e t B l o b Q u a n t u m                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetBlobQuantum set the current value of the blob quantum.  This
%  is the size in bytes to add to a blob when writing to a blob exceeds its
%  current length.
%
%  The format of the SetBlobQuantum method is:
%
%      void SetBlobQuantum(BlobInfo *blob,const size_t quantum)
%
%  A description of each parameter follows:
%
%    o blob:  A pointer to a BlobInfo structure.
%
%    o quantum: A size_t that reflects the number of bytes to increase a blob.
%
%
*/
MagickExport void SetBlobQuantum(BlobInfo *blob,const size_t quantum)
{
  assert(blob != (BlobInfo *) NULL);
  blob->quantum=quantum;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S i z e B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SizeBlob returns the current length of the image file or blob.
%
%  The format of the SizeBlob method is:
%
%      off_t SizeBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o size:  Method SizeBlob returns the current length of the image file
%      or blob.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport off_t SizeBlob(Image *image)
{
  struct stat
    attributes;

  assert(image != (Image *) NULL);
  if (image->file == (FILE *) NULL)
    return(image->blob.length);
  SyncBlob(image);
  return(fstat(fileno(image->file),&attributes) < 0 ? 0 : attributes.st_size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S y n c B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncBlob flushes the datastream if it is a file.
%
%  The format of the SyncBlob method is:
%
%      int SyncBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method SyncBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport int SyncBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  if (image->file != (FILE *) NULL)
    return(fflush(image->file));
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  T e l l B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TellBlob obtains the current value of the blob or file position.
%
%  The format of the TellBlob method is:
%
%      off_t TellBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o offset:  Method TellBlob returns the current value of the blob or
%      file position success; otherwise, it returns -1 and sets errno to
%      indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport off_t TellBlob(Image *image)
{
  assert(image != (Image *) NULL);
  if (image->file != (FILE *) NULL)
    return(ftell(image->file));
  return(image->blob.offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  U n m a p B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnmapBlob deallocates the binary large object previously allocated
%  with the MapBlob method.
%
%  The format of the UnmapBlob method is:
%
%      unsigned int UnmapBlob(void *map,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method UnmapBlob returns True on success; otherwise,  it
%      returns False and sets errno to indicate the error.
%
%    o map: The address  of the binary large object.
%
%    o length: The length of the binary large object.
%
%
*/
MagickExport unsigned int UnmapBlob(void *map,const size_t length)
{
#if defined(HAVE_MMAP)
  int
    status;

  status=munmap(map,length);
  return(status == 0);
#else
  return(False);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlob writes data to a blob or image file.  It returns the
%  number of bytes written.
%
%  The format of the WriteBlob method is:
%
%      size_t WriteBlob(Image *image,const size_t length,const void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method WriteBlob returns the number of bytes written to the
%      blob.
%
%    o image: The address of a structure of type Image.
%
%    o length:  Specifies an integer representing the number of bytes to
%      write to the file.
%
%    o data:  The address of the data to write to the blob or file.
%
%
*/
MagickExport size_t WriteBlob(Image *image,const size_t length,const void *data)
{
  off_t
    count;

  assert(image != (Image *) NULL);
  assert(data != (const char *) NULL);
  if (image->blob.data == (char *) NULL)
    {
      count=fwrite((char *) data,1,length,image->file);
      return(count);
    }
  if (length > (image->blob.extent-image->blob.offset))
    {
      image->blob.extent+=length+image->blob.quantum;
      image->blob.data=(char *)
        ReallocateMemory(image->blob.data,image->blob.extent);
      if (image->blob.data == (char *) NULL)
        {
          image->blob.extent=0;
          return(0);
        }
    }
  memcpy(image->blob.data+image->blob.offset,data,length);
  image->blob.offset+=length;
  if (image->blob.offset > image->blob.length)
    image->blob.length=image->blob.offset;
  return(length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B y t e                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteByte write an integer to a file.  It returns the number of
%  bytes written (either 0 or 1);
%
%  The format of the WriteByte method is:
%
%      size_t WriteByte(Image *image,const int value)
%
%  A description of each parameter follows.
%
%    o count:  Method WriteByte returns the number of bytes written.
%
%    o image: The address of a structure of type Image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteByte(Image *image,const int value)
{
  char
    c;

  size_t
    count;

  assert(image != (Image *) NULL);
  c=value;
  count=WriteBlob(image,1,&c);
  return(count);
}
