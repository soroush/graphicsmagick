/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               V   V  EEEEE  RRRR   SSSSS  IIIII   OOO   N   N               %
%               V   V  E      R   R  SS       I    O   O  NN  N               %
%               V   V  EEE    RRRR    SSS     I    O   O  N N N               %
%                V V   E      R R       SS    I    O   O  N  NN               %
%                 V    EEEEE  R  R   SSSSS  IIIII   OOO   N   N               %
%                                                                             %
%                                                                             %
%                         ImageMagick Version Methods                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
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

#include "studio.h"
#include "version.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k C o p y r i g h t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickCopyright() returns the ImageMagick API copyright as a string.
%
%  The format of the GetMagickCopyright method is:
%
%      const char *GetMagickCopyright(void)
%
%
*/
MagickExport const char *GetMagickCopyright(void)
{
  return(MagickCopyright);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k V e r s i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickVersion() returns the ImageMagick API version as a string and
%  as a number.
%
%  The format of the GetMagickVersion method is:
%
%      const char *GetMagickVersion(unsigned long *version)
%
%  A description of each parameter follows:
%
%    o version: The ImageMagick version is returned as a number.
%
*/
MagickExport const char *GetMagickVersion(unsigned long *version)
{
  if (version != (unsigned long *) NULL)
    *version=MagickLibVersion;
  return(MagickVersion);
}
