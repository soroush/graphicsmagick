/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            U   U  IIIII  L                                  %
%                            U   U    I    L                                  %
%                            U   U    I    L                                  %
%                            U   U    I    L                                  %
%                             UUU   IIIII  LLLLL                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
  Forward declarations.
*/
static unsigned int
  WriteUILImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r U I L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterUILImage adds attributes for the UIL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterUILImage method is:
%
%      RegisterUILImage(void)
%
*/
Export void RegisterUILImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("UIL");
  entry->encoder=WriteUILImage;
  entry->adjoin=False;
  entry->description=AllocateString("X-Motif UIL table");
  entry->module=AllocateString("UIL");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r U I L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterUILImage removes format registrations made by the
%  UIL module from the list of supported formats.
%
%  The format of the UnregisterUILImage method is:
%
%      UnregisterUILImage(void)
%
*/
Export void UnregisterUILImage(void)
{
  UnregisterMagickInfo("UIL");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U I L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteUILImage writes an image to a file in the X-Motif UIL table
%  format.
%
%  The format of the WriteUILImage method is:
%
%      unsigned int WriteUILImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUILImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteUILImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    buffer[MaxTextExtent],
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  double
    distance_squared,
    min_distance;

  int
    distance,
    j,
    k,
    y;

  long
    mean;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p;

  register const ColorlistInfo
    *q;

  unsigned int
    characters_per_pixel,
    colors,
    status,
    transparent;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  transparent=False;
  i=0;
  if (image->class == PseudoClass)
    colors=image->colors;
  else
    {
      QuantizeInfo
        quantize_info;

      unsigned char
        *matte_image;

      /*
        Convert DirectClass to PseudoClass image.
      */
      matte_image=(unsigned char *) NULL;
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          matte_image=(unsigned char *)
            AllocateMemory(image->columns*image->rows);
          if (matte_image == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitWarning,
              "Memory allocation failed",image);
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              matte_image[i]=p->opacity == Transparent;
              if (matte_image[i])
                transparent=True;
              i++;
              p++;
            }
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      colors=image->colors;
      if (transparent)
        {
          colors++;
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (int) image->columns; x++)
            {
              if (matte_image[i])
                indexes[x]=image->colors;
              p++;
            }
          }
        }
      if (matte_image != (unsigned char *) NULL)
        FreeMemory((void **) &matte_image);
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (int) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    UIL header.
  */
  (void) strcpy(buffer,"/* UIL */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  FormatString(buffer,"value\n  %.1024s_ct : color_table(\n",
    BaseFilename(image->filename));
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (i=0; i < (int) colors; i++)
  {
    PixelPacket
      *p;

    /*
      Define UIL color.
    */
    min_distance=0;
    p=image->colormap+i;
    for (q=XPMColorlist; q->name != (char *) NULL; q++)
    {
      mean=(p->red+q->red)/2;
      distance=p->red-(int) q->red;
      distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
      distance=p->green-(int) q->green;
      distance_squared+=4.0*distance*distance;
      distance=p->blue-(int) q->blue;
      distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
      if ((q == XPMColorlist) || (distance_squared <= min_distance))
        {
          min_distance=distance_squared;
          (void) strcpy(name,q->name);
        }
    }
    if (transparent)
      if (i == (int) (colors-1))
        (void) strcpy(name,"None");
    /*
      Write UIL color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    if (LocaleCompare(name,"None") == 0)
      FormatString(buffer,"    background color = '%.1024s'",symbol);
    else
      FormatString(buffer,"    color('%.1024s',%.1024s) = '%.1024s'",
        name,Intensity(*p) < ((MaxRGB+1)/2) ? "background" : "foreground",
        symbol);
    (void) WriteBlob(image,strlen(buffer),buffer);
    FormatString(buffer,"%.1024s",(i == (int) (colors-1) ? ");\n" : ",\n"));
    (void) WriteBlob(image,strlen(buffer),buffer);
  }
  /*
    Define UIL pixels.
  */
  FormatString(buffer,
    "  %.1024s_icon : icon(color_table = %.1024s_ct,\n",
    BaseFilename(image->filename),BaseFilename(image->filename));
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) strcpy(buffer,"    \"");
    (void) WriteBlob(image,strlen(buffer),buffer);
    for (x=0; x < (int) image->columns; x++)
    {
      k=indexes[x] % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) indexes[x]-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      FormatString(buffer,"%.1024s",symbol);
      (void) WriteBlob(image,strlen(buffer),buffer);
      p++;
    }
    FormatString(buffer,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? ");" : ","));
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SaveImageText,y,image->rows);
  }
  CloseBlob(image);
  return(True);
}
