/*
  X11 Utility Methods for ImageMagick.
*/
#ifndef _MAGICK_XWINDOW_H
#define _MAGICK_XWINDOW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#undef False
#undef True
#define XLIB_ILLEGAL_ACCESS  1
#if !defined(macintosh)
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#if !defined(vms)
#include <X11/XWDFile.h>
#else
#include "XWDFile.h"
#endif
#else
#include <Xos.h>
#include <Xlib.h>
#include <Xutil.h>
#include <Xresource.h>
#include <Xproto.h>
#include <Xatom.h>
#include <cursorfont.h>
#include <keysym.h>
#include <XWDFile.h>
#endif
#if defined(HasShape)
#include <X11/extensions/shape.h>
#endif
#if defined(HasSharedMemory)
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif
#if defined(HasDPS)
# include <DPS/dpsXclient.h>
# include <DPS/dpsXpreview.h>
#endif
#include "PreRvIcccm.h"

#undef index
#if defined(hpux9)
#define XFD_SET  int
#else
#define XFD_SET  fd_set
#endif

/*
  Define declarations.
*/
#define MaxNumberPens  11
#define MaxNumberFonts  11
#define MaxIconSize  96

/*
  Enumeration declarations.
*/
typedef enum
{
  ForegroundStencil,
  BackgroundStencil,
  OpaqueStencil,
  TransparentStencil
} AnnotationStencil;

typedef enum
{
  UndefinedElement = 0,
  PointElement,
  LineElement,
  RectangleElement,
  FillRectangleElement,
  CircleElement,
  FillCircleElement,
  EllipseElement,
  FillEllipseElement,
  PolygonElement,
  FillPolygonElement,
  ColorElement,
  MatteElement,
  TextElement,
  ImageElement
} ElementType;

typedef enum
{
  UndefinedColormap,
  PrivateColormap,
  SharedColormap
} XColormapType;

/*
  Typedef declarations.
*/
typedef struct _DiversityPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} DiversityPacket;

typedef struct _XAnnotateInfo
{
  struct _XAnnotateInfo
    *previous,
    *next;

  int
    x,
    y;

  unsigned int
    width,
    height;

  double
    degrees;

  XFontStruct
    *font_info;

  char
    *text;

  AnnotationStencil
    stencil;

  char
    geometry[MaxTextExtent];
} XAnnotateInfo;

typedef struct _XDrawInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  double
    degrees;

  AnnotationStencil
    stencil;

  ElementType
    element;

  Pixmap
    stipple;

  unsigned int
    line_width;

  XSegment
    line_info;

  unsigned int
    number_coordinates;

  RectangleInfo
    rectangle_info;

  XPoint
    *coordinate_info;

  char
    geometry[MaxTextExtent];
} XDrawInfo;

typedef struct _XImportInfo
{
  unsigned int
    frame,
    borders,
    screen,
    descend,
    silent;
} XImportInfo;

typedef struct _XPixelInfo
{
  unsigned long
    colors,
    *pixels;

  XColor
    foreground_color,
    background_color,
    border_color,
    matte_color,
    highlight_color,
    shadow_color,
    depth_color,
    trough_color,
    box_color,
    pen_color,
    pen_colors[MaxNumberPens];

  GC
    annotate_context,
    highlight_context,
    widget_context;

  unsigned short
    box_index,
    pen_index;
} XPixelInfo;

typedef struct _XResourceInfo
{
  XrmDatabase
    resource_database;

  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;

  unsigned long
    colors;

  unsigned int
    close_server,
    backdrop;

  char
    *background_color,
    *border_color;

  char
    *client_name;

  XColormapType
    colormap;

  unsigned int
    border_width,
    color_recovery,
    confirm_exit,
    debug,
    delay;

  char
    *display_gamma;

  char
    *font,
    *font_name[MaxNumberFonts],
    *foreground_color;

  unsigned int
    display_warnings,
    gamma_correct;

  char
    *icon_geometry;

  unsigned int
    iconic,
    immutable;

  char
    *image_geometry;

  char
    *map_type,
    *matte_color,
    *name;

  unsigned int
    magnify,
    pause;

  char
    *pen_colors[MaxNumberPens];

  char
    *text_font,
    *title;

  int
    quantum;

  unsigned int
    update,
    use_pixmap,
    use_shared_memory;

  unsigned long
    undo_cache;

  char
    *visual_type,
    *window_group,
    *window_id,
    *write_filename;

  Image
    *copy_image;

  int
    gravity;

  char
    home_directory[MaxTextExtent];
} XResourceInfo;

