/*
  ImageMagick Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if !defined(PixelCacheThreshold)
#define PixelCacheThreshold  2047  /* megabytes */
#endif

/*
  Typedef declaractions.
*/
typedef void* Cache;

typedef struct _NexusInfo
{
  unsigned int
    available,
    columns,
    rows;

  int
    x,
    y;

  off_t
    length;

  void
    *line;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;
} NexusInfo;

typedef struct _CacheInfo
{
  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CacheType
    type;

  unsigned int
    columns,
    rows;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;

  char
    cache_filename[MaxTextExtent],
    meta_filename[MaxTextExtent];

  int
    file;

  unsigned int
    persist;

  NexusInfo
    *nexus;

  unsigned long
    signature;
} CacheInfo;

/*
  MagickExported interfaces.
*/
extern MagickExport ClassType
  GetCacheClass(const Cache);

extern MagickExport ColorspaceType
  GetCacheColorspace(const Cache);

extern MagickExport IndexPacket
  *GetNexusIndexes(const Cache,const unsigned int);

extern MagickExport PixelPacket
  *GetNexusPixels(const Cache,const unsigned int),
  *SetCacheNexus(Cache,const unsigned int,const RectangleInfo *);

extern MagickExport unsigned int
  GetCacheNexus(Cache),
  IsNexusInCore(const Cache,const unsigned int),
  OpenCache(Image *),
  ReadCachePixels(Cache,const unsigned int),
  ReadCacheIndexes(Cache,const unsigned int),
  WriteCacheInfo(Image *),
  WriteCachePixels(Cache,const unsigned int),
  WriteCacheIndexes(Cache,const unsigned int);

extern MagickExport void
  AllocateCacheNexus(CacheInfo *),
  DestroyCache(void),
  DestroyCacheNexus(Cache,const unsigned int),
  GetCacheInfo(Cache *),
  SetCacheThreshold(const off_t),
  SetPixelCacheMethods(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
