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
%                   ImageMagick Image Vector Drawing Methods                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2002                                   %
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

/*
  Include declarations.
*/
#include "studio.h"

/*
  Define declarations.
*/
#define DRAW_BINARY_IMPLEMENTATION 0

#define ThrowDrawException(code,reason,description) \
{ \
  if (context->image->exception.severity > (long)code) \
    ThrowException(&context->image->exception,code,reason,description); \
  return; \
}

#define CurrentContext (context->graphic_context[context->index])
#define PixelPacketMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue) && \
  ((p)->opacity == (q)->opacity))

/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                           /* Z|z (none) */
  PathCurveToOperation,                         /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,          /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation,    /* T|t (x y)+ */
  PathCurveToSmoothOperation,                   /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                     /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,                /* H|h x+ */
  PathLineToOperation,                          /* L|l (x y)+ */
  PathLineToVerticalOperation,                  /* V|v y+ */
  PathMoveToOperation                           /* M|m (x y)+ */
} PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
} PathMode;

struct _DrawContext
{
  /* Support structures */
  Image
    *image;

  /* MVG output string and housekeeping */
  char
    *mvg;               /* MVG data */

  size_t
    mvg_alloc,          /* total allocated memory */
    mvg_length;         /* total MVG length */

  unsigned int
    mvg_width;          /* current line length */

  /* Pattern support */
  char
    *pattern_id;

  RectangleInfo
    pattern_bounds;

  size_t
    pattern_offset;

  /* Graphic context */
  unsigned int
    index;              /* array index */

  DrawInfo
    **graphic_context;

  int
    filter_off;         /* true if not filtering attributes */

  /* Pretty-printing depth */
  unsigned int
    indent_depth;       /* number of left-hand pad characters */

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;

  /* Structure unique signature */
  unsigned long
    signature;
};

/* Vector table for invoking subordinate renderers */
struct _DrawVTable
{
  void (*DrawAnnotation)
    (DrawContext context, const double x, const double y,
     const unsigned char *text);
  void (*DrawArc)
    (DrawContext context, const double sx, const double sy,
     const double ex, const double ey, const double sd, const double ed);
  void (*DrawBezier)
    (DrawContext context, const size_t num_coords, const PointInfo *coordinates);
  void (*DrawCircle)
    (DrawContext context, const double ox, const double oy,
     const double px, const double py);
  void (*DrawColor)
    (DrawContext context, const double x, const double y,
     const PaintMethod paintMethod);
  void (*DrawComment)
    (DrawContext context,const char* comment);
  void (*DrawDestroyContext)
    (DrawContext context);
  void (*DrawEllipse)
    (DrawContext context, const double ox, const double oy,
     const double rx, const double ry, const double start, const double end);
  void (*DrawComposite)
    (DrawContext context, const CompositeOperator composite_operator,
     const double x, const double y, const double width, const double height,
     const Image * image );
  void (*DrawLine)
    (DrawContext context, const double sx, const double sy,
     const double ex, const double ey);
  void (*DrawMatte)
    (DrawContext context, const double x, const double y,
     const PaintMethod paint_method);
  void (*DrawPathClose)
    (DrawContext context);
  void (*DrawPathCurveToAbsolute)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2, const double x, const double y);
  void (*DrawPathCurveToRelative)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2, const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierAbsolute)
    (DrawContext context, const double x1, const double y1,
     const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierRelative)
    (DrawContext context, const double x1, const double y1,
     const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierSmoothAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathCurveToQuadraticBezierSmoothRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathCurveToSmoothAbsolute)
    (DrawContext context, const double x2, const double y2,
     const double x, const double y);
  void (*DrawPathCurveToSmoothRelative)
    (DrawContext context, const double x2, const double y2,
     const double x, const double y);
  void (*DrawPathEllipticArcAbsolute)
    (DrawContext context, const double rx, const double ry,
     const double x_axis_rotation, unsigned int large_arc_flag,
     unsigned int sweep_flag, const double x, const double y);
  void (*DrawPathEllipticArcRelative)
    (DrawContext context, const double rx, const double ry,
     const double x_axis_rotation, unsigned int large_arc_flag,
     unsigned int sweep_flag, const double x, const double y);
  void (*DrawPathFinish)
    (DrawContext context);
  void (*DrawPathLineToAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathLineToRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathLineToHorizontalAbsolute)
    (DrawContext context, const double x);
  void (*DrawPathLineToHorizontalRelative)
    (DrawContext context, const double x);
  void (*DrawPathLineToVerticalAbsolute)
    (DrawContext context, const double y);
  void (*DrawPathLineToVerticalRelative)
    (DrawContext context, const double y);
  void (*DrawPathMoveToAbsolute)
    (DrawContext context, const double x, const double y);
  void (*DrawPathMoveToRelative)
    (DrawContext context, const double x, const double y);
  void (*DrawPathStart)
    (DrawContext context);
  void (*DrawPoint)
    (DrawContext context, const double x, const double y);
  void (*DrawPolygon)
    (DrawContext context, const size_t num_coords, const PointInfo * coordinates);
  void (*DrawPolyline)
    (DrawContext context, const size_t num_coords, const PointInfo * coordinates);
  void (*DrawPopClipPath)
    (DrawContext context);
  void (*DrawPopDefs)
    (DrawContext context);
  void (*DrawPopGraphicContext)
    (DrawContext context);
  void (*DrawPopPattern)
    (DrawContext context);
  void (*DrawPushClipPath)
    (DrawContext context, const char *clip_path_id);
  void (*DrawPushDefs)
    (DrawContext context);
  void (*DrawPushGraphicContext)
    (DrawContext context);
  void (*DrawPushPattern)
    (DrawContext context, const char *pattern_id,
     const double x, const double y, const double width, const double height);
  void (*DrawRectangle)
    (DrawContext context, const double x1, const double y1,
     const double x2, const double y2);
  void (*DrawRoundRectangle)
    (DrawContext context, double x1, double y1,
     double x2, double y2, double rx, double ry);
  void (*DrawSetAffine)
    (DrawContext context, const AffineMatrix *affine);
  void (*DrawSetClipPath)
    (DrawContext context, const char *clip_path);
  void (*DrawSetClipRule)
    (DrawContext context, const FillRule fill_rule);
  void (*DrawSetClipUnits)
    (DrawContext context, const ClipPathUnits clip_units);
  void (*DrawSetFillColor)
    (DrawContext context, const PixelPacket * fill_color);
  void (*DrawSetFillOpacity)
    (DrawContext context, const double fill_opacity);
  void (*DrawSetFillRule)
    (DrawContext context, const FillRule fill_rule);
  void (*DrawSetFillPatternURL)
    (DrawContext context, const char* fill_url);
  void (*DrawSetFont)
    (DrawContext context, const char *font_name);
  void (*DrawSetFontFamily)
    (DrawContext context, const char *font_family);
  void (*DrawSetFontSize)
    (DrawContext context, const double font_pointsize);
  void (*DrawSetFontStretch)
    (DrawContext context, const StretchType font_stretch);
  void (*DrawSetFontStyle)
    (DrawContext context, const StyleType font_style);
  void (*DrawSetFontWeight)
    (DrawContext context, const unsigned long font_weight);
  void (*DrawSetGravity)
    (DrawContext context, const GravityType gravity);
  void (*DrawSetRotate)
    (DrawContext context, const double degrees);
  void (*DrawSetScale)
    (DrawContext context, const double x, const double y);
  void (*DrawSetSkewX)
    (DrawContext context, const double degrees);
  void (*DrawSetSkewY)
    (DrawContext context, const double degrees);
