/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             U   U  TTTTT  IIIII  L      IIIII  TTTTT  Y   Y                 %
%             U   U    T      I    L        I      T     Y Y                  %
%             U   U    T      I    L        I      T      Y                   %
%             U   U    T      I    L        I      T      Y                   %
%              UUU     T    IIIII  LLLLL  IIIII    T      Y                   %
%                                                                             %
%                                                                             %
%                       ImageMagick Utility Methods                           %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1993                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "define.h"
/*
  Static declarations.
*/
static unsigned char
  AsciiMap[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
    0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xe1, 0xe2, 0xe3, 0xe4, 0xc5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
    0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3,
    0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
    0xfc, 0xfd, 0xfe, 0xff,
  };

/*
  Forward declaration.
*/
static int
  IsDirectory(const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e S t r i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireString() allocates memory for a string and copies the source string
%  to that memory location (and returns it).
%
%  The format of the AcquireString method is:
%
%      char *AcquireString(const char *source)
%
%  A description of each parameter follows:
%
%    o allocated_string:  Method AcquireString returns a copy of the source
%      string.
%
%    o source: A character string.
%
%
*/
MagickExport const char *AcquireString(const char *source)
{
  char
    *destination;

  destination=(char *) AcquireMemory(strlen(source)+1);
  if (destination == (char *) NULL)
    MagickError(ResourceLimitError,"Unable to acquire string",
      "Memory allocation failed");
  *destination='\0';
  if (source != (char *) NULL)
    (void) strcpy(destination,source);
  return((const char *) destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  A p p e n d I m a g e F o r m a t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AppendImageFormat() appends the image format type to the filename.  If an
%  extension to the file already exists, it is first removed.
%
%  The format of the AppendImageFormat method is:
%
%      void AppendImageFormat(const char *format,char *filename)
%
%  A description of each parameter follows.
%
%   o  format:  Specifies a pointer to an array of characters.  This is the
%      format of the image.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
MagickExport void AppendImageFormat(const char *format,char *filename)
{
  char
    root[MaxTextExtent];

  assert(format != (char *) NULL);
  assert(filename != (char *) NULL);
  if ((*format == '\0') || (*filename == '\0'))
    return;
  if (LocaleCompare(filename,"-") == 0)
    {
      char
        message[MaxTextExtent];

      FormatString(message,"%.1024s:%.1024s",format,filename);
      (void) strncpy(filename,message,MaxTextExtent-1);
      return;
    }
  GetPathComponent(filename,RootPath,root);
  FormatString(filename,"%.1024s.%.1024s",root,format);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a s e 6 4 D e c o d e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Base64Decode() decodes Base64-encoded text and returns its binary
%  equivalent.  NULL is returned if the text is not valid base64 data, or a
%  memory allocation failure occurs.
%
%  Contributed by Bob Friesenhahn.
%
%  The format of the Base64Decode method is:
%
%      unsigned char *Base64Decode(const char *source,length_t *length)
%
%  A description of each parameter follows:
%
%    o source:  A pointer to a Base64-encoded string.
%
%    o length: The number of bytes decoded.
%
*/

static const char
  Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

MagickExport unsigned char *Base64Decode(const char *source,size_t *length)
{
  int
    state;

  register char
    *q;

  register const char
    *p;

  register size_t
    i;

  unsigned char
    *decode;

  assert(source != (char *) NULL);
  assert(length != (size_t *) NULL);
  *length=0;
  decode=(unsigned char *) AcquireMemory(3*strlen(source)/4+1);
  if (decode == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  i=0;
  state=0;
  for (p=source; *p != '\0'; p++)
  {
    if (isspace((int) *p))
      continue;
    if (*p == '=')
      break;
    q=strchr(Base64,*p);
    if (q == (char *) NULL)
      {
        LiberateMemory((void **) &decode);
        return((unsigned char *) NULL);  /* non-base64 character */
      }
    switch (state)
    {
      case 0:
      {
        decode[i]=(q-Base64) << 2;
        state++;
        break;
      }
      case 1:
      {
        decode[i++]|=(q-Base64) >> 4;
        decode[i]=((q-Base64) & 0x0f) << 4;
        state++;
        break;
      }
      case 2:
      {
        decode[i++]|=(q-Base64) >> 2;
        decode[i]=((q-Base64) & 0x03) << 6;
        state++;
        break;
      }
      case 3:
      {
        decode[i++]|=(q-Base64);
        state=0;
        break;
      }
    }
  }
  /*
    Verify Base-64 string has proper terminal characters.
  */
  if (*p != '=')
    {
      if (state != 0)
        {
          LiberateMemory((void **) &decode);
          return((unsigned char *) NULL);
        }
    }
  else
    {
      p++;
      switch (state)
      {
        case 0:
        case 1:
        {
          /*
            Invalid '=' character.
          */
          LiberateMemory((void **) &decode);
          return((unsigned char *) NULL);
        }
        case 2:
        {
          for ( ; *p != '\0'; p++)
            if (!isspace((int) *p))
              break;
          if (*p != '=')
            {
              LiberateMemory((void **) &decode);
              return((unsigned char *) NULL);
            }
          p++;
        }
        case 3:
        {
          for ( ; *p != '\0'; p++)
            if (!isspace((int) *p))
              {
                LiberateMemory((void **) &decode);
                return((unsigned char *) NULL);
              }
          if (decode[i] != 0)
            {
              LiberateMemory((void **) &decode);
              return((unsigned char *) NULL);
            }
        }
      }
    }
  *length=i;
  return(decode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a s e 6 4 E n c o d e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Base64Encode() encodes arbitrary binary data to base64 encoded format and
%  returns the result as a null-terminated ASCII string.  NULL is returned if
%  a memory allocation failure occurs.
%
%  Contributed by Bob Friesenhahn.
%
%  The format of the Base64Encode method is:
%
%      char *Base64Encode(const unsigned char *source,const size_t length)
%
%  A description of each parameter follows:
%
%    o source:  A pointer to binary data to encode.
%
%    o length: The number of bytes to encode.
%
*/
MagickExport char *Base64Encode(const unsigned char *source,const size_t length)
{
  char
    *encode;

  register const unsigned char
    *p;

  register size_t
    i;

  size_t
    remainder;

  assert(source != (unsigned char *) NULL);
  encode=(char *) AcquireMemory(4*length/3+4);
  if (encode == (char *) NULL)
    return((char *) NULL);
  i=0;
  for (p=source; p < (source+length-2); p+=3)
  {
    encode[i++]=Base64[*p >> 2];
    encode[i++]=Base64[((*p & 0x03) << 4)+(*(p+1) >> 4)];
    encode[i++]=Base64[((*(p+1) & 0x0f) << 2)+(*(p+2) >> 6)];
    encode[i++]=Base64[*(p+2) & 0x3f];
  }
  remainder=length % 3;
  if (remainder != 0)
    {
      int
        j;

      unsigned char
        code[3];

      for (j=0; j < remainder; j++)
        code[j]=(*p++);
      for ( ; j < 2; j++)
        code[j]='\0';
      encode[i++]=Base64[code[0] >> 2];
      encode[i++]=Base64[((code[0] & 0x03) << 4)+(code[1] >> 4)];
      if (remainder == 1)
        encode[i++]='=';
      else
        encode[i++]=Base64[((code[1] & 0x0f) << 2)+(code[2] >> 6)];
      encode[i++]='=';
    }
  encode[i++]='\0';
  return(encode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e S t r i n g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneString allocates memory for the destination string and copies
%  the source string to that memory location.
%
%  The format of the CloneString method is:
%
%      unsigned int CloneString(char **destination,const char *source)
%
%  A description of each parameter follows:
%
%    o status:  Method CloneString returns True is the string is cloned,
%      otherwise False is returned.
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
%
*/
MagickExport unsigned int CloneString(char **destination,const char *source)
{
  assert(destination != (char **) NULL);
  if (*destination != (char *) NULL)
    LiberateMemory((void **) &*destination);
  if (source == (const char *) NULL)
    {
      *destination=(char *) NULL;
      return(True);
    }
  *destination=GetString(source);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e S t r i n g                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConcatenateString appends a copy of string source, including
%  the terminating null character, to the end of string destination.
%
%  The format of the ConcatenateString method is:
%
%      unsigned int ConcatenateString(char **destination,const char *source)
%
%  A description of each parameter follows:
%
%    o status:  Method ConcatenateString returns True is the string is cloned,
%      otherwise False is returned.
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
%
*/
MagickExport unsigned int ConcatenateString(char **destination,
  const char *source)
{
  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    return(True);
  ReacquireMemory((void **) &(*destination),
    strlen(*destination)+strlen(source)+MaxTextExtent);
  if (*destination == (char *) NULL)
    MagickError(ResourceLimitError,"Unable to concatenate string",
      "Memory allocation failed");
  (void) strcat(*destination,source);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilename expands '~' in a filename.
%
%  The format of the ExpandFilename function is:
%
%      ExpandFilename(char *filename)
%
%  A description of each parameter follows:
%
%    o filename: Specifies a pointer to a character array that contains the
%      filename.
%
%
*/
MagickExport void ExpandFilename(char *filename)
{
  char
    expanded_filename[MaxTextExtent];

  register char
    *p;

  if (filename == (char *) NULL)
    return;
  if (*filename != '~')
    return;
  (void) strncpy(expanded_filename,filename,MaxTextExtent-1);
  if (*(filename+1) == '/')
    {
      /*
        Substitute ~ with $HOME.
      */
      p=(char *) getenv("HOME");
      if (p == (char *) NULL)
        p=(char *) ".";
      (void) strncpy(expanded_filename,p,MaxTextExtent-1);
      (void) strncat(expanded_filename,filename+1,MaxTextExtent-
        strlen(expanded_filename)-1);
    }
  else
    {
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
      char
        username[MaxTextExtent];

      struct passwd
        *entry;

      /*
        Substitute ~ with home directory from password file.
      */
      (void) strncpy(username,filename+1,MaxTextExtent-2);
      p=strchr(username,'/');
      if (p != (char *) NULL)
        *p='\0';
      entry=getpwnam(username);
      if (entry == (struct passwd *) NULL)
        return;
      (void) strncpy(expanded_filename,entry->pw_dir,MaxTextExtent-1);
      if (p != (char *) NULL)
        {
          (void) strcat(expanded_filename,"/");
          (void) strncat(expanded_filename,p+1,MaxTextExtent-
            strlen(expanded_filename)-1);
        }
#endif
    }
  (void) strncpy(filename,expanded_filename,MaxTextExtent-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilenames checks each argument of the command line vector and
%  expands it if they have a wildcard character.  For example, *.jpg might
%  expand to:  bird.jpg rose.jpg tiki.jpg.
%
%  The format of the ExpandFilenames function is:
%
%      status=ExpandFilenames(int *argc,char ***argv)
%
%  A description of each parameter follows:
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%
*/
MagickExport unsigned int ExpandFilenames(int *argc,char ***argv)
{
  char
    **filelist,
    filename[MaxTextExtent],
    home_directory[MaxTextExtent],
    *option,
    path[MaxTextExtent],
    **vector,
    working_directory[MaxTextExtent];

  long
    count,
    expanded,
    number_files;

  register long
    i,
	  j;

  /*
    Allocate argument vector.
  */
  assert(argc != (int *) NULL);
  assert(argv != (char ***) NULL);
  for (i=1; i < *argc; i++)
    if (strlen((*argv)[i]) > (MaxTextExtent/2-1))
      MagickError(ResourceLimitError,"Token length exceeds limit",(*argv)[i]);
  vector=(char **) AcquireMemory((*argc+MaxTextExtent)*sizeof(char *));
  if (vector == (char **) NULL)
    return(False);
  /*
    Expand any wildcard filenames.
  */
  (void) getcwd(home_directory,MaxTextExtent-1);
  expanded=False;
  count=0;
  for (i=0; i < *argc; i++)
  {
    option=(*argv)[i];
    vector[count++]=option;
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      {
        if (*option == '+')
          if (LocaleNCompare("profile",option+1,4) != 0)
            continue;
        i++;
        if (i == *argc)
          continue;
        option=(*argv)[i];
        vector[count++]=option;
        continue;
      }
    if ((*option == '"') || (*option == '\''))
      continue;
    (void) strncpy(path,option,MaxTextExtent-1);
    ExpandFilename(path);
    if (!IsGlob(path))
      {
        expanded=True;
        continue;
      }
    /*
      Get the list of image file names.
    */
    GetPathComponent(path,HeadPath,working_directory);
    GetPathComponent(path,TailPath,filename);
    if (*working_directory == '\0')
      (void) getcwd(working_directory,MaxTextExtent-1);
    filelist=ListFiles(working_directory,filename,&number_files);
    if (filelist == (char **) NULL)
      continue;
    for (j=0; j < number_files; j++)
      if (IsDirectory(filelist[j]) <= 0)
        break;
    if (j == number_files)
      {
        for (j=0; j < number_files; j++)
          LiberateMemory((void **) &filelist[j]);
        LiberateMemory((void **) &filelist);
        continue;
      }
    /*
      Transfer file list to argument vector.
    */
    ReacquireMemory((void **) &vector,
      (*argc+count+number_files)*sizeof(char *));
    if (vector == (char **) NULL)
      return(False);
    count--;
    for (j=0; j < number_files; j++)
    {
      FormatString(filename,"%.1024s%s%.1024s",working_directory,
        DirectorySeparator,filelist[j]);
      if (IsDirectory(filename) != 0)
        {
          LiberateMemory((void **) &filelist[j]);
          continue;
        }
      expanded=True;
      vector[count]=GetString(filename);
      LiberateMemory((void **) &filelist[j]);
      count++;
    }
    LiberateMemory((void **) &filelist);
  }
  (void) chdir(home_directory);
  if (!expanded)
    {
      LiberateMemory((void **) &vector);
      return(True);
    }
  *argc=count;
  *argv=vector;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t S t r i n g                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FormatString prints formatted output of a variable argument list.
%
%  The format of the FormatString method is:
%
%      void FormatString(char *string,const char *format,...)
%
%  A description of each parameter follows.
%
%   o  string:  Method FormatString returns the formatted string in this
%      character buffer.
%
%   o  format:  A string describing the format to use to write the remaining
%      arguments.
%
%
*/
MagickExport void FormatString(char *string,const char *format,...)
{
  va_list
    operands;

  va_start(operands,format);
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(string,format,operands);
#else
  (void) vsnprintf(string,MaxTextExtent,format,operands);
#endif
  va_end(operands);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E x e c u t i o n P a t h                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetExecutionPath function returns the pathname of the executable
%  that started the process.
%
%  The format of the GetExecutionPath method is:
%
%      char *GetExecutionPath(const char *path)
%
%  A description of each parameter follows:
%
%    o execution_path: Method GetExecutionPath returns the pathname of the
%      executable that started the process.
%
%    o path: This character string is returned if no method is available to
%      determine the pathname of the executable.
%
%
*/
MagickExport char *GetExecutionPath(const char *path)
{
#if defined(WIN32)
  return(NTGetExecutionPath());
#endif
  return(GetString(path));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetGeometry() parses a geometry specification and returns the width
%  height, x, and y values.  It also returns flags that indicates which
%  of the four values (width, height, x, y) were located in the string, and
%  whether the x and y values are negative.  In addition, there are flags to
%  report any meta characters (%, !, <, and >).
%
%  The format of the GetGeometry method is:
%
%      int GetGeometry(const char *image_geometry,long *x,long *y,
%        unsigned long *width,unsigned long *height)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  The x and y offset as determined by the geometry specification is
%      returned here.
%
%    o width,height:  The width and height as determined by the geometry
%      specification is returned here.
%
%
*/
MagickExport int GetGeometry(const char *image_geometry,long *x,long *y,
  unsigned long *width,unsigned long *height)
{
  char
    geometry[MaxTextExtent],
    *p;

  int
    flags;

  RectangleInfo
    bounds;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  if ((image_geometry == (char *) NULL) || (*image_geometry == '\0'))
    return(NoValue);
  /*
    Remove whitespaces and % and ! characters from geometry specification.
  */
  (void) strncpy(geometry,image_geometry,MaxTextExtent-1);
  flags=NoValue;
  p=geometry;
  while (strlen(p) != 0)
  {
    if (isspace((int) (*p)))
      (void) strcpy(p,p+1);
    else
      switch (*p)
      {
        case '%':
        {
          flags|=PercentValue;
          (void) strcpy(p,p+1);
          break;
        }
        case '!':
        {
          flags|=AspectValue;
          (void) strcpy(p,p+1);
          break;
        }
        case '<':
        {
          flags|=LessValue;
          (void) strcpy(p,p+1);
          break;
        }
        case '>':
        {
          flags|=GreaterValue;
          (void) strcpy(p,p+1);
          break;
        }
        case '@':
        {
          flags|=AreaValue;
          (void) strcpy(p,p+1);
          break;
        }
        default:
          p++;
      }
  }
  /*
    Parse width/height/x/y.
  */
  p=(char *) geometry;
  while (isspace((int) *p))
    p++;
  if (*p == '\0')
    return(flags);
  if (*p == '=')
    p++;
  if ((*p != '+') && (*p != '-') && (*p != 'x') && (*p != 'X'))
    {
      char
        *q;

      /*
        Parse width.
      */
      bounds.width=(unsigned long) floor(strtol(p,&q,10)+0.5);
      if ((*q == 'x') || (*q == 'X'))
        p=q;
      else
        bounds.width=(unsigned long) floor(strtod(p,&p)+0.5);
      flags|=WidthValue;
    }
  if ((*p == 'x') || (*p == 'X'))
    {
      /*
        Parse height.
      */
      p++;
      bounds.height=(unsigned long) floor(strtod(p,&p)+0.5);
      flags|=HeightValue;
    }
  if ((*p == '+') || (*p == '-'))
    {
      /*
        Parse x value.
      */
      if (*p == '-')
        {
          p++;
          bounds.x=(long) ceil(-strtod(p,&p)-0.5);
          flags|=XNegative;
        }
      else
        {
          p++;
          bounds.x=(long) ceil(strtod(p,&p)-0.5);
        }
      flags|=XValue;
      if ((*p == '+') || (*p == '-'))
        {
          /*
            Parse y value.
          */
          if (*p == '-')
            {
              p++;
              bounds.y=(long) ceil(-strtod(p,&p)-0.5);
              flags|=YNegative;
            }
          else
            {
              p++;
              bounds.y=(long) ceil(strtod(p,&p)-0.5);
            }
          flags|=YValue;
        }
    }
  if (*p != '\0')
    return(flags);
  if (flags & XValue)
    *x=bounds.x;
  if (flags & YValue)
    *y=bounds.y;
  if (flags & WidthValue)
    *width=bounds.width;
  if (flags & HeightValue)
    *height=bounds.height;
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c k G e o m e t r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickGeometry() is similar to GetGeometry() except the returned
%  geometry is modified as determined by the meta characters:  %, !, <,
%  and >.
%
%  The format of the GetMagickGeometry method is:
%
%      int GetMagickGeometry(const char *geometry,long *x,long *y,
%        unsigned long *width,unsigned long *height)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetMagickGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  A pointer to an integer.  The x and y offset as determined by
%      the geometry specification is returned here.
%
%    o width,height:  A pointer to an unsigned integer.  The width and height
%      as determined by the geometry specification is returned here.
%
%
*/

MagickExport int ParseImageGeometry(const char *geometry,long *x,long *y,
  unsigned long *width,unsigned long *height)
{
  return(GetMagickGeometry(geometry,x,y,width,height));
}

MagickExport int GetMagickGeometry(const char *geometry,long *x,long *y,
  unsigned long *width,unsigned long *height)
{
  int
    flags;

  long
    count,
    delta;

  RectangleInfo
    media_info;

  unsigned long
    former_height,
    former_width;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(NoValue);
  /*
    Parse geometry using GetGeometry.
  */
  former_width=(*width);
  former_height=(*height);
  flags=GetGeometry(geometry,x,y,width,height);
  if (flags & PercentValue)
    {
      double
        x_scale,
        y_scale;

      /*
        Geometry is a percentage of the image size.
      */
      x_scale=(*width);
      y_scale=(*height);
      count=sscanf(geometry,"%lf%%x%lf",&x_scale,&y_scale);
      if (count != 2)
        count=sscanf(geometry,"%lfx%lf",&x_scale,&y_scale);
      if (count == 1)
        y_scale=x_scale;
      *width=(unsigned long) floor((x_scale*former_width/100.0)+0.5);
      *height=(unsigned long) floor((y_scale*former_height/100.0)+0.5);
      former_width=(*width);
      former_height=(*height);
    }
  if (flags & AreaValue)
    {
      double
        distance,
        x_area,
        y_area;

      /*
        Geometry is a maximum area in pixels.
      */
      x_area=(*width);
      y_area=(*height);
      count=sscanf(geometry,"%lf%%x%lf",&x_area,&y_area);
      if (count != 2)
        count=sscanf(geometry,"%lfx%lf",&x_area,&y_area);
      if (count == 1)
        y_area=x_area;
      distance=sqrt((double) former_width*former_height);
      *width=(unsigned long) floor((x_area*former_width/distance)+0.5);
      *height=(unsigned long) floor((y_area*former_height/distance)+0.5);
      former_width=(*width);
      former_height=(*height);
    }
  if (!(flags & AspectValue) &&
      ((*width != former_width) || (*height != former_height)))
    {
      double
        scale_factor;

      /*
        Respect aspect ratio of the image.
      */
      if ((former_width == 0) || (former_height == 0))
        scale_factor=1.0;
      else
        if (((flags & WidthValue) != 0) && (flags & HeightValue) != 0)
          {
            scale_factor=(double) *width/former_width;
            if (scale_factor > ((double) *height/former_height))
              scale_factor=(double) *height/former_height;
          }
        else
          if ((flags & WidthValue) != 0)
            scale_factor=(double) *width/former_width;
          else
            scale_factor=(double) *height/former_height;
    *width=(unsigned long) floor(scale_factor*former_width+0.5);
    *height=(unsigned long) floor(scale_factor*former_height+0.5);
  }
  if (flags & GreaterValue)
    {
      if (former_width < *width)
        *width=former_width;
      if (former_height < *height)
        *height=former_height;
    }
  if (flags & LessValue)
    {
      if (former_width > *width)
        *width=former_width;
      if (former_height > *height)
        *height=former_height;
    }
  media_info.width=(*width);
  media_info.height=(*height);
  media_info.x=(*x);
  media_info.y=(*y);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,&media_info.width,
    &media_info.height);
  if ((flags & XValue) == 0)
    {
      /*
        Center image in the X direction.
      */
      delta=(long) (media_info.width-(*width));
      if (delta >= 0)
        *x=delta >> 1;
    }
  else
    if ((flags & XNegative) != 0)
      *x+=media_info.width-(*width);
  if ((flags & YValue) == 0)
    {
      /*
        Center image in the Y direction.
      */
      delta=(long) (media_info.height-(*height));
      if (delta >= 0)
        *y=delta >> 1;
    }
  else
    if ((flags & YNegative) != 0)
      *y+=media_info.height-(*height);
  if (flags & GreaterValue)
    {
      if ((*width+((*x) << 1)) > media_info.width)
        {
          if ((long) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((long) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
      if ((*height+((*y) << 1)) > media_info.height)
        {
          if ((long) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((long) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
    }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t P a g e G e o m e t r y                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPageGeometry() replaces any page mneumonic with the equivalent size in
%  picas.
%
%  The format of the GetPageGeometry() method is:
%
%      char *GetPageGeometry(const char *page_geometry)
%
%  A description of each parameter follows.
%
%   o  page_geometry:  Specifies a pointer to an array of characters.
%      The string is either a Postscript page name (e.g. A4) or a postscript
%      page geometry (e.g. 612x792+36+36).
%
%
*/

MagickExport char *PostscriptGeometry(const char *geometry)
{
  return(GetPageGeometry(geometry));
}

MagickExport char *GetPageGeometry(const char *page_geometry)
{
  static const char
    *PageSizes[][2]=
    {
      { "4x6",  "288x432" },
      { "5x7",  "360x504" },
      { "7x9",  "504x648" },
      { "8x10", "576x720" },
      { "9x11",  "648x792" },
      { "9x12",  "648x864" },
      { "10x13",  "720x936" },
      { "10x14",  "720x1008" },
      { "11x17",  "792x1224" },
      { "A0",  "2384x3370" },
      { "A1",  "1684x2384" },
      { "A10", "73x105" },
      { "A2",  "1191x1684" },
      { "A3",  "842x1191" },
      { "A4",  "595x842" },
      { "A4SMALL", "595x842" },
      { "A5",  "420x595" },
      { "A6",  "297x420" },
      { "A7",  "210x297" },
      { "A8",  "148x210" },
      { "A9",  "105x148" },
      { "ARCHA", "648x864" },
      { "ARCHB", "864x1296" },
      { "ARCHC", "1296x1728" },
      { "ARCHD", "1728x2592" },
      { "ARCHE", "2592x3456" },
      { "B0",  "2920x4127" },
      { "B1",  "2064x2920" },
      { "B10", "91x127" },
      { "B2",  "1460x2064" },
      { "B3",  "1032x1460" },
      { "B4",  "729x1032" },
      { "B5",  "516x729" },
      { "B6",  "363x516" },
      { "B7",  "258x363" },
      { "B8",  "181x258" },
      { "B9",  "127x181" },
      { "C0",  "2599x3676" },
      { "C1",  "1837x2599" },
      { "C2",  "1298x1837" },
      { "C3",  "918x1296" },
      { "C4",  "649x918" },
      { "C5",  "459x649" },
      { "C6",  "323x459" },
      { "C7",  "230x323" },
      { "EXECUTIVE", "540x720" },
      { "FLSA", "612x936" },
      { "FLSE", "612x936" },
      { "FOLIO",  "612x936" },
      { "HALFLETTER", "396x612" },
      { "ISOB0", "2835x4008" },
      { "ISOB1", "2004x2835" },
      { "ISOB10", "88x125" },
      { "ISOB2", "1417x2004" },
      { "ISOB3", "1001x1417" },
      { "ISOB4", "709x1001" },
      { "ISOB5", "499x709" },
      { "ISOB6", "354x499" },
      { "ISOB7", "249x354" },
      { "ISOB8", "176x249" },
      { "ISOB9", "125x176" },
      { "LEDGER",  "1224x792" },
      { "LEGAL",  "612x1008" },
      { "LETTER", "612x792" },
      { "LETTERSMALL",  "612x792" },
      { "QUARTO",  "610x780" },
      { "STATEMENT",  "396x612" },
      { "TABLOID",  "792x1224" },
      { (char *) NULL, (char *) NULL }
    };

  char
    *page;

  register long
    i;

  assert(page_geometry != (char *) NULL);
  page=GetString(page_geometry);
  for (i=0; *PageSizes[i] != (char *) NULL; i++)
    if (LocaleNCompare(PageSizes[i][0],page,strlen(PageSizes[i][0])) == 0)
      {
        int
          flags;

        RectangleInfo
          geometry;

        /*
          Replace mneumonic with the equivalent size in dots-per-inch.
        */
        (void) strncpy(page,PageSizes[i][1],MaxTextExtent-1);
        (void) strncat(page,page_geometry+strlen(PageSizes[i][0]),
          MaxTextExtent-strlen(page)-2);
        flags=GetGeometry(page,&geometry.x,&geometry.y,&geometry.width,
          &geometry.height);
        if (!(flags & GreaterValue))
          (void) strcat(page,">");
        break;
      }
  return(page);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P a t h C o m p o n e n t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPathComponent returns the parent directory name, filename,
%  basename, or extension of a file path.
%
%  The format of the GetPathComponent function is:
%
%      GetPathComponent(const char *path,PathType type,char *component)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to a character array that contains the
%      file path.
%
%    o type: Specififies which file path component to return.
%
%    o component: The selected file path component is returned here.
%
*/
MagickExport void GetPathComponent(const char *path,PathType type,
  char *component)
{
  register char
    *p;

  /*
    Get basename of client.
  */
  assert(path != (const char *) NULL);
  assert(component != (const char *) NULL);
  (void) strncpy(component,path,MaxTextExtent-1);
  if (*path == '\0')
    return;
  for (p=component+(strlen(component)-1); p > component; p--)
    if (IsBasenameSeparator(*p))
      break;
  switch (type)
  {
    case RootPath:
    {
      for (p=component+(strlen(component)-1); p > component; p--)
        if (*p == '.')
          break;
      if (*p == '.')
        *p='\0';
      break;
    }
    case HeadPath:
    {
      *p='\0';
      break;
    }
    case TailPath:
    {
      if (IsBasenameSeparator(*p))
        (void) CloneMemory(component,p+1,strlen(p+1)+1);
      break;
    }
    case BasePath:
    {
      if (IsBasenameSeparator(*p))
        (void) strncpy(component,p+1,MaxTextExtent-1);
      for (p=component+(strlen(component)-1); p > component; p--)
        if (*p == '.')
          {
            *p='\0';
            break;
          }
      break;
    }
    case ExtensionPath:
    {
      if (IsBasenameSeparator(*p))
        (void) strncpy(component,p+1,MaxTextExtent-1);
      for (p=component+(strlen(component)-1); p > component; p--)
        if (*p == '.')
          break;
      *component='\0';
      if (*p == '.')
        (void) strncpy(component,p+1,MaxTextExtent-1);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetString() allocates memory for a string and copies the source string
%  to that memory location (and returns it).
%
%  The format of the GetString method is:
%
%      char *GetString(const char *source)
%
%  A description of each parameter follows:
%
%    o allocated_string:  Method GetString returns a copy of the source
%      string.
%
%    o source: A character string.
%
%
*/
MagickExport char *GetString(const char *source)
{
  char
    *destination;

  size_t
    length;

  length=MaxTextExtent;
  if (source != (char *) NULL)
    length+=strlen(source);
  destination=(char *) AcquireMemory(length+MaxTextExtent);
  if (destination == (char *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate string",
      "Memory allocation failed");
  *destination='\0';
  if (source != (char *) NULL)
    (void) strcpy(destination,source);
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T o k e n                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetToken gets a token from the token stream.  A token is defined
%  as sequence of characters delimited by whitespace (e.g. clip-path), a
%  sequence delimited with quotes (.e.g "Quote me"), or a sequence enclosed
%  in parenthesis (e.g. rgb(0,0,0)).
%
%  The format of the GetToken method is:
%
%      void GetToken(const char *start,char **end,char *token)
%
%  A description of each parameter follows:
%
%    o start: the start of the token sequence.
%
%    o end: point to the end of the token sequence.
%
%    o token: copy the token to this buffer.
%
%
*/
MagickExport void GetToken(const char *start,char **end,char *token)
{
  register char
    *p;

  register long
    i;

  i=0;
  for (p=(char *) start; *p != '\0'; )
  {
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    switch (*p)
    {
      case '"':
      {
        for (p++; *p != '\0'; p++)
        {
          if ((*p == '"') && (*(p-1) != '\\'))
            {
              p++;
              break;
            }
          token[i++]=(*p);
        }
        break;
      }
      case '\'':
      {
        for (p++; *p != '\0'; p++)
        {
          if ((*p == '\'') && (*(p-1) != '\\'))
            {
              p++;
              break;
            }
          token[i++]=(*p);
        }
        break;
      }
      case '{':
      {
        for (p++; *p != '\0'; p++)
        {
          if ((*p == '}') && (*(p-1) != '\\'))
            {
              p++;
              break;
            }
          token[i++]=(*p);
        }
        break;
      }
      default:
      {
        char
          *q;

        (void) strtod(p,&q);
        if (p != q)
          {
            for ( ; p < q; p++)
              token[i++]=(*p);
            if (*p == '%')
              token[i++]=(*p++);
            break;
          }
        if (!isalpha((int) *p) && (*p != '#') && (*p != '<'))
          {
            token[i++]=(*p++);
            break;
          }
        for ( ; *p != '\0'; p++)
        {
          if ((isspace((int) *p) || (*p == '=')) && (*(p-1) != '\\'))
            break;
          token[i++]=(*p);
          if (*p == '(')
            for (p++; *p != '\0'; p++)
            {
              token[i++]=(*p);
              if ((*p == ')') && (*(p-1) != '\\'))
                break;
            }
        }
        break;
      }
    }
    break;
  }
  token[i]='\0';
  if (LocaleNCompare(token,"url(#",5) == 0)
    {
      i=(long) strlen(token);
      (void) strncpy(token,token+5,MaxTextExtent-1);
      token[i-6]='\0';
    }
  if (end != (char **) NULL)
    *end=p;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G l o b E x p r e s s i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GlobExpression returns True if the expression matches the pattern.
%
%  The format of the GlobExpression function is:
%
%      int GlobExpression(const char *expression,const char *pattern)
%
%  A description of each parameter follows:
%
%    o expression: Specifies a pointer to a text string containing a file name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%
*/
MagickExport int GlobExpression(const char *expression,const char *pattern)
{
  unsigned int
    done,
    exempt;

  /*
    Return on empty pattern or '*'.
  */
  if (pattern == (char *) NULL)
    return(True);
  if (strlen(pattern) == 0)
    return(True);
  if (LocaleCompare(pattern,"*") == 0)
    return(True);
  if (strchr(pattern,'['))
    {
      ExceptionInfo
        exception;

      ImageInfo
        *image_info;

      /*
        Determine if pattern is a subimage, i.e. img0001.pcd[2].
      */
      image_info=CloneImageInfo((ImageInfo *) NULL);
      (void) strncpy(image_info->filename,pattern,MaxTextExtent-1);
      GetExceptionInfo(&exception);
      (void) SetImageInfo(image_info,True,&exception);
      DestroyExceptionInfo(&exception);
      exempt=(LocaleCompare(image_info->magick,"VID") == 0) ||
        (image_info->subimage &&
        (LocaleCompare(expression,image_info->filename) == 0));
      DestroyImageInfo(image_info);
      if (exempt)
        return(False);
    }
  /*
    Evaluate glob expression.
  */
  done=False;
  while ((*pattern != '\0') && !done)
  {
    if (*expression == '\0')
      if ((*pattern != '{') && (*pattern != '*'))
        break;
    switch (*pattern)
    {
      case '\\':
      {
        pattern++;
        if (*pattern != '\0')
          pattern++;
        break;
      }
      case '*':
      {
        int
          status;

        pattern++;
        status=False;
        while ((*expression != '\0') && !status)
          status=GlobExpression((char *) expression++,pattern);
        if (status)
          {
            while (*expression != '\0')
              expression++;
            while (*pattern != '\0')
              pattern++;
          }
        break;
      }
      case '[':
      {
        char
          c;

        pattern++;
        for ( ; ; )
        {
          if ((*pattern == '\0') || (*pattern == ']'))
            {
              done=True;
              break;
            }
          if (*pattern == '\\')
            {
              pattern++;
              if (*pattern == '\0')
                {
                  done=True;
                  break;
                }
             }
          if (*(pattern+1) == '-')
            {
              c=(*pattern);
              pattern+=2;
              if (*pattern == ']')
                {
                  done=True;
                  break;
                }
              if (*pattern == '\\')
                {
                  pattern++;
                  if (*pattern == '\0')
                    {
                      done=True;
                      break;
                    }
                }
              if ((*expression < c) || (*expression > *pattern))
                {
                  pattern++;
                  continue;
                }
            }
          else
            if (*pattern != *expression)
              {
                pattern++;
                continue;
              }
          pattern++;
          while ((*pattern != ']') && (*pattern != '\0'))
          {
            if ((*pattern == '\\') && (*(pattern+1) != '\0'))
              pattern++;
            pattern++;
          }
          if (*pattern != '\0')
            {
              pattern++;
              expression++;
            }
          break;
        }
        break;
      }
      case '?':
      {
        pattern++;
        expression++;
        break;
      }
      case '{':
      {
        int
          match;

        register const char
          *p;

        pattern++;
        while ((*pattern != '}') && (*pattern != '\0'))
        {
          p=expression;
          match=True;
          while ((*p != '\0') && (*pattern != '\0') &&
                 (*pattern != ',') && (*pattern != '}') && match)
          {
            if (*pattern == '\\')
              pattern++;
            match=(*pattern == *p);
            p++;
            pattern++;
          }
          if (*pattern == '\0')
            {
              match=False;
              done=True;
              break;
            }
          else
            if (match)
              {
                expression=p;
                while ((*pattern != '}') && (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if (*pattern == '}')
                        pattern++;
                    }
                }
              }
            else
              {
                while ((*pattern != '}') && (*pattern != ',') &&
                       (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if ((*pattern == '}') || (*pattern == ','))
                        pattern++;
                    }
                }
              }
            if (*pattern != '\0')
              pattern++;
          }
        break;
      }
      default:
      {
        if (*expression != *pattern)
          done=True;
        else
          {
            expression++;
            pattern++;
          }
      }
    }
  }
  while (*pattern == '*')
    pattern++;
  return((*expression == '\0') && (*pattern == '\0'));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s A c c e s s i b l e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsAccessible returns True if the file as defined by filename is
%  accessible.
%
%  The format of the IsAccessible method is:
%
%      unsigned int IsAccessible(const char *filename)
%
%  A description of each parameter follows.
%
%    o status:  Method IsAccessible returns True is the file as defined by
%      filename is accessible, otherwise False is returned.
%
%    o filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
MagickExport unsigned int IsAccessible(const char *filename)
{
  FILE
    *file;

  unsigned int
    status;

  /*
    Return False if the file cannot be opened.
  */
  if ((filename == (const char *) NULL) || (*filename == '\0'))
    return(False);
  file=fopen(filename,ReadBinaryType);
  if (file == (FILE *) NULL)
    return(False);
  (void) fgetc(file);
  status=!feof(file) && !ferror(file);
  (void) fclose(file);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I s D i r e c t o r y                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDirectory returns -1 if the filename does not exist, 0 if the
%  filename represents a file, and 1 if the filename represents a directory.
%
%  The format of the IsAccessible method is:
%
%      int IsDirectory(const char *filename)
%
%  A description of each parameter follows.
%
%   o  status:  Method IsDirectory returns -1 if the filename does not exist,
%      0 if the filename represents a file, and 1 if the filename represents
%      a directory.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
static int IsDirectory(const char *filename)
{
  int
    status;

  struct stat
    file_info;

  assert(filename != (const char *) NULL);
  status=stat(filename,&file_info);
  if (status != 0)
    return(-1);
  return(S_ISDIR(file_info.st_mode));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGeometry returns True if the geometry specification is valid
%  as determined by GetGeometry.
%
%  The format of the IsGeometry method is:
%
%      unsigned int IsGeometry(const char *geometry)
%
%  A description of each parameter follows:
%
%    o status: Method IsGeometry returns True if the geometry specification
%      is valid otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%
*/
MagickExport unsigned int IsGeometry(const char *geometry)
{
  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=GetGeometry(geometry,&x,&y,&width,&height);
  return(flags != NoValue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t F i l e s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListFiles reads the directory specified and returns a list
%  of filenames contained in the directory sorted in ascending alphabetic
%  order.
%
%  The format of the ListFiles function is:
%
%      char **ListFiles(const char *directory,const char *pattern,
%        long *number_entries)
%
%  A description of each parameter follows:
%
%    o filelist: Method ListFiles returns a list of filenames contained
%      in the directory.  If the directory specified cannot be read or it is
%      a file a NULL list is returned.
%
%    o directory: Specifies a pointer to a text string containing a directory
%      name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_entries:  This integer returns the number of filenames in the
%      list.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int FileCompare(const void *x,const void *y)
{
  register char
    **p,
    **q;

  p=(char **) x;
  q=(char **) y;
  return(LocaleCompare(*p,*q));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport char **ListFiles(const char *directory,const char *pattern,
  long *number_entries)
{
  char
    **filelist,
    filename[MaxTextExtent];

  DIR
    *current_directory;

  int
    status;

  struct dirent
    *entry;

  unsigned int
    max_entries;

  /*
    Open directory.
  */
  assert(directory != (const char *) NULL);
  assert(pattern != (char *) NULL);
  assert(number_entries != (long *) NULL);
  *number_entries=0;
  status=chdir(directory);
  if (status != 0)
    return((char **) NULL);
  (void) getcwd(filename,MaxTextExtent-1);
  current_directory=opendir(filename);
  if (current_directory == (DIR *) NULL)
    return((char **) NULL);
  (void) chdir(filename);
  /*
    Allocate filelist.
  */
  max_entries=2048;
  filelist=(char **) AcquireMemory(max_entries*sizeof(char *));
  if (filelist == (char **) NULL)
    {
      (void) closedir(current_directory);
      return((char **) NULL);
    }
  /*
    Save the current and change to the new directory.
  */
  entry=readdir(current_directory);
  while (entry != (struct dirent *) NULL)
  {
    if (*entry->d_name == '.')
      {
        entry=readdir(current_directory);
        continue;
      }
    if ((IsDirectory(entry->d_name) > 0) ||
        GlobExpression(entry->d_name,pattern))
      {
        if (*number_entries >= (int) max_entries)
          {
            /*
              Extend the file list.
            */
            max_entries<<=1;
            ReacquireMemory((void **) &filelist,max_entries*sizeof(char *));
            if (filelist == (char **) NULL)
              {
                (void) closedir(current_directory);
                return((char **) NULL);
              }
          }
#if defined(vms)
        {
          register char
            *p;

          p=strchr(entry->d_name,';');
          if (p)
            *p='\0';
          if (*number_entries > 0)
            if (LocaleCompare(entry->d_name,filelist[*number_entries-1]) == 0)
              {
                entry=readdir(current_directory);
                continue;
              }
        }
#endif
        filelist[*number_entries]=(char *)
          AcquireMemory(strlen(entry->d_name)+MaxTextExtent);
        if (filelist[*number_entries] == (char *) NULL)
          break;
        (void) strncpy(filelist[*number_entries],entry->d_name,MaxTextExtent-1);
        if (IsDirectory(entry->d_name) > 0)
          (void) strcat(filelist[*number_entries],DirectorySeparator);
        (*number_entries)++;
      }
    entry=readdir(current_directory);
  }
  (void) closedir(current_directory);
  /*
    Sort filelist in ascending order.
  */
  qsort((void *) filelist,*number_entries,sizeof(char **),FileCompare);
  return(filelist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e C o m p a r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleCompare compares two strings byte-by-byte, according to
%  the ordering of the current locale encoding. LocaleCompare returns an
%  integer greater than, equal to, or less than 0, if the string pointed
%  to by p is greater than, equal to, or less than the string pointed to
%  by q respectively.  The sign of a non-zero return value is determined
%  by the sign of the difference between the values of the first pair of
%  bytes that differ in the strings being compared.
%
%  The format of the LocaleCompare method is:
%
%      int LocaleCompare(const char *p,const char *q)
%
%  A description of each parameter follows:
%
%    o p: A pointer to a character string.
%
%    o q: A pointer to a character string to compare to p.
%
%
*/
MagickExport int LocaleCompare(const char *p,const char *q)
{
  register unsigned char
    c,
    d;

  if (p == (char *) NULL)
    return(-1);
  if (q == (char *) NULL)
    return(1);
  for ( ; ; )
  {
    c=(unsigned char) *p;
    d=(unsigned char) *q;
    if ((c == '\0') || (AsciiMap[c] != AsciiMap[d]))
      break;
    p++;
    q++;
  }
  return(AsciiMap[c]-AsciiMap[d]);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e L o w e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleLower copies a null terminated string from source to
%  destination (including the null), changing all uppercase letters to
%  lowercase.
%
%  The format of the LocaleLower method is:
%
%      void LocaleLower(char *string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to lower-case Locale.
%
%
*/
MagickExport void LocaleLower(char *string)
{
  register char
    *q;

  assert(string != (char *) NULL);
  for (q=string; *q != '\0'; q++)
    *q=(char) tolower((int) *q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e N C o m p a r e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleNCompare compares two strings byte-by-byte, according to
%  the ordering of the currnet locale encoding. LocaleNCompare returns an
%  integer greater than, equal to, or less than 0, if the string pointed
%  to by p is greater than, equal to, or less than the string pointed to
%  by q respectively.  The sign of a non-zero return value is determined
%  by the sign of the difference between the values of the first pair of
%  bytes that differ in the strings being compared.  The LocaleNCompare
%  method makes the same comparison as LocaleCompare but looks at a
%  maximum of n bytes.  Bytes following a null byte are not compared.
%
%  The format of the LocaleNCompare method is:
%
%      int LocaleNCompare(const char *p,const char *q,const size_t n)
%
%  A description of each parameter follows:
%
%    o p: A pointer to a character string.
%
%    o q: A pointer to a character string to compare to p.
%
%    o length: The number of characters to compare in strings p & q.
%
%
*/
MagickExport int LocaleNCompare(const char *p,const char *q,const size_t length)
{
  register size_t
    n;

  register unsigned char
    c,
    d;

  if (p == (char *) NULL)
    return(-1);
  if (q == (char *) NULL)
    return(1);
  for (n=length; n != 0; n--)
  {
    c=(unsigned char) *p;
    d=(unsigned char) *q;
    if (AsciiMap[c] != AsciiMap[d])
      return(AsciiMap[c]-AsciiMap[d]);
    if (c == '\0')
      return(0);
    p++;
    q++;
  }
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleUpper copies a null terminated string from source to
%  destination (including the null), changing all lowercase letters to
%  uppercase.
%
%  The format of the LocaleUpper method is:
%
%      void LocaleUpper(char *string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Locale.
%
%
*/
MagickExport void LocaleUpper(char *string)
{
  register char
    *q;

  assert(string != (char *) NULL);
  for (q=string; *q != '\0'; q++)
    *q=(char) toupper((int) *q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M u l t i l i n e C e n s u s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MultilineCensus returns the number of lines within a label.  A line
%  is represented by a \n character.
%
%  The format of the MultilineCenus method is:
%
%      unsigned long MultilineCensus(const char *label)
%
%  A description of each parameter follows.
%
%   o  label:  This character string is the label.
%
%
*/
MagickExport unsigned long MultilineCensus(const char *label)
{
  long
    number_lines;

  /*
    Determine the number of lines within this label.
  */
  if (label == (char *) NULL)
    return(0);
  for (number_lines=1; *label != '\0'; label++)
    if (*label == '\n')
      number_lines++;
  return(number_lines);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientName sets the client name if the name is specified.
%  Otherwise the current client name is returned.
%
%  The format of the SetClientName method is:
%
%      char *SetClientName(const char *name)
%
%  A description of each parameter follows:
%
%    o client_name: Method SetClientName returns the current client name.
%
%    o status: Specifies the new client name.
%
%
*/
MagickExport char *SetClientName(const char *name)
{
  static char
    client_name[MaxTextExtent] = "Magick";

  if ((name != (char *) NULL) && (*name != '\0'))
    (void) strncpy(client_name,name,MaxTextExtent-1);
  return(client_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t P a t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientPath sets the client path if the name is specified.
%  Otherwise the current client path is returned.
%
%  The format of the SetClientPath method is:
%
%      char *SetClientPath(const char *path)
%
%  A description of each parameter follows:
%
%    o client_path: Method SetClientPath returns the current client path.
%
%    o status: Specifies the new client path.
%
%
*/
MagickExport char *SetClientPath(const char *path)
{
  static char
    client_path[MaxTextExtent] = MagickModulesPath;

  if ((path != (char *) NULL) && (*path != '\0'))
    (void) strncpy(client_path,path,MaxTextExtent-1);
  return(client_path);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetGeometry sets a grometry to its default values.
%
%  The format of the SetGeometry method is:
%
%      SetGeometry(const Image *image,RectangleInfo *geometry)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o geometry: The geometry.
%
%
*/
MagickExport void SetGeometry(const Image *image,RectangleInfo *geometry)
{
  assert(image != (Image *) NULL);
  assert(geometry != (RectangleInfo *) NULL);
  (void) memset(geometry,0,sizeof(RectangleInfo));
  geometry->width=image->columns;
  geometry->height=image->rows;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o A r g v                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToArgv converts a text string into command line arguments.
%
%  The format of the StringToArgv method is:
%
%      char **StringToArgv(const char *text,int *argc)
%
%  A description of each parameter follows:
%
%    o argv:  Method StringToArgv returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%    o argc:  This integer pointer returns the number of arguments in the
%      list.
%
%
*/
MagickExport char **StringToArgv(const char *text,int *argc)
{
  char
    **argv;

  register char
    *p,
    *q;

  register long
    i;

  *argc=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int) (*p)))
      p++;
    (*argc)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  (*argc)++;
  argv=(char **) AcquireMemory((*argc+1)*sizeof(char *));
  if (argv == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to argv",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  argv[0]=GetString("magick");
  p=(char *) text;
  for (i=1; i < *argc; i++)
  {
    while (isspace((int) (*p)))
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++);
      }
    else
      if (*q == '\'')
        {
          for (q++; (*q != '\'') && (*q != '\0'); q++);
          q++;
        }
      else
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
    argv[i]=(char *) AcquireMemory(q-p+MaxTextExtent);
    if (argv[i] == (char *) NULL)
      MagickError(ResourceLimitError,"Unable to convert string to argv",
        "Memory allocation failed");
    (void) strncpy(argv[i],p,q-p);
    argv[i][q-p]='\0';
    p=q;
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  argv[i]=(char *) NULL;
  return(argv);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o D o u b l e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToDouble() converts a text string to a double.  If the string
%  contains a percent sign (e.g. 50%) that percentage of the interval is
%  returned.
%
%  The format of the StringToDouble method is:
%
%      double StringToDouble(const char *text,const double interval)
%
%  A description of each parameter follows:
%
%    o value:  Method StringToDouble returns the converted value.
%
%    o text:  Specifies the string to segment into a list.
%
%    o interval:  Specifies the interval; used for obtaining a percentage.
%
%
*/
MagickExport double StringToDouble(const char *text,const double interval)
{
  char
    *q;

  double
    value;

  value=strtod(text,&q);
  if (strchr(q,'%') != (char *) NULL)
    value*=interval/100.0;
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o L i s t                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToList converts a text string into a list by segmenting the
%  text string at each carriage return discovered.  The list is converted to
%  HEX characters if any control characters are discovered within the text
%  string.
%
%  The format of the StringToList method is:
%
%      char **StringToList(const char *text)
%
%  A description of each parameter follows:
%
%    o list:  Method StringToList returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%
*/
MagickExport char **StringToList(const char *text)
{
  char
    **textlist;

  register char
    *q;

  register const char
    *p;

  register long
    i;

  unsigned long
    lines;

  if (text == (char *) NULL)
    return((char **) NULL);
  for (p=text; *p != '\0'; p++)
    if (((unsigned char) *p < 32) && !isspace((int) (*p)))
      break;
  if (*p == '\0')
    {
      /*
        Convert string to an ASCII list.
      */
      lines=1;
      for (p=text; *p != '\0'; p++)
        if (*p == '\n')
          lines++;
      textlist=(char **) AcquireMemory((lines+MaxTextExtent)*sizeof(char *));
      if (textlist == (char **) NULL)
        MagickError(ResourceLimitError,"Unable to convert text to list",
          "Memory allocation failed");
      p=text;
      for (i=0; i < (long) lines; i++)
      {
        for (q=(char *) p; *q != '\0'; q++)
          if ((*q == '\r') || (*q == '\n'))
            break;
        textlist[i]=(char *) AcquireMemory(q-p+MaxTextExtent);
        if (textlist[i] == (char *) NULL)
          MagickError(ResourceLimitError,"Unable to convert text to list",
            "Memory allocation failed");
        (void) strncpy(textlist[i],p,q-p);
        textlist[i][q-p]='\0';
        if (*q == '\r')
          q++;
        p=q+1;
      }
    }
  else
    {
      char
        hex_string[MaxTextExtent];

      register long
        j;

      /*
        Convert string to a HEX list.
      */
      lines=(strlen(text)/0x14)+1;
      textlist=(char **) AcquireMemory((lines+MaxTextExtent)*sizeof(char *));
      if (textlist == (char **) NULL)
        MagickError(ResourceLimitError,"Unable to convert text",
          "Memory allocation failed");
      p=text;
      for (i=0; i < (long) lines; i++)
      {
        textlist[i]=(char *) AcquireMemory(2*MaxTextExtent);
        if (textlist[i] == (char *) NULL)
          MagickError(ResourceLimitError,"Unable to convert text",
            "Memory allocation failed");
        FormatString(textlist[i],"0x%08lx: ",0x14*i);
        q=textlist[i]+strlen(textlist[i]);
        for (j=1; j <= (long) Min(strlen(p),0x14); j++)
        {
          FormatString(hex_string,"%02lx",*(p+j));
          (void) strncpy(q,hex_string,MaxTextExtent-1);
          q+=2;
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        for (; j <= 0x14; j++)
        {
          *q++=' ';
          *q++=' ';
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        *q++=' ';
        for (j=1; j <= (long) Min(strlen(p),0x14); j++)
        {
          if (isprint((int) (*p)))
            *q++=(*p);
          else
            *q++='-';
          p++;
        }
        *q='\0';
      }
    }
  textlist[i]=(char *) NULL;
  return(textlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i p                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Strip strips any whitespace or quotes from the beginning and end of
%  a string of characters.
%
%  The format of the Strip method is:
%
%      void Strip(char *data)
%
%  A description of each parameter follows:
%
%    o data: Specifies an array of characters.
%
%
*/
MagickExport void Strip(char *data)
{
  long
    count;

  register char
    *p,
    *q;

  assert(data != (char *) NULL);
  if (*data == '\0')
    return;
  if (strlen(data) == 1)
    return;
  p=data;
  while (isspace((int) (*p)))
    p++;
  if ((*p == '\'') || (*p == '"'))
    p++;
  q=data+strlen(data)-1;
  while (isspace((int) (*q)) && (q > p))
    q--;
  if (q > p)
    if ((*q == '\'') || (*q == '"'))
      q--;
  count=q-p+1;
  q=data;
  (void) CloneMemory(q,p,count);
  *(q+count)='\0';
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y s t e m C o m m a n d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SystemCommand executes the specified command and waits until it
%  terminates.  The returned value is the exit status of the command.
%
%  The format of the SystemCommand method is:
%
%      int SystemCommand(const unsigned int verbose,const char *command)
%
%  A description of each parameter follows:
%
%    o status: Method SystemCommand returns False if the command is
%      executed successfully.
%
%    o verbose: An unsigned integer other than 0 prints the executed
%      command before it is invoked.
%
%    o command: This string is the command to execute.
%
%
*/
MagickExport int SystemCommand(const unsigned int verbose,const char *command)
{
  int
    status;

  errno=0;
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  status=system(command);
#endif
#if defined(vms)
  status=!system(command);
#endif
#if defined(macintosh)
  status=MACSystemCommand(command);
#endif
#if defined(WIN32)
  status=NTSystemCommand(command);
#endif
  if (verbose)
    MagickWarning(DelegateWarning,command,!status ? strerror(status) :
      (char *) NULL);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T e m p o r a r y F i l e n a m e                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TemporaryFilename replaces the contents of the string pointed to
%  by filename by a unique file name.
%
%  The format of the TemporaryFilename method is:
%
%      void TemporaryFilename(har *filename)
%
%  A description of each parameter follows.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
*/
MagickExport void TemporaryFilename(char *filename)
{
  assert(filename != (char *) NULL);
  (void) strcpy(filename,"magic");
#if !defined(vms) && !defined(macintosh)
  {
    char
      *name;

    name=(char *) tempnam((char *) NULL,filename);
    if (name == (char *) NULL)
      MagickError(ResourceLimitError,
        "Unable to create a name for a temporary file",(char *) NULL);
    (void) strncpy(filename,name,MaxTextExtent-1);
    LiberateMemory((void **) &name);
  }
#else
  (void) tmpnam(filename);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T o k e n i z e r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Tokenizer is a generalized, finite state token parser.  It extracts
%  tokens one at a time from a string of characters.  The characters used for
%  white space, for break characters, and for quotes can be specified.  Also,
%  characters in the string can be preceded by a specifiable escape character
%  which removes any special meaning the character may have.
%
%  Here is some terminology:
%
%    o token: A single unit of information in the form of a group of
%      characters.
%
%    o white space: Apace that gets ignored (except within quotes or when
%      escaped), like blanks and tabs. in addition, white space terminates a
%      non-quoted token.
%
%    o break set: One or more characters that separates non-quoted tokens.
%      Commas are a common break character. The usage of break characters to
%      signal the end of a token is the same as that of white space, except
%      multiple break characters with nothing or only white space between
%      generate a null token for each two break characters together.
%
%      For example, if blank is set to be the white space and comma is set to
%      be the break character, the line
%
%        A, B, C ,  , DEF
%
%        ... consists of 5 tokens:
%
%        1)  "A"
%        2)  "B"
%        3)  "C"
%        4)  "" (the null string)
%        5)  "DEF"
%
%    o Quote character: A character that, when surrounding a group of other
%      characters, causes the group of characters to be treated as a single
%      token, no matter how many white spaces or break characters exist in
%      the group. Also, a token always terminates after the closing quote.
%      For example, if ' is the quote character, blank is white space, and
%      comma is the break character, the following string
%
%        A, ' B, CD'EF GHI
%
%        ... consists of 4 tokens:
%
%        1)  "A"
%        2)  " B, CD" (note the blanks & comma)
%        3)  "EF"
%        4)  "GHI"
%
%      The quote characters themselves do not appear in the resultant
%      tokens.  The double quotes are delimiters i use here for
%      documentation purposes only.
%
%    o Escape character: A character which itself is ignored but which
%      causes the next character to be used as is.  ^ and \ are often used
%      as escape characters. An escape in the last position of the string
%      gets treated as a "normal" (i.e., non-quote, non-white, non-break,
%      and non-escape) character. For example, assume white space, break
%      character, and quote are the same as in the above examples, and
%      further, assume that ^ is the escape character. Then, in the string
%
%        ABC, ' DEF ^' GH' I ^ J K^ L ^
%
%        ... there are 7 tokens:
%
%        1)  "ABC"
%        2)  " DEF ' GH"
%        3)  "I"
%        4)  " "     (a lone blank)
%        5)  "J"
%        6)  "K L"
%        7)  "^"     (passed as is at end of line)
%
%  The format of the Tokenizer method is:
%
%      int Tokenizer(TokenInfo *token_info,unsigned flag,char *token,
%        size_t max_token_length,char *line,char *white,char *break_set,
%        char *quote,char escape,char *breaker,int *next,char *quoted)
%
%  A description of each parameter follows:
%
%    o flag: right now, only the low order 3 bits are used.
%
%        1 => convert non-quoted tokens to upper case
%        2 => convert non-quoted tokens to lower case
%        0 => do not convert non-quoted tokens
%
%    o token: a character string containing the returned next token
%
%    o max_token_length: the maximum size of "token".  Characters beyond
%      "max_token_length" are truncated.
%
%    o string: the string to be parsed.
%
%    o white: a string of the valid white spaces.  example:
%
%        char whitesp[]={" \t"};
%
%      blank and tab will be valid white space.
%
%    o break: a string of the valid break characters. example:
%
%        char breakch[]={";,"};
%
%      semicolon and comma will be valid break characters.
%
%    o quote: a string of the valid quote characters. An example would be
%
%        char whitesp[]={"'\"");
%
%      (this causes single and double quotes to be valid) Note that a
%      token starting with one of these characters needs the same quote
%      character to terminate it.
%
%      for example:
%
%        "ABC '
%
%      is unterminated, but
%
%        "DEF" and 'GHI'
%
%      are properly terminated.  Note that different quote characters
%      can appear on the same line; only for a given token do the quote
%      characters have to be the same.
%
%    o escape: the escape character (NOT a string ... only one
%      allowed). Use zero if none is desired.
%
%    o breaker: the break character used to terminate the current
%      token.  If the token was quoted, this will be the quote used.  If
%      the token is the last one on the line, this will be zero.
%
%    o next: this variable points to the first character of the
%      next token.  it gets reset by "tokenizer" as it steps through the
%      string.  Set it to 0 upon initialization, and leave it alone
%      after that.  You can change it if you want to jump around in the
%      string or re-parse from the beginning, but be careful.
%
%    o quoted: set to True if the token was quoted and False
%      if not.  You may need this information (for example:  in C, a
%      string with quotes around it is a character string, while one
%      without is an identifier).
%
%    o result: 0 if we haven't reached EOS (end of string), and 1
%      if we have.
%
*/

#define IN_WHITE 0
#define IN_TOKEN 1
#define IN_QUOTE 2
#define IN_OZONE 3

static long sindex(char c,char *string)
{
  register char
    *p;

  for (p=string; *p; p++)
    if (c == (*p))
      return(p-string);
  return(-1);
}

static void StoreToken(TokenInfo *token_info,char *string,
  size_t max_token_length,char c)
{
  register long
    i;

  if ((token_info->offset < 0) ||
      ((size_t) token_info->offset >= (max_token_length-1)))
    return;
  i=token_info->offset++;
  string[i]=c;
  if (token_info->state == IN_QUOTE)
    return;
  switch (token_info->flag & 0x03)
  {
    case 1:
    {
      string[i]=toupper(c);
      break;
    }
    case 2:
    {
      string[i]=tolower(c);
      break;
    }
    default:
      break;
  }
}

MagickExport int Tokenizer(TokenInfo *token_info,unsigned flag,char *token,
  size_t max_token_length,char *line,char *white,char *break_set,char *quote,
  char escape,char *breaker,int *next,char *quoted)
{
  char
    c;

  register long
    i;

  *breaker=False;
  *quoted=False;
  if (!line[*next])
    return(1);
  token_info->state=IN_WHITE;
  token_info->quote=False;
  token_info->flag=flag;
  for (token_info->offset=0; line[*next] != 0; (*next)++)
  {
    c=line[*next];
    i=sindex(c,break_set);
    if (i >= 0)
      {
        switch (token_info->state)
        {
          case IN_WHITE:
          case IN_TOKEN:
          case IN_OZONE:
          {
            (*next)++;
            *breaker=break_set[i];
            token[token_info->offset]=0;
            return(0);
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    i=sindex(c,quote);
    if (i >= 0)
      {
        switch(token_info->state)
        {
          case IN_WHITE:
          {
            token_info->state=IN_QUOTE;
            token_info->quote=quote[i];
            *quoted=True;
            break;
          }
          case IN_QUOTE:
          {
            if (quote[i] != token_info->quote)
              StoreToken(token_info,token,max_token_length,c);
            else
              {
                token_info->state=IN_OZONE;
                token_info->quote=0;
              }
            break;
          }
          case IN_TOKEN:
          case IN_OZONE:
          {
            *breaker=c;
            token[token_info->offset]=0;
            return(0);
          }
        }
        continue;
      }
    i=sindex(c,white);
    if (i >= 0)
      {
        switch(token_info->state)
        {
          case IN_WHITE:
          case IN_OZONE:
            break;
          case IN_TOKEN:
          {
            token_info->state=IN_OZONE;
            break;
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    if (c == escape)
      {
        if (line[(*next)+1] == 0)
          {
            *breaker=0;
            StoreToken(token_info,token,max_token_length,c);
            (*next)++;
            token[token_info->offset]=0;
            return(0);
          }
        switch(token_info->state)
        {
          case IN_WHITE:
          {
            (*next)--;
            token_info->state=IN_TOKEN;
            break;
          }
          case IN_TOKEN:
          case IN_QUOTE:
          {
            (*next)++;
            c=line[*next];
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
          case IN_OZONE:
          {
            token[token_info->offset]=0;
            return(0);
          }
        }
        continue;
      }
    switch(token_info->state)
    {
      case IN_WHITE:
        token_info->state=IN_TOKEN;
      case IN_TOKEN:
      case IN_QUOTE:
      {
        StoreToken(token_info,token,max_token_length,c);
        break;
      }
      case IN_OZONE:
      {
        token[token_info->offset]=0;
        return(0);
      }
    }
  }
  token[token_info->offset]=0;
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s l a t e T e x t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TranslateText replaces any embedded formatting characters with
%  the appropriate image attribute and returns the translated text.
%
%  The format of the TranslateText method is:
%
%      char *TranslateText(const ImageInfo *image_info,Image *image,
%        const char *formatted_text)
%
%  A description of each parameter follows:
%
%    o translated_text:  Method TranslateText returns the translated
%      text string.
%
%    o image_info: The imageInfo.
%
%    o image: The image.
%
%    o formatted_text: The address of a character string containing the embedded
%      formatting characters.
%
%
*/
MagickExport char *TranslateText(const ImageInfo *image_info,Image *image,
  const char *formatted_text)
{
  char
    filename[MaxTextExtent],
    *text,
    *translated_text;

  const ImageAttribute
    *attribute;

  ImageInfo
    *clone_info;

  register char
    *p,
    *q;

  register long
    i;

  size_t
    length;

  assert(image != (Image *) NULL);
  if ((formatted_text == (const char *) NULL) || (*formatted_text == '\0'))
    return((char *) NULL);
  text=(char *) formatted_text;
  if ((*text == '@') && IsAccessible(text+1))
    {
      text=(char *) FileToBlob(text+1,&length,&image->exception);
      if (text == (char *) NULL)
        return((char *) NULL);
    }
  /*
    Translate any embedded format characters.
  */
  length=strlen(text)+MaxTextExtent;
  translated_text=GetString(text);
  clone_info=CloneImageInfo(image_info);
  p=text;
  for (q=translated_text; *p != '\0'; p++)
  {
    *q='\0';
    if ((size_t) (q-translated_text+MaxTextExtent) >= length)
      {
        length<<=1;
        ReacquireMemory((void **) &translated_text,length);
        if (translated_text == (char *) NULL)
          break;
        q=translated_text+strlen(translated_text);
      }
    /*
      Process formatting characters in text.
    */
    if ((*p == '\\') && (*(p+1) == 'r'))
      {
        *q++='\r';
        p++;
        continue;
      }
    if ((*p == '\\') && (*(p+1) == 'n'))
      {
        *q++='\n';
        p++;
        continue;
      }
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'b':
      {
        if (GetBlobSize(image) >= (1 << 24))
          FormatString(q,"%lumb ",(unsigned long)
            (GetBlobSize(image)/1024/1024));
        else
          if (GetBlobSize(image) >= (1 << 16))
            FormatString(q,"%lukb ",(unsigned long) (GetBlobSize(image)/1024));
          else
            FormatString(q,"%lub ",(unsigned long) GetBlobSize(image));
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'c':
      {
        attribute=GetImageAttribute(image,"comment");
        if (attribute == (ImageAttribute *) NULL)
          break;
        (void) strncpy(q,attribute->value,MaxTextExtent-1);
        q+=strlen(attribute->value);
        break;
      }
      case 'd':
      case 'e':
      case 'f':
      case 't':
      {
        /*
          Label segment is the base of the filename.
        */
        if (strlen(image->magick_filename) == 0)
          break;
        switch (*p)
        {
          case 'd':
          {
            GetPathComponent(image->magick_filename,HeadPath,filename);
            (void) strncpy(q,filename,MaxTextExtent-1);
            q+=strlen(filename);
            break;
          }
          case 'e':
          {
            GetPathComponent(image->magick_filename,ExtensionPath,filename);
            (void) strncpy(q,filename,MaxTextExtent-1);
            q+=strlen(filename);
            break;
          }
          case 'f':
          {
            GetPathComponent(image->magick_filename,TailPath,filename);
            (void) strncpy(q,filename,MaxTextExtent-1);
            q+=strlen(filename);
            break;
          }
          case 't':
          {
            GetPathComponent(image->magick_filename,BasePath,filename);
            (void) strncpy(q,filename,MaxTextExtent-1);
            q+=strlen(filename);
            break;
          }
        }
        break;
      }
      case 'g':
      {
        FormatString(q,"0x%lx",clone_info->group);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'h':
      {
        FormatString(q,"%lu",image->magick_rows ? image->magick_rows : 256);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'i':
      {
        (void) strncpy(q,image->filename,MaxTextExtent-1);
        q+=strlen(image->filename);
        break;
      }
      case 'k':
      {
        FormatString(q,"%lu",GetNumberColors(image,(FILE *) NULL,
          &image->exception));
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'l':
      {
        attribute=GetImageAttribute(image,"label");
        if (attribute == (ImageAttribute *) NULL)
          break;
        (void) strncpy(q,attribute->value,MaxTextExtent-1);
        q+=strlen(attribute->value);
        break;
      }
      case 'm':
      {
        (void) strncpy(q,image->magick,MaxTextExtent-1);
        q+=strlen(image->magick);
        break;
      }
      case 'n':
      {
        FormatString(q,"%lu",GetImageListSize(image));
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'o':
      {
        (void) strncpy(q,clone_info->filename,MaxTextExtent-1);
        q+=strlen(clone_info->filename);
        break;
      }
      case 'p':
      {
        register const Image
          *p;

        unsigned long
          page;

        p=image;
        for (page=1; p->previous != (Image *) NULL; page++)
          p=p->previous;
        FormatString(q,"%lu",page);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'q':
      {
        FormatString(q,"%lu",image->depth);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 's':
      {
        FormatString(q,"%lu",image->scene);
        if (clone_info->subrange != 0)
          FormatString(q,"%lu",clone_info->subimage);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'u':
      {
        (void) strncpy(filename,clone_info->unique,MaxTextExtent-1);
        if (*filename == '\0')
          TemporaryFilename(filename);
        (void) strncpy(q,filename,MaxTextExtent-1);
        q+=strlen(filename);
        break;
      }
      case 'w':
      {
        FormatString(q,"%lu",
          image->magick_columns ? image->magick_columns : 256);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'x':
      {
        FormatString(q,"%g",image->x_resolution);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'y':
      {
        FormatString(q,"%g",image->y_resolution);
        q=translated_text+strlen(translated_text);
        break;
      }
      case 'z':
      {
        (void) strncpy(filename,clone_info->zero,MaxTextExtent-1);
        if (*filename == '\0')
          TemporaryFilename(filename);
        (void) strncpy(q,filename,MaxTextExtent-1);
        q+=strlen(filename);
        break;
      }
      case '[':
      {
        char
          key[MaxTextExtent];

        off_t
          offset;

        if (strchr(p,']') == (char *) NULL)
          break;
        p++;
        for (i=0; (i < MaxTextExtent) && (*p != ']'); i++)
          key[i]=(*p++);
        key[i]='\0';
        attribute=GetImageAttribute(image,key);
        if (attribute != (const ImageAttribute *) NULL)
          {
            offset=strlen(attribute->value);
            if ((size_t) (q-translated_text+offset) >= length)
              {
                length+=(offset+MaxTextExtent);
                ReacquireMemory((void **) &translated_text,length);
                if (translated_text == (char *) NULL)
                  break;
                q=translated_text+strlen(translated_text);
              }
            (void) strcpy(q,attribute->value);
            q+=offset;
            break;
          }
        attribute=GetImageInfoAttribute(clone_info,image,key);
        if (attribute == (ImageAttribute *) NULL)
          break;
        offset=strlen(attribute->value);
        if ((size_t) (q-translated_text+offset) >= length)
          {
            length+=(offset+MaxTextExtent);
            ReacquireMemory((void **) &translated_text,length);
            if (translated_text == (char *) NULL)
              break;
            q=translated_text+strlen(translated_text);
          }
        (void) strcpy(q,attribute->value);
        q+=offset;
        break;
      }
      case '#':
      {
        (void) SignatureImage(image);
        attribute=GetImageAttribute(image,"signature");
        if (attribute == (ImageAttribute *) NULL)
          break;
        (void) strncpy(q,attribute->value,MaxTextExtent-1);
        q+=strlen(attribute->value);
        break;
      }
      case '%':
      {
        *q++=(*p);
        break;
      }
      default:
      {
        *q++='%';
        *q++=(*p);
        break;
      }
    }
  }
  *q='\0';
  DestroyImageInfo(clone_info);
  if (text != (char *) formatted_text)
    LiberateMemory((void **) &text);
  return(translated_text);
}
