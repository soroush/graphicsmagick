/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        DDDD   RRRR    AAA   W   W                           %
%                        D   D  R   R  A   A  W   W                           %
%                        D   D  RRRR   AAAAA  W   W                           %
%                        D   D  R R    A   A  W W W                           %
%                        DDDD   R  R   A   A   W W                            %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Drawing Methods                       %
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
  Define declarations.
*/
#define BezierQuantum  200
#define MatteMatch(color,target,delta) \
  (ColorMatch(color,target,delta) && ((color).opacity == (target).opacity))
#define MaxStacksize  (1 << 15)
#define Push(up,left,right,delta) \
  if ((s < (segment_stack+MaxStacksize)) && (((up)+(delta)) >= 0) && \
      (((up)+(delta)) < (int) image->rows)) \
    { \
      s->y1=(up); \
      s->x1=(left); \
      s->x2=(right); \
      s->y2=(delta); \
      s++; \
    }

static unsigned int
  GeneratePath(PrimitiveInfo *,const char *);

static void
  DrawPrimitive(Image *,const DrawInfo *,SegmentInfo *,PrimitiveInfo *),
  GenerateArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  GenerateBezier(PrimitiveInfo *),
  GenerateEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,
    const PointInfo),
  GenerateLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  GeneratePoint(PrimitiveInfo *,const PointInfo),
  GenerateRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  GenerateRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e D r a w I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneDrawInfo makes a duplicate of the given draw info, or if
