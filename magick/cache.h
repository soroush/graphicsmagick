/*
  ImageMagick Cache Methods.
*/
#ifndef _MAGICK_CACHE_H
#define _MAGICK_CACHE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Typedef declaractions.
*/
typedef IndexPacket
  *(*GetIndexesFromHandler)(const Image *);

typedef struct _NexusInfo
{
  long
    x,
    y;

  off_t
    length;

  PixelPacket
    *staging,
    *pixels;

  IndexPacket
    *indexes;

  unsigned long
    columns,
    rows;

  unsigned int
    available;
} NexusInfo;

typedef struct _CacheInfo
{
  unsigned long
    id;

  NexusInfo
    *nexus_info;

  ClassType
    storage_class;

  ColorspaceType
    colorspace;

  CacheType
    type;

  unsigned long
    columns,
    rows;

  off_t
    offset,
    length;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;

  VirtualPixelType
    virtual_type;

  PixelPacket
    virtual_pixel;

  char
    cache_filename[MaxTextExtent];

  long
    reference_count;

  SemaphoreInfo
    *semaphore;

  unsigned long
    signature;
} CacheInfo;

typedef const PixelPacket
  *(*AcquirePixelHandler)(const Image *,const long,const long,
    const unsigned long,const unsigned long,ExceptionInfo *);

typedef PixelPacket
  (*AcquireOnePixelFromHandler)(const Image *,const long,const long,
    ExceptionInfo *);

typedef PixelPacket
  (*GetOnePixelFromHandler)(Image *,const long,const long);

typedef PixelPacket
  *(*GetPixelHandler)(Image *,const long,const long,const unsigned long,
    const unsigned long);

typedef PixelPacket
  *(*GetPixelsFromHandler)(const Image *);

typedef PixelPacket
  *(*SetPixelHandler)(Image *,const long,const long,const unsigned long,
    const unsigned long);

typedef unsigned int
  (*SyncPixelHandler)(Image *);

typedef void
  *Cache,
  (*DestroyPixelHandler)(Image *);

/*
  MagickExported interfaces.
*/
extern MagickExport Cache
  ReferenceCache(Cache);

extern MagickExport ClassType
  GetCacheClass(const Cache);

extern MagickExport ColorspaceType
  GetCacheColorspace(const Cache);

extern MagickExport const PixelPacket
  *AcquireCacheNexus(const Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long,ExceptionInfo *);

extern MagickExport IndexPacket
  *GetNexusIndexes(const Cache,const unsigned long);

extern MagickExport PixelPacket
  *GetCacheNexus(Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long),
  *GetNexusPixels(const Cache,const unsigned long),
  *SetCacheNexus(Image *,const long,const long,const unsigned long,
    const unsigned long,const unsigned long);

extern MagickExport unsigned int
  OpenCache(Image *,const MapMode),
  PersistCache(Image *,const char *,const unsigned int,off_t *,ExceptionInfo *),
  SyncCacheNexus(Image *,const unsigned long);

extern MagickExport unsigned long
  GetNexus(Cache);

extern MagickExport void
  AllocateCacheNexus(CacheInfo *),
  DestroyCache(void),
  DestroyCacheInfo(Cache),
  DestroyCacheNexus(Cache,const unsigned long),
  GetCacheInfo(Cache *),
  ResetPixelCacheMethods(void),
  SetCacheThreshold(const off_t),
  SetPixelCacheMethods(AcquirePixelHandler,GetPixelHandler,SetPixelHandler,
    SyncPixelHandler,GetPixelsFromHandler,GetIndexesFromHandler,
    AcquireOnePixelFromHandler,GetOnePixelFromHandler,DestroyPixelHandler);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