/*   void (*DrawSetStopColor) */
/*     (DrawContext context, const PixelPacket * color, const double offset); */
  void (*DrawSetStrokeAntialias)
    (DrawContext context, const unsigned int true_false);
  void (*DrawSetStrokeColor)
    (DrawContext context, const PixelPacket * stroke_color);
  void (*DrawSetStrokeDashArray)
    (DrawContext context,const double *dasharray);
  void (*DrawSetStrokeDashOffset)
    (DrawContext context,const double dashoffset);
  void (*DrawSetStrokeLineCap)
    (DrawContext context, const LineCap linecap);
  void (*DrawSetStrokeLineJoin)
    (DrawContext context, const LineJoin linejoin);
  void (*DrawSetStrokeMiterLimit)
    (DrawContext context,const unsigned long miterlimit);
  void (*DrawSetStrokeOpacity)
    (DrawContext context, const double opacity);
  void (*DrawSetStrokePatternURL)
    (DrawContext context, const char* stroke_url);
  void (*DrawSetStrokeWidth)
    (DrawContext context, const double width);
  void (*DrawSetTextAntialias)
    (DrawContext context, const unsigned int true_false);
  void (*DrawSetTextDecoration)
    (DrawContext context, const DecorationType decoration);
  void (*DrawSetTextUnderColor)
    (DrawContext context, const PixelPacket * color);
  void (*DrawSetTranslate)
    (DrawContext context, const double x, const double y);
  void (*DrawSetViewbox)
    (DrawContext context, unsigned long x1, unsigned long y1,
     unsigned long x2, unsigned long y2);
};

/*
  Forward declarations.
*/
static int
  MvgPrintf(DrawContext context, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
,
  MvgAutoWrapPrintf(DrawContext context, const char *format, ...)
#if defined(__GNUC__)
__attribute__ ((format (printf, 2, 3)))
#endif
;
static void
  MvgAppendColor(DrawContext context, const PixelPacket *color);


/* "Printf" for MVG commands */
static int MvgPrintf(DrawContext context, const char *format, ...)
{
  const size_t
    alloc_size = MaxTextExtent * 20; /* 40K */

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Allocate initial memory */
  if (context->mvg == (char*) NULL)
    {
      context->mvg = (char *) AcquireMemory(alloc_size);
      if( context->mvg == (char*) NULL )
        {
          ThrowException(&context->image->exception,ResourceLimitError,"Unable to draw image",
                         "Memory allocation failed");
          return -1;
        }

      context->mvg_alloc = alloc_size;
      context->mvg_length = 0;
      if (context->mvg == 0)
        {
          ThrowException(&context->image->exception,ResourceLimitError,"Unable to draw image",
                         "Memory allocation failed");
          return -1;
        }
    }

  /* Re-allocate additional memory if necessary (ensure 20K unused) */
  if (context->mvg_alloc < (context->mvg_length + MaxTextExtent * 10))
    {
      size_t realloc_size = context->mvg_alloc + alloc_size;

      ReacquireMemory((void **) &context->mvg, realloc_size);
      if (context->mvg == NULL)
        {
          ThrowException(&context->image->exception,ResourceLimitError,"Unable to draw image",
                         "Memory allocation failed");
          return -1;
        }
      context->mvg_alloc = realloc_size;
    }

  /* Write to end of existing MVG string */
  {
    int
      formatted_length; /* must be a signed type! */

    va_list
      argp;

    /* Pretty-print indentation */
    while(context->mvg_width < context->indent_depth)
      {
        context->mvg[context->mvg_length] = ' ';
        ++context->mvg_length;
        ++context->mvg_width;
      }
    context->mvg[context->mvg_length] = 0;

    va_start(argp, format);
#if !defined(HAVE_VSNPRINTF)
    formatted_length = vsprintf(context->mvg + context->mvg_length, format, argp);
#else
    formatted_length =
      vsnprintf(context->mvg + context->mvg_length,
                context->mvg_alloc - context->mvg_length - 1, format, argp);
#endif
    va_end(argp);

    if (formatted_length < 0)
      {
        ThrowException(&context->image->exception,StreamError,"vsprintf failed",
                       format);
      }
    else
      {
        context->mvg_length += formatted_length;
        context->mvg_width += formatted_length;
      }
    context->mvg[context->mvg_length] = 0;

    /* Re-evaluate mvg_width */
    if( (context->mvg_length > 1) &&
        (context->mvg[context->mvg_length-1] == '\n') )
      context->mvg_width = 0;

    assert(context->mvg_length + 1 < context->mvg_alloc);

    return formatted_length;
  }
}

/* "Printf" for MVG commands, with autowrap at 78 characters */
static int MvgAutoWrapPrintf(DrawContext context, const char *format, ...)
{
  va_list
    argp;

  int
    formatted_length;

  char
    buffer[MaxTextExtent];

  va_start(argp, format);
#if !defined(HAVE_VSNPRINTF)
  formatted_length = vsprintf(buffer, format, argp);
#else
  formatted_length =
    vsnprintf(buffer,
              sizeof(buffer) - 1, format, argp);
#endif
  va_end(argp);
  *(buffer+sizeof(buffer)-1)=0;

  if (formatted_length < 0)
    {
      ThrowException(&context->image->exception,StreamError,"vsprintf failed",
                     format);
    }
  else
    {
      if( ((context->mvg_width + formatted_length) > 78) &&
          buffer[formatted_length-1] != '\n' )
        MvgPrintf(context, "\n");

      MvgPrintf(context, "%s", buffer);
    }

  return formatted_length;
}

static void MvgAppendColor(DrawContext context, const PixelPacket *color)
{
  if(color->red == 0 && color->green == 0 && color->blue == 0 &&
     color->opacity == TransparentOpacity)
    {
      MvgPrintf(context,"none");
    }
  else
    {
      char
        tuple[MaxTextExtent];

      GetColorTuple(color,context->image->depth,context->image->matte,True,
        tuple);
      MvgPrintf(context,"%.1024s",tuple);
    }
}

static void MvgAppendPointsCommand(DrawContext context, const char* command,
                                   const size_t num_coords,
                                   const PointInfo * coordinates)
{
  const PointInfo
    *coordinate;

  size_t
    i;

  MvgPrintf(context, command);
  for (i = num_coords, coordinate = coordinates; i; i--)
    {
      MvgAutoWrapPrintf(context," %.4g,%.4g", coordinate->x, coordinate->y);
      ++coordinate;
    }

  MvgPrintf(context, "\n");
}

static void AdjustAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((affine->sx != 1.0) || (affine->rx != 0.0) || (affine->ry != 0.0) ||
      (affine->sy != 1.0) || (affine->tx != 0.0) || (affine->ty != 0.0))
    {
      AffineMatrix
        current;

      current = CurrentContext->affine;
      CurrentContext->affine.sx=current.sx*affine->sx+current.ry*affine->rx;
      CurrentContext->affine.rx=current.rx*affine->sx+current.sy*affine->rx;
      CurrentContext->affine.ry=current.sx*affine->ry+current.ry*affine->sy;
      CurrentContext->affine.sy=current.rx*affine->ry+current.sy*affine->sy;
      CurrentContext->affine.tx=current.sx*affine->tx+current.ry*affine->ty+current.tx;
      CurrentContext->affine.ty=current.rx*affine->tx+current.sy*affine->ty+current.ty;
    }
}

