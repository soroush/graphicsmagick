/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
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
  WriteSVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSVG returns True if the image format type, identified by the
%  magick string, is SVG.
%
%  The format of the IsSVG method is:
%
%      unsigned int IsSVG(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSVG returns True if the image format type is SVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSVG(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 5)
    return(False);
  if (strncmp((char *) magick,"<?XML",5) == 0)
    return(True);
  if (strncmp((char *) magick,"<?xml",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int GetToken(Image *image,char **token,int *c,
  ExceptionInfo *exception)
{
  register char
    *p;

  unsigned int
    length,
    quote;

  length=1;
  p=(*token);
  for (*p='\0'; *c != EOF; )
  {
    while (isspace((int) *c))
      *c=ReadByte(image);
    quote=(*c) == '"';
    for ( ; ; )
    {
      if (Extent(*token) >= (length-1))
        {
          length=(length << 1)+MaxTextExtent;
          *token=(char *) ReallocateMemory(*token,length);
          if (*token == (char *) NULL)
            ThrowBinaryException(ResourceLimitWarning,"Unable to get token",
              "Memory allocation failed");
          p=(*token)+Extent(*token);
        }
      if (*c != '"')
        *p++=(*c);
      *p='\0';
      *c=ReadByte(image);
      if (!quote)
        break;
      if ((*c == '"') || (*c == EOF))
        break;
    }
    if (quote)
      {
        *c=ReadByte(image);
        break;
      }
    p--;
    if (!isalnum(*p) && (*p != '-') && (*p != '>'))
      break;
    p++;
    if (!isalnum(*c) && (*c != '-') && (*c != '>'))
      break;
  }
  if (*c == EOF)
    return(False);
  return(True);
}

Export char **StringToTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int) (*p)))
      p++;
    (*number_tokens)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    while (!isspace((int) (*p)) && (*p != '\0'))
    {
      p++;
      if (!isspace((int) *p) && (*(p-1) == ':'))
        (*number_tokens)++;
    }
  }
  tokens=(char **) AllocateMemory((*number_tokens+1)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  p=(char *) text;
  for (i=0; i < *number_tokens; i++)
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
      if (*p == '\'')
        {
          for (q++; (*q != '\'') && (*q != '\0'); q++);
          q++;
        }
      else
        while (!isspace((int) (*q)) && (*q != '\0'))
        {
          q++;
          if (!isspace((int) *q) && (*(q-1) == ':'))
            break;
        }
    tokens[i]=(char *) AllocateMemory(q-p+1);
    if (tokens[i] == (char *) NULL)
      MagickError(ResourceLimitError,"Unable to convert string to tokens",
        "Memory allocation failed");
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    p=q;
    if (*(q-1) == ':')
      continue;
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  tokens[i]=(char *) NULL;
  return(tokens);
}

