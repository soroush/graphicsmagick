/*
  ImageMagick Image Methods.
*/
#ifndef _MAGICK_IMAGE_H
#define _MAGICK_IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(QuantumDepth)
#define QuantumDepth  16
#define QuantumLeap
#endif

#if QuantumDepth == 8
/*
  Color quantum is [0..255].
*/
#define Downscale(quantum)  ((Quantum) (quantum))
#define Upscale(quantum)  ((unsigned int) (quantum))
#define XDownscale(color)  ((Quantum) ((color)/257))
#define XUpscale(color)  ((unsigned int) (257*(color)))

typedef unsigned char Quantum;
#elif QuantumDepth == 16
/*
  Color quantum is [0..65535].
*/
#define Downscale(quantum)  ((Quantum) ((quantum)/257))
#define Upscale(quantum)  ((unsigned int) (257*(quantum)))
#define XDownscale(color)  ((Quantum) (color))
#define XUpscale(color)  ((unsigned int) (color))

typedef unsigned short Quantum;
#else
# error "Specified value of QuantumDepth is not supported"
#endif

#define Intensity(color)  ((Quantum) \
  ((9798L*(color)->red+19235L*(color)->green+3735L*(color)->blue)/32768L))
#define XIntensity(color)  ((unsigned short) \
  ((9798L*(color)->red+19235L*(color)->green+3735L*(color)->blue)/32768L))
#define MaxRGB  ((Quantum) ((1L << QuantumDepth)-1L))
#define OpaqueOpacity  0
#define TransparentOpacity  MaxRGB

/*
  Typedef declarations.
*/
typedef struct _PointInfo
{
  double
    x,
    y,
    z;
} PointInfo;

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  Quantum
    red,
    green,
    blue,
    opacity;
#else
#if defined(macintosh)
  Quantum
    opacity,
    red,
    green,
    blue;
#else
  Quantum
    blue,
    green,
    red,
    opacity;
#endif
#endif
} PixelPacket;

typedef struct _RectangleInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y;
} RectangleInfo;

typedef struct _Ascii85Info
{
  long
    offset,
    line_break;

  unsigned char
    buffer[10];
} Ascii85Info;

typedef struct _BlobInfo
{
  size_t
    length,
    extent,
    quantum;

  unsigned int
    mapped,
    eof;

  unsigned char
    *data;

  off_t
    offset,
    size;

  unsigned long
    signature;
} BlobInfo;

