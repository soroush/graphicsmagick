#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#if HAVE_SYS_NDIR_H || HAVE_SYS_DIR_H || HAVE_NDIR_H
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#else
# include <dirent.h>
# define NAMLEN(dirent) Extent((dirent)->d_name)
#endif
#include <pwd.h>
#else
#if defined(vms)
#include "vms.h"
#endif
#if defined(macintosh)
#include "mac.h"
#endif
#if defined(WIN32)
#include "nt.h"
#endif
#endif

#if !defined(S_ISDIR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

/*
  Utility define declarations.
*/
#if !defined(vms)
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL) || \
   (strchr(text,'[') != (char *) NULL) || \
   (strchr(text,']') != (char *) NULL))
#else
#define IsGlob(text) \
  ((strchr(text,'*') != (char *) NULL) || \
   (strchr(text,'?') != (char *) NULL) || \
   (strchr(text,'{') != (char *) NULL) || \
   (strchr(text,'}') != (char *) NULL))
#endif
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define IsBasenameSeparator(c)  ((c) == '/')
#define DirectorySeparator  "/"
#define TemporaryTemplate  "magick"
#else
#if defined(vms)
#define IsBasenameSeparator(c)  ((c) == ']')
#define DirectorySeparator  ""
#endif
#if defined(macintosh)
#define IsBasenameSeparator(c)  ((c) == ':')
#define DirectorySeparator  ":"
#endif
#if defined(WIN32)
#define IsBasenameSeparator(c)  (((c) == '/') || ((c) == '\\'))
#define DirectorySeparator  "\\"
#endif
#endif

/*
  Utilities methods.
*/
extern Export char
  *BaseFilename(const char *),
  **ListColors(const char *,int *),
  **ListFiles(char *,const char *,int *),
  *PostscriptGeometry(const char *),
  *SetClientName(const char *),
  **StringToArgv(const char *,int *),
  **StringToList(char *),
  *TranslateText(const ImageInfo *,Image *,char *);

extern Export int
  GlobExpression(char *,const char *),
  Latin1Compare(const char *,const char *),
  MultilineCensus(const char *),
  ReadDataBlock(char *,FILE *),
  SystemCommand(unsigned int,char *);

extern Export unsigned int
  ExpandFilenames(int *,char ***),
  IsAccessible(const char *),
  IsDirectory(const char *),
  ReadData(char *,const unsigned int,const unsigned int,FILE *);

extern Export unsigned long
  LSBFirstReadLong(FILE *),
  MSBFirstReadLong(FILE *);

extern Export unsigned short
  *ConvertTextToUnicode(char *,unsigned int *),
  LSBFirstReadShort(FILE *),
  MSBFirstReadShort(FILE *);

extern Export void
  AppendImageFormat(const char *,char *),
  CloneString(char **,const char *),
  DestroyPostscriptGeometry(char *),
  ExpandFilename(char *),
  FormatString(char *,const char *,...),
  Latin1Upper(char *),
  LocaleFilename(char *),
  LSBFirstWriteLong(const unsigned long,FILE *),
  LSBFirstWriteShort(const unsigned int,FILE *),
  MSBFirstOrderLong(char *,const unsigned int),
  MSBFirstOrderShort(char *,const unsigned int),
  MSBFirstWriteLong(const unsigned long,FILE *),
  MSBFirstWriteShort(const unsigned int,FILE *),
  Strip(char *),
  TemporaryFilename(char *);