%  draw info is NULL, a new one.
%
%  The format of the CloneDrawInfo method is:
%
%      DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
%        const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method CloneDrawInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type ImageInfo.
%
%    o draw_info: a structure of type DrawInfo.
%
%
*/
MagickExport DrawInfo *CloneDrawInfo(const ImageInfo *image_info,
  const DrawInfo *draw_info)
{
  DrawInfo
    *cloned_info;

  cloned_info=(DrawInfo *) AcquireMemory(sizeof(DrawInfo));
  if (cloned_info == (DrawInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone draw info",
      "Memory allocation failed");
  if (draw_info == (DrawInfo *) NULL)
    {
      GetDrawInfo(image_info,cloned_info);
      return(cloned_info);
    }
  *cloned_info=(*draw_info);
  if (draw_info->primitive != (char *) NULL)
    cloned_info->primitive=AllocateString(draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    cloned_info->font=AllocateString(draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    cloned_info->tile=
      CloneImage(draw_info->tile,0,0,True,&draw_info->tile->exception);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l o r F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ColorFloodfillImage floodfills the designated area with a color.
%  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the ColorFloodfillImage method is:
%
%      unsigned int ColorFloodfillImage(Image *image,const DrawInfo *draw_info,
%        const PixelPacket target,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: a structure of type DrawInfo.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int ColorFloodfillImage(Image *image,
  const DrawInfo *draw_info,const PixelPacket target,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  PixelPacket
    color;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  /*
    Set floodfill color.
  */
  if (ColorMatch(draw_info->fill,target,image->fuzz))
    return(False);
  segment_stack=(SegmentInfo *)
    AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Track "hot" pixels with the image index packets.
  */
  image->storage_class=PseudoClass;
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
      indexes[x]=False;
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Push initial segment on stack.
  */
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring pixels.
    */
    q=GetImagePixels(image,0,y,x1+1,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    indexes=GetIndexes(image);
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!ColorMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (ColorMatch(*q,target,image->fuzz) ||
            ColorMatch(*q,draw_info->fill,image->fuzz))
          break;
      indexes[x]=True;
      *q=draw_info->fill;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          if (x < image->columns)
            {
              q=GetImagePixels(image,x,y,image->columns-x,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (i=0; x < (int) image->columns; x++)
              {
                if (method == FloodfillMethod)
                  {
                    if (!ColorMatch(*q,target,image->fuzz))
                      break;
                  }
                else
                  if (ColorMatch(*q,target,image->fuzz) ||
                      ColorMatch(*q,draw_info->fill,image->fuzz))
                    break;
                indexes[i++]=True;
                *q=draw_info->fill;
                q++;
              }
              if (!SyncImagePixels(image))
                break;
            }
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      x++;
      if (x <= x2)
        {
          q=GetImagePixels(image,x,y,x2-x+1,1);
          if (q == (PixelPacket *) NULL)
            break;
          for ( ; x <= x2; x++)
          {
            if (method == FloodfillMethod)
              {
                if (ColorMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (!ColorMatch(*q,target,image->fuzz) &&
                  !ColorMatch(*q,draw_info->fill,image->fuzz))
                break;
            q++;
          }
        }
      start=x;
    } while (x <= x2);
  }
  if (draw_info->tile == (Image *) NULL)
    for (y=0; y < (int) image->rows; y++)
    {
      /*
        Tile fill color onto floodplane.
      */
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (int) image->columns; x++)
      {
        if (indexes[x])
          *q=draw_info->fill;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
  else
    {
      double
        alpha;

      /*
        Tile image onto floodplane.
      */
      alpha=1.0/MaxRGB;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (int) image->columns; x++)
        {
          if (indexes[x])
            {
              color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
                y % draw_info->tile->rows);
              if (!draw_info->tile->matte)
                *q=color;
              else
                {
                  q->red=(Quantum) (alpha*(color.red*(MaxRGB-color.opacity)+
                    q->red*color.opacity));
                  q->green=(Quantum) (alpha*(color.green*(MaxRGB-color.opacity)+
                    q->green*color.opacity));
                  q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-color.opacity)+
                    q->blue*color.opacity));
                  q->opacity=(Quantum) (alpha*(color.opacity*
                    (MaxRGB-color.opacity)+q->opacity*color.opacity));
                }
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  image->storage_class=DirectClass;
  LiberateMemory((void **) &segment_stack);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D r a w I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyDrawInfo deallocates memory associated with an
%  DrawInfo structure.
%
%  The format of the DestroyDrawInfo method is:
%
%      void DestroyDrawInfo(DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o draw_info: Specifies a pointer to an DrawInfo structure.
%
%
*/
MagickExport void DestroyDrawInfo(DrawInfo *draw_info)
{
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  if (draw_info->primitive != (char *) NULL)
    LiberateMemory((void **) &draw_info->primitive);
  if (draw_info->font != (char *) NULL)
    LiberateMemory((void **) &draw_info->font);
  if (draw_info->tile != (Image *) NULL)
    DestroyImage(draw_info->tile);
  LiberateMemory((void **) &draw_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawImage draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawImage method is:
%
%      unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%
*/
MagickExport unsigned int DrawImage(Image *image,const DrawInfo *draw_info)
{
  char
    keyword[MaxTextExtent],
    *marker,
    *p,
    *primitive;

  double
    alpha,
    beta,
    mid;

  DrawInfo
    *clone_info;

  int
    j,
    n,
    number_points,
    y;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register int
    i,
    x;

  SegmentInfo
    bounds;

  unsigned int
    indirection,
    length;

  /*
    Ensure the annotation info is valid.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->signature == MagickSignature);
  assert(draw_info->primitive != (char *) NULL);
  if (*draw_info->primitive == '\0')
    return(False);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  primitive=clone_info->primitive;
  indirection=(*primitive == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      register char
        *q;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(primitive+1,"r");
      if (file == (FILE *) NULL)
        {
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(FileOpenWarning,"Unable to read primitive file",
            primitive+1);
        }
      length=MaxTextExtent;
      primitive=(char *) AcquireMemory(length);
      q=primitive;
      while (primitive != (char *) NULL)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-primitive+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            ReacquireMemory((void **) &primitive,length);
            if (primitive == (char *) NULL)
              break;
            q=primitive+Extent(primitive);
          }
        *q++=c;
      }
      (void) fclose(file);
      if (primitive == (char *) NULL)
        {
          DestroyDrawInfo(clone_info);
          ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
            "Memory allocation failed");
        }
      *q='\0';
    }
  /*
    Allocate primitive info memory.
  */
  number_points=2048;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      if (indirection)
        LiberateMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
  for (p=primitive; *p != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    marker=p;
    for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
      keyword[x]=(*p++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    if (LocaleCompare("affine",keyword) == 0)
      {
        for (n=0; n < 6; n++)
        {
          clone_info->affine[n]=strtod(p,&p);
          if (*p == ',')
            break;
        }
        continue;
      }
    if (LocaleCompare("angle",keyword) == 0)
      {
        clone_info->angle=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("antialias",keyword) == 0)
      {
        clone_info->antialias=(unsigned int) strtod(p,&p);
        continue;
      }
    if (LocaleCompare("decorate",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("none",keyword) == 0)
          clone_info->decorate=NoDecoration;
        if (LocaleCompare("underline",keyword) == 0)
          clone_info->decorate=UnderlineDecoration;
        if (LocaleCompare("overline",keyword) == 0)
          clone_info->decorate=OverlineDecoration;
        if (LocaleCompare("line-through",keyword) == 0)
          clone_info->decorate=LineThroughDecoration;
        continue;
      }
    if (LocaleCompare("fill",keyword) == 0)
      {
        if ((LocaleNCompare(p,"rgb(",4) == 0))
          for (x=0; (*(p-1) != ')') && (*p != '\0'); x++)
            keyword[x]=(*p++);
        else
          for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
            keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->fill);
        continue;
      }
    if (LocaleCompare("font",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        CloneString(&clone_info->font,keyword);
        continue;
      }
    if (LocaleCompare("gravity",keyword) == 0)
      {
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (LocaleCompare("NorthWest",keyword) == 0)
          clone_info->gravity=NorthWestGravity;
        if (LocaleCompare("North",keyword) == 0)
          clone_info->gravity=NorthGravity;
        if (LocaleCompare("NorthEast",keyword) == 0)
          clone_info->gravity=NorthEastGravity;
        if (LocaleCompare("West",keyword) == 0)
          clone_info->gravity=WestGravity;
        if (LocaleCompare("Center",keyword) == 0)
          clone_info->gravity=CenterGravity;
        if (LocaleCompare("East",keyword) == 0)
          clone_info->gravity=EastGravity;
        if (LocaleCompare("SouthWest",keyword) == 0)
          clone_info->gravity=SouthWestGravity;
        if (LocaleCompare("South",keyword) == 0)
          clone_info->gravity=SouthGravity;
        if (LocaleCompare("SouthEast",keyword) == 0)
          clone_info->gravity=SouthEastGravity;
        continue;
      }
    if (LocaleCompare("linewidth",keyword) == 0)
      {
        clone_info->linewidth=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("opacity",keyword) == 0)
      {
        clone_info->opacity=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("pointsize",keyword) == 0)
      {
        clone_info->pointsize=strtod(p,&p);
        continue;
      }
    if (LocaleCompare("stroke",keyword) == 0)
      {
        if ((LocaleNCompare(p,"rgb(",4) == 0))
          for (x=0; (*(p-1) != ')') && (*p != '\0'); x++)
            keyword[x]=(*p++);
        else
          for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
            keyword[x]=(*p++);
        keyword[x]='\0';
        (void) QueryColorDatabase(keyword,&clone_info->stroke);
        continue;
      }
    primitive_type=UndefinedPrimitive;
    if (LocaleNCompare("fill",keyword,4) == 0)
      (void) strcpy(keyword,keyword+4);
    if (LocaleCompare("Point",keyword) == 0)
      primitive_type=PointPrimitive;
    if (LocaleCompare("Line",keyword) == 0)
      primitive_type=LinePrimitive;
    if (LocaleNCompare("Rect",keyword,4) == 0)
      primitive_type=RectanglePrimitive;
    if (LocaleCompare("RoundRectangle",keyword) == 0)
      primitive_type=RoundRectanglePrimitive;
    if (LocaleCompare("Arc",keyword) == 0)
      primitive_type=ArcPrimitive;
    if (LocaleCompare("Ellipse",keyword) == 0)
      primitive_type=EllipsePrimitive;
    if (LocaleCompare("Circle",keyword) == 0)
      primitive_type=CirclePrimitive;
    if (LocaleCompare("Polyline",keyword) == 0)
      primitive_type=PolylinePrimitive;
    if (LocaleCompare("Polygon",keyword) == 0)
      primitive_type=PolygonPrimitive;
    if (LocaleCompare("Bezier",keyword) == 0)
      primitive_type=BezierPrimitive;
    if (LocaleCompare("Path",keyword) == 0)
      primitive_type=PathPrimitive;
    if (LocaleCompare("Color",keyword) == 0)
      primitive_type=ColorPrimitive;
    if (LocaleCompare("Matte",keyword) == 0)
      primitive_type=MattePrimitive;
    if (LocaleCompare("Text",keyword) == 0)
      primitive_type=TextPrimitive;
    if (LocaleCompare("Image",keyword) == 0)
      primitive_type=ImagePrimitive;
    if (primitive_type == UndefinedPrimitive)
      break;
    /*
      Parse the primitive attributes.
    */
    bounds.x1=image->columns-1.0;
    bounds.y1=image->rows-1.0;
    bounds.x2=0.0;
    bounds.y2=0.0;
    i=0;
    j=0;
    for (x=0; *p != '\0'; x++)
    {
      /*
        Define points.
      */
      while (isspace((int) (*p)) && (*p != '\0'))
        p++;
      if (!IsGeometry(p))
        break;
      point.x=strtod(p,&p);
      if (*p == ',')
        p++;
      point.y=strtod(p,&p);
      if (*p == ',')
        p++;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      while ((isspace((int) (*p)) || (*p == ',')) && (*p != '\0'))
        p++;
      i++;
      if (i < (int) (number_points-6*BezierQuantum-360))
        continue;
      number_points+=6*BezierQuantum+360;
      ReacquireMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info != (PrimitiveInfo *) NULL)
        continue;
      if (indirection)
        LiberateMemory((void **) &primitive);
      DestroyDrawInfo(clone_info);
      ThrowBinaryException(ResourceLimitWarning,"Unable to draw image",
        "Memory allocation failed");
    }
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GeneratePoint(primitive_info+j,primitive_info[j].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateLine(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateRoundRectangle(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateArc(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point,0,True,False);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateEllipse(primitive_info+j,primitive_info[j].point,
          primitive_info[j+1].point,primitive_info[j+2].point);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case CirclePrimitive:
      {
        double
          radius;

        PointInfo
          arc,
          end,
          start;

        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        alpha=primitive_info[j+1].point.x-primitive_info[j].point.x;
        beta=primitive_info[j+1].point.y-primitive_info[j].point.y;
        radius=sqrt(alpha*alpha+beta*beta);
        start.x=primitive_info[j].point.x-radius;
        start.y=primitive_info[j].point.y;
        end.x=start.x;
        end.y=start.y-1.0;
        arc.x=radius;
        arc.y=radius;
        GenerateArc(primitive_info+j,start,end,arc,0,True,False);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        GenerateBezier(primitive_info+j);
        i=j+primitive_info[j].coordinates;
        break;
      }
      case PathPrimitive:
      {
        char
          *path;

        int
          number_attributes;

        number_attributes=0;
        path=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
            {
              if (isalpha((int) *p))
                number_attributes++;
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
            }
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
              {
                if (isalpha((int) *p))
                  number_attributes++;
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
              }
            }
          else
            for (p++;  *p != '\0'; p++)
            {
              if (isalpha((int) *p))
                number_attributes++;
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
            }
        if (*p != '\0')
          p++;
        *p++='\0';
        if (i > (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
            if (primitive_info == (PrimitiveInfo *) NULL)
              {
                if (indirection)
                  LiberateMemory((void **) &primitive);
                DestroyDrawInfo(clone_info);
                ThrowBinaryException(ResourceLimitWarning,
                  "Unable to draw image","Memory allocation failed");
              }
          }
        i=j+GeneratePath(primitive_info+j,path);
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        /*
          Define method.
        */
        for (x=0; !isspace((int) (*p)) && (*p != '\0'); x++)
          keyword[x]=(*p++);
        keyword[x]='\0';
        if (*keyword == '\0')
          break;
        if (LocaleCompare("point",keyword) == 0)
          primitive_info[j].method=PointMethod;
        else
          if (LocaleCompare("replace",keyword) == 0)
            primitive_info[j].method=ReplaceMethod;
          else
            if (LocaleCompare("floodfill",keyword) == 0)
              primitive_info[j].method=FloodfillMethod;
            else
              if (LocaleCompare("filltoborder",keyword) == 0)
                primitive_info[j].method=FillToBorderMethod;
              else
                if (LocaleCompare("reset",keyword) == 0)
                  primitive_info[j].method=ResetMethod;
                else
                  primitive_type=UndefinedPrimitive;
        if ((primitive_info[j].method == ReplaceMethod) ||
            (primitive_info[j].method == ResetMethod))
          {
            bounds.x1=0.0;
            bounds.y1=0.0;
            bounds.x2=image->columns-1.0;
            bounds.y2=image->rows-1.0;
          }
        while (isspace((int) (*p)) && (*p != '\0'))
          p++;
        break;
      }
      case TextPrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        if (*p == '\0')
          break;
        primitive_info[j].text=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
            }
          else
            for (p++;  *p != '\0'; p++)
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
        if (*p != '\0')
          p++;
        break;
      }
      case ImagePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            primitive_type=UndefinedPrimitive;
            break;
          }
        if (*p == '\0')
          break;
        primitive_info[j].text=p;
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              for (p++; *p != '\0'; p++)
                if ((*p == '\'') && (*(p-1) != '\\'))
                  break;
            }
          else
            for (p++;  *p != '\0'; p++)
              if (isspace((int) *p) && (*(p-1) != '\\') && (*p != '\0'))
                break;
        if (*p != '\0')
          p++;
        break;
      }
    }
    while (isspace((int) (*p)) && (*p != '\0'))
      p++;
    if (clone_info->verbose)
      {
        char
          *element;

        element=AllocateString(marker);
        element[p-marker]='\0';
        (void) fprintf(stdout,"%s\n",element);
        LiberateMemory((void **) &element);
      }
    if (primitive_type == UndefinedPrimitive)
      break;
    primitive_info[i].primitive=UndefinedPrimitive;
    /*
      Transform points.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      primitive_info[i].point.x=clone_info->affine[0]*point.x+
        clone_info->affine[2]*point.y+clone_info->affine[4];
      primitive_info[i].point.y=clone_info->affine[1]*point.x+
        clone_info->affine[3]*point.y+clone_info->affine[5];
    }
    /*
      Compute bounding box.
    */
    for (i=0; primitive_info[i].primitive != UndefinedPrimitive; i++)
    {
      point=primitive_info[i].point;
      if (point.x < bounds.x1)
        bounds.x1=point.x;
      if (point.y < bounds.y1)
        bounds.y1=point.y;
      if (point.x > bounds.x2)
        bounds.x2=point.x;
      if (point.y > bounds.y2)
        bounds.y2=point.y;
    }
    mid=clone_info->affine[0]*clone_info->linewidth/2.0+1.0;
    bounds.x1-=mid;
    if (bounds.x1 < 0.0)
      bounds.x1=0.0;
    bounds.y1-=mid;
    if (bounds.y1 < 0.0)
      bounds.y1=0.0;
    bounds.x2+=mid;
    if (bounds.x2 >= image->columns)
      bounds.x2=image->columns-1.0;
    bounds.y2+=mid;
    if (bounds.y2 >= image->rows)
      bounds.y2=image->rows-1.0;
    DrawPrimitive(image,clone_info,&bounds,primitive_info);
  }
  /*
    Free resources.
  */
  LiberateMemory((void **) &primitive_info);
  if (indirection)
    LiberateMemory((void **) &primitive);
  DestroyDrawInfo(clone_info);
  if (primitive_type == UndefinedPrimitive)
    ThrowBinaryException(OptionWarning,
      "Non-conforming drawing primitive definition",keyword);
  image->storage_class=DirectClass;
  (void) IsMatteImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D r a w P r i m i t i v e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DrawPrimitive draws a primitive (line, rectangle, ellipse) on the
%  image.
%
%  The format of the DrawPrimitive method is:
%
%      void DrawPrimitive(Image *image,const DrawInfo *draw_info,
%        SegmentInfo *bounds,PrimitiveInfo *primitive_info))
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o draw_info: The address of a DrawInfo structure.
%
%    o bounds: Specifies a pointer to a SegmentInfo structure which defines
%      the bounding box of the graphic primitive relative to the image.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%
*/
static void DrawPrimitive(Image *image,const DrawInfo *draw_info,
  SegmentInfo *bounds,PrimitiveInfo *primitive_info)
{
  int
    n,
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  x=(int) ceil(primitive_info->point.x-0.5);
  y=(int) ceil(primitive_info->point.y-0.5);
  switch (primitive_info->primitive)
  {
    case PointPrimitive:
    {
      q=GetImagePixels(image,x,y,1,1);
      if (q == (PixelPacket *) NULL)
        break;
      *q=draw_info->stroke;
      (void) SyncImagePixels(image);
    }
    case ColorPrimitive:
    {
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          *q=draw_info->stroke;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          target=GetOnePixel(image,x,y);
          (void) OpaqueImage(image,target,draw_info->fill);
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          target=GetOnePixel(image,x,y);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) ColorFloodfillImage(image,draw_info,target,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (int) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              *q=draw_info->stroke;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case MattePrimitive:
    {
      register PixelPacket
        *q;

      if (!image->matte)
        MatteImage(image,OpaqueOpacity);
      switch (primitive_info->method)
      {
        case PointMethod:
        default:
        {
          q=GetImagePixels(image,x,y,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          q->opacity=TransparentOpacity;
          (void) SyncImagePixels(image);
          break;
        }
        case ReplaceMethod:
        {
          PixelPacket
            target;

          target=GetOnePixel(image,x,y);
          (void) TransparentImage(image,target);
          break;
        }
        case FloodfillMethod:
        case FillToBorderMethod:
        {
          PixelPacket
            border_color,
            target;

          target=GetOnePixel(image,x,y);
          if (primitive_info->method == FillToBorderMethod)
            {
              border_color=draw_info->border_color;
              target=border_color;
            }
          (void) MatteFloodfillImage(image,target,TransparentOpacity,x,y,
            primitive_info->method);
          break;
        }
        case ResetMethod:
        {
          for (y=0; y < (int) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              q->opacity=draw_info->stroke.opacity;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          break;
        }
      }
      break;
    }
    case TextPrimitive:
    {
      AnnotateInfo
        *annotate;

      ImageInfo
        *clone_info;

      register char
        *p,
        *q;

      if (primitive_info->text == (char *) NULL)
        break;
      p=primitive_info->text;
      q=primitive_info->text;
      if (*q == '"')
        {
          p++;
          for (q++; *q != '\0'; q++)
            if ((*q == '"') && (*(q-1) != '\\'))
              break;
        }
      else
        if (*q == '\'')
          {
            p++;
            for (q++; *q != '\0'; q++)
              if ((*q == '\'') && (*(q-1) != '\\'))
                break;
          }
        else
          for (q++;  *q != '\0'; q++)
            if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
              break;
      clone_info=CloneImageInfo((ImageInfo *) NULL);
      clone_info->font=AllocateString(draw_info->font);
      clone_info->antialias=draw_info->antialias;
      clone_info->pointsize=draw_info->pointsize;
      for (i=0; i < 6; i++)
        clone_info->affine[i]=draw_info->affine[i];
      annotate=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
      DestroyImageInfo(clone_info);
      annotate->degrees=draw_info->angle;
      annotate->gravity=draw_info->gravity;
      annotate->decorate=draw_info->decorate;
      annotate->geometry=AllocateString("");
      annotate->fill=draw_info->fill;
      annotate->stroke=draw_info->stroke;
      annotate->box=draw_info->box;
      annotate->text=(char *) AcquireMemory(q-p+1);
      if (annotate->text == (char *) NULL)
        MagickError(ResourceLimitError,"Unable to annotate image",
          "Memory allocation failed");
      (void) strncpy(annotate->text,p,q-p);
      annotate->text[q-p]='\0';
      FormatString(annotate->geometry,"%+d%+d",x,y);
      AnnotateImage(image,annotate);
      DestroyAnnotateInfo(annotate);
      break;
    }
    case ImagePrimitive:
    {
      Image
        *composite_image;

      ImageInfo
        *composite_info;

      composite_info=CloneImageInfo((ImageInfo *) NULL);
      (void) strcpy(composite_info->filename,primitive_info->text);
      composite_image=ReadImage(composite_info,&image->exception);
      if (composite_image == (Image *) NULL)
        break;
      if ((primitive_info[1].point.x != 0) && (primitive_info[1].point.y != 0))
        {
          char
            geometry[MaxTextExtent];

          /*
            Resize image.
          */
          FormatString(geometry,"%gx%g",primitive_info[1].point.x,
            primitive_info[1].point.y);
          TransformImage(&composite_image,(char *) NULL,geometry);
        }
      if ((draw_info->affine[1] == 0.0) && (draw_info->affine[2] == 0.0))
        {
          if ((draw_info->affine[0] != 1.0) || (draw_info->affine[0] != 1.0))
            {
              Image
                *scale_image;

              unsigned int
                height,
                width;

              width=(unsigned int)
                (draw_info->affine[0]*composite_image->columns);
              height=(unsigned int)
                (draw_info->affine[3]*composite_image->rows);
              scale_image=ZoomImage(composite_image,width,height,
                &image->exception);
              if (scale_image != (Image *) NULL)
                {
                  DestroyImage(composite_image);
                  composite_image=scale_image;
                }
            }
        }
      else
        {
          if (((draw_info->affine[0]-draw_info->affine[3]) == 0.0) &&
              ((draw_info->affine[1]+draw_info->affine[2]) == 0.0))
            {
              double
                theta;

              Image
                *rotate_image;

              theta=(180.0/M_PI)*
                atan2(draw_info->affine[1],draw_info->affine[0]);
              rotate_image=
                RotateImage(composite_image,theta,&image->exception);
              if (rotate_image != (Image *) NULL)
                {
                  DestroyImage(composite_image);
                  composite_image=rotate_image;
                }
            }
        }
      CompositeImage(image,image->matte ? OverCompositeOp : ReplaceCompositeOp,
        composite_image,x,y);
      DestroyImage(composite_image);
      DestroyImageInfo(composite_info);
      break;
    }
    default:
    {
      double
        alpha,
        fill_opacity,
        stroke_opacity;

      int
        n;

      PixelPacket
        color;

      PointInfo
        target;

      alpha=1.0/MaxRGB;
      for (y=(int) ceil(bounds->y1-0.5); y <= (int) floor(bounds->y2-0.5); y++)
      {
        /*
          Fill the primitive on the image.
        */
        x=(int) ceil(bounds->x1-0.5);
        n=(int) floor(bounds->x2-0.5)-x;
        q=GetImagePixels(image,x,y,n+1,1);
        if (q == (PixelPacket *) NULL)
          break;
        target.y=y;
        for ( ; x <= (int) floor(bounds->x2-0.5); x++)
        {
          target.x=x;
          stroke_opacity=IntersectPrimitive(primitive_info,draw_info,image,
            &target,&fill_opacity);
          color=draw_info->fill;
          if (draw_info->tile != (Image *) NULL)
            color=GetOnePixel(draw_info->tile,x % draw_info->tile->columns,
              y % draw_info->tile->rows);
          if ((fill_opacity != 0.0) && (color.opacity != TransparentOpacity))
            {
              /*
                Fill.
              */
              fill_opacity=MaxRGB-
                0.01*(MaxRGB-color.opacity)*draw_info->opacity*fill_opacity;
              if (!draw_info->antialias)
                fill_opacity=(Quantum) (OpaqueOpacity*draw_info->opacity/100.0);
              q->red=(Quantum) (alpha*(color.red*(MaxRGB-fill_opacity)+
                q->red*fill_opacity));
              q->green=(Quantum) (alpha*(color.green*(MaxRGB-fill_opacity)+
                q->green*fill_opacity));
              q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-fill_opacity)+
                q->blue*fill_opacity));
              q->opacity=(Quantum) (alpha*(fill_opacity*(MaxRGB-fill_opacity)+
                q->opacity*fill_opacity));
            }
          color=draw_info->stroke;
          if ((stroke_opacity != 0.0) && (color.opacity != TransparentOpacity))
            {
              /*
                Stroke.
              */
              stroke_opacity=MaxRGB-
                0.01*(MaxRGB-color.opacity)*draw_info->opacity*stroke_opacity;
              if (!draw_info->antialias)
                stroke_opacity=(Quantum)
                  (OpaqueOpacity*draw_info->opacity/100.0);
              q->red=(Quantum) (alpha*(color.red*(MaxRGB-stroke_opacity)+
                q->red*stroke_opacity));
              q->green=(Quantum) (alpha*(color.green*(MaxRGB-stroke_opacity)+
                q->green*stroke_opacity));
              q->blue=(Quantum) (alpha*(color.blue*(MaxRGB-stroke_opacity)+
                q->blue*stroke_opacity));
              q->opacity=(Quantum) (alpha*(stroke_opacity*
                (MaxRGB-stroke_opacity)+q->opacity*stroke_opacity));
            }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e n e r a t e P r i m i t i v e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GeneratePrimitive is a collection of methods for generating graphic
%  primitives such as arcs, ellipses, paths, etc.
%
%
*/

static void GenerateArc(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo arc,const double angle,
  const unsigned int large_arc,const unsigned int sweep)
{
  double
    alpha,
    arc_angle,
    beta,
    cosine,
    factor,
    gamma,
    sine;

  int
    number_segments;

  PointInfo
    center,
    points[3];

  register int
    i;

  register PrimitiveInfo
    *p;

  cosine=cos(DegreesToRadians(angle));
  sine=sin(DegreesToRadians(angle));
  points[0].x=cosine*start.x/arc.x+sine*start.y/arc.x;
  points[0].y=cosine*start.y/arc.y-sine*start.x/arc.y;
  points[1].x=cosine*end.x/arc.x+sine*end.y/arc.x;
  points[1].y=cosine*end.y/arc.y-sine*end.x/arc.y;
  alpha=points[1].x-points[0].x;
  beta=points[1].y-points[0].y;
  factor=1.0/(alpha*alpha+beta*beta)-0.25;
  if (factor <= 0.0)
    factor=0.0;
  else
    {
      factor=sqrt(factor);
      if (sweep == large_arc)
        factor=(-factor);
    }
  center.x=0.5*(points[0].x+points[1].x)-factor*beta;
  center.y=0.5*(points[0].y+points[1].y)+factor*alpha;
  alpha=atan2(points[0].y-center.y,points[0].x-center.x);
  arc_angle=atan2(points[1].y-center.y,points[1].x-center.x)-alpha;
  if ((arc_angle < 0.0) && sweep)
    arc_angle+=2.0*M_PI;
  else
    if ((arc_angle > 0.0) && !sweep)
      arc_angle-=2.0*M_PI;
  number_segments=ceil(fabs(arc_angle/(0.5*M_PI+MagickEpsilon)));
  p=primitive_info;
  for (i=0; i < number_segments; i++)
  {
    beta=0.5*((alpha+(i+1)*arc_angle/number_segments)-
      (alpha+i*arc_angle/number_segments));
    gamma=(8.0/3.0)*sin(0.5*beta)*sin(0.5*beta)/sin(beta);
    points[0].x=center.x+cos((alpha+i*arc_angle/number_segments))-
      gamma*sin((alpha+i*arc_angle/number_segments));
    points[0].y=center.y+sin((alpha+i*arc_angle/number_segments))+
      gamma*cos((alpha+i*arc_angle/number_segments));
    points[2].x=center.x+cos((alpha+(i+1)*arc_angle/number_segments));
    points[2].y=center.y+sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].x=points[2].x+gamma*sin((alpha+(i+1)*arc_angle/number_segments));
    points[1].y=points[2].y-gamma*cos((alpha+(i+1)*arc_angle/number_segments));
    p->point.x=(p == primitive_info) ? start.x : (p-1)->point.x;
    p->point.y=(p == primitive_info) ? start.y : (p-1)->point.y;
    (p+1)->point.x=cosine*arc.x*points[0].x-sine*arc.y*points[0].y;
    (p+1)->point.y=sine*arc.x*points[0].x+cosine*arc.y*points[0].y;
    (p+2)->point.x=cosine*arc.x*points[1].x-sine*arc.y*points[1].y;
    (p+2)->point.y=sine*arc.x*points[1].x+cosine*arc.y*points[1].y;
    (p+3)->point.x=cosine*arc.x*points[2].x-sine*arc.y*points[2].y;
    (p+3)->point.y=sine*arc.x*points[2].x+cosine*arc.y*points[2].y;
    if (i == (number_segments-1))
      (p+3)->point=end;
    p->coordinates=4;
    GenerateBezier(p);
    p+=p->coordinates;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateBezier(PrimitiveInfo *primitive_info)
{
  double
    alpha,
    *coefficients,
    weight;

  PointInfo
    end,
    point,
    *points;

  register int
    i,
    j;

  register PrimitiveInfo
    *p;

  unsigned int
    control_points,
    quantum;

  /*
    Allocate coeficients.
  */
  quantum=primitive_info->coordinates;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    for (j=i+1; j < primitive_info->coordinates; j++)
    {
      alpha=fabs(primitive_info[j].point.x-primitive_info[i].point.x);
      if (alpha > quantum)
        quantum=alpha;
      alpha=fabs(primitive_info[j].point.y-primitive_info[i].point.y);
      if (alpha > quantum)
        quantum=alpha;
    }
  }
  quantum=Min(quantum/primitive_info->coordinates,BezierQuantum);
  control_points=quantum*primitive_info->coordinates;
  coefficients=(double *)
    AcquireMemory(primitive_info->coordinates*sizeof(double));
  points=(PointInfo *) AcquireMemory(control_points*sizeof(PointInfo));
  if ((coefficients == (double *) NULL) || (points == (PointInfo *) NULL))
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  /*
    Compute bezier points.
  */
  end=primitive_info[primitive_info->coordinates-1].point;
  weight=0.0;
  for (i=0; i < primitive_info->coordinates; i++)
    coefficients[i]=Permutate(primitive_info->coordinates-1,i);
  for (i=0; i < (int) control_points; i++)
  {
    p=primitive_info;
    point.x=0;
    point.y=0;
    alpha=pow(1.0-weight,primitive_info->coordinates-1);
    for (j=0; j < primitive_info->coordinates; j++)
    {
      point.x+=alpha*coefficients[j]*p->point.x;
      point.y+=alpha*coefficients[j]*p->point.y;
      alpha*=weight/(1.0-weight);
      p++;
    }
    points[i]=point;
    weight+=1.0/quantum/primitive_info->coordinates;
  }
  /*
    Bezier curves are just short segmented polys.
  */
  p=primitive_info;
  for (i=0; i < (int) control_points; i++)
  {
    point=points[i];
    p->point=point;
    p++;
  }
  p->point=end;
  p++;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
  LiberateMemory((void **) &points);
  LiberateMemory((void **) &coefficients);
}

static void GenerateEllipse(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end,const PointInfo degrees)
{
  double
    angle;

  PointInfo
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  /*
    Ellipses are just short segmented polys.
  */
  p=primitive_info;
  for (angle=(degrees.x+1.0); angle <= degrees.y; angle+=1.0)
  {
    point.x=cos(DegreesToRadians(fmod(angle,360.0)))*end.x+start.x;
    point.y=sin(DegreesToRadians(fmod(angle,360.0)))*end.y+start.y;
    p->point=point;
    p++;
  }
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateLine(PrimitiveInfo *primitive_info,const PointInfo start,
  const PointInfo end)
{
  register PrimitiveInfo
    *p,
    *q;

  p=primitive_info;
  p->point=start;
  q=p+1;
  q->primitive=primitive_info->primitive;
  q->point=end;
  primitive_info->coordinates=2;
}

static unsigned int GeneratePath(PrimitiveInfo *primitive_info,const char *path)
{
  char
    *p;

  double
    x,
    y;

  int
    attribute;

  PointInfo
    end,
    points[4],
    point,
    start;

  PrimitiveType
    primitive_type;

  register int
    i;

  register PrimitiveInfo
    *q;

  unsigned int
    number_coordinates,
    subpath,
    z_count;

  point.x=0;
  point.y=0;
  number_coordinates=0;
  subpath=False;
  z_count=0;
  primitive_type=primitive_info->primitive;
  q=primitive_info;
  p=(char *) path;
  while (*p != '\0')
  {
    while (isspace((int) *p))
      p++;
    attribute=(*p++);
    switch (attribute)
    {
      case 'a':
      case 'A':
      {
        double
          angle;

        PointInfo
          arc;

        unsigned int
          large_arc,
          sweep;

        /*
          Compute arc points.
        */
        subpath=True;
        arc.x=strtod(p,&p);
        if (*p == ',')
          p++;
        arc.y=strtod(p,&p);
        if (*p == ',')
          p++;
        angle=strtod(p,&p);
        if (*p == ',')
          p++;
        large_arc=strtod(p,&p);
        if (*p == ',')
          p++;
        sweep=strtod(p,&p);
        if (*p == ',')
          p++;
        x=strtod(p,&p);
        if (*p == ',')
          p++;
        y=strtod(p,&p);
        end.x=attribute == 'A' ? x : point.x+x;
        end.y=attribute == 'A' ? y : point.y+y;
        GenerateArc(q,point,end,arc,angle,large_arc,sweep);
        q+=q->coordinates;
        point=end;
        break;
      }
      case 'c':
      case 'C':
      {
        /*
          Compute bezier points.
        */
        subpath=True;
        do
        {
          points[0]=point;
          for (i=1; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            end.x=attribute == 'C' ? x : point.x+x;
            end.y=attribute == 'C' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i <= 3; i++)
            (q+i)->point=points[i];
          q->coordinates=4;
          GenerateBezier(q);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 'H':
      case 'h':
      {
        x=strtod(p,&p);
        point.x=attribute == 'H' ? x: point.x+x;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      case 'l':
      case 'L':
      {
        do
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          if (*p == ',')
            p++;
          point.x=attribute == 'L' ? x : point.x+x;
          point.y=attribute == 'L' ? y : point.y+y;
          GeneratePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        break;
      }
      case 'M':
      case 'm':
      {
        if (subpath)
          {
            primitive_info->coordinates=q-primitive_info;
            number_coordinates+=primitive_info->coordinates;
            primitive_info=q;
          }
        do
        {
          x=strtod(p,&p);
          if (*p == ',')
            p++;
          y=strtod(p,&p);
          point.x=attribute == 'M' ? x : point.x+x;
          point.y=attribute == 'M' ? y : point.y+y;
          GeneratePoint(q,point);
          q+=q->coordinates;
        } while (IsGeometry(p));
        start=point;
        break;
      }
      case 'q':
      case 'Q':
      {
        /*
          Compute bezier points.
        */
        subpath=True;
        do
        {
          points[0]=point;
          for (i=1; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            end.x=attribute == 'Q' ? x : point.x+x;
            end.y=attribute == 'Q' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          q->coordinates=3;
          GenerateBezier(q);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 's':
      case 'S':
      {
        /*
          Compute bezier points.
        */
        subpath=True;
        do
        {
          points[0]=points[3];
          points[1].x=2.0*points[3].x-points[2].x;
          points[1].y=2.0*points[3].y-points[2].y;
          for (i=2; i <= 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            end.x=attribute == 'S' ? x : point.x+x;
            end.y=attribute == 'S' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i <= 4; i++)
            (q+i)->point=points[i];
          q->coordinates=4;
          GenerateBezier(q);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 't':
      case 'T':
      {
        /*
          Compute bezier points.
        */
        subpath=True;
        do
        {
          points[0]=points[2];
          points[1].x=2.0*points[2].x-points[1].x;
          points[1].y=2.0*points[2].y-points[1].y;
          for (i=2; i < 3; i++)
          {
            x=strtod(p,&p);
            if (*p == ',')
              p++;
            y=strtod(p,&p);
            if (*p == ',')
              p++;
            end.x=attribute == 'T' ? x : point.x+x;
            end.y=attribute == 'T' ? y : point.y+y;
            points[i]=end;
          }
          for (i=0; i < 3; i++)
            (q+i)->point=points[i];
          q->coordinates=3;
          GenerateBezier(q);
          q+=q->coordinates;
          point=end;
        } while (IsGeometry(p));
        break;
      }
      case 'v':
      case 'V':
      {
        y=strtod(p,&p);
        point.y=attribute == 'V' ? y : point.y+y;
        GeneratePoint(q,point);
        q+=q->coordinates;
        break;
      }
      case 'z':
      case 'Z':
      {
        point=start;
        GeneratePoint(q,point);
        q+=q->coordinates;
        primitive_info->coordinates=q-primitive_info;
        number_coordinates+=primitive_info->coordinates;
        primitive_info=q;
        z_count++;
        break;
      }
      default:
      {
        if (isalpha((int) attribute))
          (void) fprintf(stderr,"attribute not recognized: %c\n",attribute);
        break;
      }
    }
  }
  if (z_count == 0)
    {
      GeneratePoint(q,point);
      q+=q->coordinates;
    }
  primitive_info->coordinates=q-primitive_info;
  number_coordinates+=primitive_info->coordinates;
  for (i=0; i < number_coordinates; i++)
  {
    q--;
    q->primitive=primitive_type;
    if (z_count > 1)
      q->method=FillToBorderMethod;
  }
  return(number_coordinates);
}

static void GeneratePoint(PrimitiveInfo *primitive_info,const PointInfo start)
{
  primitive_info->coordinates=1;
  primitive_info->point=start;
}

static void GenerateRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end)
{
  PointInfo
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  GeneratePoint(p,start);
  p+=p->coordinates;
  point.x=start.x;
  point.y=end.y;
  GeneratePoint(p,point);
  p+=p->coordinates;
  GeneratePoint(p,end);
  p+=p->coordinates;
  point.x=end.x;
  point.y=start.y;
  GeneratePoint(p,point);
  p+=p->coordinates;
  GeneratePoint(p,start);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

static void GenerateRoundRectangle(PrimitiveInfo *primitive_info,
  const PointInfo start,const PointInfo end,PointInfo arc)
{
  PointInfo
    degrees,
    offset,
    point;

  register int
    i;

  register PrimitiveInfo
    *p;

  p=primitive_info;
  offset.x=end.x-start.x;
  offset.y=end.y-start.y;
  if (arc.x > (0.5*offset.x))
    arc.x=0.5*offset.x;
  if (arc.y > (0.5*offset.y))
    arc.y=0.5*offset.y;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+arc.y;
  degrees.x=270.0;
  degrees.y=360.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+offset.x-arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=0.0;
  degrees.y=90.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+offset.y-arc.y;
  degrees.x=90.0;
  degrees.y=180.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  point.x=start.x+arc.x;
  point.y=start.y+arc.y;
  degrees.x=180.0;
  degrees.y=270.0;
  GenerateEllipse(p,point,arc,degrees);
  p+=p->coordinates;
  GeneratePoint(p,primitive_info->point);
  p+=p->coordinates;
  primitive_info->coordinates=p-primitive_info;
  for (i=0; i < primitive_info->coordinates; i++)
  {
    p->primitive=primitive_info->primitive;
    p--;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t D r a w I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDrawInfo initializes the DrawInfo structure.
%
%  The format of the GetDrawInfo method is:
%
%      void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o draw_info: Specifies a pointer to a DrawInfo structure.
%
%
*/
MagickExport void GetDrawInfo(const ImageInfo *image_info,DrawInfo *draw_info)
{
  register int
    i;

  /*
    Initialize draw attributes.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(draw_info != (DrawInfo *) NULL);
  draw_info->primitive=(char *) NULL;
  draw_info->font=AllocateString(image_info->font);
  draw_info->antialias=image_info->antialias;
  draw_info->gravity=NorthWestGravity;
  draw_info->opacity=100.0;
  draw_info->linewidth=1.0;
  draw_info->pointsize=image_info->pointsize;
  draw_info->angle=0.0;
  for (i=0; i < 6; i++)
    draw_info->affine[i]=image_info->affine[i];
  draw_info->fill=image_info->fill;
  draw_info->stroke=image_info->stroke;
  (void) QueryColorDatabase("none",&draw_info->box);
  draw_info->border_color=image_info->border_color;
  draw_info->decorate=NoDecoration;
  draw_info->tile=(Image *) NULL;
  draw_info->verbose=image_info->verbose;
  draw_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a t t e F l o o d f i l l I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MatteFloodfillImage floodfills the designated area with a matte
%  value.  The floodfill algorithm is strongly based on a similar algorithm in
%  "Graphics Gems" by Paul Heckbert.
%
%  The format of the MatteFloodfillImage method is:
%
%      unsigned int MatteFloodfillImage(Image *image,const PixelPacket target,
%        const unsigned int matte,const int x_offset,const int y_offset,
%        const PaintMethod method)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o target: A PixelPacket structure.  This is the RGB value of the target
%      color.
%
%    o matte: A integer value representing the amount of transparency.
%
%    o x,y: Unsigned integers representing the current location of the pen.
%
%    o method: drawing method of type PrimitiveType: floodfill or fill to
%      border.
%
%
*/
MagickExport unsigned int MatteFloodfillImage(Image *image,
  const PixelPacket target,const unsigned int matte,const int x_offset,
  const int y_offset,const PaintMethod method)
{
  int
    offset,
    skip,
    start,
    x1,
    x2,
    y;

  register int
    x;

  register PixelPacket
    *q;

  register SegmentInfo
    *s;

  SegmentInfo
    *segment_stack;

  /*
    Check boundary conditions.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((x_offset < 0) || (x_offset >= (int) image->columns))
    return(False);
  if ((y_offset < 0) || (y_offset >= (int) image->rows))
    return(False);
  if (target.opacity == matte)
    return(False);
  q=GetImagePixels(image,x_offset,y_offset,1,1);
  if (q == (PixelPacket *) NULL)
    return(False);
  if (q->opacity == matte)
    return(False);
  /*
    Allocate segment stack.
  */
  segment_stack=(SegmentInfo *)
    AcquireMemory(MaxStacksize*sizeof(SegmentInfo));
  if (segment_stack == (SegmentInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to floodfill image",
      image->filename);
  /*
    Push initial segment on stack.
  */
  image->storage_class=DirectClass;
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
  x=x_offset;
  y=y_offset;
  start=0;
  s=segment_stack;
  Push(y,x,x,1);
  Push(y+1,x,x,-1);
  while (s > segment_stack)
  {
    /*
      Pop segment off stack.
    */
    s--;
    x1=(int) s->x1;
    x2=(int) s->x2;
    offset=(int) s->y2;
    y=(int) s->y1+offset;
    /*
      Recolor neighboring points.
    */
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    q+=x1;
    for (x=x1; x >= 0 ; x--)
    {
      if (method == FloodfillMethod)
        {
          if (!MatteMatch(*q,target,image->fuzz))
            break;
        }
      else
        if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
          break;
      q->opacity=matte;
      q--;
    }
    if (!SyncImagePixels(image))
      break;
    skip=x >= x1;
    if (!skip)
      {
        start=x+1;
        if (start < x1)
          Push(y,start,x1-1,-offset);
        x=x1+1;
      }
    do
    {
      if (!skip)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          q+=x;
          for ( ; x < (int) image->columns; x++)
          {
            if (method == FloodfillMethod)
              {
                if (!MatteMatch(*q,target,image->fuzz))
                  break;
              }
            else
              if (MatteMatch(*q,target,image->fuzz) || (q->opacity == matte))
                break;
            q->opacity=matte;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          Push(y,start,x-1,offset);
          if (x > (x2+1))
            Push(y,x2+1,x-1,-offset);
        }
      skip=False;
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      q+=x;
      for (x++; x <= x2 ; x++)
      {
        q++;
        if (method == FloodfillMethod)
          {
            if (MatteMatch(*q,target,image->fuzz))
              break;
          }
        else
          if (!MatteMatch(*q,target,image->fuzz) && (q->opacity != matte))
            break;
      }
      start=x;
    } while (x <= x2);
  }
  LiberateMemory((void **) &segment_stack);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O p a q u e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpaqueImage changes the color of an opaque point to the pen color.
%
%  The format of the OpaqueImage method is:
%
%      unsigned int OpaqueImage(Image *image,const PixelPacket target,
%        const PixelPacket pen_color)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: the color to search for in the image.
%
%    o pen_color: the color to replace it with.
%
%
*/
MagickExport unsigned int OpaqueImage(Image *image,const PixelPacket target,
  const PixelPacket pen_color)
{
#define OpaqueImageText  "  Setting opaque color in the image...  "

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  /*
    Make image color opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Make DirectClass image opaque.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          if (ColorMatch(*q,target,image->fuzz))
            *q=pen_color;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(OpaqueImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Make PseudoClass image opaque.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        if (ColorMatch(image->colormap[i],target,image->fuzz))
          image->colormap[i]=pen_color;
        if (QuantumTick(i,image->colors))
          MagickMonitor(OpaqueImageText,i,image->colors);
      }
      SyncImage(image);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s p a r e n t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransparentImage creates a matte image associated with the
%  image.  All pixel locations are initially set to opaque.  Any pixel
%  that matches the specified color are set to transparent.
%
%  The format of the TransparentImage method is:
%
%      unsigned int TransparentImage(Image *image,const PixelPacket target)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o target: The color to search for in the image.
%
%
*/
MagickExport unsigned int TransparentImage(Image *image,
  const PixelPacket target)
{
#define TransparentImageText  "  Setting transparent color in the image...  "

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  /*
    Make image color transparent.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    MatteImage(image,OpaqueOpacity);
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      if (ColorMatch(*q,target,image->fuzz))
        q->opacity=TransparentOpacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(TransparentImageText,y,image->rows);
  }
  return(True);
}