MagickExport void DrawAnnotation(DrawContext context,
                                 const double x, const double y,
                                 const unsigned char *text)
{
  char
    *escaped_text;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(text != (const unsigned char *) NULL);

  escaped_text=EscapeString((const char*)text,'\'');
  MvgPrintf(context, "text %.4g,%.4g '%.1024s'\n", x, y, escaped_text);
  LiberateMemory((void**)&escaped_text);
}

MagickExport void DrawSetAffine(DrawContext context, const AffineMatrix *affine)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  AdjustAffine( context, affine );

  MvgPrintf(context, "affine %.6g,%.6g,%.6g,%.6g,%.6g,%.6g\n",
            affine->sx, affine->rx, affine->ry, affine->sy,
            affine->tx, affine->ty);
}

MagickExport DrawContext DrawAllocateContext(const DrawInfo *draw_info,
                                             Image *image)
{
  DrawContext
    context;

  /* Allocate initial drawing context */
  context = (DrawContext) AcquireMemory(sizeof(struct _DrawContext));
  if(context == (DrawContext) NULL)
    MagickFatalError(ResourceLimitFatalError,
                     "Unable to allocate initial drawing context",
                     "Memory allocation failed");

  /* Support structures */
  context->image = image;

  /* MVG output string and housekeeping */
  context->mvg = NULL;
  context->mvg_alloc = 0;
  context->mvg_length = 0;
  context->mvg_width = 0;

  /* Pattern support */
  context->pattern_id = NULL;
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  /* Graphic context */
  context->index = 0;
  context->graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if(context->graphic_context == (DrawInfo **) NULL)
    {
      ThrowException(&context->image->exception,ResourceLimitError,"Unable to draw image",
                     "Memory allocation failed");
      return (DrawContext) NULL;
    }
  CurrentContext=CloneDrawInfo((ImageInfo*)NULL,draw_info);
  if(CurrentContext == (DrawInfo*) NULL)
    {
      ThrowException(&context->image->exception,ResourceLimitError,"Unable to draw image",
                     "Memory allocation failed");
      return (DrawContext) NULL;
    }

  context->filter_off = False;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Structure unique signature */
  context->signature = MagickSignature;

  return context;
}

MagickExport void DrawArc(DrawContext context,
                          const double sx, const double sy,
                          const double ex, const double ey,
                          const double sd, const double ed)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "arc %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
            sx, sy, ex, ey, sd, ed);
}

MagickExport void DrawBezier(DrawContext context, const size_t num_coords,
                             const PointInfo *coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(coordinates != (const PointInfo *) NULL);

  MvgAppendPointsCommand(context,"bezier",num_coords,coordinates);
}

MagickExport void DrawCircle(DrawContext context, const double ox,
                             const double oy, const double px, const double py)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "circle %.4g,%.4g %.4g,%.4g\n", ox, oy, px, py);
}

MagickExport void DrawSetClipPath(DrawContext context, const char *clip_path)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path != (const char *) NULL);

  if( CurrentContext->clip_path == NULL || context->filter_off ||
      LocaleCompare(CurrentContext->clip_path,clip_path) != 0)
    {
      CloneString(&CurrentContext->clip_path,clip_path);
      if(CurrentContext->clip_path == (char*)NULL)
        ThrowDrawException(ResourceLimitError, "Unable to draw image",
                           "Memory allocation failed");

#if DRAW_BINARY_IMPLEMENTATION
      (void) DrawClipPath(context->image,CurrentContext,CurrentContext->clip_path);
#endif

      MvgPrintf(context, "clip-path url(#%s)\n", clip_path);
    }
}

MagickExport void DrawSetClipRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;
      
      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }
      
      if (p != NULL)
        MvgPrintf(context, "clip-rule %s\n", p);
    }
}

MagickExport void DrawSetClipUnits(DrawContext context,
                                   const ClipPathUnits clip_units)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->clip_units != clip_units))
    {
      CurrentContext->clip_units = clip_units;

      if ( clip_units == ObjectBoundingBox )
        {
          AffineMatrix
            affine;

          IdentityAffine(&affine);

          affine.sx=CurrentContext->bounds.x2;
          affine.sy=CurrentContext->bounds.y2;
          affine.tx=CurrentContext->bounds.x1;
          affine.ty=CurrentContext->bounds.y1;

          AdjustAffine( context, &affine );
        }

      switch (clip_units)
        {
        case UserSpace:
          p = "userSpace";
          break;
        case UserSpaceOnUse:
          p = "userSpaceOnUse";
          break;
        case ObjectBoundingBox:
          p = "objectBoundingBox";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "clip-units %s\n", p);
    }
}

MagickExport void DrawColor(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paintMethod)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paintMethod)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    MvgPrintf(context, "color %.4g,%.4g %s\n", x, y, p);
}

MagickExport void DrawComment(DrawContext context,const char* comment)
{
  MvgPrintf(context, "#%s\n", comment);
}

MagickExport void DrawDestroyContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  /* Path operation support */
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;

  /* Pretty-printing depth */
  context->indent_depth = 0;

  /* Graphic context */
  for ( ; context->index > 0; context->index--)
    {
      DestroyDrawInfo(CurrentContext);
      CurrentContext = (DrawInfo*) NULL;
    }
  DestroyDrawInfo(CurrentContext);
  CurrentContext = (DrawInfo*) NULL;
  LiberateMemory((void **) &context->graphic_context);

  /* Pattern support */
  LiberateMemory((void **) &context->pattern_id);
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  /* MVG output string and housekeeping */
  LiberateMemory((void **) &context->mvg);
  context->mvg_alloc = 0;
  context->mvg_length = 0;

  /* Support structures */
  context->image = (Image*)NULL;

  /* Context itself */
  context->signature = 0;
  LiberateMemory((void **) &context);
}

