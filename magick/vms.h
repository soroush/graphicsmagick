/*
  VMS Utility Methods for ImageMagick.
*/
#ifndef _VMS_H
#define _VMS_H

#if defined(__DECC) || defined(__DECCXX)
#  include <lib$routines.h>
#  include <starlet.h>
#endif

#include <errno.h>
#include <descrip.h>
#include <rmsdef.h>
#include <ctype.h> 
#include <dvidef.h>
#include <dcdef.h>
#include <ssdef.h>

#pragma __member_alignment __save
typedef struct _ile3
{
#pragma __nomember_alignment
  unsigned short int
    ile3$w_length,
    ile3$w_code;

  void
    *ile3$ps_bufaddr;

  unsigned short int
    *ile3$ps_retlen_addr;
} ile3;
#pragma __member_alignment __restore


#if defined(__VMS_VER) && (__VMS_VER >= 70000000)
#include <dirent.h>
#else

/*
  Typedef declarations.
*/
struct dirent
{
  char
     d_name[255];

  int
    d_namlen;
};

typedef struct _dirdesc
{
  long
    context;

  char
    *pattern;

  struct dirent
    entry;

  struct dsc$descriptor_s
    pat;
} DIR;

/*
  VMS utilities routines.
*/
extern DIR
  *opendir(char *);

extern struct dirent
  *readdir(DIR *);

extern void
  closedir(DIR *);
#endif /*  defined(__VMS_VER) && (__VMS_VER >= 70000000) */

extern MagickExport int
  ImageFormatConflict(char *);

#endif
