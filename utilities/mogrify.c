/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              M   M   OOO    GGGG  RRRR   IIIII  FFFFF  Y   Y                %
%              MM MM  O   O  G      R   R    I    F       Y Y                 %
%              M M M  O   O  G GG   RRRRR    I    FFF      Y                  %
%              M   M  O   O  G   G  R R      I    F        Y                  %
%              M   M   OOO    GGG   R  R   IIIII  F        Y                  %
%                                                                             %
%                                                                             %
%               Transmogrify an Image or Sequence of Images.                  %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            December 1992                                    %
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
%  `Mogrify' applies one or more image transforms to an image or sequence of
%   images and overwrites the original image.
%
%  The Mogrify program command syntax is:
%
%  Usage: mogrify [ -option value ... ] file [ [ -option value ... ] file ... ]
%
%  Where options include:
%    -affine matrix       drawing transform matrix
%    -antialias           remove pixel-aliasing
%    -background color    background color
%    -blur geometry       blur the image
%    -border geometry     surround image with a border of color
%    -bordercolor color   border color
%    -box color           color for annotation bounding box
%    -cache threshold     megabytes of memory available to the pixel cache
%    -channel type        extract a particular color channel from image
%    -charcoal radius     simulate a charcoal drawing
%    -chop geometry       remove pixels from the image interior
%    -colorize value      colorize the image with the fill color
%    -colors value        preferred number of colors in the image
%    -colorspace type     alternate image colorspace
%    -comment string      annotate image with comment
%    -compress type       type of image compression
%    -contrast            enhance or reduce the image contrast
%    -crop geometry       preferred size and location of the cropped image
%    -cycle amount        cycle the image colormap
%    -debug               display copious debugging information
%    -delay value         display the next image after pausing
%    -density geometry    vertical and horizontal density of the image
%    -depth value         depth of the image
%    -despeckle           reduce the speckles within an image",
%    -display server      obtain image or font from this X server
%    -dispose method      GIF disposal method
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -draw string         annotate the image with a graphic primitive
%    -edge radius         apply a filter to detect edges in the image
%    -emboss radius       emboss an image
%    -enhance             apply a digital filter to enhance a noisy image
%    -equalize            perform histogram equalization to an image
%    -fill color          color to use when filling a graphic primitive
%    -filter type         use this filter when resizing an image
%    -flip                flip image in the vertical direction
%    -flop                flop image in the horizontal direction
%    -font name           font for rendering text
%    -format type         image format type
%    -frame geometry      surround image with an ornamental border
%    -fuzz distance       colors within this distance are considered equal
%    -gamma value         level of gamma correction
%    -gaussian geometry   blur an image
%    -geometry geometry   perferred size or location of the image
%    -help                print program options
%    -implode amount      implode image pixels about the center
%    -interlace type      None, Line, Plane, or Partition
%    -label name          assign a label to an image
%    -level value         adjust the level of image contrast
%    -list type           Color, Delegate, Format, Magic, Module, or Type
%    -loop iterations     add Netscape loop extension to your GIF animation
%    -map filename        transform image colors to match this set of colors
%    -matte               store matte channel if the image has one
%    -median radius       apply a median filter to the image
%    -modulate value      vary the brightness, saturation, and hue
%    -monochrome          transform image to black and white
%    -negate              replace every pixel with its complementary color
%    -noise radius        add or reduce noise in an image
%    -normalize           transform image to span the full range of colors
%    -opaque color        change this color to the fill color
%    -page geometry       size and location of an image canvas
%    -paint radius        simulate an oil painting
%    -pointsize value     pointsize of Postscript font
%    -profile filename    add ICM or IPTC information profile to image
%    -quality value       JPEG/MIFF/PNG compression level
%    -raise value         lighten/darken image edges to create a 3-D effect
%    -region geometry     apply options to a portion of the image
%    -resize geometry     resize the image
%    -roll geometry       roll an image vertically or horizontally
%    -rotate degrees      apply Paeth rotation to the image
%    -sample geometry     scale image with pixel sampling
%    -scale geometry      scale the image
%    -scene value         image scene number
%    -seed value          pseudo-random number generator seed value
%    -segment values      segment an image
%    -shade degrees       shade the image using a distant light source
%    -sharpen geometry    sharpen the image
%    -shear geometry      slide one edge of the image along the X or Y axis
%    -size geometry       width and height of image
%    -solarize threshold  negate all pixels above the threshold level
%    -spread amount       displace image pixels by a random amount
%    -stroke color        color to use when stoking a graphic primitive
%    -strokewidth value   width of stroke
%    -swirl degrees       swirl image pixels about the center
%    -texture filename    name of texture to tile onto the image background
%    -threshold value     threshold the image
%    -tile filename       tile image when filling a graphic primitive
%    -transparent color   make this color transparent within the image
%    -treedepth value     depth of the color tree
%    -type type           image type
%    -units type          PixelsPerInch, PixelsPerCentimeter, or Undefined
%    -unsharp geometry    sharpen the image
%    -verbose             print detailed information about the image
%    -view                FlashPix viewing transforms
%    -wave geometry       alter an image along a sine wave
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. ps:image) or specify the
%  image type as the filename suffix (i.e. image.ps).  Specify 'file' as
%  '-' for standard input or output.
%
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Usage displays the program command syntax.
%
%  The format of the Usage method is:
%
%      void Usage()
%
%
*/
static void Usage(void)
{
  static const char
    *options[]=
    {
      "-affine matrix       drawing transform matrix",
      "-antialias           remove pixel-aliasing",
      "-background color    background color",
      "-blur radius         blur the image",
      "-border geometry     surround image with a border of color",
      "-bordercolor color   border color",
      "-box color           color for annotation bounding box",
      "-cache threshold     megabytes of memory available to the pixel cache",
      "-channel type        extract a particular color channel from image",
      "-charcoal radius     simulate a charcoal drawing",
      "-chop geometry       remove pixels from the image interior",
      "-colorize value      colorize the image with the fill color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       type of image compression",
      "-contrast            enhance or reduce the image contrast",
      "-crop geometry       preferred size and location of the cropped image",
      "-cycle amount        cycle the image colormap",
      "-debug               display copious debugging information",
      "-delay value         display the next image after pausing",
      "-density geometry    vertical and horizontal density of the image",
      "-depth value         depth of the image",
      "-despeckle           reduce the speckles within an image",
      "-display server      obtain image or font from this X server",
      "-dispose method      GIF disposal method",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-edge radius          apply a filter to detect edges in the image",
      "-emboss radius       emboss an image",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           font for rendering text",
      "-format type         image format type",
      "-frame geometry      surround image with an ornamental border",
      "-fuzz distance       colors within this distance are considered equal",
      "-gamma value         level of gamma correction",
      "-gaussian geometry   gaussian blur an image",
      "-geometry geometry   perferred size or location of the image",
      "-implode amount      implode image pixels about the center",
      "-interlace type      None, Line, Plane, or Partition",
      "-help                print program options",
      "-label name          assign a label to an image",
      "-level value         adjust the level of image contrast",
      "-list type           Color, Delegate, Format, Magic, Module, or Type",
      "-loop iterations     add Netscape loop extension to your GIF animation",
      "-map filename        transform image colors to match this set of colors",
      "-matte               store matte channel if the image has one",
      "-median radius       apply a median filter to the image",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "-noise radius        add or reduce noise in an image.",
      "-normalize           transform image to span the full range of colors",
      "-opaque color        change this color to the fill color",
      "-page geometry       size and location of an image canvas",
      "-paint radius        simulate an oil painting",
      "-fill color           color for annotating or changing opaque color",
      "-pointsize value     pointsize of Postscript font",
      "-profile filename    add ICM or IPTC information profile to image",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-region geometry     apply options to a portion of the image",
      "-resize geometry     perferred size or location of the image",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-scale geometry      scale the image",
      "-scene number        image scene number",
      "-seed value          pseudo-random number generator seed value",
      "-segment values      segment an image",
      "-shade degrees       shade the image using a distant light source",
      "-sharpen radius      sharpen the image",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-solarize threshold  negate all pixels above the threshold level",
      "-spread amount       displace image pixels by a random amount",
      "-stroke color        color to use when stoking a graphic primitive",
      "-strokewidth value   width of stroke",
      "-swirl degrees       swirl image pixels about the center",
      "-texture filename    name of texture to tile onto the image background",
      "-threshold value     threshold the image",
      "-tile filename       tile image when filling a graphic primitive",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     depth of the color tree",
      "-type type           image type",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-view                FlashPix viewing transforms",
      "-wave geometry       alter an image along a sine wave",
      (char *) NULL
    };

  const char
    **p;

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf(
    "Usage: %.1024s [ -option ... ] file [ [ -option value ... ] file ... ]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a i n                                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
  char
    filename[MaxTextExtent],
    *format,
    *option;

  double
    sans;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  Image
    *image;

  int
    x;

  long
    scene;

  register Image
    *p;

  register int
    i;

  unsigned int
    global_colormap,
    status;

  /*
    Initialize command line arguments.
  */
  if (LocaleCompare("-mogrify",argv[0]) == 0)
    {
      if (argc < 3)
        return(False);
    }
  else
    {
      ReadCommandlLine(argc,&argv);
      if (LocaleCompare("mogrify",argv[0]) == 0)
        InitializeMagick(GetExecutionPath(argv[0]));
      else
        InitializeMagick(*argv);
      status=ExpandFilenames(&argc,&argv);
      if (status == False)
        MagickError(ResourceLimitError,"Memory allocation failed",
          (char *) NULL);
      if (argc < 3)
        Usage();
    }
  /*
    Set defaults.
  */
  GetExceptionInfo(&exception);
  format=(char *) NULL;
  global_colormap=False;
  image=(Image *) NULL;
  image_info=CloneImageInfo((ImageInfo *) NULL);
  scene=0;
  status=False;
  /*
    Parse command line.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'a':
        {
          if (LocaleCompare("affine",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing matrix",option);
                }
              break;
            }
          if (LocaleCompare("antialias",option+1) == 0)
            {
              image_info->antialias=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'b':
        {
          if (LocaleCompare("background",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing background color",option);
                  (void) QueryColorDatabase(argv[i],
                    &image_info->background_color);
                }
              break;
            }
          if (LocaleCompare("blur",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing geometry",option);
              break;
            }
          if (LocaleCompare("border",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("bordercolor",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing border color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->border_color);
                }
              break;
            }
          if (LocaleCompare("box",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing box color",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'c':
        {
          if (LocaleCompare("cache",option+1) == 0)
            {
              SetCacheThreshold(0);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                  SetCacheThreshold(atol(argv[i]));
                }
              break;
            }
          if (LocaleCompare("channel",option+1) == 0)
            {
              if (*option == '-')
                {
                  ChannelType
                    channel;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  channel=UndefinedChannel;
                 if (LocaleCompare("Red",option) == 0)
                   channel=RedChannel;
                 if (LocaleCompare("Cyan",option) == 0)
                   channel=CyanChannel;
                 if (LocaleCompare("Green",option) == 0)
                   channel=GreenChannel;
                 if (LocaleCompare("Magenta",option) == 0)
                   channel=MagentaChannel;
                 if (LocaleCompare("Blue",option) == 0)
                   channel=BlueChannel;
                 if (LocaleCompare("Yellow",option) == 0)
                   channel=YellowChannel;
                 if (LocaleCompare("Opacity",option) == 0)
                   channel=OpacityChannel;
                 if (LocaleCompare("Black",option) == 0)
                   channel=BlackChannel;
                 if (LocaleCompare("Matte",option) == 0)
                   channel=MatteChannel;
                  if (channel == UndefinedChannel)
                    MagickError(OptionError,"Invalid channel type",option);
                }
              break;
            }
          if (LocaleCompare("charcoal",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("chop",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("colorize",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleCompare("colors",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing colors",option);
                }
              break;
            }
          if (LocaleCompare("colorspace",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->colorspace=UndefinedColorspace;
                  if (LocaleCompare("cmyk",option) == 0)
                    image_info->colorspace=CMYKColorspace;
                  if (LocaleCompare("gray",option) == 0)
                    image_info->colorspace=GRAYColorspace;
                  if (LocaleCompare("ohta",option) == 0)
                    image_info->colorspace=OHTAColorspace;
                  if (LocaleCompare("rgb",option) == 0)
                    image_info->colorspace=RGBColorspace;
                  if (LocaleCompare("srgb",option) == 0)
                    image_info->colorspace=sRGBColorspace;
                  if (LocaleCompare("transparent",option) == 0)
                    image_info->colorspace=TransparentColorspace;
                  if (LocaleCompare("xyz",option) == 0)
                    image_info->colorspace=XYZColorspace;
                  if (LocaleCompare("ycbcr",option) == 0)
                    image_info->colorspace=YCbCrColorspace;
                  if (LocaleCompare("ycc",option) == 0)
                    image_info->colorspace=YCCColorspace;
                  if (LocaleCompare("yiq",option) == 0)
                    image_info->colorspace=YIQColorspace;
                  if (LocaleCompare("ypbpr",option) == 0)
                    image_info->colorspace=YPbPrColorspace;
                  if (LocaleCompare("yuv",option) == 0)
                    image_info->colorspace=YUVColorspace;
                  if (image_info->colorspace == UndefinedColorspace)
                    MagickError(OptionError,"Invalid colorspace type",option);
                }
              break;
            }
          if (LocaleCompare("comment",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing comment",option);
                }
              break;
            }
          if (LocaleCompare("compress",option+1) == 0)
            {
              image_info->compression=NoCompression;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->compression=UndefinedCompression;
                  if (LocaleCompare("None",option) == 0)
                    image_info->compression=NoCompression;
                  if (LocaleCompare("BZip",option) == 0)
                    image_info->compression=BZipCompression;
                  if (LocaleCompare("Fax",option) == 0)
                    image_info->compression=FaxCompression;
                  if (LocaleCompare("Group4",option) == 0)
                    image_info->compression=Group4Compression;
                  if (LocaleCompare("JPEG",option) == 0)
                    image_info->compression=JPEGCompression;
                  if (LocaleCompare("Lossless",option) == 0)
                    image_info->compression=LosslessJPEGCompression;
                  if (LocaleCompare("LZW",option) == 0)
                    image_info->compression=LZWCompression;
                  if (LocaleCompare("RLE",option) == 0)
                    image_info->compression=RunlengthEncodedCompression;
                  if (LocaleCompare("Zip",option) == 0)
                    image_info->compression=ZipCompression;
                  if (image_info->compression == UndefinedCompression)
                    MagickError(OptionError,"Invalid compression type",option);
                }
              break;
            }
          if (LocaleCompare("contrast",option+1) == 0)
            break;
          if (LocaleCompare("crop",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("cycle",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (LocaleCompare("debug",option+1) == 0)
            {
              image_info->debug=(*option == '-');
              break;
            }
          if (LocaleCompare("delay",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing delay",option);
                }
              break;
            }
          if (LocaleCompare("density",option+1) == 0)
            {
              (void) CloneString(&image_info->density,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&image_info->density,argv[i]);
                }
              break;
            }
          if (LocaleCompare("depth",option+1) == 0)
            {
              image_info->depth=QuantumDepth;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing image depth",option);
                  image_info->depth=atol(argv[i]);
                }
              break;
            }
          if (LocaleCompare("despeckle",option+1) == 0)
            break;
          if (LocaleCompare("display",option+1) == 0)
            {
              (void) CloneString(&image_info->server_name,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing server name",option);
                  (void) CloneString(&image_info->server_name,argv[i]);
                }
              break;
            }
          if (LocaleCompare("dispose",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing method",option);
                }
              break;
            }
          if (LocaleCompare("dither",option+1) == 0)
            {
              image_info->dither=(*option == '-');
              break;
            }
          if (LocaleCompare("draw",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing primitive",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'e':
        {
          if (LocaleCompare("edge",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("emboss",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("enhance",option+1) == 0)
            break;
          if (LocaleCompare("equalize",option+1) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (LocaleCompare("fill",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing fill color",option);
                }
              break;
            }
          if (LocaleCompare("filter",option+1) == 0)
            {
              if (*option == '-')
                {
                  FilterTypes
                    filter;

                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  filter=UndefinedFilter;
                  if (LocaleCompare("Point",option) == 0)
                    filter=PointFilter;
                  if (LocaleCompare("Box",option) == 0)
                    filter=BoxFilter;
                  if (LocaleCompare("Triangle",option) == 0)
                    filter=TriangleFilter;
                  if (LocaleCompare("Hermite",option) == 0)
                    filter=HermiteFilter;
                  if (LocaleCompare("Hanning",option) == 0)
                    filter=HanningFilter;
                  if (LocaleCompare("Hamming",option) == 0)
                    filter=HammingFilter;
                  if (LocaleCompare("Blackman",option) == 0)
                    filter=BlackmanFilter;
                  if (LocaleCompare("Gaussian",option) == 0)
                    filter=GaussianFilter;
                  if (LocaleCompare("Quadratic",option) == 0)
                    filter=QuadraticFilter;
                  if (LocaleCompare("Cubic",option) == 0)
                    filter=CubicFilter;
                  if (LocaleCompare("Catrom",option) == 0)
                    filter=CatromFilter;
                  if (LocaleCompare("Mitchell",option) == 0)
                    filter=MitchellFilter;
                  if (LocaleCompare("Lanczos",option) == 0)
                    filter=LanczosFilter;
                  if (LocaleCompare("Bessel",option) == 0)
                    filter=BesselFilter;
                  if (LocaleCompare("Sinc",option) == 0)
                    filter=SincFilter;
                  if (filter == UndefinedFilter)
                    MagickError(OptionError,"Invalid filter type",option);
                }
              break;
            }
          if (LocaleCompare("flip",option+1) == 0)
            break;
          if (LocaleCompare("flop",option+1) == 0)
            break;
          if (LocaleCompare("font",option+1) == 0)
            {
              (void) CloneString(&image_info->font,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing font name",option);
                  (void) CloneString(&image_info->font,argv[i]);
                }
              break;
            }
          if (LocaleCompare("format",option+1) == 0)
            {
              (void) CloneString(&format,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing image type",option);
                  (void) CloneString(&format,argv[i]);
                  (void) strncpy(image_info->filename,format,MaxTextExtent-1);
                  (void) strcat(image_info->filename,":");
                  (void) SetImageInfo(image_info,False,&exception);
                  if (*image_info->magick == '\0')
                    MagickError(OptionError,"Unrecognized image format",format);
                }
              break;
            }
          if (LocaleCompare("frame",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("fuzz",option+1) == 0)
            {
              image_info->fuzz=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing distance",option);
                  image_info->fuzz=StringToDouble(argv[i],MaxRGB);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'g':
        {
          if (LocaleCompare("gamma",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing value",option);
              break;
            }
          if (LocaleCompare("gaussian",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing geometry",option);
              break;
            }
          if (LocaleCompare("geometry",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("gravity",option+1) == 0)
            {
              GravityType
                gravity;

              gravity=ForgetGravity;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("NorthWest",option) == 0)
                    gravity=NorthWestGravity;
                  if (LocaleCompare("North",option) == 0)
                    gravity=NorthGravity;
                  if (LocaleCompare("NorthEast",option) == 0)
                    gravity=NorthEastGravity;
                  if (LocaleCompare("West",option) == 0)
                    gravity=WestGravity;
                  if (LocaleCompare("Center",option) == 0)
                    gravity=CenterGravity;
                  if (LocaleCompare("East",option) == 0)
                    gravity=EastGravity;
                  if (LocaleCompare("SouthWest",option) == 0)
                    gravity=SouthWestGravity;
                  if (LocaleCompare("South",option) == 0)
                    gravity=SouthGravity;
                  if (LocaleCompare("SouthEast",option) == 0)
                    gravity=SouthEastGravity;
                  if (gravity == ForgetGravity)
                    MagickError(OptionError,"Invalid gravity type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'h':
        {
          if (LocaleCompare("help",option+1) == 0)
            {
              Usage();
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'i':
        {
          if (LocaleCompare("implode",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          if (LocaleCompare("interlace",option+1) == 0)
            {
              image_info->interlace=NoInterlace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->interlace=UndefinedInterlace;
                  if (LocaleCompare("None",option) == 0)
                    image_info->interlace=NoInterlace;
                  if (LocaleCompare("Line",option) == 0)
                    image_info->interlace=LineInterlace;
                  if (LocaleCompare("Plane",option) == 0)
                    image_info->interlace=PlaneInterlace;
                  if (LocaleCompare("Partition",option) == 0)
                    image_info->interlace=PartitionInterlace;
                  if (image_info->interlace == UndefinedInterlace)
                    MagickError(OptionError,"Invalid interlace type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'l':
        {
          if (LocaleCompare("label",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing label name",option);
                }
              break;
            }
          if (LocaleCompare("level",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing value",option);
              break;
            }
          if (LocaleCompare("linewidth",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                }
              break;
            }
          if (LocaleCompare("list",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing list name",option);
                  option=argv[i];
                  switch (*option)
                  {
                    case 'C':
                    case 'c':
                    {
                      if (LocaleCompare("Color",option) == 0)
                        {
                          (void) ListColorInfo((FILE *) NULL,&exception);
                          break;
                        }
                      MagickError(OptionError,"Invalid list type",option);
                      break;
                    }
                    case 'D':
                    case 'd':
                    {
                      if (LocaleCompare("Delegate",option) == 0)
                        {
                          (void) ListDelegateInfo((FILE *) NULL,&exception);
                          break;
                        }
                      MagickError(OptionError,"Invalid list type",option);
                      break;
                    }
                    case 'F':
                    case 'f':
                    {
                      if (LocaleCompare("Format",option) == 0)
                        {
                          (void) ListMagickInfo((FILE *) NULL,&exception);
                          break;
                        }
                      MagickError(OptionError,"Invalid list type",option);
                      break;
                    }
                    case 'M':
                    case 'm':
                    {
                      if (LocaleCompare("Magic",option) == 0)
                        {
                          (void) ListMagicInfo((FILE *) NULL,&exception);
                          break;
                        }
#if defined(HasMODULES)
                      if (LocaleCompare("Module",option) == 0)
                        {
                          (void) ListModuleAliases((FILE *) NULL,&exception);
                          break;
                        }
#endif /* HasMODULES */
                      MagickError(OptionError,"Invalid list type",option);
                      break;
                    }
                    case 'T':
                    case 't':
                    {
                      if (LocaleCompare("Type",option) == 0)
                        {
                          (void) ListTypeInfo((FILE *) NULL,&exception);
                          break;
                        }
                      MagickError(OptionError,"Invalid list type",option);
                    }
                    default:
                      MagickError(OptionError,"Invalid list type",option);
                  }
                  if (exception.severity != UndefinedException)
                    MagickError(exception.severity,exception.reason,
                      exception.description);
                  DestroyMagick();
                  Exit(0);
                }
              break;
            }
          if (LocaleCompare("loop",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing iterations",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (LocaleCompare("map",option+1) == 0)
            {
              global_colormap=(*option == '+');
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing file name",option);
                }
              break;
            }
          if (LocaleCompare("matte",option+1) == 0)
            break;
          if (LocaleCompare("mattecolor",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing matte color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->matte_color);
                }
              break;
            }
          if (LocaleCompare("modulate",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleCompare("median",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("monochrome",option+1) == 0)
            {
              image_info->monochrome=(*option == '-');
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (LocaleCompare("negate",option+1) == 0)
            break;
         if (LocaleCompare("noise",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing value",option);
                }
              if (*option == '+')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if ((LocaleCompare("Uniform",option) != 0) &&
                      (LocaleCompare("Gaussian",option) != 0) &&
                      (LocaleCompare("Multiplicative",option) != 0) &&
                      (LocaleCompare("Impulse",option) != 0) &&
                      (LocaleCompare("Laplacian",option) != 0) &&
                      (LocaleCompare("Poisson",option) != 0))
                    MagickError(OptionError,"Invalid noise type",option);
                }
              break;
            }
          if (LocaleCompare("normalize",option+1) == 0)
            break;
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'o':
        {
          if (LocaleCompare("opaque",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing opaque color",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'p':
        {
          if (LocaleCompare("page",option+1) == 0)
            {
              (void) CloneString(&image_info->page,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing page geometry",option);
                  image_info->page=PostscriptGeometry(argv[i]);
                }
              break;
            }
          if (LocaleCompare("paint",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing radius",option);
                }
              break;
            }
          if (LocaleCompare("pen",option+1) == 0)
            {
              (void) QueryColorDatabase("none",&image_info->pen);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing fill color",option);
                  (void) QueryColorDatabase(argv[i],&image_info->pen);
                }
              break;
            }
          if (LocaleCompare("pointsize",option+1) == 0)
            {
              image_info->pointsize=12;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                  image_info->pointsize=atof(argv[i]);
                }
              break;
            }
          if (LocaleCompare("profile",option+1) == 0)
            {
              i++;
              if (i == argc)
                MagickError(OptionError,"Missing profile",option);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'q':
        {
          if (LocaleCompare("quality",option+1) == 0)
            {
              image_info->quality=75;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing quality",option);
                  image_info->quality=atol(argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'r':
        {
          if (LocaleCompare("raise",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing bevel width",option);
              break;
            }
          if (LocaleCompare("region",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("resize",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("roll",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("rotate",option+1) == 0)
            {
              i++;
              if ((i == argc) || !IsGeometry(argv[i]))
                MagickError(OptionError,"Missing degrees",option);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 's':
        {
          if (LocaleCompare("sample",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("scale",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("scene",option+1) == 0)
            {
              scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing scene number",option);
                  scene=atol(argv[i]);
                }
              break;
            }
          if (LocaleCompare("seed",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              srand(atoi(argv[i]));
              break;
            }
          if (LocaleCompare("segment",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing threshold",option);
                }
              break;
            }
          if (LocaleCompare("shade",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing azimuth",option);
              break;
            }
          if (LocaleCompare("sharpen",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing geometry",option);
              break;
            }
          if (LocaleCompare("shave",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                }
              break;
            }
          if (LocaleCompare("shear",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing geometry",option);
              break;
            }
          if (LocaleCompare("size",option+1) == 0)
            {
              (void) CloneString(&image_info->size,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !IsGeometry(argv[i]))
                    MagickError(OptionError,"Missing geometry",option);
                  (void) CloneString(&image_info->size,argv[i]);
                }
              break;
            }
          if (LocaleCompare("solarize",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing threshold",option);
                }
              break;
            }
          if (LocaleCompare("spread",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing amount",option);
                }
              break;
            }
          if (LocaleCompare("stroke",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing color",option);
                }
              break;
            }
          if (LocaleCompare("strokewidth",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing size",option);
                }
              break;
            }
          if (LocaleCompare("swirl",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                    MagickError(OptionError,"Missing degrees",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (LocaleCompare("texture",option+1) == 0)
            {
              (void) CloneString(&image_info->texture,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing filename",option);
                  (void) CloneString(&image_info->texture,argv[i]);
                }
              break;
            }
          if (LocaleCompare("threshold",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing value",option);
                }
              break;
            }
          if (LocaleCompare("tile",option+1) == 0)
            {
              i++;
              if (i == argc)
                MagickError(OptionError,"Missing tile",option);
              break;
            }
          if (LocaleCompare("transparent",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing transparent color",option);
                }
              break;
            }
          if (LocaleCompare("treedepth",option+1) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    MagickError(OptionError,"Missing depth",option);
                }
              break;
            }
          if (LocaleCompare("type",option+1) == 0)
            {
              image_info->type=UndefinedType;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  if (LocaleCompare("Bilevel",option) == 0)
                    image_info->type=BilevelType;
                  if (LocaleCompare("Grayscale",option) == 0)
                    image_info->type=GrayscaleType;
                  if (LocaleCompare("GrayscaleMatte",option) == 0)
                    image_info->type=GrayscaleMatteType;
                  if (LocaleCompare("Palette",option) == 0)
                    image_info->type=PaletteType;
                  if (LocaleCompare("PaletteMatte",option) == 0)
                    image_info->type=PaletteMatteType;
                  if (LocaleCompare("TrueColor",option) == 0)
                    image_info->type=TrueColorType;
                  if (LocaleCompare("TrueColorMatte",option) == 0)
                    image_info->type=TrueColorMatteType;
                  if (LocaleCompare("ColorSeparation",option) == 0)
                    image_info->type=ColorSeparationType;
                  if (LocaleCompare("ColorSeparationMatte",option) == 0)
                    image_info->type=ColorSeparationMatteType;
                  if (LocaleCompare("Optimize",option) == 0)
                    image_info->type=OptimizeType;
                  if (image_info->type == UndefinedType)
                    MagickError(OptionError,"Invalid image type",option);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'u':
        {
          if (LocaleCompare("units",option+1) == 0)
            {
              image_info->units=UndefinedResolution;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing type",option);
                  option=argv[i];
                  image_info->units=UndefinedResolution;
                  if (LocaleCompare("PixelsPerInch",option) == 0)
                    image_info->units=PixelsPerInchResolution;
                  if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                    image_info->units=PixelsPerCentimeterResolution;
                }
              break;
            }
          if (LocaleCompare("unsharp",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                MagickError(OptionError,"Missing geometry",option);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'v':
        {
          if (LocaleCompare("verbose",option+1) == 0)
            {
              image_info->verbose=(*option == '-');
              break;
            }
          if (LocaleCompare("view",option+1) == 0)
            {
              (void) CloneString(&image_info->view,(char *) NULL);
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    MagickError(OptionError,"Missing view transform",option);
                  (void) CloneString(&image_info->view,argv[i]);
                }
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case 'w':
        {
          if (LocaleCompare("wave",option+1) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                MagickError(OptionError,"Missing amplitude",option);
              break;
            }
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
        case '?':
        {
          Usage();
          break;
        }
        default:
        {
          MagickError(OptionError,"Unrecognized option",option);
          break;
        }
      }
    else
      {
        /*
          Option is a file name: begin by reading image from specified file.
        */
        (void) strncpy(image_info->filename,argv[i],MaxTextExtent-1);
        image=ReadImage(image_info,&exception);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
        if (image == (Image *) NULL)
          continue;
        if (scene != 0)
          image->scene=scene;
        if (format != (char *) NULL)
          {
            register char
              *p;

            /*
              Modify filename to include a user specified image format.
            */
            p=image->filename+strlen(image->filename)-1;
            while ((*p != '.') && (p > (image->filename+1)))
              p--;
            p++;
            if (LocaleCompare(p,image->magick) == 0)
              (void) strncpy(p,format,MaxTextExtent-1);
            else
              {
                FormatString(image_info->filename,"%.1024s:%.1024s",format,
                  image->filename);
                (void) strncpy(image->filename,image_info->filename,
                  MaxTextExtent-1);
              }
          }
        /*
          Transmogrify image as defined by the image processing options.
        */
        status=MogrifyImages(image_info,i,argv,&image);
        CatchImageException(image);
        if (global_colormap)
          (void) MapImages(image,(Image *) NULL,image_info->dither);
        /*
          Write transmogrified image to disk.
        */
        if (format != (char *) NULL)
          AppendImageFormat(format,image->filename);
        else
          if (LocaleCompare(image_info->filename,"-") != 0)
            {
              (void) strncpy(filename,image->filename,MaxTextExtent-1);
              AppendImageFormat("mgk",image->filename);
              if (IsAccessible(image->filename))
                {
                  (void) strcat(image->filename,"~");
                  if (IsAccessible(image->filename))
                    MagickError(FileOpenError,"Unable to create temporary file",
                      filename);
                }
            }
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          (void) strncpy(p->filename,image->filename,MaxTextExtent-1);
          p->scene=scene++;
        }
        (void) SetImageInfo(image_info,True,&image->exception);
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          status=WriteImage(image_info,p);
          CatchImageException(p);
          if (image_info->adjoin)
            break;
        }
        for (p=image; p != (Image *) NULL; p=p->next)
          scene--;
        if (image_info->verbose)
          DescribeImage(image,stderr,False);
        if ((format == (char *) NULL) && (status != False))
          if (LocaleCompare(image_info->filename,"-") != 0)
            {
              (void) remove(filename);
              (void) rename(image->filename,filename);
            }
        DestroyImage(image);
      }
  }
  if ((i != argc) || (image == (Image *) NULL))
    MagickError(OptionError,"Missing an image file name",(char *) NULL);
  DestroyImageInfo(image_info);
  if (LocaleCompare("-mogrify",argv[0]) == 0)
    return(True);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