MagickExport void DrawEllipse(DrawContext context,
                              const double ox, const double oy,
                              const double rx, const double ry,
                              const double start, const double end)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "ellipse %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
            ox, oy, rx, ry, start, end);
}

MagickExport void DrawSetFillColor(DrawContext context,
                                   const PixelPacket * fill_color)
{
  PixelPacket
    *current_fill,
    new_fill;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(fill_color != (const PixelPacket *) NULL);

  new_fill = *fill_color;
  if(new_fill.opacity != TransparentOpacity)
    new_fill.opacity = CurrentContext->opacity;

  current_fill = &CurrentContext->fill;
  if( context->filter_off || !(PixelPacketMatch(current_fill,&new_fill)) )
    {
      CurrentContext->fill = new_fill;

      MvgPrintf(context, "fill ");
      MvgAppendColor(context, fill_color);
      MvgPrintf(context, "\n");
    }
}
MagickExport void DrawSetFillColorString(DrawContext context, const char* fill_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(fill_color,&pixel_packet,&context->image->exception))
    DrawSetFillColor(context,&pixel_packet);
}
MagickExport void DrawSetFillPatternURL(DrawContext context, const char* fill_url)
{
  char
    pattern[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(fill_url != NULL);

  if(fill_url[0] != '#')
    ThrowDrawException(OptionWarning, "Not relative URL", fill_url);

  FormatString(pattern,"[%.1024s]",fill_url+1);

  if (GetImageAttribute(context->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowDrawException(OptionWarning, "URL not found", fill_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      FormatString(pattern_spec,"url(%.1024s)",fill_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(context->image,CurrentContext,pattern_spec,&CurrentContext->fill_pattern);
#endif
      if (CurrentContext->fill.opacity != TransparentOpacity)
        CurrentContext->fill.opacity=CurrentContext->opacity;

      MvgPrintf(context, "fill %s\n",pattern_spec);
    }
}

MagickExport void DrawSetFillOpacity(DrawContext context,
                                     const double fill_opacity)
{
  Quantum
    opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  opacity = (Quantum)(MaxRGB*(1.0-(fill_opacity <= 1.0 ? fill_opacity : 1.0 ))+0.5);

  if (context->filter_off || (CurrentContext->opacity != opacity))
    {
      CurrentContext->opacity = opacity;
      MvgPrintf(context, "fill-opacity %.4g\n", fill_opacity);
    }
}

MagickExport void DrawSetFillRule(DrawContext context,
                                  const FillRule fill_rule)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->fill_rule != fill_rule))
    {
      CurrentContext->fill_rule = fill_rule;

      switch (fill_rule)
        {
        case EvenOddRule:
          p = "evenodd";
          break;
        case NonZeroRule:
          p = "nonzero";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "fill-rule %s\n", p);
    }
}

MagickExport void DrawSetFont(DrawContext context, const char *font_name)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_name != (const char *) NULL);

  if(context->filter_off || (CurrentContext->font == NULL) ||
     LocaleCompare(CurrentContext->font,font_name) != 0)
    {
      (void) CloneString(&CurrentContext->font,font_name);
      if(CurrentContext->font == (char*)NULL)
        ThrowDrawException(ResourceLimitError, "Unable to draw image",
                           "Memory allocation failed");
      MvgPrintf(context, "font '%s'\n", font_name);
    }
}

MagickExport void DrawSetFontFamily(DrawContext context,
                                    const char *font_family)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(font_family != (const char *) NULL);

  if(context->filter_off || (CurrentContext->family == NULL) ||
     LocaleCompare(CurrentContext->family,font_family) != 0)
    {
      (void) CloneString(&CurrentContext->family,font_family);
      if(CurrentContext->family == (char*)NULL)
        ThrowDrawException(ResourceLimitError, "Unable to draw image",
                           "Memory allocation failed");
      MvgPrintf(context, "font-family %s\n", font_family);
    }
}

MagickExport void DrawSetFontSize(DrawContext context,
                                  const double pointsize)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off ||
     (AbsoluteValue(CurrentContext->pointsize-pointsize) > MagickEpsilon))
    {
      CurrentContext->pointsize=pointsize;

      MvgPrintf(context, "font-size %.4g\n", pointsize);
    }
}

MagickExport void DrawSetFontStretch(DrawContext context,
                                     const StretchType font_stretch)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stretch != font_stretch))
    {
      CurrentContext->stretch=font_stretch;

      switch (font_stretch)
        {
        case NormalStretch:
          p = "normal";
          break;
        case UltraCondensedStretch:
          p = "ultra-condensed";
          break;
        case ExtraCondensedStretch:
          p = "extra-condensed";
          break;
        case CondensedStretch:
          p = "condensed";
          break;
        case SemiCondensedStretch:
          p = "semi-condensed";
          break;
        case SemiExpandedStretch:
          p = "semi-expanded";
          break;
        case ExpandedStretch:
          p = "expanded";
          break;
        case ExtraExpandedStretch:
          p = "extra-expanded";
          break;
        case UltraExpandedStretch:
          p = "ultra-expanded";
          break;
        case AnyStretch:
          p = "all";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "font-stretch %s\n", p);
    }
}

MagickExport void DrawSetFontStyle(DrawContext context,
                                   const StyleType style)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->style != style))
    {
      CurrentContext->style=style;

      switch (style)
        {
        case NormalStyle:
          p = "normal";
          break;
        case ItalicStyle:
          p = "italic";
          break;
        case ObliqueStyle:
          p = "oblique";
          break;
        case AnyStyle:
          p = "all";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "font-style %s\n", p);
    }
}

MagickExport void DrawSetFontWeight(DrawContext context,
                                    const unsigned long font_weight)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->weight != font_weight))
    {
      CurrentContext->weight=font_weight;
      MvgPrintf(context, "font-weight %lu\n", font_weight);
    }
}

MagickExport void DrawSetGravity(DrawContext context,
                                 const GravityType gravity)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->gravity != gravity))
    {
      CurrentContext->gravity=gravity;

      switch (gravity)
        {
        case NorthWestGravity:
          p = "NorthWest";
          break;
        case NorthGravity:
          p = "North";
          break;
        case NorthEastGravity:
          p = "NorthEast";
          break;
        case WestGravity:
          p = "West";
          break;
        case CenterGravity:
          p = "Center";
          break;
        case EastGravity:
          p = "East";
          break;
        case SouthWestGravity:
          p = "SouthWest";
          break;
        case SouthGravity:
          p = "South";
          break;
        case SouthEastGravity:
          p = "SouthEast";
          break;
        case StaticGravity:
        case ForgetGravity:
          {
          }
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "gravity %s\n", p);
    }
}

MagickExport void DrawComposite(DrawContext context,
                                const CompositeOperator composite_operator,
                                const double x, const double y,
                                const double width, const double height,
                                const Image * image )

