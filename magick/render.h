/*
  Drawing methods.
*/
#ifndef _MAGICK_RENDER_H
#define _MAGICK_RENDER_H

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
  NormalStretch,
  UltraCondensedStretch,
  ExtraCondensedStretch,
  CondensedStretch,
  SemiCondensedStretch,
  SemiExpandedStretch,
  ExpandedStretch,
  ExtraExpandedStretch,
  UltraExpandedStretch,
  AnyStretch
} StretchType;

typedef enum
{
  NormalStyle,
  ItalicStyle,
  ObliqueStyle,
  AnyStyle
} StyleType;

/*
  Typedef declarations.
*/
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
    render;

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

typedef struct _TypeInfo
{
  const char
    *path,
    *name,
    *description,
    *family;

  StyleType
    style;

  StretchType
    stretch;

  unsigned long
    weight;

  const char
    *encoding,
    *foundry,
    *format,
    *metrics,
    *glyphs;

  unsigned int
    stealth;

  unsigned long
    signature;

  volatile struct _TypeInfo
    *previous,
    *next;
} TypeInfo;

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
extern MagickExport const TypeInfo
  *GetTypeInfo(const char *,ExceptionInfo *),
  *GetTypeInfoByFamily(const char *,const StyleType,const StretchType,
    const unsigned long,ExceptionInfo *);

extern MagickExport DrawInfo
  *CloneDrawInfo(const ImageInfo *,const DrawInfo *);

extern MagickExport unsigned int
  AnnotateImage(Image *,const DrawInfo *),
  ColorFloodfillImage(Image *,const DrawInfo *,const PixelPacket,const long,
    const long,const PaintMethod),
  DrawAffineImage(Image *,const Image *,const AffineMatrix *),
  DrawClipPath(Image *,const DrawInfo *,const char *),
  DrawImage(Image *,const DrawInfo *),
  DrawPatternPath(Image *,const DrawInfo *,const char *,Image **),
  DrawPrimitive(Image *,const DrawInfo *,const PrimitiveInfo *),
  GetTypeMetrics(Image *,const DrawInfo *,TypeMetric *),
  ListTypeInfo(FILE *,ExceptionInfo *),
  MatteFloodfillImage(Image *,const PixelPacket,const unsigned int,const long,
    const long,const PaintMethod);

extern MagickExport unsigned long
  TracePath(PrimitiveInfo *,const char *);

extern MagickExport void
  DestroyDrawInfo(DrawInfo *),
  DestroyTypeInfo(void),
  GetDrawInfo(const ImageInfo *,DrawInfo *),
  TraceArc(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo,
    const double,const unsigned int,const unsigned int),
  TraceBezier(PrimitiveInfo *,const unsigned long),
  TraceCircle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceEllipse(PrimitiveInfo *,const PointInfo,const PointInfo,const PointInfo),
  TraceLine(PrimitiveInfo *,const PointInfo,const PointInfo),
  TracePoint(PrimitiveInfo *,const PointInfo),
  TraceRectangle(PrimitiveInfo *,const PointInfo,const PointInfo),
  TraceRoundRectangle(PrimitiveInfo *,const PointInfo,const PointInfo,
    PointInfo);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
