/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Magic Methods                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "magick.h"
#include "defines.h"

/*
  Static declarations.
*/
static MagicInfo
  *magic_list = (MagicInfo *) NULL;

static SemaphoreInfo
  *magic_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadConfigurationFile(const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagicInfo deallocates memory associated with the magic list.
%
%  The format of the DestroyMagicInfo method is:
%
%      DestroyMagicInfo(void)
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport void DestroyMagicInfo(void)
{
  register MagicInfo
    *p;

  AcquireSemaphore(&magic_semaphore);
  for (p=magic_list; p != (MagicInfo *) NULL; )
  {
    if (p->name != (char *) NULL)
      LiberateMemory((void **) &p->name);
    if (p->target != (char *) NULL)
      LiberateMemory((void **) &p->target);
    if (p->magic != (unsigned char *) NULL)
      LiberateMemory((void **) &p->magic);
    magic_list=p;
    p=p->next;
    LiberateMemory((void **) &magic_list);
  }
  magic_list=(MagicInfo *) NULL;
  LiberateSemaphore(&magic_semaphore);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagicInfo searches the magic list for the specified name and if
%  found returns attributes for that magic.
%
%  The format of the GetMagicInfo method is:
%
%      MagicInfo *GetMagicInfo(const unsigned char *magic,
%        const unsigned int length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o magic_info: Method GetMagicInfo searches the magic list for any image
%      format tag that matches the specified image signature and if found
%      returns attributes for that image format.
%
%    o image: The address of a structure of type Image.
%
%    o magic: A binary string generally representing the first few characters
%      of the image file or blob.
%
%    o length: The length of the binary signature.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport MagicInfo *GetMagicInfo(const unsigned char *magic,
  const unsigned int length,ExceptionInfo *exception)
{
  register MagicInfo
    *p;

  AcquireSemaphore(&magic_semaphore);
  if (magic_list == (MagicInfo *) NULL)
    {
      unsigned int
        status;

      /*
        Read magic list.
      */
      status=ReadConfigurationFile("magic.mgk");
      if (status == False)
        ThrowException(exception,FileOpenWarning,
          "Unable to read magic configuration file","magic.mgk");
      atexit(DestroyMagicInfo);
    }
  LiberateSemaphore(&magic_semaphore);
  if (LocaleCompare(magic,"*") == 0)
    return(magic_list);
  /*
    Search for requested magic.
  */
  for (p=magic_list; p != (MagicInfo *) NULL; p=p->next)
    if (memcmp(magic+p->offset,p->magic,p->length) == 0)
      break;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagicInfo lists the magic info to a file.
%
%  The format of the ListMagicInfo method is:
%
%      unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
{
  register MagicInfo
    *p;

  register int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"\n\nImageMagick understands these magic strings:\n\n");
  (void) fprintf(file,"Name      Offset Target\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  p=GetMagicInfo("*",0,exception);
  if (p == (MagicInfo *) NULL)
    return(False);
  for (p=magic_list; p != (MagicInfo *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=Extent(p->name); i <= 9; i++)
      (void) fprintf(file," ");
    (void) fprintf(file,"%6d ",p->offset);
    if (p->target != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->target);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r a t i o n F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the magic configuration file which maps
%  magic strings with a particular image format.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *filename)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one magic
%      is defined otherwise False.
%
%    o filename:  The magic configuration filename.
%
%
*/
static unsigned int ReadConfigurationFile(const char *filename)
{
  char
    keyword[MaxTextExtent],
    *path,
    value[MaxTextExtent];

  FILE
    *file;

  int
    c;

  register char
    *p;

  /*
    Read the magic configuration file.
  */
  path=GetMagickConfigurePath(filename);
  if (path == (char *) NULL)
    return(False);
  file=fopen(path,"r");
  LiberateMemory((void **) &path);
  if (file == (FILE *) NULL)
    return(False);
  for (c=fgetc(file); c != EOF; c=fgetc(file))
  {
    /*
      Parse keyword.
    */
    while (isspace(c))
      c=fgetc(file);
    p=keyword;
    do
    {
      if ((p-keyword) < (MaxTextExtent-1))
        *p++=c;
      c=fgetc(file);
    } while ((c == '<') || isalnum(c));
    *p='\0';
    if (LocaleCompare(keyword,"<magic") == 0)
      {
        MagicInfo
          *magic_info;

        /*
          Allocate memory for the magic list.
        */
        magic_info=(MagicInfo *) AcquireMemory(sizeof(MagicInfo));
        if (magic_info == (MagicInfo *) NULL)
          MagickError(ResourceLimitError,"Unable to allocate magic list",
            "Memory allocation failed");
        memset(magic_info,0,sizeof(MagicInfo));
        if (magic_list == (MagicInfo *) NULL)
          magic_list=magic_info;
        else
          {
            magic_list->next=magic_info;
            magic_info->previous=magic_list;
            magic_list=magic_list->next;
          }
      }
    if (*keyword == '<')
      continue;
    while (isspace(c))
      c=fgetc(file);
    if (c != '=')
      continue;
    do
    {
      c=fgetc(file);
    }
    while (isspace(c));
    if ((c != '"') && (c != '\''))
      continue;
    /*
      Parse value.
    */
    p=value;
    if (c == '"')
      {
        for (c=fgetc(file); (c != '"') && (c != EOF); c=fgetc(file))
          if ((p-value) < (MaxTextExtent-1))
            *p++=c;
      }
    else
      for (c=fgetc(file); (c != '\'') && (c != EOF); c=fgetc(file))
        if ((p-value) < (MaxTextExtent-1))
          *p++=c;
    *p='\0';
    if (magic_list == (MagicInfo *) NULL)
      continue;
    switch (*keyword) 
    {
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            magic_list->name=AllocateString(value);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"offset") == 0)
          {
            magic_list->offset=atoi(value);
            break;
          }
        break;
      }
      case 'T':
      case 't':
      {
        if (LocaleCompare((char *) keyword,"target") == 0)
          {
            char
              *p;

            register unsigned char
              *q;

            magic_list->target=AllocateString(value);
            magic_list->magic=AllocateString(value);
            q=magic_list->magic;
            for (p=magic_list->target; *p != '\0'; )
            {
              if (*p == '\\')
                {
                  p++;
                  if (isdigit((int) *p))
                    {
                      *q++=(unsigned char) strtol(p,&p,8);
                      magic_list->length++;
                      continue;
                    }
                  switch (*p)
                  {
                    case 'b': *q='\b'; break;
                    case 'f': *q='\f'; break;
                    case 'n': *q='\n'; break;
                    case 'r': *q='\r'; break;
                    case 't': *q='\t'; break;
                    case 'v': *q='\v'; break;
                    case 'a': *q='a'; break;
                    case '?': *q='\?'; break;
                    default: *q=(*p); break;
                  }
                  p++;
                  q++;
                  magic_list->length++;
                }
              *q++=(*p++);
              magic_list->length++;
            }
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  (void) fclose(file);
  if (magic_list == (MagicInfo *) NULL)
    return(False);
  while (magic_list->previous != (MagicInfo *) NULL)
    magic_list=magic_list->previous;
  return(True);
}