{
  ImageInfo
    *image_info;

  Image
    *clone_image;

  char
    *media_type = NULL,
    *base64 = NULL;

  const char
    *mode = NULL;

  unsigned char
    *blob = (unsigned char*)NULL;

  size_t
    blob_length = 2048,
    encoded_length = 0;

  MonitorHandler
    handler;

  assert(context != (DrawContext)NULL);
  assert(image != (Image *) NULL);
  assert(width != 0);
  assert(height != 0);
  assert(*image->magick != '\0');

  clone_image = CloneImage(image,0,0,True,&context->image->exception);
  if(!clone_image)
    return;

  image_info = CloneImageInfo((ImageInfo*)NULL);
  if(!image_info)
    ThrowDrawException(ResourceLimitError, "Unable to draw image",
                       "Memory allocation failed");
  handler=SetMonitorHandler((MonitorHandler) NULL);
  blob = (unsigned char*)ImageToBlob( image_info, clone_image, &blob_length,
                                      &context->image->exception );
  (void) SetMonitorHandler(handler);
  DestroyImageInfo(image_info);
  DestroyImageList(clone_image);
  if(!blob)
    return;

  base64 = Base64Encode(blob,blob_length,&encoded_length);
  LiberateMemory((void**)&blob);
  if(!base64)
    {
      char
        buffer[MaxTextExtent];

      FormatString(buffer,"%ld bytes", (4L*blob_length/3L+4L));
      ThrowDrawException(ResourceLimitWarning, "allocating Base64 memory",buffer);
    }

  mode = "copy";
  switch (composite_operator)
    {
    case AddCompositeOp:
      mode = "add";
      break;
    case AtopCompositeOp:
      mode = "atop";
      break;
    case BumpmapCompositeOp:
      mode = "bumpmap";
      break;
    case ClearCompositeOp:
      mode = "clear";
      break;
    case ColorizeCompositeOp:
      mode = "colorize_not_supported";
      break;
    case CopyBlueCompositeOp:
      mode = "copyblue";
      break;
    case CopyCompositeOp:
      mode = "copy";
      break;
    case CopyGreenCompositeOp:
      mode = "copygreen";
      break;
    case CopyOpacityCompositeOp:
      mode = "copyopacity";
      break;
    case CopyRedCompositeOp:
      mode = "copyred";
      break;
    case DarkenCompositeOp:
      mode = "darken_not_supported";
      break;
    case DifferenceCompositeOp:
      mode = "difference";
      break;
    case DisplaceCompositeOp:
      mode = "displace_not_supported";
      break;
    case DissolveCompositeOp:
      mode = "dissolve_not_supported";
      break;
    case HueCompositeOp:
      mode = "hue_not_supported";
      break;
    case InCompositeOp:
      mode = "in";
      break;
    case LightenCompositeOp:
      mode = "lighten_not_supported";
      break;
    case LuminizeCompositeOp:
      mode = "luminize_not_supported";
      break;
    case MinusCompositeOp:
      mode = "minus";
      break;
    case ModulateCompositeOp:
      mode = "modulate_not_supported";
      break;
    case MultiplyCompositeOp:
      mode = "multiply";
      break;
    case NoCompositeOp:
      mode = "no_not_supported";
      break;
    case OutCompositeOp:
      mode = "out";
      break;
    case OverCompositeOp:
      mode = "over";
      break;
    case OverlayCompositeOp:
      mode = "overlay_not_supported";
      break;
    case PlusCompositeOp:
      mode = "plus";
      break;
    case SaturateCompositeOp:
      mode = "saturate_not_supported";
      break;
    case ScreenCompositeOp:
      mode = "screen_not_supported";
      break;
    case SubtractCompositeOp:
      mode = "subtract";
      break;
    case ThresholdCompositeOp:
      mode = "threshold";
      break;
    case XorCompositeOp:
      mode = "xor";
      break;
    default:
      break;
    }

  media_type = MagickToMime( image->magick );

  if( media_type != NULL )
    {
      char
        *str;

      int
        remaining;

      MvgPrintf(context, "image %s %.4g,%.4g %.4g,%.4g 'data:%s;base64,\n",
                mode, x, y, width, height, media_type);

      remaining = (int)encoded_length;
      str = base64;
      while( remaining > 0 )
        {
          MvgPrintf(context,"%.76s", str);
          remaining -= 76;
          str += 76;
          if(remaining > 0)
            MvgPrintf(context,"\n");
        }

      MvgPrintf(context,"'\n");
    }

  LiberateMemory((void**)&media_type);
}

MagickExport void DrawLine(DrawContext context,
                           const double sx, const double sy,
                           const double ex, const double ey)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "line %.4g,%.4g %.4g,%.4g\n", sx, sy, ex, ey);
}

MagickExport void DrawMatte(DrawContext context,
                            const double x, const double y,
                            const PaintMethod paint_method)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  switch (paint_method)
    {
    case PointMethod:
      p = "point";
      break;
    case ReplaceMethod:
      p = "replace";
      break;
    case FloodfillMethod:
      p = "floodfill";
      break;
    case FillToBorderMethod:
      p = "filltoborder";
      break;
    case ResetMethod:
      p = "reset";
      break;
    }

  if (p != NULL)
    MvgPrintf(context, "matte %.4g,%.4g %s\n", x, y, p);
}

MagickExport void DrawPathClose(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgAutoWrapPrintf(context, "%s", context->path_mode == AbsolutePathMode ? "Z" : "z");
}

static void DrawPathCurveTo(DrawContext context,
                            const PathMode mode,
                            const double x1, const double y1,
                            const double x2, const double y2,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'C' : 'c',
                        x1, y1, x2, y2, x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g %.4g,%.4g",
                      x1, y1, x2, y2, x, y);
}

MagickExport void DrawPathCurveToAbsolute(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, AbsolutePathMode, x1, y1, x2, y2, x, y);
}

MagickExport void DrawPathCurveToRelative(DrawContext context,
                                          const double x1, const double y1,
                                          const double x2, const double y2,
                                          const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveTo(context, RelativePathMode, x1, y1, x2, y2, x, y);
}

static void DrawPathCurveToQuadraticBezier(DrawContext context,
                                           const PathMode mode,
                                           const double x1, double y1,
                                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'Q' : 'q', x1, y1, x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g", x1, y1, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierAbsolute(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, AbsolutePathMode, x1, y1, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierRelative(DrawContext context,
                                                         const double x1,
                                                         const double y1,
                                                         const double x,
                                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezier(context, RelativePathMode, x1, y1, x, y);
}

