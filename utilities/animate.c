/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              AAA   N   N  IIIII  M   M   AAA   TTTTT  EEEEE                 %
%             A   A  NN  N    I    MM MM  A   A    T    E                     %
%             AAAAA  N N N    I    M M M  AAAAA    T    EEE                   %
%             A   A  N  NN    I    M   M  A   A    T    E                     %
%             A   A  N   N  IIIII  M   M  A   A    T    EEEEE                 %
%                                                                             %
%                                                                             %
%                 Interactively Animate an Image Sequence.                    %
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
%  Animate displays a sequence of images in the MIFF format on any
%  workstation display running an X server.  Animate first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in an image is less than or equal to the number the workstation
%  can support, the image is displayed in an X window.  Otherwise the
%  number of colors in the image is first reduced to match the color
%  resolution of the workstation before it is displayed.
%
%  This means that a continuous-tone 24 bits-per-pixel image can display on a
%  8 bit pseudo-color device or monochrome device.  In most instances the
%  reduced color image closely resembles the original.  Alternatively, a
%  monochrome or pseudo-color image can display on a continuous-tone 24
%  bits-per-pixel device.
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "cache.h"
#include "command.h"
#include "log.h"
#include "resource.h"
#include "utility.h"
#include "version.h"
#include "xwindow.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnimateUsage() displays the program command syntax.
%
%  The format of the AnimateUsage method is:
%
%      void AnimateUsage()
%
%  A description of each parameter follows:
%
%
*/
static void AnimateUsage(void)
{
  const char
    **p;

  static const char
    *buttons[]=
    {
      "Press any button to map or unmap the Command widget",
      (char *) NULL
    },
    *options[]=
    {
      "-authenticate value  decrypt image with this password",
      "-backdrop            display image centered on a backdrop",
      "-colormap type       Shared or Private",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-display server      display image to this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the Image window",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-limit type value    Disk, Map, or Memory resource limit",
      "-matte               store matte channel if the image has one",
      "-map type            display image using this Standard Colormap",
      "-monochrome          transform image to black and white",
      "-noop                do not apply options to image",
      "-pause               seconds to pause before reanimating",
      "-remote command      execute a command in an remote display process",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling-factor geometry",
      "                     horizontal and vertical sampling factor",
      "-scenes range        image scene range",
      "-size geometry       width and height of image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-visual type         display image using this visual type",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-window id           display image to background of this window",
      (char *) NULL
    };

  (void) printf("Version: %.1024s\n",GetMagickVersion((unsigned long *) NULL));
  (void) printf("Copyright: %.1024s\n\n",GetMagickCopyright());
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -name,\n");
  (void) printf("-mattecolor, -shared-memory, or -title.\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  (void) printf("\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
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

#if defined(WIN32)
int WINAPI WinMain(HINSTANCE instance,HINSTANCE last,LPSTR command,int state)
{
  char
    **argv;
  
  int
    argc,
    main(int,char **);
  
  argv=StringToArgv(command,&argc);
  return(main(argc,argv));
}
#endif

int main(int argc,char **argv)
{
#if defined(HasX11)
  char
    *client_name,
    *option,
    *resource_value,
    *server_name;

  Display
    *display;

  double
    sans;

  ExceptionInfo
    exception;

  Image
    *image,
    *image_list,
    *loaded_image,
    *next_image;

  ImageInfo
    *image_info;

  long
    first_scene,
    j,
    k,
    last_scene,
    scene,
    x;

  QuantizeInfo
    *quantize_info;

  register Image
    *p;

  register long
    i;

  unsigned int
    status;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Initialize command line arguments.
  */
  InitializeMagick(*argv);
  ReadCommandlLine(argc,&argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      (char *) NULL);
  /*
    Set defaults.
  */
  SetNotifyHandlers;
  display=(Display *) NULL;
  GetExceptionInfo(&exception);
  first_scene=0;
  image=(Image *) NULL;
  image_list=(Image *) NULL;
  last_scene=0;
  server_name=(char *) NULL;
  status=True;
  /*
    Check for server name specified on the command line.
  */
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
          MagickFatalError(OptionFatalError,"MissingServerName",option);
        server_name=argv[i];
        break;
      }
    if (LocaleCompare("help",option+1) == 0)
      AnimateUsage();
    if (LocaleCompare("version",option+1) == 0)
      {
        (void) fprintf(stdout,"Version: %.1024s\n",
          GetMagickVersion((unsigned long *) NULL));
        (void) fprintf(stdout,"Copyright: %.1024s\n\n",GetMagickCopyright());
        Exit(0);
      }
  }
  /*
    Get user defaults from X resource database.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickFatalError(XServerFatalError,"UnableToOpenXServer",
      XDisplayName(server_name));
 (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  image_info->density=
    XGetResourceInstance(resource_database,client_name,"density",(char *) NULL);
  if (image_info->density == (char *) NULL)
    image_info->density=XGetScreenDensity(display);
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
    MagickError(OptionFatalError,"InvalidImageInterlace",resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=IsTrue(resource_value);
  resource_value=
    XGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=IsTrue(resource_value);
  /*
    Parse command line.
  */
  j=1;
  k=0;
  for (i=1; i <= argc; i++)
  {
    if (i < argc)
      option=argv[i];
    else
      if (image != (Image *) NULL)
        break;
      else
        if (!isatty(STDIN_FILENO))
          option=(char *) "-";
        else
          option=(char *) "logo:Untitled";
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        /*
          Option is a file name.
        */
        k=i;
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Read image.
          */
          (void) strncpy(image_info->filename,option,MaxTextExtent-1);
          if (first_scene != last_scene)
            {
              char
                filename[MaxTextExtent];

              /*
                Form filename for multi-part images.
              */
              FormatString(filename,image_info->filename,scene);
              if (LocaleCompare(filename,image_info->filename) == 0)
                FormatString(filename,"%.1024s[%lu]",image_info->filename,
                  scene);
              (void) strncpy(image_info->filename,filename,MaxTextExtent-1);
            }
          image_info->colorspace=quantize_info->colorspace;
          image_info->dither=quantize_info->dither;
          next_image=ReadImage(image_info,&exception);
          if (exception.severity != UndefinedException)
            CatchException(&exception);
          status&=next_image != (Image *) NULL;
          if (next_image == (Image *) NULL)
            continue;
          if (image == (Image *) NULL)
            {
              image=next_image;
              continue;
            }
          /*
            Link image into image list.
          */
          for (p=image; p->next != (Image *) NULL; p=p->next);
          next_image->previous=p;
          p->next=next_image;
        }
        continue;
      }
    if (j != (k+1))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        (void) CatchImageException(image);
        PushImageList(&image_list,image,&exception);
        DestroyImageList(image);
        image=(Image *) NULL;
        j=k+1;
      }
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingPassword",option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("backdrop",option+1) == 0)
          {
            resource_info.backdrop=(*option == '-');
            break;
          }
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingColor",option);
                resource_info.background_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  &exception);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingColor",option);
                resource_info.border_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  &exception);
              }
            break;
          }
        if (LocaleCompare("borderwidth",option+1) == 0)
          {
            resource_info.border_width=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"MissingWidth",option);
                resource_info.border_width=atoi(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'c':
      {
        if (LocaleCompare("cache",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"MissingThreshold",option);
                SetMagickResourceLimit(MemoryResource,atol(argv[i]));
                SetMagickResourceLimit(MapResource,2*atol(argv[i]));
              }
            break;
          }
        if (LocaleCompare("colormap",option+1) == 0)
          {
            resource_info.colormap=PrivateColormap;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingType",option);
                option=argv[i];
                resource_info.colormap=UndefinedColormap;
                if (LocaleCompare("private",option) == 0)
                  resource_info.colormap=PrivateColormap;
                if (LocaleCompare("shared",option) == 0)
                  resource_info.colormap=SharedColormap;
                if (resource_info.colormap == UndefinedColormap)
                  MagickFatalError(OptionFatalError,"InvalidColormapType",
                    option);
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
                  MagickFatalError(OptionFatalError,"MissingColors",option);
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
                  MagickFatalError(OptionFatalError,"MissingType",option);
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
                  MagickFatalError(OptionFatalError,"InvalidColorspaceType",
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
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingEventMask",
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
                  MagickFatalError(OptionFatalError,"MissingSeconds",option);
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
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
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
                  MagickFatalError(OptionFatalError,"MissingImageDepth",option);
                image_info->depth=atol(argv[i]);
              }
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingServerName",option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info->dither=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingFontName",option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            if ((image_info->font == (char *) NULL) ||
                (*image_info->font != '@'))
              resource_info.font=AllocateString(image_info->font);
            break;
          }
        if (LocaleCompare("foreground",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingForeground",
                    option);
                resource_info.foreground_color=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              MagickFatalError(OptionFatalError,"Missing value",option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            resource_info.image_geometry=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
                resource_info.image_geometry=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            AnimateUsage();
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("iconGeometry",option+1) == 0)
          {
            resource_info.icon_geometry=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
                resource_info.icon_geometry=argv[i];
              }
            break;
          }
        if (LocaleCompare("iconic",option+1) == 0)
          {
            resource_info.iconic=(*option == '-');
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingType",option);
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
                  MagickFatalError(OptionFatalError,"InvalidInterlaceType",
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'l':
      {
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                char
                  *type;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingResourceType",
                    option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"MissingResourceLimit",
                    option);
                if (LocaleCompare("disk",type) == 0)
                  SetMagickResourceLimit(DiskResource,atol(argv[i]));
                else
                  if (LocaleCompare("map",type) == 0)
                    SetMagickResourceLimit(MapResource,atol(argv[i]));
                  else
                    if (LocaleCompare("memory",type) == 0)
                      SetMagickResourceLimit(MemoryResource,atol(argv[i]));
                    else
                      MagickFatalError(OptionFatalError,
                        "UnrecognizedResourceType",type);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            (void) strcpy(argv[i]+1,"sans");
            resource_info.map_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingMapType",option);
                resource_info.map_type=argv[i];
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
                  MagickFatalError(OptionFatalError,"MissingColor",option);
                resource_info.matte_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  &exception);
              }
            break;
          }
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
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'n':
      {
        if (LocaleCompare("name",option+1) == 0)
          {
            resource_info.name=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingName",option);
                resource_info.name=argv[i];
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("pause",option+1) == 0)
          {
            resource_info.pause=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"MissingSeconds",option);
              }
            resource_info.pause=atoi(argv[i]);
            break;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("remote",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,"MissingCommand",option);
            status=XRemoteCommand(display,resource_info.window_id,argv[i]);
            Exit(!status);
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              MagickFatalError(OptionFatalError,"MissingDegrees",option);
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
              }
            break;
          }
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,"MissingSceneNumber",
                    option);
                first_scene=atol(argv[i]);
                last_scene=first_scene;
                (void) sscanf(argv[i],"%ld-%ld",&first_scene,&last_scene);
              }
            break;
          }
        if (LocaleCompare("shared-memory",option+1) == 0)
          {
            resource_info.use_shared_memory=(*option == '-');
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,"MissingGeometry",option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("text-font",option+1) == 0)
          {
            resource_info.text_font=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingFontName",option);
                resource_info.text_font=argv[i];
              }
            break;
          }
        if (LocaleCompare("title",option+1) == 0)
          {
            resource_info.title=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingTitle",option);
                resource_info.title=argv[i];
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
                  MagickFatalError(OptionFatalError,"MissingDepth",option);
                quantize_info->tree_depth=atoi(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          break;
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingVirtualPixelMethod",
                    option);
                option=argv[i];
                virtual_pixel_method=UndefinedVirtualPixelMethod;
                if (LocaleCompare("Constant",option) == 0)
                  virtual_pixel_method=ConstantVirtualPixelMethod;
                if (LocaleCompare("Edge",option) == 0)
                  virtual_pixel_method=EdgeVirtualPixelMethod;
                if (LocaleCompare("Mirror",option) == 0)
                  virtual_pixel_method=MirrorVirtualPixelMethod;
                if (LocaleCompare("Tile",option) == 0)
                  virtual_pixel_method=TileVirtualPixelMethod;
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  MagickFatalError(OptionFatalError,
                    "UnrecognizedVirtualPixelMethod",option);
              }
            break;
          }
        if (LocaleCompare("visual",option+1) == 0)
          {
            resource_info.visual_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingVisualClass",
                    option);
                resource_info.visual_type=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case 'w':
      {
        if (LocaleCompare("window",option+1) == 0)
          {
            resource_info.window_id=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,"MissingIDNameOrRoot",
                    option);
                resource_info.window_id=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
      case '?':
      {
        AnimateUsage();
        break;
      }
      default:
      {
        MagickFatalError(OptionFatalError,"UnrecognizedOption",option);
        break;
      }
    }
  }
  i--;
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    MagickFatalError(OptionFatalError,"MissingAnImageFilename",(char *) NULL);
  if (image == (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image_list);
      (void) CatchImageException(image_list);
    }
  else
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      (void) CatchImageException(image);
      PushImageList(&image_list,image,&exception);
      DestroyImageList(image);
    }
  if (resource_info.window_id != (char *) NULL)
    XAnimateBackgroundImage(display,&resource_info,image_list);
  else
    {
      /*
        Animate image to X server.
      */
      loaded_image=XAnimateImages(display,&resource_info,argv,argc,image_list);
      while (loaded_image != (Image *) NULL)
      {
        image_list=loaded_image;
        loaded_image=
          XAnimateImages(display,&resource_info,argv,argc,image_list);
      }
    }
  DestroyImageList(image_list);
  LiberateMemory((void **) &argv);
  DestroyMagick();
  Exit(!status);
#else
  MagickFatalError(MissingDelegateError,"XWindowLibraryIsNotAvailable",
    (char *) NULL);
#endif
  return(False);
}
