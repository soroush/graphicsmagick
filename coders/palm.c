/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        PPPP    AAA   L      M   M                           %
%                        P   P  A   A  L      MM MM                           %
%                        PPPP   AAAAA  L      M M M                           %
%                        P      A   A  L      M   M                           %
%                        P      A   A  LLLLL  M   M                           %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                            Christopher R. Hawks                             %
%                               December 2001                                 %
%                                                                             %
%                                                                             %
%  Copyright (C 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick",  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  Licensor ("ImageMagick Studio LLC") warrants that the copyright in and to  %
%  the Software ("ImageMagick") is owned by ImageMagick Studio LLC or is      %
%  distributed by ImageMagick Studio LLC under a valid current license.       %
%  Except as expressly stated in the immediately preceding sentence,          %
%  ImageMagick is provided by ImageMagick Studio LLC, distributors, and       %
%  copyright owners "AS IS", without warranty of any kind, express or         %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and non-infringement.  In no event shall  %
%  ImageMagick Studio LLC, contributors or copyright owners be liable for     %
%  any claim, damages, or other liability, whether in an action of contract,  %
%  tort or otherwise, arising from, out of or in connection with              %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Based on pnmtopalm by Bill Janssen and ppmtobmp by Ian Goldberg.
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "magick.h"
#include "quantize.h"
#include "utility.h"

#define PALM_IS_COMPRESSED_FLAG  0x8000
#define PALM_HAS_COLORMAP_FLAG  0x4000
#define PALM_HAS_TRANSPARENCY_FLAG  0x2000
#define PALM_IS_INDIRECT  0x1000
#define PALM_IS_FOR_SCREEN  0x0800
#define PALM_IS_DIRECT_COLOR  0x0400
#define PALM_HAS_FOUR_BYTE_FIELD  0x0200
#define PALM_COMPRESSION_SCANLINE  0x00
#define PALM_COMPRESSION_RLE  0x01
#define PALM_COMPRESSION_NONE  0xFF

/*
 The 256 color system palette for Palm Computing Devices.
*/
static Quantum
  PalmPalette[256][3] = 
  {
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(255)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(204)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(153)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(255), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(204), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(153), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(102), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 51), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(102)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum( 51)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(255),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(204),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(153),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(102),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum( 51),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum( 17), ScaleCharToQuantum( 17),ScaleCharToQuantum( 17)},
    {ScaleCharToQuantum( 34), ScaleCharToQuantum( 34),ScaleCharToQuantum( 34)},
    {ScaleCharToQuantum( 68), ScaleCharToQuantum( 68),ScaleCharToQuantum( 68)},
    {ScaleCharToQuantum( 85), ScaleCharToQuantum( 85),ScaleCharToQuantum( 85)},
    {ScaleCharToQuantum(119), ScaleCharToQuantum(119),ScaleCharToQuantum(119)},
    {ScaleCharToQuantum(136), ScaleCharToQuantum(136),ScaleCharToQuantum(136)},
    {ScaleCharToQuantum(170), ScaleCharToQuantum(170),ScaleCharToQuantum(170)},
    {ScaleCharToQuantum(187), ScaleCharToQuantum(187),ScaleCharToQuantum(187)},
    {ScaleCharToQuantum(221), ScaleCharToQuantum(221),ScaleCharToQuantum(221)},
    {ScaleCharToQuantum(238), ScaleCharToQuantum(238),ScaleCharToQuantum(238)},
    {ScaleCharToQuantum(192), ScaleCharToQuantum(192),ScaleCharToQuantum(192)},
    {ScaleCharToQuantum(128), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(128), ScaleCharToQuantum(  0),ScaleCharToQuantum(128)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(128),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(128),ScaleCharToQuantum(128)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)},
    {ScaleCharToQuantum(  0), ScaleCharToQuantum(  0),ScaleCharToQuantum(  0)}
  };

/*
  Forward declarations.
*/
static unsigned int
  WritePALMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i n d C o l o r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FindColor returns the index of the matching entry from