static void DrawPathCurveToQuadraticBezierSmooth(DrawContext context,
                                                 const PathMode mode,
                                                 const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToQuadraticBezierSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToQuadraticBezierSmoothOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'T' : 't', x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierSmoothAbsolute(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToQuadraticBezierSmooth(context, AbsolutePathMode, x, y);
}

MagickExport void DrawPathCurveToQuadraticBezierSmoothRelative(DrawContext
                                                               context,
                                                               const double x,
                                                               const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(context, RelativePathMode, x, y);
}

static void DrawPathCurveToSmooth(DrawContext context, const PathMode mode,
                                  const double x2, const double y2,
                                  const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathCurveToSmoothOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathCurveToSmoothOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g,%.4g",
                        mode == AbsolutePathMode ? 'S' : 's', x2, y2, x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g,%.4g", x2, y2, x, y);
}

MagickExport void DrawPathCurveToSmoothAbsolute(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, AbsolutePathMode, x2, y2, x, y);
}

MagickExport void DrawPathCurveToSmoothRelative(DrawContext context,
                                                const double x2, const double y2,
                                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathCurveToSmooth(context, RelativePathMode, x2, y2, x, y);
}

static void DrawPathEllipticArc(DrawContext context, const PathMode mode,
                                const double rx, const double ry,
                                const double x_axis_rotation,
                                unsigned int large_arc_flag,
                                unsigned int sweep_flag,
                                const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathEllipticArcOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathEllipticArcOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g %.4g %u %u %.4g,%.4g",
                        mode == AbsolutePathMode ? 'A' : 'a', rx, ry, x_axis_rotation,
                        large_arc_flag, sweep_flag, x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g %.4g %u %u %.4g,%.4g", rx, ry,
                      x_axis_rotation, large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathEllipticArcAbsolute(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathEllipticArc(context, AbsolutePathMode, rx, ry, x_axis_rotation,
                      large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathEllipticArcRelative(DrawContext context,
                                              const double rx, const double ry,
                                              const double x_axis_rotation,
                                              unsigned int large_arc_flag,
                                              unsigned int sweep_flag,
                                              const double x, const double y)
{
  DrawPathEllipticArc(context, RelativePathMode, rx, ry, x_axis_rotation,
                      large_arc_flag, sweep_flag, x, y);
}

MagickExport void DrawPathFinish(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "'\n");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

static void DrawPathLineTo(DrawContext context,
                           const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'L' : 'l', x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathLineToAbsolute(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, AbsolutePathMode, x, y);
}

MagickExport void DrawPathLineToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineTo(context, RelativePathMode, x, y);
}

static void DrawPathLineToHorizontal(DrawContext context,
                                     const PathMode mode, const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToHorizontalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToHorizontalOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g",
                        mode == AbsolutePathMode ? 'H' : 'h', x);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g", x);
}

MagickExport void DrawPathLineToHorizontalAbsolute(DrawContext context,
                                                   const double x)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToHorizontal(context, AbsolutePathMode, x);
}

MagickExport void DrawPathLineToHorizontalRelative(DrawContext context,
                                                   const double x)
{
  DrawPathLineToHorizontal(context, RelativePathMode, x);
}

static void DrawPathLineToVertical(DrawContext context, const PathMode mode,
                                   const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathLineToVerticalOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathLineToVerticalOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g",
                        mode == AbsolutePathMode ? 'V' : 'v', y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g", y);
}

MagickExport void DrawPathLineToVerticalAbsolute(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, AbsolutePathMode, y);
}

MagickExport void DrawPathLineToVerticalRelative(DrawContext context,
                                                 const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathLineToVertical(context, RelativePathMode, y);
}

static void DrawPathMoveTo(DrawContext context, const PathMode mode,
                           const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if ((context->path_operation != PathMoveToOperation)
      || (context->path_mode != mode))
    {
      context->path_operation = PathMoveToOperation;
      context->path_mode = mode;
      MvgAutoWrapPrintf(context, "%c%.4g,%.4g",
                        mode == AbsolutePathMode ? 'M' : 'm', x, y);
    }
  else
    MvgAutoWrapPrintf(context, " %.4g,%.4g", x, y);
}

MagickExport void DrawPathMoveToAbsolute(DrawContext context, const double x,
                                         const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, AbsolutePathMode, x, y);
}
MagickExport void DrawPathMoveToRelative(DrawContext context,
                                         const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  DrawPathMoveTo(context, RelativePathMode, x, y);
}

MagickExport void DrawPathStart(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "path '");
  context->path_operation = PathDefaultOperation;
  context->path_mode = DefaultPathMode;
}

MagickExport void DrawPoint(DrawContext context,
                            const double x, const double y)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "point %.4g,%.4g\n", x, y);
}

MagickExport void DrawPolygon(DrawContext context,
                              const size_t num_coords,
                              const PointInfo * coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgAppendPointsCommand(context,"polygon",num_coords,coordinates);
}

MagickExport void DrawPolyline(DrawContext context,
                               const size_t num_coords,
                               const PointInfo * coordinates)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgAppendPointsCommand(context,"polyline",num_coords,coordinates);
}

MagickExport void DrawPopClipPath(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->indent_depth > 0)
    context->indent_depth--;
  MvgPrintf(context, "pop clip-path\n");
}

MagickExport void DrawPopDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->indent_depth > 0)
    context->indent_depth--;
  MvgPrintf(context, "pop defs\n");
}

MagickExport void DrawPopGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->index > 0)
    {
      /* Destroy clip path if not same in preceding context */
#if DRAW_BINARY_IMPLEMENTATION
      if (CurrentContext->clip_path != (char *) NULL)
        if (LocaleCompare(CurrentContext->clip_path,
                          context->graphic_context[context->index-1]->clip_path) != 0)
          (void) SetImageClipMask(context->image,(Image *) NULL);
#endif

      DestroyDrawInfo(CurrentContext);
      CurrentContext=(DrawInfo*)NULL;
      context->index--;

      if(context->indent_depth > 0)
        context->indent_depth--;
      MvgPrintf(context, "pop graphic-context\n");
    }
  else
    {
      ThrowDrawException(CorruptImageError,"unbalanced graphic context push/pop",NULL);
    }
}

MagickExport void DrawPopPattern(DrawContext context)
{
  char
    geometry[MaxTextExtent],
    key[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if( context->pattern_id == NULL )
    ThrowDrawException(OptionWarning,"Not currently pushing pattern definition",NULL);

  FormatString(key,"[%.1024s]",context->pattern_id);

  (void) SetImageAttribute(context->image,key,context->mvg+context->pattern_offset);
  FormatString(geometry,"%lux%lu%+ld%+ld",
               context->pattern_bounds.width,context->pattern_bounds.height,
               context->pattern_bounds.x,context->pattern_bounds.y);
  (void) SetImageAttribute(context->image,key,geometry);

  LiberateMemory( (void**)&context->pattern_id );
  context->pattern_offset = 0;

  context->pattern_bounds.x = 0;
  context->pattern_bounds.y = 0;
  context->pattern_bounds.width = 0;
  context->pattern_bounds.height = 0;

  context->filter_off = False;

  if(context->indent_depth > 0)
    context->indent_depth--;
  MvgPrintf(context, "pop pattern\n");
}

MagickExport void DrawPushClipPath(DrawContext context,
                                   const char *clip_path_id)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(clip_path_id != (const char *) NULL);

  MvgPrintf(context, "push clip-path %s\n", clip_path_id);
  context->indent_depth++;
}

