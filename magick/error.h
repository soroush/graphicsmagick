/*
  ImageMagick Exception Methods.
*/
#ifndef _MAGICK_ERROR_H
#define _MAGICK_ERROR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Exception define definitions.
*/
#if defined(macintosh)
#define ExceptionInfo MagickExceptionInfo
#endif

#define ThrowBinaryException(code,reason,description) \
{ \
  if (image != (Image *) NULL) \
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
  if ((image) == (Image *) NULL) \
    ThrowException(exception,code,reason,(char *) NULL); \
  else \
    { \
      ThrowException(exception,code,reason,(image)->filename); \
      if (image->blob->type != UndefinedStream) \
        CloseBlob(image); \
      DestroyImageList(image); \
    } \
  return((Image *) NULL); \
}
#define ThrowWriterException(code,reason,image) \
{ \
  assert(image != (Image *) NULL); \
  ThrowException(&(image)->exception,code,reason,(image)->filename); \
  if (image_info->adjoin) \
    while ((image)->previous != (Image *) NULL) \
      (image)=(image)->previous; \
  CloseBlob(image); \
  return(False); \
}

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedException,
  WarningException = 300,
  ResourceLimitWarning = 300,
  TypeWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  MissingDelegateWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330,
  BlobWarning = 335,
  StreamWarning = 340,
  CacheWarning = 345,
  CoderWarning = 350,
  ModuleWarning = 355,
  DrawWarning = 360,
  ImageWarning = 365,
  XServerWarning = 380,
  MonitorWarning = 385,
  RegistryWarning = 390,
  ConfigureWarning = 395,
  ErrorException = 400,
  ResourceLimitError = 400,
  TypeError = 405,
  OptionError = 410,
  DelegateError = 415,
  MissingDelegateError = 420,
  CorruptImageError = 425,
  FileOpenError = 430,
  BlobError = 435,
  StreamError = 440,
  CacheError = 445,
  CoderError = 450,
  ModuleError = 455,
  DrawError = 460,
  ImageError = 465,
  XServerError = 480,
  MonitorError = 485,
  RegistryError = 490,
  ConfigureError = 495,
  FatalErrorException = 700,
  ResourceLimitFatalError = 700,
  TypeFatalError = 705,
  OptionFatalError = 710,
  DelegateFatalError = 715,
  MissingDelegateFatalError = 720,
  CorruptImageFatalError = 725,
  FileOpenFatalError = 730,
  BlobFatalError = 735,
  StreamFatalError = 740,
  CacheFatalError = 745,
  CoderFatalError = 750,
  ModuleFatalError = 755,
  DrawFatalError = 760,
  ImageFatalError = 765,
  XServerFatalError = 780,
  MonitorFatalError = 785,
  RegistryFatalError = 790,
  ConfigureFatalError = 795
} ExceptionType;

/*
  Typedef declarations.
*/
typedef struct _ExceptionInfo
{
  char
    *reason,
    *description;

  ExceptionType
    severity;

  unsigned long
    signature;
} ExceptionInfo;

/*
  Exception typedef declarations.
*/
typedef void
  (*ErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*FatalErrorHandler)(const ExceptionType,const char *,const char *);

typedef void
  (*WarningHandler)(const ExceptionType,const char *,const char *);

/*
  Exception declarations.
*/
extern MagickExport const char
  *GetLocaleExceptionMessage(const ExceptionType,const char *),
  *GetLocaleMessage(const char *);

extern MagickExport ErrorHandler
  SetErrorHandler(ErrorHandler);

extern MagickExport FatalErrorHandler
  SetFatalErrorHandler(FatalErrorHandler);

extern MagickExport void
  CatchException(const ExceptionInfo *),
  DestroyExceptionInfo(ExceptionInfo *),
  GetExceptionInfo(ExceptionInfo *),
  MagickError(const ExceptionType,const char *,const char *),
  MagickFatalError(const ExceptionType,const char *,const char *),
  MagickWarning(const ExceptionType,const char *,const char *),
  SetExceptionInfo(ExceptionInfo *,ExceptionType),
  ThrowException(ExceptionInfo *,const ExceptionType,const char *,const char *);

extern MagickExport WarningHandler
  SetWarningHandler(WarningHandler);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
