/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 IIIII  M   M  PPPP    OOO   RRRR   TTTTT                    %
%                   I    MM MM  P   P  O   O  R   R    T                      %
%                   I    M M M  PPPP   O   O  RRRR     T                      %
%                   I    M   M  P      O   O  R R      T                      %
%                 IIIII  M   M  P       OOO   R  R     T                      %
%                                                                             %
%                                                                             %
%               Import image to a machine independent format.                 %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
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
%  Import is an X Window System window dumping utility.  Import allows X
%  users to store window images in a specially formatted dump file.  This
%  file can then be read by the Display utility for redisplay, printing,
%  editing, formatting, archiving, image processing, etc.  The target
%  window can be specified by id or name or be selected by clicking the
%  mouse in the desired window.  The keyboard bell is rung once at the
%  beginning of the dump and twice when the dump is completed.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/cache.h"
#include "magick/command.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/version.h"
#include "magick/utility.h"
#include "magick/xwindow.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t U s a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportUsage() displays the program command syntax.
%
%  The format of the ImportUsage method is:
%
%      void ImportUsage()
%
%
*/
static void ImportUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-border              include image borders in the output image",
      "-cache geometry      min/max megabytes of memory of pixel cache memory",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       image compression tyhpe",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-descend             obtain image by descending window hierarchy",
      "-display server      X server to contact",
      "-dispose method      GIF disposal method",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-frame               include window manager frame",
      "-encoding type       text encoding type",
      "-endian type         LSB or MSB",
      "-geometry geometry   perferred size or location of the image",
      "-interlace type      None, Line, Plane, or Partition",
      "-help                print program options",
      "-label name          assign a label to an image",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "-page geometry       size and location of an image canvas",
      "-pause value         seconds delay between snapshots",
      "-pointsize value     font point size",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-resize geometry     resize the image",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling_factor geometry",
      "                     horizontal and vertical sampling factor",
      "-scene value         image scene number",
      "-screen              select image from root window",
      "-silent              operate silently, i.e. don't ring any bells ",
      "-snaps value         number of screen snapshots",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      "-virtual_pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-window id           select window with this id or name",
      (char *) NULL
    };

  (void) printf("Version: %.1024s\n",GetMagickVersion((unsigned long *) NULL));
  (void) printf("Copyright: %.1024s\n\n",GetMagickCopyright());
  (void) printf("Usage: %.1024s [options ...] [ file ]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
  "\nBy default, 'file' is written in the MIFF image format.  To\n");
  (void) printf(
    "specify a particular image format, precede the filename with an image\n");
  (void) printf(
    "format name and a colon (i.e. ps:image) or specify the image type as\n");
  (void) printf(
    "the filename suffix (i.e. image.ps).  Specify 'file' as '-' for\n");
  (void) printf("standard input or output.\n");
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
#if defined(HasX11)
  char
    *client_name,
    *filename,
    *option,
    *resource_value,
    *server_name,
    *target_window;

  Display
    *display;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image,
    *next_image;

  ImageInfo
    *image_info;

  long
    snapshots,
    x;

  QuantizeInfo
    *quantize_info;

  register long
    i;

  unsigned int
    status;

  XImportInfo
    ximage_info;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  ReadCommandlLine(argc,&argv);
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,"Memory allocation failed",
      (char *) NULL);
  /*
    Check for server name specified on the command line.
  */
  server_name=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("display",option+1) == 0)
      {
        /*
          User specified server name.
        */
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,"Missing server name on -display",
            option);
        server_name=argv[i];
        break;
      }
    if (LocaleCompare("help",option+1) == 0)
      ImportUsage();
  }
  /*
    Get user defaults from X resource database.
  */
  SetNotifyHandlers;
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickFatalError(OptionFatalError,"Unable to connect to X server",
      XDisplayName(server_name));
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetImportInfo(&ximage_info);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  resource_value=
    XGetResourceInstance(resource_database,client_name,"border","False");
  ximage_info.borders=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"delay","0");
  resource_info.delay=atol(resource_value);
  image_info->density=XGetResourceInstance(resource_database,client_name,
    "density",(char *) NULL);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"descend","True");
  ximage_info.descend=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"frame","False");
  ximage_info.frame=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"interlace","none");
  image_info->interlace=UndefinedInterlace;
  if (LocaleCompare("None",resource_value) == 0)
    image_info->interlace=NoInterlace;
  if (LocaleCompare("Line",resource_value) == 0)
    image_info->interlace=LineInterlace;
  if (LocaleCompare("Plane",resource_value) == 0)
    image_info->interlace=PlaneInterlace;
  if (LocaleCompare("Partition",resource_value) == 0)
    image_info->interlace=PartitionInterlace;
  if (image_info->interlace == UndefinedInterlace)
    MagickError(OptionError,"Unrecognized interlace type",resource_value);
  image_info->page=XGetResourceInstance(resource_database,client_name,
    "pageGeometry",(char *) NULL);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"pause","0");
  resource_info.pause=atol(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"quality","85");
  image_info->quality=atol(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"screen","False");
  ximage_info.screen=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"silent","False");
  ximage_info.silent=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=IsTrue(resource_value);
  snapshots=1;
  status=True;
  filename=(char *) NULL;
  target_window=(char *) NULL;
  /*
    Check command syntax.
  */
  GetExceptionInfo(&exception);
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        filename=argv[i];
        continue;
      }
    switch(*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("border",option+1) == 0)
          {
            ximage_info.borders=(*option == '-');
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing border color",
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  &exception);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                int
                  count;

                double
                  maximum,
                  minimum;

                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,"Missing threshold",
                    option);
                minimum=0.0;
                maximum=0.0;
                count=sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
                if (count == 1)
                  maximum=minimum;
                SetCacheThreshold((size_t) minimum,(size_t) maximum);
							}
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info->number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing colors",option);
                quantize_info->number_colors=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            quantize_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing type",option);
                option=argv[i];
                quantize_info->colorspace=UndefinedColorspace;
                if (LocaleCompare("cmyk",option) == 0)
                  quantize_info->colorspace=CMYKColorspace;
                if (LocaleCompare("gray",option) == 0)
                  {
                    quantize_info->colorspace=GRAYColorspace;
                    quantize_info->number_colors=256;
                    quantize_info->tree_depth=8;
                  }
                if (LocaleCompare("ohta",option) == 0)
                  quantize_info->colorspace=OHTAColorspace;
                if (LocaleCompare("rgb",option) == 0)
                  quantize_info->colorspace=RGBColorspace;
                if (LocaleCompare("srgb",option) == 0)
                  quantize_info->colorspace=sRGBColorspace;
                if (LocaleCompare("transparent",option) == 0)
                  quantize_info->colorspace=TransparentColorspace;
                if (LocaleCompare("xyz",option) == 0)
                  quantize_info->colorspace=XYZColorspace;
                if (LocaleCompare("ycbcr",option) == 0)
                  quantize_info->colorspace=YCbCrColorspace;
                if (LocaleCompare("ycc",option) == 0)
                  quantize_info->colorspace=YCCColorspace;
                if (LocaleCompare("yiq",option) == 0)
                  quantize_info->colorspace=YIQColorspace;
                if (LocaleCompare("ypbpr",option) == 0)
                  quantize_info->colorspace=YPbPrColorspace;
                if (LocaleCompare("yuv",option) == 0)
                  quantize_info->colorspace=YUVColorspace;
                if (quantize_info->colorspace == UndefinedColorspace)
                  MagickFatalError(OptionFatalError,"Invalid colorspace type",
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing comment",option);
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
                  MagickFatalError(OptionFatalError,"Missing type",option);
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
                  image_info->compression=RLECompression;
                if (LocaleCompare("Zip",option) == 0)
                  image_info->compression=ZipCompression;
                if (image_info->compression == UndefinedCompression)
                  MagickFatalError(OptionFatalError,"Invalid compression type",
                    option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing event mask",
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing seconds",option);
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
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
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
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing image depth",
                    option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("descend",option+1) == 0)
          {
            ximage_info.descend=(*option == '-');
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing server name",
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing method",option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info->dither=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing encoding type",option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing type",option);
                option=argv[i];
                image_info->endian=UndefinedEndian;
                if (LocaleCompare("LSB",option) == 0)
                  image_info->endian=LSBEndian;
                if (LocaleCompare("MSB",option) == 0)
                  image_info->endian=MSBEndian;
                if (image_info->endian == UndefinedEndian)
                  MagickFatalError(OptionFatalError,"Invalid endian type",
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("frame",option+1) == 0)
          {
            argv[i]=(char *) "-ignore";  /* resolve option confict */
            ximage_info.frame=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'g':
      {
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            ImportUsage();
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=NoInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing type",option);
                option=argv[i];
                image_info->interlace=UndefinedInterlace;
                if (LocaleCompare("No",option) == 0)
                  image_info->interlace=NoInterlace;
                if (LocaleCompare("Line",option) == 0)
                  image_info->interlace=LineInterlace;
                if (LocaleCompare("Plane",option) == 0)
                  image_info->interlace=PlaneInterlace;
                if (LocaleCompare("Partition",option) == 0)
                  image_info->interlace=PartitionInterlace;
                if (image_info->interlace == UndefinedInterlace)
                  MagickFatalError(OptionFatalError,"Invalid interlace type",
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
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
                  MagickFatalError(OptionFatalError,"Missing label name",
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'm':
      {
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            if (image_info->monochrome)
              {
                quantize_info->number_colors=2;
                quantize_info->tree_depth=8;
                quantize_info->colorspace=GRAYColorspace;
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
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
                  MagickFatalError(OptionFatalError,"Missing page geometry",
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("pause",option+1) == 0)
          {
            resource_info.pause=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing seconds",option);
                resource_info.pause=atoi(argv[i]);
              }
            break;
          }
        if (LocaleCompare("ping",option+1) == 0)
          {
            MagickError(OptionFatalError,"Deprecated option",option);
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing size",option);
                image_info->pointsize=atof(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing quality",option);
                image_info->quality=atol(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'r':
      {
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
               MagickFatalError(OptionFatalError,"Missing degrees",option);
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing scene number",
                    option);
              }
            break;
          }
        if (LocaleCompare("screen",option+1) == 0)
          {
            ximage_info.screen=(*option == '-');
            break;
          }
        if (LocaleCompare("silent",option+1) == 0)
          {
            ximage_info.silent=(*option == '-');
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"Missing geometry",option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("snaps",option+1) == 0)
          {
            (void) strcpy(argv[i]+1,"sans");
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              MagickFatalError(OptionFatalError,"Missing snaps value",option);
            snapshots=atol(argv[i]);
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing color",option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info->tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"Missing depth",option);
                quantize_info->tree_depth=atoi(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"Missing type",option);
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
                  MagickFatalError(OptionFatalError,"Invalid image type",
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case 'w':
      {
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,"Missing id, name, or 'root'",
            option);
        (void) CloneString(&target_window,argv[i]);
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
      case '?':
      {
        ImportUsage();
        break;
      }
      default:
      {
        MagickFatalError(OptionFatalError,"Unrecognized option",option);
        break;
      }
    }
  }
  DestroyExceptionInfo(&exception);
  if (filename == (char *) NULL)
    filename=(char *) "magick.miff";
  /*
    Read image from X server.
  */
  if (target_window != (char *) NULL)
    (void) strncpy(image_info->filename,target_window,MaxTextExtent-1);
  image_info->colorspace=quantize_info->colorspace;
  image_info->dither=quantize_info->dither;
  image=(Image *) NULL;
  for (i=0; i < (long) Max(snapshots,1); i++)
  {
    (void) sleep(resource_info.pause);
    next_image=XImportImage(image_info,&ximage_info);
    status&=next_image != (Image *) NULL;
    if (next_image == (Image *) NULL)
      continue;
    (void) strncpy(next_image->filename,filename,MaxTextExtent-1);
    (void) strcpy(next_image->magick,"PS");
    next_image->scene=i;
    next_image->previous=image;
    if (image != (Image *) NULL)
      image->next=next_image;
    image=next_image;
  }
  if (image == (Image *) NULL)
    MagickFatalError(OptionFatalError,"Missing an image file name",
      (char *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  status&=MogrifyImages(image_info,argc-1,argv,&image);
  (void) CatchImageException(image);
  status&=WriteImages(image_info,image,filename,&image->exception);
  DestroyImageList(image);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
#endif
  return(False);
}
