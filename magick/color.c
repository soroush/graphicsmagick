/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                       CCCC   OOO   L       OOO   RRRR                       %
%                      C      O   O  L      O   O  R   R                      %
%                      C      O   O  L      O   O  RRRR                       %
%                      C      O   O  L      O   O  R R                        %
%                       CCCC   OOO   LLLLL   OOO   R  R                       %
%                                                                             %
%                                                                             %
%                  Methods to Count the Colors in an Image                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
#define ColorFilename  "colors.mgk"

/*
  Declare color map.
*/
static char
  *ColorMap =
    "<?xml version=\"1.0\"?>"
    "<colormap>"
    "  <color name=\"none\" red=\"0\" green=\"0\" blue=\"0\" opacity=\"255\" />"
    "  <color name=\"white\" red=\"255\" green=\"255\" blue=\"255\" />"
    "  <color name=\"red\" red=\"255\" green=\"0\" blue=\"0\" />"
    "  <color name=\"green\" red=\"0\" green=\"255\" blue=\"0\" />"
    "  <color name=\"gray\" red=\"190\" green=\"190\" blue=\"190\" />"
    "  <color name=\"blue\" red=\"0\" green=\"0\" blue=\"255\" />"
    "  <color name=\"cyan\" red=\"0\" green=\"255\" blue=\"255\" />"
    "  <color name=\"magenta\" red=\"255\" green=\"0\" blue=\"255\" />"
    "  <color name=\"yellow\" red=\"255\" green=\"255\" blue=\"0\" />"
    "  <color name=\"black\" red=\"0\" green=\"0\" blue=\"0\" />"
    "</colormap>";

/*
  Structures.
*/
typedef struct _ColorPacket
{
  PixelPacket
    pixel;

  unsigned short
    index;

  unsigned long
    count;
} ColorPacket;

typedef struct _NodeInfo
{
  struct _NodeInfo
    *child[8];

  ColorPacket
    *list;

  unsigned long
    number_unique;

  unsigned char
    level;
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned long
    progress,
    colors,
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_queue;
} CubeInfo;

/*
  Static declarations.
*/
static ColorInfo
  *color_list = (ColorInfo *) NULL;

static SemaphoreInfo
  *color_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int);

static unsigned int
  ReadConfigurationFile(const char *,ExceptionInfo *);

