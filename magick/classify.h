/*
  ImageMagick enumerations.
*/
#ifndef _MAGICK_CLASSIFY_H
#define _MAGICK_CLASSIFY_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Classify image attributes.
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
  UndefinedBlobMode,
  ReadBlobMode,
  ReadBinaryBlobMode,
  WriteBlobMode,
  WriteBinaryBlobMode
} BlobMode;

typedef enum
{
  UndefinedCache,
  MemoryCache,
  DiskCache,
  MemoryMappedCache
} CacheType;

typedef enum
{
  UndefinedChannel,
  RedChannel,
  CyanChannel,
  GreenChannel,
  MagentaChannel,
  BlueChannel,
  YellowChannel,
  OpacityChannel,
  BlackChannel,
  MatteChannel
} ChannelType;

typedef enum
{
  UndefinedClass,
  DirectClass,
  PseudoClass
} ClassType;

typedef enum
{
  UserSpace,
  UserSpaceOnUse,
  ObjectBoundingBox
} ClipPathUnits;

typedef enum
{
  UndefinedColorspace,
  RGBColorspace,
  GRAYColorspace,
  TransparentColorspace,
  OHTAColorspace,
  XYZColorspace,
  YCbCrColorspace,
  YCCColorspace,
  YIQColorspace,
  YPbPrColorspace,
  YUVColorspace,
  CMYKColorspace,
  sRGBColorspace
} ColorspaceType;

typedef enum
{
  UndefinedCompliance = 0x0000,
  SVGCompliance = 0x0001,
  X11Compliance = 0x0002,
  XPMCompliance = 0x0004
} ComplianceType;

typedef enum
{
  UndefinedCompositeOp = 0,
  OverCompositeOp,
  InCompositeOp,
  OutCompositeOp,
  AtopCompositeOp,
  XorCompositeOp,
  PlusCompositeOp,
  MinusCompositeOp,
  AddCompositeOp,
  SubtractCompositeOp,
  DifferenceCompositeOp,
  MultiplyCompositeOp,
  BumpmapCompositeOp,
  CopyCompositeOp,
  CopyRedCompositeOp,
  CopyGreenCompositeOp,
  CopyBlueCompositeOp,
  CopyOpacityCompositeOp,
  ClearCompositeOp,
  DissolveCompositeOp,
  DisplaceCompositeOp,
  ModulateCompositeOp,
  ThresholdCompositeOp,
  NoCompositeOp,
  DarkenCompositeOp,
  LightenCompositeOp,
  HueCompositeOp,
  SaturateCompositeOp,
  ColorizeCompositeOp,
  LuminizeCompositeOp,
  ScreenCompositeOp,
  OverlayCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
  FaxCompression,
  Group4Compression,
  JPEGCompression,
  LosslessJPEGCompression,
  LZWCompression,
  RLECompression,
  ZipCompression
} CompressionType;

typedef enum
{
  NoDecoration,
  UnderlineDecoration,
  OverlineDecoration,
  LineThroughDecoration
} DecorationType;

typedef enum
{
  UndefinedEndian,
  LSBEndian,
  MSBEndian
} EndianType;

typedef enum
{
  UndefinedException,
  WarningException = 300,
  ResourceLimitWarning = 300,
  TypeWarning = 305,
  OptionWarning = 310,
  DelegateWarning = 315,
  MissingDelegateWarning = 320,
  CorruptImageWarning = 325,
  FileOpenWarning = 330,
  BlobWarning = 335,
  StreamWarning = 340,
  CacheWarning = 345,
  XServerWarning = 385,
  RegistryWarning = 390,
  ConfigureWarning = 395,
  ErrorException = 400,
  ResourceLimitError = 400,
  TypeError = 405,
  OptionError = 410,
  DelegateError = 415,
  MissingDelegateError = 420,
  CorruptImageError = 425,
  FileOpenError = 430,
  BlobError = 435,
  StreamError = 440,
  CacheError = 445,
  XServerError = 485,
  RegistryError = 490,
  ConfigureError = 495,
  FatalErrorException = 700,
  ResourceLimitFatalError = 700,
  TypeFatalError = 705,
  OptionFatalError = 710,
  DelegateFatalError = 715,
  MissingDelegateFatalError = 720,
  CorruptImageFatalError = 725,
  FileOpenFatalError = 730,
  BlobFatalError = 735,
  StreamFatalError = 740,
  CacheFatalError = 745,
  XServerFatalError = 785,
  RegistryFatalError = 790,
  ConfigureFatalError = 795
} ExceptionType;

typedef enum
{
  UndefinedRule,
#undef EvenOddRule
  EvenOddRule,
  NonZeroRule
} FillRule;

typedef enum
{
  UndefinedFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
} FilterTypes;

typedef enum
{
#undef NoValue
  NoValue = 0x0000,
#undef XValue
  XValue = 0x0001,
#undef YValue
  YValue = 0x0002,
#undef WidthValue
  WidthValue = 0x0004,
#undef HeightValue
  HeightValue = 0x0008,
#undef AllValues
  AllValues = 0x000F,
#undef XNegative
  XNegative = 0x0010,
#undef YNegative
  YNegative = 0x0020,
  PercentValue = 0x1000,
  AspectValue = 0x2000,
  LessValue = 0x4000,
  GreaterValue = 0x8000,
  AreaValue = 0x10000
} GeometryFlags;