typedef struct _ChromaticityInfo
{
  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _ColorInfo
{
  const char
    *filename,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

typedef struct _ElementInfo
{   
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

typedef struct _ExceptionInfo
{
  char
    *reason,
    *description;

  ExceptionType
    severity;

  unsigned long
    signature;
} ExceptionInfo;

typedef struct _FrameInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y,
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef struct _ImageAttribute
{
  char
    *key,
    *value;

  unsigned int
    compression;

  struct _ImageAttribute
    *previous,
    *next;
} ImageAttribute;

typedef Quantum IndexPacket;

typedef struct _MontageInfo
{
  char
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *font;

  double
    pointsize;

  unsigned long
    border_width;

  unsigned int
    shadow;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  GravityType
    gravity;

  char
    filename[MaxTextExtent];

  unsigned long
    signature;
} MontageInfo;

typedef struct _ProfileInfo
{
  size_t
    length;

  char
    *name;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

struct SemaphoreInfo;

typedef struct SemaphoreInfo
  SemaphoreInfo;

typedef struct _Timer
{
  double
    start,
    stop,
    total;
} Timer;

typedef struct _TimerInfo
{
  Timer
    user,
    elapsed;

  TimerState
    state;

  unsigned long
    signature;
} TimerInfo;

typedef struct _Image
{
  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CompressionType
    compression;

  unsigned int
    orphan,  /* deprecated */
    taint,
    matte;

  unsigned long
    columns,
    rows;

  unsigned long
    depth,
    colors;

  PixelPacket
    *colormap;

  PixelPacket
    background_color,
    border_color,
    matte_color;

  double
    gamma;

  ChromaticityInfo
    chromaticity;

  ProfileInfo
    color_profile,
    iptc_profile;

  unsigned long
    generic_profiles;

  ProfileInfo
    *generic_profile;

  RenderingIntent
    rendering_intent;

  ResolutionType
    units;

  char
    *montage,
    *directory,
    *geometry;

  long
    offset;

  double
    x_resolution,
    y_resolution;

  RectangleInfo
    page;

  RectangleInfo
    tile_info;

  double
    blur,
    fuzz;

  FilterTypes
    filter;

  InterlaceType
    interlace;

  EndianType
    endian;

  GravityType
    gravity;

  CompositeOperator
    compose;

  ImageAttribute
    *attributes;

  struct _Image
    *clip_mask;

  unsigned long
    scene,
    dispose,
    delay,
    iterations,
    total_colors;

  double
    mean_error_per_pixel,
    normalized_mean_error,
    normalized_maximum_error;

  SemaphoreInfo
    *semaphore;

  TimerInfo
    timer;

  ExceptionInfo
    exception;

  void
    *client_data;

  long
    start_loop;

  void
    *cache;

  int
    (*fifo)(const struct _Image *,const void *,const size_t);

  long
    reference_count;

  Ascii85Info
    ascii85;

  unsigned int
    exempt,
    status,
    temporary,
    pipet;

  FILE
    *file;

  BlobInfo
    *blob;

  unsigned long
    magick_columns,
    magick_rows;

  char
    magick_filename[MaxTextExtent],
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  unsigned long
    signature;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

typedef struct _ImageInfo
{
  CompressionType
    compression;

  unsigned int
    temporary,
    adjoin,
    antialias;

  unsigned long
    subimage,
    subrange,
    depth;

  char
    *size,
    *tile,
    *page;

  InterlaceType
    interlace;

  EndianType
    endian;

  ResolutionType
    units;

  unsigned long
    quality;

  char
    *server_name,
    *font,
    *texture,
    *density;

  double
    pointsize;

  double
    fuzz;

  PixelPacket
    pen,
    background_color,
    border_color,
    matte_color;

  unsigned int
    dither,
    monochrome;

  ColorspaceType
    colorspace;

  ImageType
    type;

  PreviewType
    preview_type;

  long
    group;

  unsigned int
    ping,
    verbose,
    debug;

  char
    *view;

  Image
    *attributes;

  void
    *client_data;

  int
    (*fifo)(const Image *,const void *,const size_t);

  FILE
    *file;

  BlobInfo
    *blob;

  char
    magick[MaxTextExtent],
    unique[MaxTextExtent],
    zero[MaxTextExtent],
    filename[MaxTextExtent];

  unsigned long
    signature;
} ImageInfo;

typedef struct _MagickInfo
{
  const char
    *name,
    *description,
    *version,
    *module;

  ImageInfo
    *image_info;

  Image
    *(*decoder)(const ImageInfo *,ExceptionInfo *);

  unsigned int
    (*encoder)(const ImageInfo *,Image *),
    (*magick)(const unsigned char *,const size_t);

  void
    *client_data;

  unsigned int
    adjoin,
    raw,
    stealth,
    blob_support,
    thread_support;

  unsigned long
    signature;

  struct _MagickInfo
    *previous,
    *next;
} MagickInfo;

/*
  Image const declarations.
*/
extern MagickExport const char
  *Alphabet,
  *AppendBinaryType,
  *BackgroundColor,
  *BorderColor,
  *DefaultPointSize,
  *DefaultTileFrame,
  *DefaultTileGeometry,
  *DefaultTileLabel,
  *ForegroundColor,
  *MatteColor,
  *LoadImageText,
  *LoadImagesText,
  *PSDensityGeometry,
  *PSPageGeometry,
  *ReadBinaryType,
  *ReadBinaryUnbufferedType,
  *SaveImageText,
  *SaveImagesText,
  *WriteBinaryType;

extern MagickExport const unsigned long
  DefaultCompressionQuality;

/*
  Image utilities methods.
*/
extern MagickExport char
  **GetColorList(const char *,int *),
  *GetMagickConfigurePath(const char *,ExceptionInfo *);

extern MagickExport const char
  *GetImageMagick(const unsigned char *,const size_t),
  *GetMagickVersion(unsigned int *);

extern MagickExport const ColorInfo
  *GetColorInfo(const char *,ExceptionInfo *);

extern const MagickExport MagickInfo
  *GetMagickInfo(const char *,ExceptionInfo *exception);

extern MagickExport const PixelPacket
  *AcquireImagePixels(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

extern MagickExport Image
  *AddNoiseImage(const Image *,const NoiseType,ExceptionInfo *),
  *AllocateImage(const ImageInfo *),
  *AppendImages(const Image *,const unsigned int,ExceptionInfo *),
  *AverageImages(const Image *,ExceptionInfo *),
  *BlurImage(const Image *,const double,const double,ExceptionInfo *),
  *BorderImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *CharcoalImage(const Image *,const double,const double,ExceptionInfo *),
  *ChopImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *CloneImage(const Image *,const unsigned long,const unsigned long,
    const unsigned int,ExceptionInfo *),
  *CoalesceImages(const Image *,ExceptionInfo *),
  *ColorizeImage(const Image *,const char *,const PixelPacket,ExceptionInfo *),
  *ConstituteImage(const unsigned long,const unsigned long,const char *,
    const StorageType,const void *,ExceptionInfo *),
  *ConvolveImage(const Image *,const unsigned int,const double *,
    ExceptionInfo *),
  *CropImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *DeconstructImages(const Image *,ExceptionInfo *),
  *DespeckleImage(const Image *,ExceptionInfo *),
  *EdgeImage(const Image *,const double,ExceptionInfo *),
  *EmbossImage(const Image *,const double,const double,ExceptionInfo *),
  *EnhanceImage(const Image *,ExceptionInfo *),
  *FlattenImages(const Image *,ExceptionInfo *),
  *FlipImage(const Image *,ExceptionInfo *),
  *FlopImage(const Image *,ExceptionInfo *),
  *FrameImage(const Image *,const FrameInfo *,ExceptionInfo *),
  *GaussianBlurImage(const Image *,const double,const double,ExceptionInfo *),
  *ImplodeImage(const Image *,const double,ExceptionInfo *),
  *MagnifyImage(const Image *,ExceptionInfo *),
  *MedianFilterImage(const Image *,const double,ExceptionInfo *),
  *MinifyImage(const Image *,ExceptionInfo *),
  *MontageImages(const Image *,const MontageInfo *,ExceptionInfo *),
  *MorphImages(const Image *,const unsigned long,ExceptionInfo *),
  *MosaicImages(const Image *,ExceptionInfo *),
  *MotionBlurImage(const Image *,const double,const double,const double,
    ExceptionInfo *),
  *OilPaintImage(const Image *,const double,ExceptionInfo *),
  *PingImage(const ImageInfo *,ExceptionInfo *),
  *ReadImage(const ImageInfo *,ExceptionInfo *),
  *ReadInlineImage(const ImageInfo *,const char *,ExceptionInfo *),
  *ReadStream(const ImageInfo *,
    int (*)(const Image *,const void *,const size_t),ExceptionInfo *),
  *ReduceNoiseImage(const Image *,const double,ExceptionInfo *),
  *ReferenceImage(Image *),
  *ResizeImage(const Image *,const unsigned long,const unsigned long,
    const FilterTypes,const double,ExceptionInfo *),
  *RollImage(const Image *,const long,const long,ExceptionInfo *),
  *RotateImage(const Image *,const double,ExceptionInfo *),
  *SampleImage(const Image *,const unsigned long,const unsigned long,
    ExceptionInfo *),
  *ScaleImage(const Image *,const unsigned long,const unsigned long,
    ExceptionInfo *),
  *ShadeImage(const Image *,const unsigned int,double,double,ExceptionInfo *),
  *SharpenImage(const Image *,const double,const double,ExceptionInfo *),
  *ShaveImage(const Image *,const RectangleInfo *,ExceptionInfo *),
  *ShearImage(const Image *,const double,const double,ExceptionInfo *),
  *SpreadImage(const Image *,const unsigned int,ExceptionInfo *),
  *SteganoImage(const Image *,const Image *,ExceptionInfo *),
  *StereoImage(const Image *,const Image *,ExceptionInfo *),
  *SwirlImage(const Image *,double,ExceptionInfo *),
  *UnsharpMaskImage(const Image *,const double,const double,const double,
    const double,ExceptionInfo *),
  *WaveImage(const Image *,const double,const double,ExceptionInfo *),
  *ZoomImage(const Image *,const unsigned long,const unsigned long,
    ExceptionInfo *);

extern MagickExport ImageInfo
  *CloneImageInfo(const ImageInfo *);

extern MagickExport ImageType
  GetImageType(const Image *,ExceptionInfo *);

extern MagickExport IndexPacket
  ConstrainColormapIndex(Image *,const unsigned long),
  *GetIndexes(const Image *);

extern MagickExport int
  GetImageGeometry(const Image *,const char *,const unsigned int,
    RectangleInfo *);

extern MagickExport MagickInfo
  *RegisterMagickInfo(MagickInfo *),
  *SetMagickInfo(const char *);

extern MagickExport MontageInfo
  *CloneMontageInfo(const ImageInfo *,const MontageInfo *);

extern MagickExport PixelPacket
  AcquireOnePixel(const Image *,const long,const long,ExceptionInfo *),
  *GetImagePixels(Image *,const long,const long,const unsigned long,
    const unsigned long),
  GetOnePixel(Image *,const long,const long),
  *GetPixels(const Image *),
  *SetImagePixels(Image *,const long,const long,const unsigned long,
    const unsigned long);

extern MagickExport RectangleInfo
  GetImageBoundingBox(const Image *,ExceptionInfo *exception);

extern MagickExport unsigned int
  AllocateImageColormap(Image *,const unsigned long),
  AnimateImages(const ImageInfo *image_info,Image *image),
  ChannelImage(Image *,const ChannelType),
  ClipImage(Image *),
  CompositeImage(Image *,const CompositeOperator,const Image *,const long,
    const long),
  ContrastImage(Image *,const unsigned int),
  DispatchImage(const Image *,const long,const long,const unsigned long,
    const unsigned long,const char *,const StorageType,void *,ExceptionInfo *),
  DisplayImages(const ImageInfo *image_info,Image *image),
  EqualizeImage(Image *),
  GammaImage(Image *,const char *),
  GetNumberScenes(const Image *),
  GradientImage(Image *,const PixelPacket *,const PixelPacket *),
  IsGrayImage(const Image *,ExceptionInfo *),
  IsImagesEqual(Image *,Image *),
  IsTaintImage(const Image *),
  IsMagickConflict(const char *),
  IsMonochromeImage(const Image *,ExceptionInfo *),
  IsOpaqueImage(const Image *,ExceptionInfo *),
  IsPaletteImage(const Image *,ExceptionInfo *),
  IsSubimage(const char *,const unsigned int),
  LevelImage(Image *,const char *),
  ListColorInfo(FILE *,ExceptionInfo *),
  ListMagickInfo(FILE *,ExceptionInfo *),
  ModulateImage(Image *,const char *),
  MogrifyImage(const ImageInfo *,const int,char **,Image **),
  MogrifyImages(const ImageInfo *,const int,char **,Image **),
  NegateImage(Image *,const unsigned int),
  NormalizeImage(Image *),
  OpaqueImage(Image *,const PixelPacket,const PixelPacket),
  PlasmaImage(Image *,const SegmentInfo *,int,int),
  PopImagePixels(const Image *,const QuantumType,unsigned char *),
  ProfileImage(Image *,const char *,const unsigned char *,const size_t),
  PushImagePixels(Image *,const QuantumType,const unsigned char *),
  QueryColorDatabase(const char *,PixelPacket *),
  QueryColorname(const Image *,const PixelPacket *,const ComplianceType,char *,
    ExceptionInfo *),
  RaiseImage(Image *,const RectangleInfo *,const int),
  RGBTransformImage(Image *,const ColorspaceType),
  SetImageClipMask(Image *,Image *),
  SetImageDepth(Image *,const unsigned long),
  SetImageInfo(ImageInfo *,const unsigned int,ExceptionInfo *),
  SignatureImage(Image *),
  SortColormapByIntensity(Image *),
  SyncImagePixels(Image *),
  ThresholdImage(Image *,const double),
  TransformRGBImage(Image *,const ColorspaceType),
  TransparentImage(Image *,const PixelPacket,const unsigned int),
  UnregisterMagickInfo(const char *),
  WriteImage(const ImageInfo *,Image *),
  WriteImages(ImageInfo *,Image *,const char *,ExceptionInfo *),
  WriteStream(const ImageInfo *,Image *,
    int (*)(const Image *,const void *,const size_t));

extern MagickExport unsigned long
  GetImageDepth(const Image *,ExceptionInfo *),
  GetNumberColors(const Image *,FILE *,ExceptionInfo *);

extern MagickExport void
  AllocateNextImage(const ImageInfo *,Image *),
  CompressColormap(Image *),
  CycleColormapImage(Image *,const int),
  DescribeImage(Image *,FILE *,const unsigned int),
  DestroyColorInfo(void),
  DestroyConstitute(void),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  DestroyImagePixels(Image *),
  DestroyImages(Image *),
  DestroyMagick(void),
  DestroyMagickInfo(void),
  DestroyMontageInfo(MontageInfo *),
  GetImageInfo(ImageInfo *),
  GetMontageInfo(const ImageInfo *,MontageInfo *),
  InitializeMagick(const char *),
  ModifyImage(Image **,ExceptionInfo *),
  RegisterARTImage(void),
  RegisterAVIImage(void),
  RegisterAVSImage(void),
  RegisterBIEImage(void),
  RegisterBMPImage(void),
  RegisterCAPTIONImage(void),
  RegisterCMYKImage(void),
  RegisterCUTImage(void),
  RegisterDCMImage(void),
  RegisterDCXImage(void),
  RegisterDIBImage(void),
  RegisterDPSImage(void),
  RegisterDPXImage(void),
  RegisterEPDFImage(void),
  RegisterEPIImage(void),
  RegisterEPSImage(void),
  RegisterEPS2Image(void),
  RegisterEPSFImage(void),
  RegisterEPSIImage(void),
  RegisterEPTImage(void),
  RegisterFAXImage(void),
  RegisterFITSImage(void),
  RegisterFPXImage(void),
  RegisterG3Image(void),
  RegisterGIFImage(void),
  RegisterGIF87Image(void),
  RegisterGRADIENTImage(void),
  RegisterGRANITEImage(void),
  RegisterGRAYImage(void),
  RegisterHImage(void),
  RegisterHDFImage(void),
  RegisterHISTOGRAMImage(void),
  RegisterHTMImage(void),
  RegisterHTMLImage(void),
  RegisterICBImage(void),
  RegisterICONImage(void),
  RegisterJBGImage(void),
  RegisterJBIGImage(void),
  RegisterJPGImage(void),
  RegisterJPEGImage(void),
  RegisterJP2Image(void),
  RegisterLABELImage(void),
  RegisterLOGOImage(void),
  RegisterMAPImage(void),
  RegisterMETAImage(void),
  RegisterMATImage(void),
  RegisterMATTEImage(void),
  RegisterMIFFImage(void),
  RegisterMNGImage(void),
  RegisterMONOImage(void),
  RegisterMPCImage(void),
  RegisterMPEGImage(void),
  RegisterMPRImage(void),
  RegisterMSLImage(void),
  RegisterMTVImage(void),
  RegisterMVGImage(void),
  RegisterNETSCAPEImage(void),
  RegisterNULLImage(void),
  RegisterP7Image(void),
  RegisterPBMImage(void),
  RegisterOTBImage(void),
  RegisterPALMImage(void),
  RegisterPCDImage(void),
  RegisterPCDSImage(void),
  RegisterPCLImage(void),
  RegisterPCTImage(void),
  RegisterPCXImage(void),
  RegisterPDBImage(void),
  RegisterPDFImage(void),
  RegisterPICImage(void),
  RegisterPICTImage(void),
  RegisterPIXImage(void),
  RegisterPLASMAImage(void),
  RegisterPGMImage(void),
  RegisterPMImage(void),
  RegisterPNGImage(void),
  RegisterPNMImage(void),
  RegisterPPMImage(void),
  RegisterPREVIEWImage(void),
  RegisterPSImage(void),
  RegisterPS2Image(void),
  RegisterPS3Image(void),
  RegisterPSDImage(void),
  RegisterPTIFImage(void),
  RegisterPWPImage(void),
  RegisterRASImage(void),
  RegisterRGBImage(void),
  RegisterRGBAImage(void),
  RegisterRLAImage(void),
  RegisterRLEImage(void),
  RegisterSCTImage(void),
  RegisterSFWImage(void),
  RegisterSGIImage(void),
  RegisterSHTMLImage(void),
  RegisterSTEGANOImage(void),
  RegisterSUNImage(void),
  RegisterSVGImage(void),
  RegisterTEXTImage(void),
  RegisterTGAImage(void),
  RegisterTIFImage(void),
  RegisterTIFFImage(void),
  RegisterTILEImage(void),
  RegisterTIMImage(void),
  RegisterTTFImage(void),
  RegisterTXTImage(void),
  RegisterUILImage(void),
  RegisterURLImage(void),
  RegisterUYVYImage(void),
  RegisterVDAImage(void),
  RegisterVICARImage(void),
  RegisterVIDImage(void),
  RegisterVIFFImage(void),
  RegisterVSTImage(void),
  RegisterWBMPImage(void),
  RegisterWMFImage(void),
  RegisterWPGImage(void),
  RegisterXImage(void),
  RegisterXBMImage(void),
  RegisterXCImage(void),
  RegisterXCFImage(void),
  RegisterXPMImage(void),
  RegisterXVImage(void),
  RegisterXWDImage(void),
  RegisterYUVImage(void),
  SetImage(Image *,const Quantum),
  SetImageOpacity(Image *,const unsigned int),
  SetImageType(Image *,const ImageType),
  SolarizeImage(Image *,const double),
  SyncImage(Image *),
  TextureImage(Image *,const Image *),
  TransformHSL(const Quantum,const Quantum,const Quantum,double *,double *,
    double *),
  TransformImage(Image **,const char *,const char *),
  UnregisterARTImage(void),
  UnregisterAVIImage(void),
  UnregisterAVSImage(void),
  UnregisterBIEImage(void),
  UnregisterBMPImage(void),
  UnregisterCAPTIONImage(void),
  UnregisterCMYKImage(void),
  UnregisterCUTImage(void),
  UnregisterDCMImage(void),
  UnregisterDCXImage(void),
  UnregisterDIBImage(void),
  UnregisterDPSImage(void),
  UnregisterDPXImage(void),
  UnregisterEPDFImage(void),
  UnregisterEPIImage(void),
  UnregisterEPSImage(void),
  UnregisterEPS2Image(void),
  UnregisterEPSFImage(void),
  UnregisterEPSIImage(void),
  UnregisterEPTImage(void),
  UnregisterFAXImage(void),
  UnregisterFITSImage(void),
  UnregisterFPXImage(void),
  UnregisterG3Image(void),
  UnregisterGIFImage(void),
  UnregisterGIF87Image(void),
  UnregisterGRADIENTImage(void),
  UnregisterGRANITEImage(void),
  UnregisterGRAYImage(void),
  UnregisterHImage(void),
  UnregisterHDFImage(void),
  UnregisterHISTOGRAMImage(void),
  UnregisterHTMImage(void),
  UnregisterHTMLImage(void),
  UnregisterICBImage(void),
  UnregisterICONImage(void),
  UnregisterJBGImage(void),
  UnregisterJBIGImage(void),
  UnregisterJPGImage(void),
  UnregisterJPEGImage(void),
  UnregisterJP2Image(void),
  UnregisterLABELImage(void),
  UnregisterLOGOImage(void),
  UnregisterMAPImage(void),
  UnregisterMATImage(void),
  UnregisterMATTEImage(void),
  UnregisterMETAImage(void),
  UnregisterMIFFImage(void),
  UnregisterMNGImage(void),
  UnregisterMONOImage(void),
  UnregisterMPCImage(void),
  UnregisterMPEGImage(void),
  UnregisterMPRImage(void),
  UnregisterMSLImage(void),
  UnregisterMTVImage(void),
  UnregisterMVGImage(void),
  UnregisterNETSCAPEImage(void),
  UnregisterNULLImage(void),
  UnregisterP7Image(void),
  UnregisterPBMImage(void),
  UnregisterOTBImage(void),
  UnregisterPALMImage(void),
  UnregisterPCDImage(void),
  UnregisterPCDSImage(void),
  UnregisterPCLImage(void),
  UnregisterPCTImage(void),
  UnregisterPCXImage(void),
  UnregisterPDBImage(void),
  UnregisterPDFImage(void),
  UnregisterPICImage(void),
  UnregisterPICTImage(void),
  UnregisterPIXImage(void),
  UnregisterPLASMAImage(void),
  UnregisterPGMImage(void),
  UnregisterPMImage(void),
  UnregisterPNGImage(void),
  UnregisterPNMImage(void),
  UnregisterPPMImage(void),
  UnregisterPREVIEWImage(void),
  UnregisterPSImage(void),
  UnregisterPS2Image(void),
  UnregisterPS3Image(void),
  UnregisterPSDImage(void),
  UnregisterPTIFImage(void),
  UnregisterPWPImage(void),
  UnregisterRASImage(void),
  UnregisterRGBImage(void),
  UnregisterRGBAImage(void),
  UnregisterRLAImage(void),
  UnregisterRLEImage(void),
  UnregisterSCTImage(void),
  UnregisterSFWImage(void),
  UnregisterSGIImage(void),
  UnregisterSHTMLImage(void),
  UnregisterSTEGANOImage(void),
  UnregisterSUNImage(void),
  UnregisterSVGImage(void),
  UnregisterTEXTImage(void),
  UnregisterTGAImage(void),
  UnregisterTIFImage(void),
  UnregisterTIFFImage(void),
  UnregisterTILEImage(void),
  UnregisterTIMImage(void),
  UnregisterTTFImage(void),
  UnregisterTXTImage(void),
  UnregisterUILImage(void),
  UnregisterURLImage(void),
  UnregisterUYVYImage(void),
  UnregisterVDAImage(void),
  UnregisterVICARImage(void),
  UnregisterVIDImage(void),
  UnregisterVIFFImage(void),
  UnregisterVSTImage(void),
  UnregisterWBMPImage(void),
  UnregisterWMFImage(void),
  UnregisterWPGImage(void),
  UnregisterXImage(void),
  UnregisterXBMImage(void),
  UnregisterXCImage(void),
  UnregisterXCFImage(void),
  UnregisterXPMImage(void),
  UnregisterXVImage(void),
  UnregisterXWDImage(void),
  UnregisterYUVImage(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