static void
  DestroyColorList(const NodeInfo *),
  Histogram(const Image *,CubeInfo *,const NodeInfo *,FILE *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n s t r a i n C o l o r m a p I n d e x                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstrainColormapIndex() validates the colormap index.  If the index does
%  not range from 0 to the number of colors in the colormap an exception
%  is issued and 0 is returned.
%
%  The format of the ConstrainColormapIndex method is:
%
%      IndexPacket ConstrainColormapIndex(Image *image,const unsigned int index)
%
%  A description of each parameter follows:
%
%    o index: Method ConstrainColormapIndex returns colormap index if it is
%      valid other an exception is issued and 0 is returned.
%
%    o image: The image.
%
%    o index: This integer is the colormap index.
%
%
*/

MagickExport IndexPacket ValidateColormapIndex(Image *image,
  const unsigned long index)
{
  return(ConstrainColormapIndex(image,index));
}

MagickExport IndexPacket ConstrainColormapIndex(Image *image,
  const unsigned long index)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (index < image->colors)
    return((IndexPacket) index);
  ThrowException(&image->exception,CorruptImageError,"invalid colormap index",
    image->filename);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o l o r I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorInfo deallocates memory associated with the color list.
%
%  The format of the DestroyColorInfo method is:
%
%      DestroyColorInfo(void)
%
%
*/
MagickExport void DestroyColorInfo(void)
{
  ColorInfo
    *color_info;

  register ColorInfo
    *p;

  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; )
  {
    color_info=p;
    p=p->next;
    if (color_info->filename != (char *) NULL)
      LiberateMemory((void **) &color_info->filename);
    if (color_info->name != (char *) NULL)
      LiberateMemory((void **) &color_info->name);
    LiberateMemory((void **) &color_info);
  }
  color_list=(ColorInfo *) NULL;
  DestroySemaphoreInfo(&color_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCubeInfo deallocates memory associated with an image.
%
%  The format of the DestroyCubeInfo method is:
%
%      DestroyCubeInfo(CubeInfo *cube_info)
%
%  A description of each parameter follows:
%
%    o cube_info: The address of a structure of type CubeInfo.
%
%
*/
static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  DestroyColorList(cube_info->root);
  do
  {
    nodes=cube_info->node_queue->next;
    LiberateMemory((void **) &cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  LiberateMemory((void **) &cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y C o l o r L i s t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorList traverses the color cube tree and frees the list of
%  unique colors.
%
%  The format of the DestroyColorList method is:
%
%      void DestroyColorList(const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyColorList(const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyColorList(node_info->child[id]);
  if (node_info->list != (ColorPacket *) NULL)
    LiberateMemory((void **) &node_info->list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   F u z z y C o l o r M a t c h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FuzzyColorMatch() returns true if two pixels are identical in color.
%
%  The format of the ColorMatch method is:
%
%      void ColorMatch(const PixelPacket *p,const PixelPacket *q,
%        const double fuzz)
%
%  A description of each parameter follows:
%
%    o p: Pixel p.
%
%    o q: Pixel q.
%
%    o distance:  Define how much tolerance is acceptable to consider
%      two colors as the same.
%
%
*/
MagickExport unsigned int FuzzyColorMatch(const PixelPacket *p,
  const PixelPacket *q,const double fuzz)
{
  register double
    blue,
    distance,
    green,
    red;

  if ((fuzz == 0.0) && (p->red == q->red) && (p->green == q->green) &&
      (p->blue == q->blue))
    return(True);
  red=p->red-(double) q->red;
  distance=red*red;
  if (distance > (fuzz*fuzz))
    return(False);
  green=p->green-(double) q->green;
  distance+=green*green;
  if (distance > (fuzz*fuzz))
    return(False);
  blue=p->blue-(double) q->blue;
  distance+=blue*blue;
  if (distance > (fuzz*fuzz))
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorInfo searches the color list for the specified name and if
%  found returns attributes for that color.
%
%  The format of the GetColorInfo method is:
%
%      const PixelPacket *GetColorInfo(const char *name,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o color_info: Method GetColorInfo searches the color list for the
%      specified name and if found returns attributes for that color.
%
%    o name: The color name.
%
%    o compliance: Define the required color standard.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const ColorInfo *GetColorInfo(const char *name,
  ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  register char
    *q;

  register ColorInfo
    *p;

  AcquireSemaphoreInfo(&color_semaphore);
  if (color_list == (ColorInfo *) NULL)
    (void) ReadConfigurationFile(ColorFilename,exception);
  LiberateSemaphoreInfo(&color_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return(color_list);
  /*
    Search for named color.
  */
  FormatString(colorname,"%.1024s",name);
  for (q=colorname; *q != '\0'; q++)
  {
    if (*q != ' ')
      continue;
    (void) strcpy(q,q+1);
    q--;
  }
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (LocaleCompare(colorname,p->name) == 0)
      break;
  if (p == (ColorInfo *) NULL)
    ThrowException(exception,OptionWarning,"Unrecognized color",name);
  else
    if (p != color_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (ColorInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (ColorInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(ColorInfo *) NULL;
        p->next=color_list;
        color_list->previous=p;
        color_list=p;
      }
  LiberateSemaphoreInfo(&color_semaphore);
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r l i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern
%  and color standard.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,int number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern and color standard.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/
MagickExport char **GetColorList(const char *pattern,int *number_colors)
{
  char
    **colorlist;

  register const ColorInfo
    *p;

  ExceptionInfo
    exception;

  register long
    i;

  /*
    Allocate color list.
  */
  assert(pattern != (char *) NULL);
  assert(number_colors != (int *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const ColorInfo *) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    i++;
  colorlist=(char **) AcquireMemory(i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AllocateString(p->name);
  *number_colors=i;
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r T u p l e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorTuple() returns a color as a color tuple string.
%
%  The format of the GetColorTuple method is:
%
%      GetColorTuple(const PixelPacket *color,const unsigned int depth,
%        const unsigned int matte,const unsigned int hex,char *tuple)
%
%  A description of each parameter follows.
%
%    o color: The color.
%
%    o depth: The color depth.
%
%    o matte: A value other than zero returns the opacity in the tuple.
%
%    o hex: A value other than zero returns the tuple in a hexidecimal format.
%
%    o tuple: Return the color tuple as this string.
%
%
*/
MagickExport void GetColorTuple(const PixelPacket *color,
  const unsigned int depth,const unsigned int matte,const unsigned int hex,
  char *tuple)
{
  assert(color != (const PixelPacket *) NULL);
  assert(tuple != (char *) NULL);
  if (matte)
    {
      if (depth <= 8)
        {
          FormatString(tuple,hex ? "#%02X%02X%02X%02X" : "(%3u,%3u,%3u,%3u)",
            ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
            ScaleQuantumToChar(color->blue),ScaleQuantumToChar(color->opacity));
          return;
        }
      if (depth <= 16)
        {
          FormatString(tuple,hex ? "#%04X%04X%04X%04X" : "(%5u,%5u,%5u,%5u)",
            ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
            ScaleQuantumToShort(color->blue),
            ScaleQuantumToShort(color->opacity));
          return;
        }
      FormatString(tuple,
        hex ? "#%08lX%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu,%10lu)",
        ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
        ScaleQuantumToLong(color->blue),ScaleQuantumToLong(color->opacity));
      return;
    }
  if (depth <= 8)
    {
      FormatString(tuple,hex ? "#%02X%02X%02X" : "(%3u,%3u,%3u)",
        ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
        ScaleQuantumToChar(color->blue));
      return;
    }
  if (depth <= 16)
    {
      FormatString(tuple,hex ? "#%04X%04X%04X" : "(%5u,%5u,%5u)",
        ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
        ScaleQuantumToShort(color->blue));
      return;
    }
  FormatString(tuple,hex ? "#%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu)",
    ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
    ScaleQuantumToLong(color->blue));
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C u b e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCubeInfo initialize the CubeInfo data structure.
%
%  The format of the GetCubeInfo method is:
%
%      cube_info=GetCubeInfo()
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%
*/
static CubeInfo *GetCubeInfo(void)
{
  CubeInfo
    *cube_info;

  /*
    Initialize tree to describe color cube.
  */
  cube_info=(CubeInfo *) AcquireMemory(sizeof(CubeInfo));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(CubeInfo));
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  return(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t N o d e I n f o                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNodeInfo allocates memory for a new node in the color cube tree
%  and presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      node_info=GetNodeInfo(cube_info,level)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int level)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AcquireMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->node_info=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->free_nodes--;
  node_info=cube_info->node_info++;
  (void) memset(node_info,0,sizeof(NodeInfo));
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t N u m b e r C o l o r s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberColors returns the number of unique colors in an image.
%
%  The format of the GetNumberColors method is:
%
%      unsigned long GetNumberColors(const Image *image,FILE *file,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o file:  Write a histogram of the color distribution to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned long GetNumberColors(const Image *image,FILE *file,
  ExceptionInfo *exception)
{
#define ComputeImageColorsText  "  Compute image colors...  "

  CubeInfo
    *cube_info;

  long
    y;

  NodeInfo
    *node_info;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned int
    id,
    index,
    level;

  unsigned long
    number_colors;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitError,
        "unable to determine the number of image colors",
        "Memory allocation failed");
      return(0);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level <= MaxTreeDepth; level++)
      {
        id=(unsigned int)
          (((ScaleQuantumToChar(p->red) >> index) & 0x01) << 2 |
           ((ScaleQuantumToChar(p->green) >> index) & 0x01) << 1 |
           ((ScaleQuantumToChar(p->blue) >> index) & 0x01));
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitError,
                  "unable to determine the number of image colors",
                  "memory allocation failed");
                return(0);
              }
          }
        node_info=node_info->child[id];
        index--;
        if (level != MaxTreeDepth)
          continue;
        for (i=0; i < (long) node_info->number_unique; i++)
           if (ColorMatch(p,&node_info->list[i].pixel))
             break;
        if (i < (long) node_info->number_unique)
          {
            node_info->list[i].count++;
            continue;
          }
        if (node_info->number_unique == 0)
          node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
        else
          ReacquireMemory((void **) &node_info->list,
            (i+1)*sizeof(ColorPacket));
        if (node_info->list == (ColorPacket *) NULL)
          {
            ThrowException(exception,ResourceLimitError,
              "unable to determine the number of image colors",
              "memory allocation failed");
            return(0);
          }
        node_info->list[i].pixel=(*p);
        node_info->list[i].count=1;
        node_info->number_unique++;
        cube_info->colors++;
      }
      p++;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(ComputeImageColorsText,y,image->rows);
  }
  if (file != (FILE *) NULL)
    {
      (void) fprintf(file,"\n");
      Histogram(image,cube_info,cube_info->root,file,exception);
      (void) fflush(file);
    }
  number_colors=cube_info->colors;
  DestroyCubeInfo(cube_info);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  H i s t o g r a m                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Histogram traverses the color cube tree and produces a list of
%  unique pixel field values and the number of times each occurs in the image.
%
%  The format of the Histogram method is:
%
%      void Histogram(const Image *image,CubeInfo *cube_info,
%        const NodeInfo *node_info,FILE *file,ExceptionInfo *exception
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Histogram(const Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info,FILE *file,ExceptionInfo *exception)
{
#define HistogramImageText  "  Compute image histogram...  "

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      Histogram(image,cube_info,node_info->child[id],file,exception);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent],
        tuple[MaxTextExtent];

      register ColorPacket
        *p;

      register long
        i;

      p=node_info->list;
      for (i=0; i < (long) node_info->number_unique; i++)
      {
        GetColorTuple(&p->pixel,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"%10lu: %.1024s  ",p->count,tuple);
        (void) fprintf(file,"  ");
        (void) QueryColorname(image,&p->pixel,SVGCompliance,name,exception);
        (void) fprintf(file,"%.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
      if (QuantumTick(cube_info->progress,cube_info->colors))
        MagickMonitor(HistogramImageText,cube_info->progress,
          cube_info->colors);
      cube_info->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsGrayImage() returns True if all the pixels in the image have the same
%  red, green, and blue intensities.
%
%  The format of the IsGrayImage method is:
%
%      unsigned int IsGrayImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsGrayImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=0; x < (long) image->columns; x++)
        {
          if (!IsGray(*p))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < (long) image->colors; i++)
        if (!IsGray(image->colormap[i]))
          return(False);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsMonochromeImage() returns True if all the pixels in the image have
%  the same red, green, and blue intensities and the intensity is either
%  0 or MaxRGB.
%
%  The format of the IsMonochromeImage method is:
%
%      unsigned int IsMonochromeImage(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsMonochromeImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=0; x < (long) image->columns; x++)
        {
          if (!IsGray(*p))
            return(False);
          if ((p->red != 0) && (p->red != MaxRGB))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < (long) image->colors; i++)
      {
        if (!IsGray(image->colormap[i]))
          return(False);
        if ((image->colormap[i].red != 0) && (image->colormap[i].red != MaxRGB))
          return(False);
      }
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s O p a q u e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsOpaqueImage() returns True if none of the pixels in the image have an
%  opacity value other than opaque (0).
%
%  The format of the IsOpaqueImage method is:
%
%      unsigned int IsOpaqueImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsOpaqueImage returns False if the image has one or more
%      pixels that are transparent otherwise True is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsOpaqueImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(True);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      if (p->opacity != OpaqueOpacity)
        return(False);
      p++;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P a l e t t e I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPaletteImage returns True if the image is PseudoClass and has 256
%  unique colors or less.
%
%  The format of the IsPaletteImage method is:
%
%      unsigned int IsPaletteImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPaletteImage returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsPaletteImage(const Image *image,
  ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register NodeInfo
    *node_info;

  register long
    i;

  unsigned int
    id,
    index,
    level;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class == PseudoClass) && (image->colors <= 256))
    return(True);
  if (image->storage_class == PseudoClass)
    return(False);
  /*
    Initialize color description tree.
  */
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitError,
        "Unable to determine image class","Memory allocation failed");
      return(False);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        id=(unsigned int)
          (((ScaleQuantumToChar(p->red) >> index) & 0x01) << 2 |
           ((ScaleQuantumToChar(p->green) >> index) & 0x01) << 1 |
           ((ScaleQuantumToChar(p->blue) >> index) & 0x01));
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitError,
                  "Unable to determine image class","Memory allocation failed");
                return(False);
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (long) node_info->number_unique; i++)
        if (ColorMatch(p,&node_info->list[i].pixel))
          break;
      if (i == (long) node_info->number_unique)
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
          else
            ReacquireMemory((void **) &node_info->list,
              (i+1)*sizeof(ColorPacket));
          if (node_info->list == (ColorPacket *) NULL)
            {
              ThrowException(exception,ResourceLimitError,
                "Unable to determine image class","Memory allocation failed");
              return(False);
            }
          node_info->list[i].pixel=(*p);
          node_info->list[i].index=(unsigned short) cube_info->colors++;
          node_info->number_unique++;
          if (cube_info->colors > 256)
            {
              DestroyCubeInfo(cube_info);
              return(False);
            }
        }
      p++;
    }
  }
  DestroyCubeInfo(cube_info);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t C o l o r I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListColorInfo() lists color names to the specified file.  Color names
%  are a convenience.  Rather than defining a color by its red, green, and
%  blue intensities just use a color name such as white, blue, or yellow.
%
%  The format of the ListColorInfo method is:
%
%      unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  List color names to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
{
  register const ColorInfo
    *p;

  register long
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetColorInfo("*",exception);
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if ((p->previous == (ColorInfo *) NULL) ||
        (LocaleCompare(p->filename,p->previous->filename) != 0))
      {
        if (p->previous != (ColorInfo *) NULL)
          (void) fprintf(file,"\n");
        (void) fprintf(file,"Filename: %.1024s\n\n",p->filename);
        (void) fprintf(file,
          "Name                   Color                   Compliance\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 22; i++)
      (void) fprintf(file," ");
    if (p->color.opacity == OpaqueOpacity)
      (void) fprintf(file,"%5d,%5d,%5d       ",p->color.red,p->color.green,
        p->color.blue);
    else
      (void) fprintf(file,"%5d,%5d,%5d,%5d ",p->color.red,p->color.green,
        p->color.blue,p->color.opacity);
    if (p->compliance & SVGCompliance)
      (void) fprintf(file,"SVG ");
    if (p->compliance & X11Compliance)
      (void) fprintf(file,"X11 ");
    if (p->compliance & XPMCompliance)
      (void) fprintf(file,"XPM ");
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&color_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorDatabase() returns the red, green, blue, and opacity intensities
%  for a given color name.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *name,PixelPacket *color,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The color name (e.g. white, blue, yellow).
%
%    o color: The red, green, blue, and opacity intensities values of the
%      named color in this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorDatabase(const char *name,
  PixelPacket *color,ExceptionInfo *exception)
{
  double
    blue,
    green,
    opacity,
    red,
    scale;

  register const ColorInfo
    *p;

  register long
    i;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  (void) memset(color,0,sizeof(PixelPacket));
  color->opacity=TransparentOpacity;
  if ((name == (char *) NULL) || (*name == '\0'))
    name=BackgroundColor;
  while (isspace((int) (*name)))
    name++;
  if (*name == '#')
    {
      char
        c;

      long
        n;

      LongPixelPacket
        pixel;

      memset(&pixel,0,sizeof(LongPixelPacket));
      name++;
      for (n=0; isxdigit((int) name[n]); n++);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            pixel.red=pixel.green;
            pixel.green=pixel.blue;
            pixel.blue=0;
            for (i=n-1; i >= 0; i--)
            {
              c=(*name++);
              pixel.blue<<=4;
              if ((c >= '0') && (c <= '9'))
                pixel.blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  pixel.blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    pixel.blue|=c-('a'-10);
                  else
                    return(False);
            }
          } while (isxdigit((int) *name));
        }
      else
        if ((n != 4) && (n != 8) && (n != 16))
          return(False);
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              pixel.red=pixel.green;
              pixel.green=pixel.blue;
              pixel.blue=pixel.opacity;
              opacity=0;
              for (i=n-1; i >= 0; i--)
              {
                c=(*name++);
                pixel.opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  pixel.opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    pixel.opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      pixel.opacity|=c-('a'-10);
                    else
                      return(False);
              }
            } while (isxdigit((int) *name));
          }
      n<<=2;
      color->red=(Quantum) ((MaxRGB*pixel.red)/((1 << n)-1));
      color->green=(Quantum) ((MaxRGB*pixel.green)/((1 << n)-1));
      color->blue=(Quantum) ((MaxRGB*pixel.blue)/((1 << n)-1));
      color->opacity=OpaqueOpacity;
      if ((n != 3) && (n != 6) && (n != 9) && (n != 12))
        color->opacity=(Quantum) ((MaxRGB*pixel.opacity)/((1 << n)-1));
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf",&red,&green,&blue);
      color->red=ScaleCharToQuantum(scale*red);
      color->green=ScaleCharToQuantum(scale*green);
      color->blue=ScaleCharToQuantum(scale*blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  if (LocaleNCompare(name,"rgba(",5) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf",&red,&green,
        &blue,&opacity);
      color->red=ScaleCharToQuantum(scale*red);
      color->green=ScaleCharToQuantum(scale*green);
      color->blue=ScaleCharToQuantum(scale*blue);
      color->opacity=ScaleCharToQuantum(scale*opacity);
      return(True);
    }
  p=GetColorInfo(name,exception);
  if (p == (const ColorInfo *) NULL)
    return(False);
  if ((LocaleCompare(p->name,"opaque") == 0) ||
      (LocaleCompare(p->name,"transparent") == 0))
    {
      color->opacity=p->color.opacity;
      return(True);
    }
  *color=p->color;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorname() returns a named color for the given color intensity.  If
%  an exact match is not found, a hex value is return instead.  For example
%  an intensity of rgb:(0,0,0) returns black whereas rgb:(223,223,223)
%  returns #dfdfdf.
%
%  The format of the QueryColorname method is:
%
%      unsigned int QueryColorname(const Image *image,const PixelPacket *color,
%        const ComplianceType compliance,char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o color: The color intensities.
%
%    o Compliance: Adhere to this color standard: SVG, X11, or XPM.
%
%    o name: Return the color name or hex value.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorname(const Image *image,
  const PixelPacket *color,const ComplianceType compliance,char *name,
  ExceptionInfo *exception)
{
  register const ColorInfo
    *p;

  *name='\0';
  p=GetColorInfo("*",exception);
  if (p != (const ColorInfo *) NULL)
    {
      for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
      {
        if (!(p->compliance & compliance))
          continue;
        if ((p->color.red != color->red) || (p->color.green != color->green) ||
            (p->color.blue != color->blue) ||
            (p->color.opacity != color->opacity))
          continue;
        (void) strncpy(name,p->name,MaxTextExtent-1);
        return(True);
      }
    }
  GetColorTuple(color,image->depth,image->matte,True,name);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r a t i o n F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *basename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigurationFile(const char *basename,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    keyword[MaxTextExtent],
    *path,
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the color configuration file.
  */
  FormatString(filename,"%.1024s",basename);
  path=GetMagickConfigurePath(basename,False,exception);
  if (path != (char *) NULL)
    {
      FormatString(filename,"%.1024s",path);
      LiberateMemory((void **) &path);
    }
  xml=(char *) FileToBlob(filename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(ColorMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    FormatString(keyword,"%.1024s",token);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              (void) ReadConfigurationFile(token,exception);
              while (color_list->next != (ColorInfo *) NULL)
                color_list=color_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<color") == 0)
      {
        ColorInfo
          *color_info;

        /*
          Allocate memory for the color list.
        */
        color_info=(ColorInfo *) AcquireMemory(sizeof(ColorInfo));
        if (color_info == (ColorInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"Unable to allocate colors",
            "Memory allocation failed");
        (void) memset(color_info,0,sizeof(ColorInfo));
        color_info->filename=AcquireString(filename);
        color_info->signature=MagickSignature;
        if (color_list == (ColorInfo *) NULL)
          {
            color_list=color_info;
            continue;
          }
        color_list->next=color_info;
        color_info->previous=color_list;
        color_list=color_list->next;
        continue;
      }
    if (color_list == (ColorInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        if (LocaleCompare((char *) keyword,"blue") == 0)
          {
            color_list->color.blue=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"compliance") == 0)
          {
            long
              compliance;

            compliance=color_list->compliance;
            if (GlobExpression(token,"*SVG*"))
              compliance|=SVGCompliance;
            if (GlobExpression(token,"*X11*"))
              compliance|=X11Compliance;
            if (GlobExpression(token,"*XPM*"))
              compliance|=XPMCompliance;
            color_list->compliance=(ComplianceType) compliance;
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"green") == 0)
          {
            color_list->color.green=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            color_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"opacity") == 0)
          {
            color_list->color.opacity=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'R':
      case 'r':
      {
        if (LocaleCompare((char *) keyword,"red") == 0)
          {
            color_list->color.red=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            color_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  LiberateMemory((void **) &xml);
  if (color_list == (ColorInfo *) NULL)
    return(False);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(True);
}
