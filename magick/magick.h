/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  System include declarations.
*/
#define _XOPEN_SOURCE  500
#define _POSIX_C_SOURCE  199506L

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if !defined(_VISUALC_)
#  include <unistd.h>
#else
#  include <direct.h>
#  define HAVE_STRERROR
#if defined(_DEBUG)
#  include <crtdbg.h>
/*
  _ASSERTE(size != 6840);
  int tmpDbgFlag;
  tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
  tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
  _CrtSetDbgFlag(tmpDbgFlag);
  _ASSERTE(_CrtCheckMemory());
 */
#endif
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define fseek  fseeko
#define ftell  ftello
#endif

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#if !defined(vms) && !defined(macintosh)
#  include <sys/types.h>
#  include <sys/stat.h>
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#  if HAVE_SYS_NDIR_H || HAVE_SYS_DIR_H || HAVE_NDIR_H
#  define dirent direct
#  define NAMLEN(dirent) (dirent)->d_namlen
#  if HAVE_SYS_NDIR_H
#   include <sys/ndir.h>
#  endif
#  if HAVE_SYS_DIR_H
#   include <sys/dir.h>
#  endif
#  if HAVE_NDIR_H
#   include <ndir.h>
#  endif
#  else
#  include <dirent.h>
#  define NAMLEN(dirent) Extent((dirent)->d_name)
#  endif
#  include <pwd.h>
#endif
#if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#  include "magick/api.h"
#if !defined(WIN32)
#  include <sys/time.h>
#  include <sys/times.h>
#endif
#else
#  include <types.h>
#  include <stat.h>
#if defined(macintosh)
#  include <SIOUX.h>
#  include <console.h>
#  include <unix.h>
#endif
#  include "api.h"
#endif

#if defined(HAVE_MMAP) && !defined(_VISUALC_)
#  include <sys/mman.h>
#endif
#if defined(HasPTHREADS)
#  include <pthread.h>
#endif
#if defined(HAVE_POLL)
#  include <sys/poll.h>
#endif
#if defined(WIN32)
#  include "nt.h"
#endif
#if defined(macintosh)
#  include "mac.h"
#endif
#if defined(vms)
#  include "vms.h"
#endif

#undef index
#undef pipe

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