MagickExport void DrawPushDefs(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "push defs\n");
  context->indent_depth++;
}

MagickExport void DrawPushGraphicContext(DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  context->index++;
  ReacquireMemory((void **) &context->graphic_context,
                  (context->index+1)*sizeof(DrawInfo *));
  if (context->graphic_context == (DrawInfo **) NULL)
    {
      ThrowDrawException(ResourceLimitError, "Unable to draw image",
                         "Memory allocation failed");
    }
  CurrentContext=
    CloneDrawInfo((ImageInfo *) NULL,context->graphic_context[context->index-1]);
  if(CurrentContext == (DrawInfo*) NULL)
    ThrowDrawException(ResourceLimitError, "Unable to draw image",
                       "Failed to clone drawing context");

  MvgPrintf(context, "push graphic-context\n");
  context->indent_depth++;
}

MagickExport void DrawPushPattern(DrawContext context,
                                  const char *pattern_id,
                                  const double x, const double y,
                                  const double width, const double height)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(pattern_id != (const char *) NULL);

  if( context->pattern_id != NULL )
    ThrowDrawException(OptionWarning,"Already pushing pattern definition",
                       context->pattern_id);

  context->filter_off = True;

  MvgPrintf(context, "push pattern %s %.4g,%.4g %.4g,%.4g\n",
            pattern_id, x, y, width, height);
  context->indent_depth++;

  /* Record current pattern ID, bounds, and start position in MVG */
  context->pattern_id = AllocateString(pattern_id);
  context->pattern_bounds.x = (long) ceil(x-0.5);
  context->pattern_bounds.y = (long) ceil(y-0.5);
  context->pattern_bounds.width = (unsigned long) floor(width+0.5);
  context->pattern_bounds.height = (unsigned long) floor(height+0.5);
  context->pattern_offset = context->mvg_length;
}

MagickExport void DrawRectangle(DrawContext context,
                                const double x1, const double y1,
                                const double x2, const double y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "rectangle %.4g,%.4g %.4g,%.4g\n", x1, y1, x2, y2);
}

MagickExport int DrawRender(const DrawContext context)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  CurrentContext->primitive = context->mvg;
  if(CurrentContext->debug)
    (void) fprintf(stdout,"MVG:\n'%s'\n",context->mvg);
  DrawImage(context->image, CurrentContext);
  CurrentContext->primitive = (char *) NULL;

  return True;
}

MagickExport void DrawSetRotate(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=cos(DegreesToRadians(fmod(degrees,360.0)));
  affine.rx=sin(DegreesToRadians(fmod(degrees,360.0)));
  affine.ry=(-sin(DegreesToRadians(fmod(degrees,360.0))));
  affine.sy=cos(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine( context, &affine );

  MvgPrintf(context, "rotate %.4g\n", degrees);
}

MagickExport void DrawRoundRectangle(DrawContext context,
                                     double x1, double y1,
                                     double x2, double y2,
                                     double rx, double ry)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "roundrectangle %.4g,%.4g %.4g,%.4g %.4g,%.4g\n",
            x1, y1, x2, y2, rx, ry);
}

MagickExport void DrawSetScale(DrawContext context,
                               const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.sx=x;
  affine.sy=y;
  AdjustAffine( context, &affine );

  MvgPrintf(context, "scale %.4g,%.4g\n", x, y);
}

MagickExport void DrawSetSkewX(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.ry=tan(DegreesToRadians(fmod(degrees,360.0)));
  AdjustAffine(context,&affine);

  MvgPrintf(context, "skewX %.4g\n", degrees);
}

MagickExport void DrawSetSkewY(DrawContext context, const double degrees)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.rx=tan(DegreesToRadians(fmod(degrees,360.0)));
  DrawSetAffine(context,&affine);

  MvgPrintf(context, "skewY %.4g\n", degrees);
}

#if 0
/* This is gradient stuff so it shouldn't be supported yet */
MagickExport void DrawSetStopColor(DrawContext context,
                                   const PixelPacket * stop_color,
                                   const double offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stop_color != (const PixelPacket *) NULL);


  MvgPrintf(context, "stop-color ");
  MvgAppendColor(context, stop_color);
  MvgPrintf(context, "\n");
}
#endif

MagickExport void DrawSetStrokeColor(DrawContext context,
                                     const PixelPacket * stroke_color)
{
  PixelPacket
    *current_stroke,
    new_stroke;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stroke_color != (const PixelPacket *) NULL);

  new_stroke = *stroke_color;
  if(new_stroke.opacity != TransparentOpacity)
    new_stroke.opacity = CurrentContext->opacity;

  current_stroke = &CurrentContext->stroke;
  if( context->filter_off || !(PixelPacketMatch(current_stroke,&new_stroke)) )
    {
      CurrentContext->stroke = new_stroke;

      MvgPrintf(context, "stroke ");
      MvgAppendColor(context, stroke_color);
      MvgPrintf(context, "\n");
    }
}
MagickExport void DrawSetStrokeColorString(DrawContext context, const char* stroke_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(stroke_color,&pixel_packet,&context->image->exception))
    DrawSetStrokeColor(context,&pixel_packet);
}
MagickExport void DrawSetStrokePatternURL(DrawContext context, const char* stroke_url)
{
  char
    pattern[MaxTextExtent];

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(stroke_url != NULL);

  if(stroke_url[0] != '#')
    ThrowDrawException(OptionWarning, "Not relative URL", stroke_url);

  FormatString(pattern,"[%.1024s]",stroke_url+1);

  if (GetImageAttribute(context->image,pattern) == (ImageAttribute *) NULL)
    {
      ThrowDrawException(OptionWarning, "URL not found", stroke_url);
    }
  else
    {
      char
        pattern_spec[MaxTextExtent];

      FormatString(pattern_spec,"url(%.1024s)",stroke_url);
#if DRAW_BINARY_IMPLEMENTATION
      DrawPatternPath(context->image,CurrentContext,pattern_spec,&CurrentContext->stroke_pattern);
#endif
      if (CurrentContext->stroke.opacity != TransparentOpacity)
        CurrentContext->stroke.opacity=CurrentContext->opacity;

      MvgPrintf(context, "stroke %s\n",pattern_spec);
    }
}

MagickExport void DrawSetStrokeAntialias(DrawContext context,
                                         const unsigned int stroke_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->stroke_antialias != stroke_antialias))
    {
      CurrentContext->stroke_antialias = stroke_antialias;

      MvgPrintf(context, "stroke-antialias %i\n", stroke_antialias ? 1 : 0);
    }
}

