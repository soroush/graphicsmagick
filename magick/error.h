/*
  ImageMagick Exception Methods.
*/
#ifndef _ERROR_H
#define _ERROR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Exception define definitions.
*/
#if defined(sun) && !defined(SVR4)
#if !defined(strerror)
#define strerror(n) \
  (((n) >= 0 && (n) < sys_nerr) ? sys_errlist[n] : "unknown error")

extern char
  *sys_errlist[];

extern int
  sys_nerr;
#endif
#endif
#define ThrowException(exception,code,reason,description) \
{ \
  if (code >= (exception)->severity) \
    { \
      (exception)->severity=code; \
      CloneString(&(exception)->message,reason); \
      CloneString(&(exception)->qualifier,description); \
    } \
}
#define ThrowBinaryException(code,reason,description) \
{ \
  ThrowException(&image->exception,code,reason,description); \
  return(False); \
}
#define ThrowImageException(code,reason,description) \
{ \
  ThrowException(exception,code,reason,description); \
  return((Image *) NULL); \
}
#define ThrowReaderException(code,reason,image) \
{ \
  ThrowException(exception,code,reason,image->filename); \
  DestroyImages(image); \
  return((Image *) NULL); \
}
#define ThrowWriterException(code,reason,image) \
{ \
  ThrowException(&image->exception,code,reason,image->filename); \
  if (image_info->adjoin) \
    while (image->previous != (Image *) NULL) \
      image=image->previous; \
  CloseBlob(image); \
  return(False); \
}

/*
  Exception typedef declarations.
*/
typedef void
  (*ErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*WarningHandler)(const ExceptionType,const char *,const char *);

/*
  Exception declarations.
*/
extern Export ErrorHandler
  SetErrorHandler(ErrorHandler);

extern Export void
  CatchImageException(Image *),
  DestroyExceptionInfo(ExceptionInfo *),
  GetImageException(Image *,ExceptionInfo *),
  GetExceptionInfo(ExceptionInfo *),
  MagickError(const ExceptionType,const char *,const char *),
  MagickWarning(const ExceptionType,const char *,const char *);

extern Export WarningHandler
  SetWarningHandler(WarningHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