%  PalmPalette for a given PixelPacket.
%
%  The format of the FindColor method is:
%
%      Int FindColor(PixelPacket *pixel)
%
%  A description of each parameter follows:
%
%    o int: the index of the matching color or -1 if not found/
%
%    o pixel: a pointer to the PixelPacket to be matched.
%
%
*/
static int FindColor(PixelPacket *pixel)
{
  register long
    i;

  for (i=0; i < 256; i++)
    if ((pixel->red == ScaleCharToQuantum(PalmPalette[i][0])) &&
        (pixel->green == ScaleCharToQuantum(PalmPalette[i][1])) &&
        (pixel->blue == ScaleCharToQuantum(PalmPalette[i][2])))
      return(i);
  return(-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P A L M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPALMImage reads an image of raw bites in LSB order and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPALMImage method is:
%
%      Image *ReadPALMImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPALMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPALMImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned char
    *lastrow,
    *one_row,
    *ptr;

  unsigned int
    status;

  unsigned long
    bytes_per_row,
    flags,
    bpp,
    version,
    nextDepthOffset,
    transparentIndex,
    compressionType,
    byte,
    count,
    mask,
    pad,
    size,
    bit;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  image->columns = ReadBlobMSBShort(image);
  image->rows = ReadBlobMSBShort(image);
  bytes_per_row = ReadBlobMSBShort(image);
  flags = ReadBlobMSBShort(image);
  bpp = ReadBlobByte(image);
  version = ReadBlobByte(image);
  nextDepthOffset = ReadBlobMSBShort(image);
  transparentIndex = ReadBlobByte(image);
  compressionType = ReadBlobByte(image);
  pad = ReadBlobMSBShort(image);

  /*
    Initialize image colormap.
  */
  if (!AllocateImageColormap(image,1L << bpp))
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);

  if(bpp < 8 && flags & PALM_IS_COMPRESSED_FLAG)    /* compressed size */
    {
    if(flags & PALM_HAS_FOUR_BYTE_FIELD)  /* big size */
      size = ReadBlobMSBLong(image);
    else
      size = ReadBlobMSBShort(image);
    }
  else  /* is color */
  if(bpp == 8)
    {
    i = 0;
    if(flags & PALM_HAS_COLORMAP_FLAG)
      {
      count = ReadBlobMSBShort(image);
      for(i = 0; i < (long) count; i++)
        {
        ReadBlobByte(image);
        image->colormap[255 - i].red = ScaleCharToQuantum(ReadBlobByte(image));
        image->colormap[255 - i].green = ScaleCharToQuantum(ReadBlobByte(image));
        image->colormap[255 - i].blue = ScaleCharToQuantum(ReadBlobByte(image));
        }
      }
    for(; i < (long) (1L << bpp); i++)
      {
      image->colormap[255 - i].red = ScaleCharToQuantum(PalmPalette[i][0]);
      image->colormap[255 - i].green = ScaleCharToQuantum(PalmPalette[i][1]);
      image->colormap[255 - i].blue = ScaleCharToQuantum(PalmPalette[i][2]);
      }
    }

  image->storage_class = PseudoClass;
  image->depth = 8;

  one_row = (unsigned char *) AcquireMemory(bytes_per_row);
  if (one_row == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  if (compressionType == PALM_COMPRESSION_SCANLINE)
    lastrow = (unsigned char *) AcquireMemory(bytes_per_row);

  mask = (1l << bpp) - 1;

  for(y = 0; y < (long) image->rows; y++)
    {
    if (compressionType == PALM_COMPRESSION_RLE)
      {
      image->compression = RLECompression;
      for (i = 0; i < (long) bytes_per_row; )
        {
        count = ReadBlobByte(image);
        byte = ReadBlobByte(image);
        memset(one_row + i, (int) byte, count);
        i += count;
        }
      }
    else
    if (compressionType == PALM_COMPRESSION_SCANLINE)
      {
      image->compression = FaxCompression;
      for (i = 0; i < (long) bytes_per_row; i += 8)
        {
        count = ReadBlobByte(image);
        byte = Min(bytes_per_row - i, 8);
        for (bit = 0; bit < byte; bit++)
          {
          if (y == 0 || count & (1 << (7 - bit)))
            one_row[i + bit] = ReadBlobByte(image);
          else
            one_row[i + bit] = lastrow[i + bit];
          }
        }
      memcpy(lastrow, one_row, bytes_per_row);
      }
    else
      {
      image->compression = NoCompression;
      (void) ReadBlob(image, bytes_per_row, one_row);
      }

    ptr = one_row;
    q = SetImagePixels(image, 0, y, image->columns, 1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit = 8 - bpp;
    for(x = 0; x < (long) image->columns; x++)
      {
      index =(IndexPacket) (mask - (((*ptr) & (mask << bit)) >> bit));
      indexes[x] = index;
      *q++ = image->colormap[index];
      if (!bit)
        {
        ++ptr;
        bit = 8 - bpp;
        }
      else
        {
        bit -= bpp;
        }
      }
    }
  LiberateMemory((void **) &one_row);
  if (compressionType == PALM_COMPRESSION_SCANLINE)
    LiberateMemory((void **) &lastrow);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P A L M I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPALMImage adds attributes for the PALM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPALMImage method is:
%
%      RegisterPALMImage(void)
%
*/
ModuleExport void RegisterPALMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PALM");
  entry->decoder=ReadPALMImage;
  entry->encoder=WritePALMImage;
  entry->adjoin=False;
  entry->description=AcquireString("Palm pixmap");
  entry->module=AcquireString("PALM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P A L M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPALMImage removes format registrations made by the
%  PALM module from the list of supported formats.
%
%  The format of the UnregisterPALMImage method is:
%
%      UnregisterPALMImage(void)
%
*/
ModuleExport void UnregisterPALMImage(void)
{
  UnregisterMagickInfo("PALM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P A L M I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePALMImage writes an image of raw bits in LSB order to a file.
%
%  The format of the WritePALMImage method is:
%
%      unsigned int WritePALMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePALMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePALMImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  QuantizeInfo
    quantize_info;

  Image
    *map;

  ExceptionInfo
    exception;

  const ImageAttribute
    *attribute;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *p;

  unsigned char
    version = 0,
    bit,
    byte,
    color,
    *lastrow,
    *one_row,
    *ptr;

  unsigned int
    status;

  unsigned long
    count,
    bpp,
    bytes_per_row;

  unsigned short
    flags = 0;


  GetExceptionInfo(&exception);
  attribute = GetImageAttribute(image, "Comment");
  if(attribute != (ImageAttribute *)NULL)
    if(0 == LocaleCompare("COLORMAP", attribute->value))
      flags |= PALM_HAS_COLORMAP_FLAG;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info, image, WriteBinaryBlobMode, &exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);

  for (bpp = 1;  (1 << bpp) < (long) image->colors;  bpp *= 2)
      ;

  if(!IsGrayImage(image, &exception))  /* is color */
    bpp = 8;

  (void) SortColormapByIntensity(image);

  /* Write Tbmp header. */
  (void) WriteBlobMSBShort(image, image->columns);  /* width */
  (void) WriteBlobMSBShort(image, image->rows );  /* height */
  /* bytes per row - always a word boundary */
  bytes_per_row = ((image->columns + (16 / bpp - 1)) / (16 / bpp)) * 2;
  (void) WriteBlobMSBShort(image, bytes_per_row);
  if(image->compression == RLECompression
      || image->compression == FaxCompression)
    flags |= PALM_IS_COMPRESSED_FLAG;
  if(bytes_per_row * image->rows > 48000 && flags & PALM_IS_COMPRESSED_FLAG)
    flags |= PALM_HAS_FOUR_BYTE_FIELD;
  (void) WriteBlobMSBShort(image, flags);
  (void) WriteBlobByte(image, bpp);
  if(bpp > 1)
    version = 1;
  if(image->compression == RLECompression
      || image->compression == FaxCompression)
    version = 2;
  (void) WriteBlobByte(image,version);
  (void) WriteBlobMSBShort(image, 0);  /* offset */
  (void) WriteBlobByte(image,0);  /* trans index */
  if(image->compression == RLECompression)
    (void) WriteBlobByte(image, PALM_COMPRESSION_RLE);
  else
  if(image->compression == FaxCompression)
    (void) WriteBlobByte(image, PALM_COMPRESSION_SCANLINE);
  else
    (void) WriteBlobByte(image, PALM_COMPRESSION_NONE);
  (void) WriteBlobMSBShort(image, 0);  /* reserved */

  if(bpp < 8)  /* not color */
    {
    if(flags & PALM_IS_COMPRESSED_FLAG)  /* compressed size */
      {
      if(flags & PALM_HAS_FOUR_BYTE_FIELD)  /* big size */
        WriteBlobMSBLong(image, 0);
      else
        WriteBlobMSBShort(image, 0);
      }
    }
  else  /* is color */
    {
    if(flags & PALM_HAS_COLORMAP_FLAG)  /* Write out colormap */
      {
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      quantize_info.number_colors=image->colors;
      (void) QuantizeImage(&quantize_info,image);
      WriteBlobMSBShort(image, image->colors);
      for(count = 0; count < image->colors; count++)
        {
        WriteBlobByte(image, count);
        WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].red));
        WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].green));
        WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].blue));
        }
      }
    else  /* Map colors to Palm standard colormap */
      {
      map = ConstituteImage(256, 1, "RGB", IntegerPixel,
                      &PalmPalette, &exception);
      SetImageType(map, PaletteType);
      MapImage(image, map, False);
      for(y = 0; y < (long) image->rows; y++)
        {
        p = GetImagePixels(image, 0, y, image->columns, 1);
        indexes=GetIndexes(image);
        for(x = 0; x < (long) image->columns; x++)
          indexes[x] = FindColor(&image->colormap[indexes[x]]);
        }
      DestroyImage(map);
      }
    }

  if (image->compression == FaxCompression)
    lastrow = (unsigned char *) AcquireMemory(bytes_per_row);
  one_row = (unsigned char *) AcquireMemory(bytes_per_row);
  if (one_row == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,
                                "Memory allocation failed", image);

  for (y=0; y < (int) image->rows; y++)
    {
    ptr = one_row;
    memset(ptr, 0, bytes_per_row);
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    byte = 0x00;
    bit = (unsigned char) (8 - bpp);
    for (x=0; x < (int) image->columns; x++)
      {
      if(bpp < 8) /* Make sure we use the entire colorspace for bpp */
        color = (unsigned char) (indexes[x] * ((1 << bpp) - 1) /
          (image->colors - 1));
      else
        color = (unsigned char) indexes[x];
      byte |= color << bit;
      if (bit == 0)
        {
        *ptr++ = byte;
        byte = 0x00;
        bit = (unsigned char) (8 - bpp);
        }
      else
        {
        bit -= bpp;
        }
      }
    if ((image->columns % (8 / bpp)) != 0) /* Handle case of partial byte */
      *ptr++ = byte;

    if(image->compression == RLECompression)
      {
      x = 0;
      while(x < (long) bytes_per_row)
        {
        byte = one_row[x];
        count = 1;
        while(one_row[++x] == byte && count < 255 && x < (long) bytes_per_row)
          count++;
        (void) WriteBlobByte(image, count);
        (void) WriteBlobByte(image, byte);
        }
      }
    else
    if(image->compression == FaxCompression)  /* Scanline really */
      {
      char tmpbuf[8];
      char *tptr;

      for (x = 0;  x < (long) bytes_per_row;  x += 8)
        {
        tptr = tmpbuf;
        for(bit = 0, byte = 0; bit < Min(8, bytes_per_row - x); bit++)
          {
          if((y == 0) || (lastrow[x + bit] != one_row[x + bit]))
            {
            byte |= (1 << (7 - bit));
            *tptr++ = one_row[x + bit];
            }
          }
        (void) WriteBlobByte(image, byte);
        (void) WriteBlob(image, tptr - tmpbuf, tmpbuf);
       }
      memcpy (lastrow, one_row, bytes_per_row);
      }
    else
      {
      (void) WriteBlob(image, bytes_per_row, one_row);
      }
    }

  if(bpp < 8 && flags & PALM_IS_COMPRESSED_FLAG)
    {
    count = (unsigned long) SizeBlob(image);  /* compressed size */
    SeekBlob(image, 16, SEEK_SET);
    if(flags & PALM_HAS_FOUR_BYTE_FIELD)
      (void) WriteBlobMSBLong(image, count - 16);
    else
      (void) WriteBlobMSBShort(image, count - 16);
    }

  CloseBlob(image);
  LiberateMemory((void **) &one_row);
  if (image->compression == FaxCompression)
    LiberateMemory((void **) &lastrow);
  DestroyExceptionInfo(&exception);
  return(True);
}
