/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           AAA   N   N  N   N   OOO   TTTTT   AAA   TTTTT  EEEEE             %
%          A   A  NN  N  NN  N  O   O    T    A   A    T    E                 %
%          AAAAA  N N N  N N N  O   O    T    AAAAA    T    EEE               %
%          A   A  N  NN  N  NN  O   O    T    A   A    T    E                 %
%          A   A  N   N  N   N   OOO     T    A   A    T    EEEEE             %
%                                                                             %
%                                                                             %
%                  ImageMagick Image Annotation Methods                       %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Constant declarations.
*/
const char
  *Alphabet = "`-=[]\\;',./~!@#$%^&*()_+{}|:\"<>?" \
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n n o t a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AnnotateImage annotates an image with text.  Optionally the
%  annotation can include the image filename, type, width, height, or scene
%  number by embedding special format characters.
%
%  The format of the AnnotateImage method is:
%
%      unsigned int AnnotateImage(Image *image,
%        const AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o annotate_info: The address of a AnnotateInfo structure.
%
%
*/
MagickExport unsigned int AnnotateImage(Image *image,
  const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *clone_info;

  char
    label[MaxTextExtent],
    *text,
    **textlist;

  Image
    *annotate_image;

  ImageInfo
    *image_info;

  int
    x,
    y;

  register int
    i;

  unsigned int
    height,
    length,
    matte,
    number_lines,
    width;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  if (annotate_info->text == (char *) NULL)
    return(False);
  if (*annotate_info->text == '\0')
    return(False);
  /*
    Translate any embedded format characters (e.g. %f).
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  clone_info=CloneAnnotateInfo(image_info,annotate_info);
  text=TranslateText((ImageInfo *) NULL,image,clone_info->text);
  if (text == (char *) NULL)
    {
      DestroyAnnotateInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
        "Memory allocation failed");
    }
  textlist=StringToList(text);
  FreeMemory((void **) &text);
  if (textlist == (char **) NULL)
    {
      DestroyAnnotateInfo(clone_info);
      return(False);
    }
  length=Extent(textlist[0]);
  for (i=1; textlist[i] != (char *) NULL; i++)
    if (Extent(textlist[i]) > (int) length)
      length=Extent(textlist[i]);
  number_lines=i;
  text=(char *) AllocateMemory(length+MaxTextExtent);
  if (text == (char *) NULL)
    {
      DestroyAnnotateInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to annotate image",
        "Memory allocation failed");
    }
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  if (clone_info->geometry != (char *) NULL)
    {
      unsigned int
        flags;

      /*
        User specified annotation geometry.
      */
      flags=ParseGeometry(clone_info->geometry,&x,&y,&width,&height);
      if ((flags & XNegative) != 0)
        x+=image->columns;
      if ((flags & WidthValue) == 0)
        width-=2*x > width ? width : 2*x;
      if ((flags & YNegative) != 0)
        y+=image->rows;
      if ((flags & HeightValue) == 0)
        height-=2*y > height ? height : 2*y;
    }
  /*
    Annotate image.
  */
  matte=image->matte;
  image_info->font=AllocateString(annotate_info->font);
  image_info->pointsize=annotate_info->pointsize;
  for (i=0; i < 6; i++)
    image_info->affine[i]=annotate_info->affine[i];
  image_info->antialias=annotate_info->antialias;
  image_info->fill=annotate_info->fill;
  image_info->stroke=annotate_info->stroke;
  for (i=0; textlist[i] != (char *) NULL; i++)
  {
    if (*textlist[i] == '\0')
      {
        FreeMemory((void **) &textlist[i]);
        continue;
      }
    /*
      Convert text to image.
    */
    FormatString(label,"label:%.1024s",textlist[i]);
    FreeMemory((void **) &textlist[i]);
    (void) strcpy(image_info->filename,label);
    annotate_image=ReadImage(image_info,&image->exception);
    if (annotate_image == (Image *) NULL)
      break;
    if (annotate_info->decorate != NoDecoration)
      {
        register PixelPacket
          *q;

        /*
          Decorate text.
        */
        if (annotate_info->decorate == OverlineDecoration)
          q=GetImagePixels(annotate_image,0,0,annotate_image->columns,1);
        else
          if (annotate_info->decorate == UnderlineDecoration)
            q=GetImagePixels(annotate_image,0,(int)
              annotate_image->bounding_box.y2,annotate_image->columns,1);
          else
            q=GetImagePixels(annotate_image,0,(int)
              (annotate_image->bounding_box.y2/2.0),annotate_image->columns,1);
        if (q != (PixelPacket *) NULL)
          {
            register int
              j;

            for (j=0; j < annotate_image->columns; j++)
              *q++=annotate_info->fill;
            SyncImagePixels(annotate_image);
          }
     }
    if (annotate_info->degrees != 0.0)
      {
        Image
          *rotated_image;

        /*
          Rotate text.
        */
        rotated_image=RotateImage(annotate_image,annotate_info->degrees,
          &annotate_image->exception);
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(annotate_image);
            annotate_image=rotated_image;
          }
        clone_info->bounds.height=annotate_image->rows;
      }
    /*
      Composite text onto the image.
    */
    switch (clone_info->gravity)
    {
      case NorthWestGravity:
      {
        clone_info->bounds.x=x;
        clone_info->bounds.y=y+i*clone_info->bounds.height;
        break;
      }
      case NorthGravity:
      {
        clone_info->bounds.x=x+(width/2.0)-(annotate_image->columns/2.0);
        clone_info->bounds.y=y+i*clone_info->bounds.height;
        break;
      }
      case NorthEastGravity:
      {
        clone_info->bounds.x=width-annotate_image->columns+x;
        clone_info->bounds.y=y+i*clone_info->bounds.height;
        break;
      }
      case WestGravity:
      {
        clone_info->bounds.x=x;
        clone_info->bounds.y=y+(height/2.0)+i*clone_info->bounds.height-
          (clone_info->bounds.height/2.0);
        break;
      }
      case ForgetGravity:
      case StaticGravity:
      case CenterGravity:
      default:
      {
        clone_info->bounds.x=x+(width/2.0)-(annotate_image->columns/2.0);
        clone_info->bounds.y=y+(height/2.0)+i*clone_info->bounds.height-
          (clone_info->bounds.height/2.0);
        break;
      }
      case EastGravity:
      {
        clone_info->bounds.x=x+width-annotate_image->columns;
        clone_info->bounds.y=y+(height/2.0)+i*clone_info->bounds.height-
          (clone_info->bounds.height/2.0);
        break;
      }
      case SouthWestGravity:
      {
        clone_info->bounds.x=x;
        clone_info->bounds.y=y+height+i*clone_info->bounds.height-
          clone_info->bounds.height;
        break;
      }
      case SouthGravity:
      {
        clone_info->bounds.x=x+(width/2.0)-(annotate_image->columns/2.0);
        clone_info->bounds.y=y+height+i*clone_info->bounds.height-
          clone_info->bounds.height;
        break;
      }
      case SouthEastGravity:
      {
        clone_info->bounds.x=x+width-annotate_image->columns;
        clone_info->bounds.y=y+height-i*clone_info->bounds.height-
          clone_info->bounds.height;
        break;
      }
    }
    if (annotate_info->box.opacity != TransparentOpacity)
      {
        Image
          *box_image;

        /*
          Surround text with a border.
        */
        box_image=CloneImage(annotate_image,annotate_image->columns,
          clone_info->bounds.height,True,&image->exception);
        if (box_image != (Image *) NULL)
          {
            box_image->background_color=annotate_info->box;
            SetImage(box_image,OpaqueOpacity);
            CompositeImage(box_image,AnnotateCompositeOp,annotate_image,0,0);
            box_image->matte=False;
            DestroyImage(annotate_image);
            annotate_image=box_image;
          }
      }
    CompositeImage(image,
      annotate_image->matte ? AnnotateCompositeOp : ReplaceCompositeOp,
      annotate_image,clone_info->bounds.x,clone_info->bounds.y);
    DestroyImage(annotate_image);
  }
  image->matte=matte;
  DestroyImageInfo(image_info);
  DestroyAnnotateInfo(clone_info);
  FreeMemory((void **) &text);
  for ( ; textlist[i] != (char *) NULL; i++)
    FreeMemory((void **) &textlist[i]);
  FreeMemory((void **) &textlist);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e A n n o t a t e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneAnnotateInfo makes a duplicate of the given annotate info, or if