typedef struct _XWindowInfo
{
  Window
    id;

  Window
    root;

  Visual
    *visual;

  int
    storage_class,
    depth;

  XVisualInfo
    *visual_info;

  XStandardColormap
    *map_info;

  XPixelInfo
    *pixel_info;

  XFontStruct
    *font_info;

  GC
    annotate_context,
    highlight_context,
    widget_context;

  Cursor
    cursor,
    busy_cursor;

  char
    *name,
    *geometry,
    *icon_name,
    *icon_geometry,
    *crop_geometry;

  unsigned long
    data,
    flags;

  int
    x,
    y;

  unsigned int
    width,
    height,
    min_width,
    min_height,
    width_inc,
    height_inc,
    border_width,
    use_pixmap,
    immutable,
    shape,
    shared_memory;

  int
    screen;

  XImage
    *ximage,
    *matte_image;

  Pixmap
    highlight_stipple,
    shadow_stipple,
    pixmap,
    *pixmaps,
    matte_pixmap,
    *matte_pixmaps;

  XSetWindowAttributes
    attributes;

  XWindowChanges
    window_changes;

#if defined(HasSharedMemory)
  XShmSegmentInfo
    segment_info[2];
#endif

  int
    mask;

  unsigned int
    orphan,
    mapped,
    stasis;

  Image
    *image;

  unsigned int
    destroy;
} XWindowInfo;

typedef struct _XWindows
{
  Display
    *display;

  XStandardColormap
    *map_info,
    *icon_map;

  XVisualInfo
    *visual_info,
    *icon_visual;

  XPixelInfo
    *pixel_info,
    *icon_pixel;

  XFontStruct
    *font_info;

  XResourceInfo
    *icon_resources;

  XClassHint
    *class_hints;

  XWMHints
    *manager_hints;

  XWindowInfo
    context,
    group_leader,
    backdrop,
    icon,
    image,
    info,
    magnify,
    pan,
    command,
    widget,
    popup;

  Atom
    wm_protocols,
    wm_delete_window,
    wm_take_focus,
    im_protocols,
    im_remote_command,
    im_update_widget,
    im_update_colormap,
    im_former_image,
    im_retain_colors,
    im_next_image,
    im_exit,
    dnd_protocols;
} XWindows;

/*
  X utilities methods.
*/
extern MagickExport char
  *XGetResourceClass(XrmDatabase,const char *,const char *,char *),
  *XGetResourceInstance(XrmDatabase,const char *,const char *,const char *),
  *XGetScreenDensity(Display *);

extern MagickExport Cursor
  XMakeCursor(Display *,Window,Colormap,char *,char *);

extern MagickExport Image
  *XAnimateImages(Display *,XResourceInfo *,char **,const int,Image *),
  *XDisplayImage(Display *,XResourceInfo *,char **,int,Image **,
    unsigned long *),
  *XImportImage(const ImageInfo *,XImportInfo *);

extern MagickExport int
  XError(Display *,XErrorEvent *);

extern MagickExport unsigned int
  IsTrue(const char *),
  XAnnotateImage(Display *,const XPixelInfo *,XAnnotateInfo *,Image *),
  XDisplayBackgroundImage(Display *,XResourceInfo *,Image *),
  XDrawImage(Display *,const XPixelInfo *,XDrawInfo *,Image *),
  XGetWindowColor(Display *,XWindows *,char *),
  XMakeImage(Display *,const XResourceInfo *,XWindowInfo *,Image *,
    unsigned int,unsigned int),
  XQueryColorDatabase(const char *,XColor *);

