/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

/*
  System include declarations.
*/
#define __EXTENSIONS__  1
#define _POSIX_C_SOURCE  199506L
#define _XOPEN_SOURCE  500
#ifndef __NetBSD__
#define _XOPEN_SOURCE_EXTENDED  1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define fseek  fseeko
#define ftell  ftello
#endif

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/time.h>
#  include <sys/times.h>
#if defined(HAVE_CONFIG_H)
#  include "magick/config.h"
#endif
#if defined(HAVE_VARARGS_H)
#  include <varargs.h>
#endif
#if defined(HAVE_MMAP)
#  include <sys/mman.h>
#endif
#if defined(HasPTHREADS)
#  include <pthread.h>
#endif
#if defined(sysv)
#  include <sys/poll.h>
#endif
#  include "magick/api.h"
#else
#if defined(WIN32)
#  include <sys/types.h>
#  include <sys/stat.h>
#else
#  include <types.h>
#  include <stat.h>
#endif
#if defined(macintosh)
#  include <SIOUX.h>
#  include <console.h>
#  include <unix.h>
#endif
#  include "api.h"
#endif
#include <fcntl.h>
#if defined(_VISUALC_)
#  include <direct.h>
#  define HAVE_STRERROR
#else
#  include <unistd.h>
#endif

/*
  ImageMagick API headers
*/
#if defined(macintosh)
#define HasJPEG
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasZLIB
#define HAVE_STRERROR
#endif

#if defined(VMS)
#define inline __inline
#define HasJPEG
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasX11
#define HasZLIB
#endif

#if defined(WIN32)
#define inline __inline
#define HasJBIG
#define HasJPEG
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasX11
#define HasZLIB
#define HasBZLIB
#define HasHDF
#define HasFPX
#define HAVE_MMAP
#define HasXML
#define HasLCMS
#endif

#define MagickSignature  0xabacadab

#undef index

#endif
