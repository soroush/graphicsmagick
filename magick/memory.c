/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                    M   M  EEEEE  M   M   OOO   RRRR   Y   Y                 %
%                    MM MM  E      MM MM  O   O  R   R   Y Y                  %
%                    M M M  EEE    M M M  O   O  RRRR     Y                   %
%                    M   M  E      M   M  O   O  R R      Y                   %
%                    M   M  EEEEE  M   M   OOO   R  R     Y                   %
%                                                                             %
%                                                                             %
%                    ImageMagick Memory Allocation Methods                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1998                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e M e m o r y                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireMemory() returns a pointer to a block of memory at least size
%  bytes suitably aligned for any use.
%
%  The format of the AcquireMemory method is:
%
%      void *AcquireMemory(const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *AcquireMemory(const size_t size)
{
  void
    *allocation;

  assert(size != 0);
  allocation=malloc(size);
  return(allocation);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e M e m o r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneMemory() copies size bytes from memory area source to the
%  destination.  Copying between objects that overlap will take place
%  correctly.  It returns destination.
%
%  The format of the CloneMemory method is:
%
%      void *CloneMemory(void *destination,const void *source,const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *CloneMemory(void *destination,const void *source,
  const size_t size)
{
  register unsigned char
    *q;

  register const unsigned char
    *p;

  register long
    i;

  assert(destination != (void *) NULL);
  assert(source != (const void *) NULL);
  p=(const unsigned char *) source;
  q=(unsigned char *) destination;
  if ((p <= q) || ((p+size) >= q))
    return(memcpy(destination,source,size));
  /*
    Overlap, copy backwards.
  */
  p+=size;
  q+=size;
  for (i=(long) (size-1); i >= 0; i--)
    *--q=(*--p);
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e M e m o r y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateMemory() frees memory that has already been allocated, and
%  NULLs the pointer to it.
%
%  The format of the LiberateMemory method is:
%
%      void LiberateMemory(void **memory)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a block of memory to free for reuse.
%
%
*/
MagickExport void LiberateMemory(void **memory)
{
  assert(memory != (void **) NULL);
  if (*memory == (void *) NULL)
    return;
  free(*memory);
  *memory=(void *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a c q u i r e M e m o r y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReacquireMemory() changes the size of the memory and returns a
%  pointer to the (possibly moved) block.  The contents will be unchanged
%  up to the lesser of the new and old sizes.
%
%  The format of the ReacquireMemory method is:
%
%      void ReacquireMemory(void **memory,const size_t size)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a memory allocation.  On return the pointer
%      may change but the contents of the original allocation will not.
%
%    o size: The new size of the allocated memory.
%
%
*/
MagickExport void ReacquireMemory(void **memory,const size_t size)
{
  void
    *allocation;

  assert(memory != (void **) NULL);
  if (*memory == (void *) NULL)
    {
      *memory=AcquireMemory(size);
      return;
    }
  allocation=realloc(*memory,size);
  if (allocation == (void *) NULL)
    LiberateMemory((void **) memory);
  *memory=allocation;
}