static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  typedef struct _EllipseInfo
  {
    int
      cx,
      cy,
      major,
      minor,
      angle;
  } EllipseInfo;

  char
    *filename,
    *fill,
    geometry[MaxTextExtent],
    *keyword,
    points[MaxTextExtent],
    *primitive,
    *stroke,
    *text,
    *token,
    **tokens,
    *value,
    *vertices;

  double
    opacity;

  DrawInfo
    *draw_info;

  EllipseInfo
    ellipse;

  Image
    *canvas,
    *clone_image,
    *image,
    *tile;

  ImageInfo
    *clone_info;

  int
    c,
    length,
    number_tokens;

  RectangleInfo
    page_info;

  SegmentInfo
    segment;

  register int
    i;

  unsigned int
    quote,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->filename,"xc:white");
  canvas=ReadImage(clone_info,exception);
  if (canvas == (Image *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  (void) strcpy(canvas->filename,image->filename);
  SetImage(canvas,Opaque);
  /*
    Interpret SVG language.
  */
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  GetPageInfo(&page_info);
  primitive=AllocateString("none");
  fill=AllocateString("none");
  filename=AllocateString("logo:");
  keyword=AllocateString("none");
  quote=False;
  opacity=1.0;
  stroke=AllocateString("none");
  text=AllocateString("none");
  token=AllocateString("none");
  value=AllocateString("none");
  vertices=AllocateString("0,0 1,1");
  /*
    Parse SVG drawing primitives.
  */
  length=MaxTextExtent;
  c=ReadByte(image);
  while (GetToken(image,&token,&c,exception))
  {
    /*
      Interpret the SVG tokens.
    */
    (void) printf("token: %s\n",token);
    if (*token == '>')
      {
        quote=True;
        CloneString(&text,"");
      }
    else
      if (*token == '<')
        quote=False;
    if (quote)
      {
        if (isalnum(*token))
          (void) ConcatenateString(&text," ");
        (void) ConcatenateString(&text,token);
      }
    if (Latin1Compare(token,"=") == 0)
      (void) GetToken(image,&value,&c,exception);
    if (Latin1Compare(keyword,"angle") == 0)
      (void) sscanf(value,"%d",&ellipse.angle);
    if (Latin1Compare(keyword,"circle") == 0)
      CloneString(&primitive,"circle");
    if (Latin1Compare(keyword,"cx") == 0)
      (void) sscanf(value,"%d",&ellipse.cx);
    if (Latin1Compare(keyword,"cy") == 0)
      (void) sscanf(value,"%d",&ellipse.cy);
    if (Latin1Compare(keyword,"d") == 0)
      {
        (void) ConcatenateString(&vertices," ");
        (void) ConcatenateString(&vertices,value);
        if (strchr(value,'M') != (char *) NULL)
          CloneString(&primitive,"none");
      }
    if (Latin1Compare(keyword,"ellipse") == 0)
      CloneString(&primitive,"ellipse");
    if (Latin1Compare(keyword,"g") == 0)
      {
         CloneString(&primitive,"none");
         (void) CloneString(&fill,"none");
         (void) CloneString(&stroke,"none");
         opacity=1.0;
      }
    if (Latin1Compare(keyword,"height") == 0)
      {
        (void) sscanf(value,"%u",&page_info.height);
        if (Latin1Compare(value+strlen(value)-2,"cm") == 0)
          page_info.height=72*page_info.height/2.5;
        if (Latin1Compare(value+strlen(value)-2,"in") == 0)
          page_info.height=72*page_info.height;
        FormatString(geometry,"%ux%u!",canvas->columns,page_info.height);
        if ((canvas->columns < page_info.width) ||
            (canvas->rows < page_info.height))
          TransformImage(&canvas,(char *) NULL,geometry);
      }
    if (Latin1Compare(keyword,"href") == 0)
      (void) CloneString(&filename,value);
    if (Latin1Compare(keyword,"line") == 0)
      CloneString(&primitive,"line");
    if (Latin1Compare(keyword,"image") == 0)
      CloneString(&primitive,"image");
    if (Latin1Compare(keyword,"major") == 0)
      (void) sscanf(value,"%d",&ellipse.major);
    if (Latin1Compare(keyword,"minor") == 0)
      (void) sscanf(value,"%d",&ellipse.minor);
    if (Latin1Compare(keyword,"polygon") == 0)
      CloneString(&primitive,"polygon");
    if (Latin1Compare(keyword,"polyline") == 0)
      CloneString(&primitive,"polyline");
    if (Latin1Compare(keyword,"path") == 0)
      CloneString(&primitive,"polygon");
    if (Latin1Compare(keyword,"points") == 0)
      (void) CloneString(&vertices,value);
    if (Latin1Compare(keyword,"r") == 0)
      {
        (void) sscanf(value,"%d",&ellipse.major);
        (void) sscanf(value,"%d",&ellipse.minor);
      }
    if (Latin1Compare(keyword,"rect") == 0)
      CloneString(&primitive,"rectangle");
    if (Latin1Compare(keyword,"style") == 0)
      {
        tokens=StringToTokens(value,&number_tokens);
        for (i=0; i < (number_tokens-1); i++)
        {
          if (Latin1Compare(tokens[i],"fill:") == 0)
            {
              (void) CloneString(&fill,tokens[++i]);
              if (Latin1Compare(fill+Extent(fill)-1,";") == 0)
                fill[Extent(fill)-1]='\0';
            }
          if (Latin1Compare(tokens[i],"fill-opacity:") == 0)
            (void) sscanf(tokens[++i],"%lf",&opacity);
          if (Latin1Compare(tokens[i],"font-size:") == 0)
            (void) sscanf(tokens[++i],"%lf",&draw_info->pointsize);
          if (Latin1Compare(tokens[i],"stroke:") == 0)
            {
              (void) CloneString(&stroke,tokens[++i]);
              if (Latin1Compare(stroke+Extent(stroke)-1,";") == 0)
                stroke[Extent(stroke)-1]='\0';
            }
          if (Latin1Compare(tokens[i],"stroke-antialiasing:") == 0)
            draw_info->antialias=Latin1Compare(tokens[++i],"true");
          if (Latin1Compare(tokens[i],"stroke-width:") == 0)
            (void) sscanf(tokens[++i],"%d",&draw_info->linewidth);
          if (Latin1Compare(tokens[i],"text-antialiasing:") == 0)
            draw_info->antialias=Latin1Compare(tokens[++i],"true");
          FreeMemory((void *) &tokens[i]);
        }
        FreeMemory((void *) &tokens);
      }
    if (Latin1Compare(keyword,"text") == 0)
      CloneString(&primitive,"text");
    if (Latin1Compare(keyword,"verts") == 0)
      (void) CloneString(&vertices,value);
    if (Latin1Compare(keyword,"viewBox") == 0)
      (void) sscanf(value,"%d %d %u %u",&page_info.x,&page_info.y,
        &page_info.width,&page_info.height);
    if (Latin1Compare(keyword,"width") == 0)
      {
        (void) sscanf(value,"%u",&page_info.width);
        if (Latin1Compare(value+strlen(value)-2,"cm") == 0)
          page_info.width=72*page_info.width/2.5;
        if (Latin1Compare(value+strlen(value)-2,"in") == 0)
          page_info.width=72*page_info.width;
        FormatString(geometry,"%ux%u!",page_info.width,canvas->rows);
        if ((canvas->columns < page_info.width) ||
            (canvas->rows < page_info.height))
          TransformImage(&canvas,(char *) NULL,geometry);
      }
    if (Latin1Compare(keyword,"x") == 0)
      (void) sscanf(value,"%u",&page_info.x);
    if (Latin1Compare(keyword,"x1") == 0)
      (void) sscanf(value,"%lf",&segment.x1);
    if (Latin1Compare(keyword,"x2") == 0)
      (void) sscanf(value,"%lf",&segment.x2);
    if (Latin1Compare(keyword,"y") == 0)
      (void) sscanf(value,"%u",&page_info.y);
    if (Latin1Compare(keyword,"y1") == 0)
      (void) sscanf(value,"%lf",&segment.y1);
    if (Latin1Compare(keyword,"y2") == 0)
      (void) sscanf(value,"%lf",&segment.y2);
    if (((Latin1Compare(keyword,">") == 0) ||
         (Latin1Compare(keyword,"text>") == 0)) &&
        (Latin1Compare(primitive,"none") != 0))
      {
        char
          *command;

        unsigned int
          length;

        /*
          Render graphic primitive.
        */
        length=strlen(primitive)+MaxTextExtent;
        if (vertices != (char *) NULL)
          length+=strlen(vertices);
        command=(char *) AllocateMemory(length);
        if (command == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,
            "Unable to allocate memory",image);
        (void) strcpy(command,"    ");
        (void) strcat(command,primitive);
        (void) strcat(command," ");
        if (Latin1Compare(primitive,"circle") == 0)
          {
            if (Latin1Compare(fill,"none") != 0)
              (void) strncpy(command,"fill",4);
            FormatString(points,"%d,%d %d,%d",ellipse.cx,ellipse.cy,
              ellipse.cx,ellipse.cy+ellipse.minor);
            (void) strcat(command,points);
          }
        if (Latin1Compare(primitive,"ellipse") == 0)
          {
            if (Latin1Compare(fill,"none") != 0)
              (void) strncpy(command,"fill",4);
            FormatString(points,"%d,%d %d,%d 0,360",ellipse.cx,ellipse.cy,
              ellipse.angle == 0 ? ellipse.major: ellipse.minor,
              ellipse.angle == 0 ? ellipse.minor: ellipse.major);
            (void) strcat(command,points);
          }
        if (Latin1Compare(primitive,"line") == 0)
          {
            FormatString(points,"%f,%f %f,%f",segment.x1,segment.y1,
              segment.x2,segment.y2);
            (void) strcat(command,points);
          }
        if (Latin1Compare(primitive,"polyline") == 0)
          {
            for (i=0; i < strlen(vertices); i++)
              if (!isdigit(vertices[i]) && (vertices[i] != ','))
                vertices[i]=' ';
            (void) strcat(command,vertices);
          }
        if (Latin1Compare(primitive,"polygon") == 0)
          {
            if (Latin1Compare(fill,"none") != 0)
              (void) strncpy(command,"fill",4);
            for (i=0; i < strlen(vertices); i++)
              if (!isdigit(vertices[i]) && (vertices[i] != ','))
                vertices[i]=' ';
            (void) strcat(command,vertices);
          }
        if (Latin1Compare(primitive,"rectangle") == 0)
          {
            if (Latin1Compare(fill,"none") != 0)
              (void) strncpy(command,"fill",4);
            FormatString(points,"%d,%d %d,%d",page_info.x,page_info.y,
              page_info.x+page_info.width,page_info.y+page_info.height);
            (void) strcat(command,points);
          }
        if (Latin1Compare(primitive,"image") == 0)
          {
            FormatString(points,"%d,%d",page_info.x,page_info.y);
            (void) strcat(command,points);
            (void) strcat(command," ");
            (void) strcat(command,filename);
          }
        if (Latin1Compare(primitive,"text") == 0)
          {
            FormatString(points,"%d,%f",page_info.x,page_info.y-
              draw_info->pointsize/2);
            (void) strcat(command,points);
            (void) strcat(command," '");
            (void) strcat(command,text+1);
            (void) strcat(command,"'");
          }
        tile=ReadImage(clone_info,exception);
        if (tile == (Image *) NULL)
          ThrowReaderException(ResourceLimitWarning,
            "Unable to draw primitive",image);
        (void) CloneString(&draw_info->pen,"black");
        if (Latin1Compare(stroke,"none") != 0)
          (void) CloneString(&draw_info->pen,stroke);
        if ((strncmp(command,"fill",4) == 0) ||
            (Latin1Compare(stroke,"none") == 0))
          (void) CloneString(&draw_info->pen,fill);
        (void) QueryColorDatabase(draw_info->pen,&tile->background_color);
        SetImage(tile,(Quantum) (Opaque*opacity));
        if (draw_info->tile != (Image *) NULL)
          DestroyImage(draw_info->tile);
        draw_info->tile=tile;
        (void) CloneString(&draw_info->primitive,command);
        (void) printf("draw:  %s\n",command);
        status=DrawImage(canvas,draw_info);
        if (status == False)
          ThrowReaderException(ResourceLimitWarning,
            "Unable to draw primitive",image);
        if ((strncmp(command,"fill",4) == 0) &&
            (Latin1Compare(stroke,"none") != 0))
          {
            tile=ReadImage(clone_info,exception);
            if (tile == (Image *) NULL)
              ThrowReaderException(ResourceLimitWarning,
                "Unable to draw primitive",image);
            (void) CloneString(&draw_info->pen,stroke);
            (void) QueryColorDatabase(draw_info->pen,&tile->background_color);
            SetImage(tile,(Quantum) (Opaque*opacity));
            if (draw_info->tile != (Image *) NULL)
              DestroyImage(draw_info->tile);
            draw_info->tile=tile;
            (void) CloneString(&draw_info->primitive,command+4);
            (void) printf("draw:  %s\n",command);
            status=DrawImage(canvas,draw_info);
            if (status == False)
              ThrowReaderException(ResourceLimitWarning,
                "Unable to draw primitive",image);
          }
        FreeMemory((void *) &command);
        CloneString(&primitive,"none");
        CloneString(&fill,"none");
        CloneString(&stroke,"none");
      }
    (void) CloneString(&keyword,token);
  }
  DestroyDrawInfo(draw_info);
  DestroyImage(image);
  FreeMemory((void *) &vertices);
  FreeMemory((void *) &value);
  FreeMemory((void *) &token);
  FreeMemory((void *) &text);
  FreeMemory((void *) &stroke);
  FreeMemory((void *) &primitive);
  FreeMemory((void *) &keyword);
  FreeMemory((void *) &fill);
  FreeMemory((void *) &filename);
  return(canvas);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
Export void RegisterSVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SVG");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  RegisterMagickInfo(entry);
}
