/*
 * nanohttp.c: minimalist HTTP implementation to fetch external subsets.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */
 
#ifndef __NANO_HTTP_H__
#define __NANO_HTTP_H__

#if defined(WIN32) && defined(_MSC_VER)
#include <libxml/xmlwin32version.h>
#else
#include <libxml/xmlversion.h>
#endif
#ifdef LIBXML_HTTP_ENABLED

#ifdef __cplusplus
extern "C" {
#endif
void	xmlNanoHTTPInit		(void);
LIBXML_DLL_IMPORT void	xmlNanoHTTPCleanup	(void);
void	xmlNanoHTTPScanProxy	(const char *URL);
int	xmlNanoHTTPFetch	(const char *URL,
				 const char *filename,
				 char **contentType);
void *	xmlNanoHTTPMethod	(const char *URL,
				 const char *method,
				 const char *input,
				 char **contentType,
				 const char *headers,
				 int   ilen);
void *	xmlNanoHTTPMethodRedir	(const char *URL,
				 const char *method,
				 const char *input,
				 char **contentType,
				 char **redir,
				 const char *headers,
				 int   ilen);
LIBXML_DLL_IMPORT void *	xmlNanoHTTPOpen		(const char *URL,
				 char **contentType);
void *	xmlNanoHTTPOpenRedir	(const char *URL,
				 char **contentType,
				 char **redir);
int	xmlNanoHTTPReturnCode	(void *ctx);
const char * xmlNanoHTTPAuthHeader(void *ctx);
LIBXML_DLL_IMPORT int	xmlNanoHTTPRead		(void *ctx,
				 void *dest,
				 int len);
int	xmlNanoHTTPSave		(void *ctxt,
				 const char *filename);
LIBXML_DLL_IMPORT void	xmlNanoHTTPClose	(void *ctx);
#ifdef __cplusplus
}

#endif /* LIBXML_HTTP_ENABLED */
#endif
#endif /* __NANO_HTTP_H__ */