typedef enum
{
#undef ForgetGravity
  ForgetGravity,
#undef NorthWestGravity
  NorthWestGravity,
#undef NorthGravity
  NorthGravity,
#undef NorthEastGravity
  NorthEastGravity,
#undef WestGravity
  WestGravity,
#undef CenterGravity
  CenterGravity,
#undef EastGravity
  EastGravity,
#undef SouthWestGravity
  SouthWestGravity,
#undef SouthGravity
  SouthGravity,
#undef SouthEastGravity
  SouthEastGravity,
#undef StaticGravity
  StaticGravity
} GravityType;

typedef enum
{
  UndefinedType,
  BilevelType,
  GrayscaleType,
  GrayscaleMatteType,
  PaletteType,
  PaletteMatteType,
  TrueColorType,
  TrueColorMatteType,
  ColorSeparationType,
  ColorSeparationMatteType,
  OptimizeType
} ImageType;

typedef enum
{
  UndefinedInterlace,
  NoInterlace,
  LineInterlace,
  PlaneInterlace,
  PartitionInterlace
} InterlaceType;

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
  NoEvents = 0x0000,
  ConfigureEvent = 0x0001,
  RenderEvent = 0x0002,
  LocaleEvent = 0x0004,
  CoderEvent = 0x0008,
  X11Event = 0x0010,
  BlobEvent = 0x0020,
  AllEvents = 0xFFFF
} LogEventType;

typedef enum
{
  ReadMode,
  WriteMode,
  IOMode
} MapMode;

typedef enum
{
  UndefinedMode,
  FrameMode,
  UnframeMode,
  ConcatenateMode
} MontageMode;

typedef enum
{
  UniformNoise,
  GaussianNoise,
  MultiplicativeGaussianNoise,
  ImpulseNoise,
  LaplacianNoise,
  PoissonNoise
} NoiseType;

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
  RootPath,
  HeadPath,
  TailPath,
  BasePath,
  ExtensionPath
} PathType;

typedef enum
{
  UndefinedProfile,
  ICMProfile,
  IPTCProfile
} ProfileType;

typedef enum
{
  UndefinedPreview = 0,
  RotatePreview,
  ShearPreview,
  RollPreview,
  HuePreview,
  SaturationPreview,
  BrightnessPreview,
  GammaPreview,
  SpiffPreview,
  DullPreview,
  GrayscalePreview,
  QuantizePreview,
  DespecklePreview,
  ReduceNoisePreview,
  AddNoisePreview,
  SharpenPreview,
  BlurPreview,
  ThresholdPreview,
  EdgeDetectPreview,
  SpreadPreview,
  SolarizePreview,
  ShadePreview,
  RaisePreview,
  SegmentPreview,
  SwirlPreview,
  ImplodePreview,
  WavePreview,
  OilPaintPreview,
  CharcoalDrawingPreview,
  JPEGPreview
} PreviewType;

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
  IndexQuantum,
  GrayQuantum,
  IndexAlphaQuantum,
  GrayAlphaQuantum,
  RedQuantum,
  CyanQuantum,
  GreenQuantum,
  YellowQuantum,
  BlueQuantum,
  MagentaQuantum,
  AlphaQuantum,
  BlackQuantum,
  RGBQuantum,
  RGBAQuantum,
  CMYKQuantum,
  CMYKAQuantum
} QuantumType;

typedef enum
{
  UndefinedRegistryType,
  ImageRegistryType,
  ImageInfoRegistryType
} RegistryType;

typedef enum
{
  UndefinedIntent,
  SaturationIntent,
  PerceptualIntent,
  AbsoluteIntent,
  RelativeIntent
} RenderingIntent;

typedef enum
{
  UndefinedResolution,
  PixelsPerInchResolution,
  PixelsPerCentimeterResolution
} ResolutionType;

typedef enum
{
  CharPixel,
  ShortPixel,
  IntegerPixel,
  LongPixel,
  FloatPixel,
  DoublePixel
} StorageType;

typedef enum
{
  UndefinedTimerState,
  StoppedTimerState,
  RunningTimerState
} TimerState;

typedef enum
{
  UndefinedTransmitType,
  FileTransmitType,
  BlobTransmitType,
  StreamTransmitType,
  ImageTransmitType
} TransmitType;

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

typedef enum
{
  UndefinedVirtualPixelMethod,
  ConstantVirtualPixelMethod,
  EdgeVirtualPixelMethod,
  MirrorVirtualPixelMethod,
  TileVirtualPixelMethod
} VirtualPixelMethod;

typedef enum
{
  ControlState = 0x0001,
  InactiveWidgetState = 0x0004,
  JumpListState = 0x0008,
  RedrawActionState = 0x0010,
  RedrawListState = 0x0020,
  RedrawWidgetState = 0x0040,
  UpdateListState = 0x0100
} WidgetState;

typedef enum
{
#undef DoRed
  DoRed = 0x0001,
#undef DoGreen
  DoGreen = 0x0002,
#undef DoBlue
  DoBlue = 0x0004,
  DoMatte = 0x0008
} XColorFlags;

typedef enum
{
  DefaultState = 0x0000,
  EscapeState = 0x0001,
  ExitState = 0x0002,
  FormerImageState = 0x0004,
  ModifierState = 0x0008,
  MontageImageState = 0x0010,
  NextImageState = 0x0020,
  RetainColorsState = 0x0040,
  SuspendTime = 50,
  UpdateConfigurationState = 0x0080,
  UpdateRegionState = 0x0100
} XState;

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
