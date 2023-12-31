/*
  Copyright (C) 2003 - 2009 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Drawing methods.
*/
#ifndef _MAGICK_RENDER_H
#define _MAGICK_RENDER_H

#include "magick/type.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedAlign,
  LeftAlign,
  CenterAlign,
  RightAlign
} AlignType;

typedef enum
{
  UserSpace,
  UserSpaceOnUse,
  ObjectBoundingBox
} ClipPathUnits;

typedef enum
{
  NoDecoration,
  UnderlineDecoration,
  OverlineDecoration,
  LineThroughDecoration
} DecorationType;

typedef enum
{
  UndefinedRule,
#undef EvenOddRule
  EvenOddRule,
  NonZeroRule
} FillRule;

typedef enum
{
  UndefinedGradient,
  LinearGradient,
  RadialGradient
} GradientType;

typedef enum
{
  UndefinedCap,
  ButtCap,
  RoundCap,
  SquareCap
} LineCap;

typedef enum
{
  UndefinedJoin,
  MiterJoin,
  RoundJoin,
  BevelJoin
} LineJoin;

typedef enum
{
  PointMethod = 0,
  ReplaceMethod,
  FloodfillMethod,
  FillToBorderMethod,
  ResetMethod
} PaintMethod;

typedef enum
{
  UndefinedPrimitive = 0,
  PointPrimitive,
  LinePrimitive,
  RectanglePrimitive,
  RoundRectanglePrimitive,
  ArcPrimitive,
  EllipsePrimitive,
  CirclePrimitive,
  PolylinePrimitive,
  PolygonPrimitive,
  BezierPrimitive,
  ColorPrimitive,
  MattePrimitive,
  TextPrimitive,
  ImagePrimitive,
  PathPrimitive
} PrimitiveType;

typedef enum
{
  UndefinedReference,
  GradientReference
} ReferenceType;

typedef enum
{
  UndefinedSpread,
  PadSpread,
  ReflectSpead,
  RepeatSpread
} SpreadMethod;

/*
  Typedef declarations.
*/
typedef struct _GradientInfo
{
  GradientType
    type;

  PixelPacket
    color;

  SegmentInfo
    stop;

  unsigned long
    length;

  SpreadMethod
    spread;

  unsigned long
    signature;

  struct _GradientInfo
    *previous,
    *next;
} GradientInfo;

typedef struct _ElementReference
{
  char
    *id;

  ReferenceType
    type;

  GradientInfo
    gradient;

  unsigned long
    signature;

  struct _ElementReference
    *previous,
    *next;
} ElementReference;

typedef struct _DrawInfo
{
  char
    *primitive,
    *geometry;

  AffineMatrix
    affine;

  GravityType
    gravity;

  PixelPacket
    fill,
    stroke;

  double
    stroke_width;

  GradientInfo
    gradient;

  Image
    *fill_pattern,
    *tile,
    *stroke_pattern;

  unsigned int
    stroke_antialias,
    text_antialias;

  FillRule
    fill_rule;

  LineCap
    linecap;

  LineJoin
    linejoin;

  unsigned long
    miterlimit;

  double
    dash_offset;

  DecorationType
    decorate;

  CompositeOperator
    compose;

  char
    *text,
    *font,
    *family;

  StyleType
    style;

  StretchType
    stretch;

  unsigned long
    weight;

  char
    *encoding;

  double
    pointsize;

  char
    *density;

  AlignType
    align;

  PixelPacket
    undercolor,
    border_color;

  char
    *server_name;

  double
    *dash_pattern;

  char
    *clip_path;

  SegmentInfo
    bounds;

  ClipPathUnits
    clip_units;

  Quantum
    opacity;

  unsigned int
    render,
    debug;  /* deprecated */

  ElementReference
    element_reference;

  unsigned long
    signature;
} DrawInfo;

typedef struct _ElementInfo
{
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

typedef struct _PointInfo
{
  double
    x,
    y;
} PointInfo;

typedef struct _PrimitiveInfo
{
  PointInfo
    point;

  unsigned long
    coordinates;

  PrimitiveType
    primitive;

  PaintMethod
    method;

  char
    *text;
} PrimitiveInfo;

typedef struct _TypeMetric
{
  PointInfo
    pixels_per_em;

  double
    ascent,
    descent,
    width,
    height,
    max_advance;

  SegmentInfo
    bounds;

  double
    underline_position,
    underline_thickness;
} TypeMetric;

/*
  Method declarations.
*/
extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport MagickPassFail
  AnnotateImage(Image *,const DrawInfo *),
  DrawAffineImage(Image *,const Image *,const AffineMatrix *),
  DrawClipPath(Image *,const DrawInfo *,const char *),
  DrawImage(Image *,const DrawInfo *),
  DrawPatternPath(Image *,const DrawInfo *,const char *,Image **),
  GetTypeMetrics(Image *,const DrawInfo *,TypeMetric *);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  GetDrawInfo(const ImageInfo *,DrawInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_RENDER_H */
