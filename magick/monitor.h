/*
  ImageMagick Progress Monitor Methods.
*/
#ifndef _MONITOR_H
#define _MONITOR_H

/*
  Monitor typedef declarations.
*/
typedef void
  (*MonitorHandler)(const char *,const off_t,const off_t);

/*
  Monitor declarations.
*/
extern MagickExport MonitorHandler
  SetMonitorHandler(MonitorHandler);

extern MagickExport void
  MagickMonitor(const char *,const off_t,const off_t);

#endif