%  annotate info is NULL, a new one.
%
%  The format of the CloneAnnotateInfo method is:
%
%      AnnotateInfo *CloneAnnotateInfo(const ImageInfo *image_info,
%        const AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneAnnotateInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type info.
%
%    o annotate_info: a structure of type info.
%
%
*/
MagickExport AnnotateInfo *CloneAnnotateInfo(const ImageInfo *image_info,
  const AnnotateInfo *annotate_info)
{
  AnnotateInfo
    *clone_info;

  clone_info=(AnnotateInfo *) AllocateMemory(sizeof(AnnotateInfo));
  if (clone_info == (AnnotateInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone annotate info",
      "Memory allocation failed");
  if (annotate_info == (AnnotateInfo *) NULL)
    {
      GetAnnotateInfo(image_info,clone_info);
      return(clone_info);
    }
  *clone_info=(*annotate_info);
  if (annotate_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(annotate_info->geometry);
  if (annotate_info->text != (char *) NULL)
    clone_info->text=AllocateString(annotate_info->text);
  if (annotate_info->font != (char *) NULL)
    clone_info->font=AllocateString(annotate_info->font);
  if (annotate_info->font_name != (char *) NULL)
    clone_info->font_name=AllocateString(annotate_info->font_name);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y A n n o t a t e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyAnnotateInfo deallocates memory associated with an
%  AnnotateInfo structure.
%
%  The format of the DestroyAnnotateInfo method is:
%
%      void DestroyAnnotateInfo(AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to an AnnotateInfo structure.
%
%
*/
MagickExport void DestroyAnnotateInfo(AnnotateInfo *annotate_info)
{
  assert(annotate_info != (AnnotateInfo *) NULL);
  if (annotate_info->geometry != (char *) NULL)
    FreeMemory((void **) &annotate_info->geometry);
  if (annotate_info->text != (char *) NULL)
    FreeMemory((void **) &annotate_info->text);
  if (annotate_info->font != (char *) NULL)
    FreeMemory((void **) &annotate_info->font);
  if (annotate_info->font_name != (char *) NULL)
    FreeMemory((void **) &annotate_info->font_name);
  FreeMemory((void **) &annotate_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t A n n o t a t e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the GetAnnotateInfo method is:
%
%      void GetAnnotateInfo(const ImageInfo *image_info,
%        AnnotateInfo *annotate_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o annotate_info: Specifies a pointer to a AnnotateInfo structure.
%
%
*/
MagickExport void GetAnnotateInfo(const ImageInfo *image_info,
  AnnotateInfo *annotate_info)
{
  ExceptionInfo
    exception;

  ImageAttribute
    *attribute;

  Image
    *annotate_image;

  ImageInfo
    *clone_info;

  register int
    i;

  /*
    Initialize annotate attributes;
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  annotate_info->geometry=(char *) NULL;
  annotate_info->text=(char *) NULL;
  annotate_info->font=AllocateString(image_info->font);
  annotate_info->antialias=image_info->antialias;
  annotate_info->gravity=NorthWestGravity;
  annotate_info->pointsize=image_info->pointsize;
  annotate_info->degrees=0.0;
  for (i=0; i < 6; i++)
    annotate_info->affine[i]=(i == 0) || (i == 3) ? 1.0 : 0.0;
  annotate_info->fill=image_info->fill;
  annotate_info->stroke=image_info->stroke;
  (void) QueryColorDatabase("none",&annotate_info->box);
  annotate_info->decorate=NoDecoration;
  annotate_info->font_name=(char *) NULL;
  annotate_info->bounds.width=ceil(image_info->pointsize);
  annotate_info->bounds.height=ceil(image_info->pointsize);
  annotate_info->bounds.x=0;
  annotate_info->bounds.y=0;
  if (annotate_info->font == (char *) NULL)
    return;
  /*
    Get font bounds.
  */
  GetExceptionInfo(&exception);
  clone_info=CloneImageInfo(image_info);
  FormatString(clone_info->filename,"label:%.1024s",Alphabet);
  annotate_image=ReadImage(clone_info,&exception);
  DestroyImageInfo(clone_info);
  if (annotate_image == (Image *) NULL)
    return;
  attribute=GetImageAttribute(annotate_image,"Label");
  if (attribute != (ImageAttribute *) NULL)
    annotate_info->font_name=AllocateString(attribute->value);
  annotate_info->bounds.width=
    (annotate_image->columns+(strlen(Alphabet) >> 1))/strlen(Alphabet);
  annotate_info->bounds.height=annotate_image->rows;
  DestroyImage(annotate_image);
}