MagickExport void DrawSetStrokeDashArray(DrawContext context,
                                         const double *dasharray)
{
  register const double
    *p;
  
  register double
    *q;
  
  unsigned int
    updated = False,
    n_new = 0,
    n_old = 0;
  
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  
  p = dasharray;
  if( p != (const double *) NULL )
    while( *p++ != 0)
      n_new++;
  
  q = CurrentContext->dash_pattern;
  if( q != (const double *) NULL )
    while( *q++ != 0)
      n_old++;
  
  if( (n_old == 0) && (n_new == 0) )
    {
      updated = False;
    }
  else if( n_old != n_new )
    {
      updated = True;
    }
  else if((CurrentContext->dash_pattern != (double*)NULL)
          && (dasharray != (double*)NULL))
    {
      p = dasharray;
      q = CurrentContext->dash_pattern;
      while( *p && *q )
        if(AbsoluteValue(*p - *q) > MagickEpsilon)
          {
            updated = True;
            break;
          }
    }
  
  if( context->filter_off || updated )
    {
      if(CurrentContext->dash_pattern != (double*)NULL)
        LiberateMemory((void **) &CurrentContext->dash_pattern);
      
      if( n_new != 0)
        {
          CurrentContext->dash_pattern = (double *)
            AcquireMemory((n_new+1)*sizeof(double));
          if(CurrentContext->dash_pattern)
            {
              q=CurrentContext->dash_pattern;
              p=dasharray;
              while( *p )
                *q++=*p++;
              *q=0;
            }
          else
            {
              ThrowDrawException(ResourceLimitError, "Unable to draw image",
                                 "Memory allocation failed");
            }
        }
      
      MvgPrintf(context, "stroke-dasharray ");
      if ( n_new == 0 )
        MvgPrintf(context, "none");
      else
        {
          p = dasharray;
          MvgPrintf(context, "%.4g", *p++);
          while (*p)
            MvgPrintf(context, ",%.4g", *p++);
        }
      MvgPrintf(context, "\n");
    }
}

MagickExport void DrawSetStrokeDashOffset(DrawContext context,
                                          const double dash_offset)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off ||
     (AbsoluteValue(CurrentContext->dash_offset-dash_offset) > MagickEpsilon))
    {
      CurrentContext->dash_offset = dash_offset;

      MvgPrintf(context, "stroke-dashoffset %.4g\n", dash_offset);
    }
}

MagickExport void DrawSetStrokeLineCap(DrawContext context,
                                       const LineCap linecap)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linecap != linecap))
    {
      const char
        *p = NULL;

      CurrentContext->linecap = linecap;

      switch (linecap)
        {
        case ButtCap:
          p = "butt";
          break;
        case RoundCap:
          p = "round";
          break;
        case SquareCap:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "stroke-linecap %s\n", p);
    }
}

MagickExport void DrawSetStrokeLineJoin(DrawContext context,
                                        const LineJoin linejoin)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(context->filter_off || (CurrentContext->linejoin != linejoin))
    {
      const char
        *p = NULL;

      CurrentContext->linejoin = linejoin;

      switch (linejoin)
        {
        case MiterJoin:
          p = "miter";
          break;
        case RoundJoin:
          p = "round";
          break;
        case BevelJoin:
          p = "square";
          break;
        default:
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "stroke-linejoin %s\n", p);
    }
}

MagickExport void DrawSetStrokeMiterLimit(DrawContext context,
                                          const unsigned long miterlimit)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if(CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit = miterlimit;

      MvgPrintf(context, "stroke-miterlimit %lu\n", miterlimit);
    }
}

MagickExport void DrawSetStrokeOpacity(DrawContext context,
                                       const double stroke_opacity)
{
  double
    opacity;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  opacity = (Quantum)(MaxRGB*(1.0-(stroke_opacity <= 1.0 ? stroke_opacity : 1.0 ))+0.5);

  if (context->filter_off || (CurrentContext->stroke.opacity != opacity))
    {
      CurrentContext->stroke.opacity = (Quantum) ceil(opacity);
      MvgPrintf(context, "stroke-opacity %.4g\n", stroke_opacity);
    }
}

MagickExport void DrawSetStrokeWidth(DrawContext context, const double stroke_width)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off ||
      (AbsoluteValue(CurrentContext->stroke_width-stroke_width) > MagickEpsilon))
    {
      CurrentContext->stroke_width = stroke_width;

      MvgPrintf(context, "stroke-width %.4g\n", stroke_width);
    }
}

MagickExport void DrawSetTextAntialias(DrawContext context,
                                       const unsigned int text_antialias)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->text_antialias != text_antialias))
    {
      CurrentContext->text_antialias = text_antialias;

      MvgPrintf(context, "text-antialias %i\n", text_antialias ? 1 : 0);
    }
}

MagickExport void DrawSetTextDecoration(DrawContext context,
                                        const DecorationType decoration)
{
  const char
    *p = NULL;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  if (context->filter_off || (CurrentContext->decorate != decoration))
    {
      CurrentContext->decorate = decoration;

      switch (decoration)
        {
        case NoDecoration:
          p = "none";
          break;
        case UnderlineDecoration:
          p = "underline";
          break;
        case OverlineDecoration:
          p = "overline";
          break;
        case LineThroughDecoration:
          p = "line-through";
          break;
        }

      if (p != NULL)
        MvgPrintf(context, "decorate %s\n", p);
    }
}

MagickExport void DrawSetTextUnderColor(DrawContext context,
                                        const PixelPacket * under_color)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);
  assert(under_color != (const PixelPacket *)NULL);

  if (context->filter_off || !(PixelPacketMatch(&CurrentContext->undercolor, under_color)))
    {
      CurrentContext->undercolor = *under_color;
      MvgPrintf(context, "text-undercolor ");
      MvgAppendColor(context, under_color);
      MvgPrintf(context, "\n");
    }
}

MagickExport void DrawSetTextUnderColorString(DrawContext context, const char* under_color)
{
  PixelPacket
    pixel_packet;

  if(QueryColorDatabase(under_color,&pixel_packet,&context->image->exception))
    DrawSetTextUnderColor(context,&pixel_packet);  
}

MagickExport void DrawSetTranslate(DrawContext context,
                                   const double x, const double y)
{
  AffineMatrix
    affine;

  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  IdentityAffine(&affine);
  affine.tx=x;
  affine.ty=y;
  AdjustAffine( context, &affine );

  MvgPrintf(context, "translate %.4g,%.4g\n", x, y);
}

MagickExport void DrawSetViewbox(DrawContext context,
                                 unsigned long x1, unsigned long y1,
                                 unsigned long x2, unsigned long y2)
{
  assert(context != (DrawContext)NULL);
  assert(context->signature == MagickSignature);

  MvgPrintf(context, "viewbox %lu %lu %lu %lu\n", x1, y1, x2, y2);
}
