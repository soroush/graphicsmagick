/*
  Windows NT Utility Methods for ImageMagick.
*/
#ifndef _MAGICK_NT_H
#define _MAGICK_NT_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <io.h>
#include <errno.h>
#if defined(_DEBUG)
#include <crtdbg.h>
#endif

/*
  Define declarations.
*/
#define PROT_READ  1
#define PROT_WRITE  2
#define PROT_READWRITE  3
#define MAP_SHARED  1
#define MAP_PRIVATE  2
#define F_OK 0
#define R_OK 4
#define W_OK 2
#define RW_OK 6
#if !defined(HasLTDL)
#  define lt_dlerror NTGetLastError
#endif
#if defined(_MT) && defined(WIN32)
#define SAFE_GLOBAL __declspec(thread)
#else
#define SAFE_GLOBAL
#endif
/*
  Typedef declarations.
*/
typedef UINT (CALLBACK *LPFNDLLFUNC1)(DWORD,UINT);

#if !defined(XS_VERSION)
struct dirent
{
  char
    d_name[2048];

  int
    d_namlen;
};

typedef struct _DIR
{
  HANDLE
    hSearch;

  WIN32_FIND_DATA
    Win32FindData;

  BOOL
    firsttime;

  struct dirent
    file_info;
} DIR;

#endif

/*
  NT utilities routines.
*/
extern MagickExport char
  *NTGetExecutionPath(void),
  *NTGetLastError(void);

extern MagickExport int
  Exit(int),
  IsWindows95(),
  NTSystemCommand(const char *);

#if !defined(XS_VERSION)
extern MagickExport DIR
  *opendir(char *);

extern MagickExport double
  NTElapsedTime(void),
  NTUserTime(void);

extern MagickExport int
#if !defined(HasLTDL)
  lt_dlexit(void),
  lt_dlinit(void),
#endif
  munmap(void *,size_t);

extern MagickExport long
  telldir(DIR *);

extern MagickExport struct dirent
  *readdir(DIR *);

extern MagickExport unsigned char
  *NTResourceToBlob(const char *);

extern MagickExport unsigned int
  NTIsMagickConflict(const char *);

extern MagickExport void
  closedir(DIR *),
  DebugString(char *, ...),
  DestroyTracingCriticalSection(void),
  InitializeTracingCriticalSection(void),
  *mmap(char *,size_t,int,int,int,off_t),
  NTErrorHandler(const ExceptionType,const char *,const char *),
  NTWarningHandler(const ExceptionType,const char *,const char *),
  seekdir(DIR *,long)
#if !defined(HasLTDL)
  ,
  lt_dlsetsearchpath(char *),
  *lt_dlopen(char *),
  lt_dlclose(void *),
  *lt_dlsym(void *, char *)
#endif
  ;

#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
