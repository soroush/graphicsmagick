/*
  Magic methods.
*/
#ifndef _MAGICK_MAGIC_H
#define _MAGICK_MAGIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declarations.
*/
typedef struct _MagicInfo
{  
  const char
    *path,
    *name,
    *target;

  unsigned char
    *magic;

  unsigned long
    length,
    offset;

  unsigned int
    stealth;

  unsigned long
    signature;

  volatile struct _MagicInfo
    *previous,
    *next;
} MagicInfo;

/*
  Method declarations.
*/
extern MagickExport const MagicInfo
  *GetMagicInfo(const unsigned char *,const size_t,ExceptionInfo *);

extern MagickExport unsigned int
  ListMagicInfo(FILE *,ExceptionInfo *);

extern MagickExport void
  DestroyMagicInfo(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