extern MagickExport void
  XAnimateBackgroundImage(Display *,XResourceInfo *,Image *),
  XBestIconSize(Display *,XWindowInfo *,Image *),
  XBestPixel(Display *,const Colormap,XColor *,unsigned int,XColor *),
  XCheckRefreshWindows(Display *,XWindows *),
  XClientMessage(Display *,const Window,const Atom,const Atom,const Time),
  XConfigureImageColormap(Display *,XResourceInfo *,XWindows *,Image *),
  XConstrainWindowPosition(Display *,XWindowInfo *),
  XDelay(Display *,const unsigned long),
  XDestroyWindowColors(Display *,Window),
  XDisplayImageInfo(Display *,const XResourceInfo *,XWindows *,Image *,Image *),
  XFreeResources(Display *,XVisualInfo *,XStandardColormap *,XPixelInfo *,
    XFontStruct *,XResourceInfo *,XWindowInfo *),
  XFreeStandardColormap(Display *,const XVisualInfo *,XStandardColormap *,
    XPixelInfo *),
  XGetAnnotateInfo(XAnnotateInfo *),
  XGetImportInfo(XImportInfo *),
  XGetMapInfo(const XVisualInfo *,const Colormap,XStandardColormap *),
  XGetPixelPacket(Display *,const XVisualInfo *,const XStandardColormap *,
    const XResourceInfo *,Image *,XPixelInfo *),
  XGetResourceInfo(XrmDatabase,char *,XResourceInfo *),
  XGetWindowInfo(Display *,XVisualInfo *,XStandardColormap *,XPixelInfo *,
    XFontStruct *,XResourceInfo *,XWindowInfo *),
  XHighlightEllipse(Display *,Window,GC,const RectangleInfo *),
  XHighlightLine(Display *,Window,GC,const XSegment *),
  XHighlightRectangle(Display *,Window,GC,const RectangleInfo *),
  XMakeMagnifyImage(Display *,XWindows *),
  XMakeStandardColormap(Display *,XVisualInfo *,XResourceInfo *,Image *,
    XStandardColormap *,XPixelInfo *),
  XMakeWindow(Display *,Window,char **,int,XClassHint *,XWMHints *,
    XWindowInfo *),
  XMagickMonitor(const char *,const off_t,const off_t),
  XQueryPosition(Display *,const Window,int *,int *),
  XRefreshWindow(Display *,const XWindowInfo *,const XEvent *),
  XRemoteCommand(Display *,const char *,const char *),
  XRetainWindowColors(Display *,const Window),
  XSignalHandler(int),
  XSetCursorState(Display *,XWindows *,const unsigned int),
  XUserPreferences(XResourceInfo *),
  XWarning(const ExceptionType,const char *,const char *);

extern MagickExport Window
  XWindowByID(Display *,const Window,const unsigned long),
  XWindowByName(Display *,const Window,const char *),
  XWindowByProperty(Display *,const Window,const Atom);

extern MagickExport XFontStruct
  *XBestFont(Display *,const XResourceInfo *,const unsigned int);

extern MagickExport XrmDatabase
  XGetResourceDatabase(Display *,const char *);

extern MagickExport XVisualInfo
  *XBestVisualInfo(Display *,XStandardColormap *,XResourceInfo *);

extern MagickExport XWindows
  *XInitializeWindows(Display *,XResourceInfo *),
  *XSetWindows(XWindows *);

/*
  Invoke pre-X11R6 ICCCM routines if XlibSpecificationRelease is not 6.
*/
#if XlibSpecificationRelease < 6
#if !defined(PRE_R6_ICCCM)
#define PRE_R6_ICCCM
#endif
#endif
/*
  Invoke pre-X11R5 ICCCM routines if XlibSpecificationRelease is not defined.
*/
#if !defined(XlibSpecificationRelease)
#define PRE_R5_ICCCM
#endif
/*
  Invoke pre-X11R4 ICCCM routines if PWinGravity is not defined.
*/
#if !defined(PWinGravity)
#define PRE_R4_ICCCM
#endif
#include "widget.h"

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
