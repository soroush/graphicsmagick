/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  V   V   GGGG                              %
%                            SS     V   V  G                                  %
%                             SSS   V   V  G GG                               %
%                               SS   V V   G   G                              %
%                            SSSSS    V     GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                              Bill Radcliffe                                 %
%                                March 2000                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "magick.h"
#include "defines.h"
#if defined(HasXML)
#ifdef WIN32
#include <win32config.h>
#endif
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#if defined(HAVE_LIBXML_XML_ERROR_H)
#include <libxml/xml-error.h>
#elif defined(HAVE_LIBXML_XMLERROR_H)
#include <libxml/xmlerror.h>
#endif
#endif

#if defined(HasAUTOTRACE)
#include "types.h"
#include "image-header.h"
#include "fit.h"
#include "output.h"
#include "pxl-outline.h"
#include "atquantize.h"
#include "thin-image.h"

char
  *version_string = "AutoTrace version 0.24a";
#endif

/*
  Typedef declaractions.
*/
#if defined(HasXML)
typedef struct _ElementInfo
{
  double
    cx,
    cy,
    major,
    minor,
    angle;
} ElementInfo;

typedef struct _SVGInfo
{
  FILE
    *file;

  unsigned int
    verbose;

  ExceptionInfo
    *exception;

  AffineMatrix
    affine;

  int
    width,
    height;

  char
    *size,
    *page,
    *title,
    *description,
    *comment;

  int
    n;

  double
    *scale,
    pointsize;

  ElementInfo
    element;

  SegmentInfo
    segment;

  BoundingBox
    bounds,
    view_box;

  PointInfo
    radius;

  char
    *text,
    *vertices,
    *url,
    *entities;

  xmlParserCtxtPtr
    parser;

  xmlDocPtr
    document;
} SVGInfo;
#endif

/*
  Forward declarations.
*/
static unsigned int
  WriteSVGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S V G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSVG returns True if the image format type, identified by the
%  magick string, is SVG.
%
%  The format of the IsSVG method is:
%
%      unsigned int IsSVG(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSVG returns True if the image format type is SVG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSVG(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"<?xml",5) == 0)
    return(True);
  return(False);
}

#if defined(HasXML)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S V G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSVGImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSVGImage method is:
%
%      Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSVGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static double GetUserSpaceCoordinateValue(const SVGInfo *svg_info,
  const char *string)
{
  char
    *p;

  double
    value;

  register int
    n;

  assert(string != (const char *) NULL);
  n=svg_info->n;
  p=(char *) string;
  value=strtod(p,&p);
  if (*p == '%')
    {
      double
        scale;

      scale=ExpandAffine(&svg_info->affine);
      return(value*svg_info->width/svg_info->scale[n]/scale/100.0);
    }
  if (LocaleNCompare(p,"cm",2) == 0)
    return(72.0/2.54*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"em",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"ex",2) == 0)
    return(svg_info->pointsize*value/svg_info->scale[n]/2.0);
  if (LocaleNCompare(p,"in",2) == 0)
    return(72.0*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"mm",2) == 0)
    return(72.0/25.4*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"pc",2) == 0)
    return(72.0/6.0*value/svg_info->scale[n]);
  if (LocaleNCompare(p,"pt",2) == 0)
    return(value/svg_info->scale[n]);
  if (LocaleNCompare(p,"px",2) == 0)
    return(value/svg_info->scale[n]);
  return(value);
}

static char **GetStyleTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; p++)
  {
    if (*p == ':')
      (*number_tokens)+=2;
  }
  tokens=(char **) AcquireMemory((*number_tokens+2)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
  {
    if ((*q != ':') && (*q != ';') && (*q != '\0'))
      continue;
    tokens[i]=AllocateString(p);
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    Strip(tokens[i++]);
    p=q+1;
  }
  tokens[i]=AllocateString(p);
  (void) strncpy(tokens[i],p,q-p);
  tokens[i][q-p]='\0';
  Strip(tokens[i++]);
  tokens[i]=(char *) NULL;
  return(tokens);
}

static char **GetTransformTokens(const char *text,int *number_tokens)
{
  char
    **tokens;

  register const char
    *p,
    *q;

  register int
    i;

  *number_tokens=0;
  if (text == (const char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=text; *p != '\0'; p++)
  {
    if (*p == '(')
      (*number_tokens)+=2;
  }
  tokens=(char **) AcquireMemory((*number_tokens+2)*sizeof(char *));
  if (tokens == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to convert string to tokens",
      "Memory allocation failed");
  /*
    Convert string to an ASCII list.
  */
  i=0;
  p=text;
  for (q=p; *q != '\0'; q++)
  {
    if ((*q != '(') && (*q != ')') && (*q != '\0'))
      continue;
    tokens[i]=AllocateString(p);
    (void) strncpy(tokens[i],p,q-p);
    tokens[i][q-p]='\0';
    Strip(tokens[i++]);
    p=q+1;
  }
  tokens[i]=AllocateString(p);
  (void) strncpy(tokens[i],p,q-p);
  tokens[i][q-p]='\0';
  Strip(tokens[i++]);
  tokens[i]=(char *) NULL;
  return(tokens);
}

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int SVGIsStandalone(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Is this document tagged standalone?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.SVGIsStandalone()\n");
  return(svg_info->document->standalone == 1);
}

static int SVGHasInternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.SVGHasInternalSubset()\n");
  return(svg_info->document->intSubset != NULL);
}

static int SVGHasExternalSubset(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an external subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.SVGHasExternalSubset()\n");
  return(svg_info->document->extSubset != NULL);
}

static void SVGInternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  /*
    Does this document has an internal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.internalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s",system_id);
      (void) fprintf(stdout,"  \n");
    }
  (void) xmlCreateIntSubset(svg_info->document,name,external_id,system_id);
}

static xmlParserInputPtr SVGResolveEntity(void *context,
  const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserInputPtr
    stream;

  /*
    Special entity resolver, better left to the parser, it has more
    context than the application layer.  The default behaviour is to
    not resolve the entities, in that case the ENTITY_REF nodes are
    built in the structure (and the parameter values).
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.resolveEntity(");
      if (public_id != NULL)
        (void) fprintf(stdout,"  %s",(char *) public_id);
      else
        (void) fprintf(stdout,"   ");
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s)\n",(char *) system_id);
      else
        (void) fprintf(stdout,"  , )\n");
    }
  stream=xmlLoadExternalEntity((const char *) system_id,(const char *)
    public_id,svg_info->parser);
  return(stream);
}

static xmlEntityPtr SVGGetEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get an entity by name.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.SVGGetEntity(%s)\n",name);
  return(xmlGetDocEntity(svg_info->document,name));
}

static xmlEntityPtr SVGGetParameterEntity(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Get a parameter entity by name.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.getParameterEntity(%s)\n",name);
  return(xmlGetParameterEntity(svg_info->document,name));
}

static void SVGEntityDeclaration(void *context,const xmlChar *name,int type,
  const xmlChar *public_id,const xmlChar *system_id,xmlChar *content)
{
  SVGInfo
    *svg_info;

  /*
    An entity definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.entityDecl(%s, %d, %s, %s, %s)\n",name,type,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",content);
  if (svg_info->parser->inSubset == 1)
    (void) xmlAddDocEntity(svg_info->document,name,type,public_id,system_id,
      content);
  else
    if (svg_info->parser->inSubset == 2)
      (void) xmlAddDtdEntity(svg_info->document,name,type,public_id,system_id,
        content);
}

void SVGAttributeDeclaration(void *context,const xmlChar *element,
  const xmlChar *name,int type,int value,const xmlChar *default_value,
  xmlEnumerationPtr tree)
{
  SVGInfo
    *svg_info;

  xmlChar
    *fullname,
    *prefix;

  xmlParserCtxtPtr
    parser;

  /*
    An attribute definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
      element,name,type,value,default_value);
  fullname=(xmlChar *) NULL;
  prefix=(xmlChar *) NULL;
  parser=svg_info->parser;
  fullname=(xmlChar *) xmlSplitQName(parser,name,&prefix);
  if (parser->inSubset == 1)
    (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->intSubset,
      element,fullname,prefix,(xmlAttributeType) type,
      (xmlAttributeDefault) value,default_value,tree);
  else
    if (parser->inSubset == 2)
      (void) xmlAddAttributeDecl(&parser->vctxt,svg_info->document->extSubset,
        element,fullname,prefix,(xmlAttributeType) type,
        (xmlAttributeDefault) value,default_value,tree);
  if (prefix != (xmlChar *) NULL)
    xmlFree(prefix);
  if (fullname != (xmlChar *) NULL)
    xmlFree(fullname);
}

static void SVGElementDeclaration(void *context,const xmlChar *name,int type,
  xmlElementContentPtr content)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    An element definition has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.elementDecl(%s, %d, ...)\n",name,type);
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->intSubset,
      name,(xmlElementTypeVal) type,content);
  else
    if (parser->inSubset == 2)
      (void) xmlAddElementDecl(&parser->vctxt,svg_info->document->extSubset,
        name,(xmlElementTypeVal) type,content);
}

static void SVGNotationDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    What to do when a notation declaration has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.notationDecl(%s, %s, %s)\n",(char *) name,
      public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none");
  parser=svg_info->parser;
  if (parser->inSubset == 1)
    (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
      name,public_id,system_id);
  else
    if (parser->inSubset == 2)
      (void) xmlAddNotationDecl(&parser->vctxt,svg_info->document->intSubset,
        name,public_id,system_id);
}

static void SVGUnparsedEntityDeclaration(void *context,const xmlChar *name,
  const xmlChar *public_id,const xmlChar *system_id,const xmlChar *notation)
{
  SVGInfo
    *svg_info;

  /*
    What to do when an unparsed entity declaration is parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.unparsedEntityDecl(%s, %s, %s, %s)\n",
      (char *) name,public_id ? (char *) public_id : "none",
      system_id ? (char *) system_id : "none",(char *) notation);
  xmlAddDocEntity(svg_info->document,name,XML_EXTERNAL_GENERAL_UNPARSED_ENTITY,
    public_id,system_id,notation);

}

static void SVGSetDocumentLocator(void *context,xmlSAXLocatorPtr location)
{
  SVGInfo
    *svg_info;

  /*
    Receive the document locator at startup, actually xmlDefaultSAXLocator.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.setDocumentLocator()\n");
}

static void SVGStartDocument(void *context)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when the document start being processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.startDocument()\n");
  GetExceptionInfo(svg_info->exception);
  parser=svg_info->parser;
  svg_info->document=xmlNewDoc(parser->version);
  if (svg_info->document == (xmlDocPtr) NULL)
    return;
  if (parser->encoding == NULL)
    svg_info->document->encoding=NULL;
  else
    svg_info->document->encoding=xmlStrdup(parser->encoding);
  svg_info->document->standalone=parser->standalone;
}

static void SVGEndDocument(void *context)
{
  SVGInfo
    *svg_info;

  /*
    Called when the document end has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.endDocument()\n");
  if (svg_info->scale != (double *) NULL)
    LiberateMemory((void **) &svg_info->scale);
  if (svg_info->text != (char *) NULL)
    LiberateMemory((void **) &svg_info->text);
  if (svg_info->vertices != (char *) NULL)
    LiberateMemory((void **) &svg_info->vertices);
  if (svg_info->url != (char *) NULL)
    LiberateMemory((void **) &svg_info->url);
}

static void SVGStartElement(void *context,const xmlChar *name,
  const xmlChar **attributes)
{
  char
    *color,
    *font_family,
    *font_style,
    *font_weight,
    *id,
    *p,
    **tokens,
    *units;

  const char
    *keyword,
    *value;

  int
    number_tokens;

  SVGInfo
    *svg_info;

  register int
    i,
    j;

  /*
    Called when an opening tag has been processed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.startElement(%s",(char *) name);
  svg_info->n++;
  ReacquireMemory((void **) &svg_info->scale,(svg_info->n+1)*sizeof(double));
  if (svg_info->scale == (double *) NULL)
    MagickError(ResourceLimitError,"Unable to convert SVG image",
      "Memory allocation failed");
  svg_info->scale[svg_info->n]=svg_info->scale[svg_info->n-1];
  if (LocaleCompare((char *) name,"svg") != 0)
    (void) fprintf(svg_info->file,"push graphic-context\n");
  color=AllocateString("none");
  id=AllocateString("*");
  font_family=(char *) NULL;
  font_style=(char *) NULL;
  font_weight=(char *) NULL;
  units=AllocateString("userSpaceOnUse");
  if (attributes != (const xmlChar **) NULL)
    for (i=0; (attributes[i] != (const xmlChar *) NULL); i+=2)
    {
      keyword=(const char *) attributes[i];
      value=(const char *) attributes[i+1];
      if (svg_info->verbose)
        {
          (void) fprintf(stdout,"  , %s='",keyword);
          (void) fprintf(stdout,"  %s'",value);
        }
      switch (*keyword)
      {
        case 'A':
        case 'a':
        {
          if (LocaleCompare(keyword,"angle") == 0)
            {
              (void) fprintf(svg_info->file,"angle %g\n",
                GetUserSpaceCoordinateValue(svg_info,value));
              break;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (LocaleCompare(keyword,"clip-path") == 0)
            {
              (void) fprintf(svg_info->file,"clip-path %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"clipPathUnits") == 0)
            {
              CloneString(&units,value);
              break;
            }
          if (LocaleCompare(keyword,"color") == 0)
            {
              CloneString(&color,value);
              break;
            }
          if (LocaleCompare(keyword,"cx") == 0)
            {
              svg_info->element.cx=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"cy") == 0)
            {
              svg_info->element.cy=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'D':
        case 'd':
        {
          if (LocaleCompare(keyword,"d") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (LocaleCompare(keyword,"fill") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) fprintf(svg_info->file,"fill %s\n",color);
                  break;
                }
              (void) fprintf(svg_info->file,"fill %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fillcolor") == 0)
            {
              (void) fprintf(svg_info->file,"fill %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fill-rule") == 0)
            {
              (void) fprintf(svg_info->file,"fill-rule %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"fill-opacity") == 0)
            {
              (void) fprintf(svg_info->file,"fill-opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
              break;
            }
          if (LocaleCompare(keyword,"font-family") == 0)
            {
              font_family=AllocateString(value);
              break;
            }
          if (LocaleCompare(keyword,"font-style") == 0)
            {
              font_style=AllocateString(value);
              *font_style=toupper((int) *font_style);
              break;
            }
          if (LocaleCompare(keyword,"font-size") == 0)
            {
              svg_info->pointsize=GetUserSpaceCoordinateValue(svg_info,value);
              (void) fprintf(svg_info->file,"pointsize %g\n",
                svg_info->pointsize);
              break;
            }
          if (LocaleCompare(keyword,"font-weight") == 0)
            {
              font_weight=AllocateString(value);
              *font_weight=toupper((int) *font_weight);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (LocaleCompare(keyword,"height") == 0)
            {
              svg_info->bounds.height=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"href") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          break;
        }
        case 'I':
        case 'i':
        {
          if (LocaleCompare(keyword,"id") == 0)
            {
              CloneString(&id,value);
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (LocaleCompare(keyword,"major") == 0)
            {
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"minor") == 0)
            {
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'O':
        case 'o':
        {
          if (LocaleCompare(keyword,"opacity") == 0)
            {
              (void) fprintf(svg_info->file,"opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
              break;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (LocaleCompare(keyword,"path") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"points") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (LocaleCompare(keyword,"r") == 0)
            {
              svg_info->element.major=
                GetUserSpaceCoordinateValue(svg_info,value);
              svg_info->element.minor=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"rx") == 0)
            {
              if (LocaleCompare((char *) name,"ellipse") == 0)
                svg_info->element.major=
                  GetUserSpaceCoordinateValue(svg_info,value);
              else
                svg_info->radius.x=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"ry") == 0)
            {
              if (LocaleCompare((char *) name,"ellipse") == 0)
                svg_info->element.minor=
                  GetUserSpaceCoordinateValue(svg_info,value);
              else
                svg_info->radius.y=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (LocaleCompare(keyword,"stroke") == 0)
            {
              if (LocaleCompare(value,"currentColor") == 0)
                {
                  (void) fprintf(svg_info->file,"stroke %s\n",color);
                  break;
                }
              (void) fprintf(svg_info->file,"stroke %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dasharray") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-dasharray %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-dashoffset %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-linecap") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-linecap %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-linejoin") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-linejoin %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-miterlimit %s\n",value);
              break;
            }
          if (LocaleCompare(keyword,"stroke-opacity") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-opacity %g\n",atof(value)*
                (strchr(value,'%') != (char *) NULL ? 1.0 : 100.0));
              break;
            }
          if (LocaleCompare(keyword,"stroke-width") == 0)
            {
              (void) fprintf(svg_info->file,"stroke-width %g\n",
                GetUserSpaceCoordinateValue(svg_info,value));
              break;
            }
          if (LocaleCompare(keyword,"style") == 0)
            {
              if (svg_info->verbose)
                (void) fprintf(stdout,"  \n");
              tokens=GetStyleTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->verbose)
                  (void) fprintf(stdout,"    %s: %s\n",keyword,value);
                switch (*keyword)
                {
                  case 'C':
                  case 'c':
                  {
                     if (LocaleCompare(keyword,"clip-path") == 0)
                       {
                         (void) fprintf(svg_info->file,"clip-path %s\n",value);
                         break;
                       }
                     if (LocaleCompare(keyword,"clipPathUnits") == 0)
                       {
                         CloneString(&units,value);
                         break;
                       }
                    if (LocaleCompare(keyword,"color") == 0)
                      {
                        CloneString(&color,value);
                        break;
                      }
                    break;
                  }
                  case 'F':
                  case 'f':
                  {
                    if (LocaleCompare(keyword,"fill") == 0)
                      {
                         if (LocaleCompare(value,"currentColor") == 0)
                           {
                             (void) fprintf(svg_info->file,"fill %s\n",color);
                             break;
                           }
                        (void) fprintf(svg_info->file,"fill %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fillcolor") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fill-rule") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill-rule %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"fill-opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"fill-opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
                        break;
                      }
                    if (LocaleCompare(keyword,"font-family") == 0)
                      {
                        font_family=AllocateString(value);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-style") == 0)
                      {
                        font_style=AllocateString(value);
                        *font_style=toupper((int) *font_style);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-size") == 0)
                      {
                        svg_info->pointsize=
                          GetUserSpaceCoordinateValue(svg_info,value);
                        (void) fprintf(svg_info->file,"pointsize %g\n",
                          svg_info->pointsize);
                        break;
                      }
                    if (LocaleCompare(keyword,"font-weight") == 0)
                      {
                        font_weight=AllocateString(value);
                        *font_weight=toupper((int) *font_weight);
                        break;
                      }
                    break;
                  }
                  case 'O':
                  case 'o':
                  {
                    if (LocaleCompare(keyword,"opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"stroke") == 0)
                      {
                         if (LocaleCompare(value,"currentColor") == 0)
                           {
                             (void) fprintf(svg_info->file,"stroke %s\n",color);
                             break;
                           }
                        (void) fprintf(svg_info->file,"stroke %s\n",value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-antialiasing") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-antialias %d\n",
                          LocaleCompare(value,"true") == 0);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-dasharray") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-dasharray %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-dashoffset") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-dashoffset %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-linecap") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-linecap %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-linejoin") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-linejoin %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-miterlimit") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-miterlimit %s\n",
                          value);
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-opacity") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-opacity %g\n",
                          atof(value)*(strchr(value,'%') != (char *) NULL ?
                          1.0 : 100.0));
                        break;
                      }
                    if (LocaleCompare(keyword,"stroke-width") == 0)
                      {
                        (void) fprintf(svg_info->file,"stroke-width %g\n",
                          GetUserSpaceCoordinateValue(svg_info,value));
                        break;
                      }
                    break;
                  }
                  case 't':
                  case 'T':
                  {
                    if (LocaleCompare(keyword,"text-align") == 0)
                      {
                        if (LocaleCompare(value,"center") == 0)
                          (void) fprintf(svg_info->file,"gravity North\n");
                        if (LocaleCompare(value,"left") == 0)
                          (void) fprintf(svg_info->file,"gravity NorthWest\n");
                        if (LocaleCompare(value,"right") == 0)
                          (void) fprintf(svg_info->file,"gravity NorthEast\n");
                        break;
                      }
                    if (LocaleCompare(keyword,"text-decoration") == 0)
                      {
                        if (LocaleCompare(value,"underline") == 0)
                          (void) fprintf(svg_info->file,"decorate underline\n");
                        if (LocaleCompare(value,"line-through") == 0)
                          (void) fprintf(svg_info->file,
                            "decorate line-through\n");
                        if (LocaleCompare(value,"overline") == 0)
                          (void) fprintf(svg_info->file,"decorate overline\n");
                        break;
                      }
                    if (LocaleCompare(keyword,"text-antialiasing") == 0)
                      {
                        (void) fprintf(svg_info->file,"text-antialias %d\n",
                          LocaleCompare(value,"true") == 0);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
              }
              for (j=0; j < number_tokens; j++)
                LiberateMemory((void **) &tokens[j]);
              LiberateMemory((void **) &tokens);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (LocaleCompare(keyword,"text-align") == 0)
            {
              if (LocaleCompare(value,"center") == 0)
                (void) fprintf(svg_info->file,"gravity North\n");
              if (LocaleCompare(value,"left") == 0)
                (void) fprintf(svg_info->file,"gravity NorthWest\n");
              if (LocaleCompare(value,"right") == 0)
                (void) fprintf(svg_info->file,"gravity NorthEast\n");
              break;
            }
          if (LocaleCompare(keyword,"text-decoration") == 0)
            {
              if (LocaleCompare(value,"underline") == 0)
                (void) fprintf(svg_info->file,"decorate underline\n");
              if (LocaleCompare(value,"line-through") == 0)
                (void) fprintf(svg_info->file,"decorate line-through\n");
              if (LocaleCompare(value,"overline") == 0)
                (void) fprintf(svg_info->file,"decorate overline\n");
              break;
            }
          if (LocaleCompare(keyword,"text-antialiasing") == 0)
            {
              (void) fprintf(svg_info->file,"text-antialias %d\n",
                LocaleCompare(value,"true") == 0);
              break;
            }
          if (LocaleCompare(keyword,"transform") == 0)
            {
              AffineMatrix
                affine,
                current,
                transform;

              IdentityAffine(&transform);
              if (svg_info->verbose)
                (void) fprintf(stdout,"  \n");
              tokens=GetTransformTokens(value,&number_tokens);
              for (j=0; j < (number_tokens-1); j+=2)
              {
                keyword=(char *) tokens[j];
                value=(char *) tokens[j+1];
                if (svg_info->verbose)
                  (void) fprintf(stdout,"    %s: %s\n",keyword,value);
                current=transform;
                IdentityAffine(&affine);
                switch (*keyword)
                {
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare(keyword,"matrix") == 0)
                      {
                        p=(char *) value;
                        affine.sx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.rx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.ry=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.sy=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.tx=strtod(p,&p);
                        if (*p ==',')
                          p++;
                        affine.ty=strtod(p,&p);
                        break;
                      }
                    break;
                  }
                  case 'R':
                  case 'r':
                  {
                    if (LocaleCompare(keyword,"rotate") == 0)
                      {
                        double
                          angle;

                        angle=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                        affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                        affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                        affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                        break;
                      }
                    break;
                  }
                  case 'S':
                  case 's':
                  {
                    if (LocaleCompare(keyword,"scale") == 0)
                      {
                        for (p=(char *) value; *p != '\0'; p++)
                          if (isspace((int) (*p)) || (*p == ','))
                            break;
                        affine.sx=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.sy=affine.sx;
                        if (*p != '\0')
                          affine.sy=GetUserSpaceCoordinateValue(svg_info,p+1);
                        svg_info->scale[svg_info->n]=ExpandAffine(&affine);
                        break;
                      }
                    if (LocaleCompare(keyword,"skewX") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.ry=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    if (LocaleCompare(keyword,"skewY") == 0)
                      {
                        affine.sx=svg_info->affine.sx;
                        affine.rx=tan(DegreesToRadians(fmod(
                          GetUserSpaceCoordinateValue(svg_info,value),
                          360.0)));
                        affine.sy=svg_info->affine.sy;
                        break;
                      }
                    break;
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare(keyword,"translate") == 0)
                      {
                        for (p=(char *) value; *p != '\0'; p++)
                          if (isspace((int) (*p)) || (*p == ','))
                            break;
                        affine.tx=GetUserSpaceCoordinateValue(svg_info,value);
                        affine.ty=affine.tx;
                        if (*p != '\0')
                          affine.ty=GetUserSpaceCoordinateValue(svg_info,p+1);
                        break;
                      }
                    break;
                  }
                  default:
                    break;
                }
                transform.sx=current.sx*affine.sx+current.ry*affine.rx;
                transform.rx=current.rx*affine.sx+current.sy*affine.rx;
                transform.ry=current.sx*affine.ry+current.ry*affine.sy;
                transform.sy=current.rx*affine.ry+current.sy*affine.sy;
                transform.tx=current.sx*affine.tx+current.ry*affine.ty+
                  current.tx;
                transform.ty=current.rx*affine.tx+current.sy*affine.ty+
                  current.ty;
              }
              (void) fprintf(svg_info->file,"affine %g %g %g %g %g %g\n",
                transform.sx,transform.rx,transform.ry,transform.sy,
                transform.tx,transform.ty);
              for (j=0; j < number_tokens; j++)
                LiberateMemory((void **) &tokens[j]);
              LiberateMemory((void **) &tokens);
              break;
            }
          break;
        }
        case 'V':
        case 'v':
        {
          if (LocaleCompare(keyword,"verts") == 0)
            {
              CloneString(&svg_info->vertices,value);
              break;
            }
          if (LocaleCompare(keyword,"viewBox") == 0)
            {
              p=(char *) value;
              svg_info->view_box.x=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.y=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.width=strtod(p,&p);
              if (*p == ',');
                p++;
              svg_info->view_box.height=strtod(p,&p);
              if (*p == ',');
                p++;
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (LocaleCompare(keyword,"width") == 0)
            {
              svg_info->bounds.width=
                GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (LocaleCompare(keyword,"x") == 0)
            {
              svg_info->bounds.x=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"xlink:href") == 0)
            {
              CloneString(&svg_info->url,value);
              break;
            }
          if (LocaleCompare(keyword,"x1") == 0)
            {
              svg_info->segment.x1=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"x2") == 0)
            {
              svg_info->segment.x2=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (LocaleCompare(keyword,"y") == 0)
            {
              svg_info->bounds.y=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"y1") == 0)
            {
              svg_info->segment.y1=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          if (LocaleCompare(keyword,"y2") == 0)
            {
              svg_info->segment.y2=GetUserSpaceCoordinateValue(svg_info,value);
              break;
            }
          break;
        }
        default:
          break;
      }
      if (font_family != (char *) NULL)
        {
          (void) fprintf(svg_info->file,"font %s",font_family);
          if ((font_style != (char *) NULL) && (font_weight != (char *) NULL))
            (void) fprintf(svg_info->file,"-%s-%s",font_weight,
              font_style);
          else
            if (font_style != (char *) NULL)
              (void) fprintf(svg_info->file,"-%s",font_style);
            else
              if (font_weight != (char *) NULL)
                (void) fprintf(svg_info->file,"-%s",font_weight);
          (void) fprintf(svg_info->file,"\n");
        }
      if (font_family != (char *) NULL)
        LiberateMemory((void **) &font_family);
      if (font_style != (char *) NULL)
        LiberateMemory((void **) &font_style);
      if (font_weight != (char *) NULL)
        LiberateMemory((void **) &font_weight);
    }
  if (svg_info->verbose)
    (void) fprintf(stdout,"  )\n");
  if (LocaleCompare((char *) name,"svg") == 0)
    {
      if (attributes != (const xmlChar **) NULL)
        {
          char
            *geometry,
            *p;

          RectangleInfo
            page;

          if ((svg_info->view_box.width == 0.0) ||
              (svg_info->view_box.height == 0.0))
            svg_info->view_box=svg_info->bounds;
          page.width=svg_info->bounds.width;
          page.height=svg_info->bounds.height;
          page.x=0;
          page.y=0;
          geometry=(char *) NULL;
          if (svg_info->size != (char *) NULL)
            geometry=PostscriptGeometry(svg_info->size);
          if (svg_info->page != (char *) NULL)
            geometry=PostscriptGeometry(svg_info->page);
          if (geometry != (char *) NULL)
            {
              p=strchr(geometry,'>');
              if (p != (char *) NULL)
                *p='\0';
              (void) ParseImageGeometry(geometry,&page.x,&page.y,
                &page.width,&page.height);
              DestroyPostscriptGeometry(geometry);
            }
          if (svg_info->affine.sx != 1.0)
            page.width=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.width-0.5);
          if (svg_info->affine.sy != 0.0)
            page.height=(unsigned int)
              ceil(ExpandAffine(&svg_info->affine)*page.height-0.5);
          (void) fprintf(svg_info->file,"viewbox %g %g %g %g\n",
            svg_info->view_box.x,svg_info->view_box.y,svg_info->view_box.width,
            svg_info->view_box.height);
          (void) fprintf(svg_info->file,"affine %g 0.0 0.0 %g 0.0 0.0\n",
            (double) page.width/svg_info->view_box.width,
            (double) page.height/svg_info->view_box.height);
          svg_info->width=page.width;
          svg_info->height=page.height;
        }
    }
  if (LocaleCompare((char *) name,"clipPath") == 0)
    (void) fprintf(svg_info->file,"push clip-path-%s %s\n",id,units);
  LiberateMemory((void **) &id);
  LiberateMemory((void **) &units);
  if (color != (char *) NULL)
    LiberateMemory((void **) &color);
}

static void SVGEndElement(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  /*
    Called when the end of an element has been detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.endElement(%s)\n",(char *) name);
  if (svg_info->text != (char *) NULL)
    Strip(svg_info->text);
  switch (*name)
  {
    case 'C':
    case 'c':
    {
      if (LocaleCompare((char *) name,"circle") == 0)
        {
          (void) fprintf(svg_info->file,"circle %g,%g %g,%g\n",
            svg_info->element.cx,svg_info->element.cy,svg_info->element.cx,
            svg_info->element.cy+svg_info->element.minor);
          break;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((char *) name,"desc") == 0)
        {
          CloneString(&svg_info->description,svg_info->text);
          break;
        }
      break;
    }
    case 'E':
    case 'e':
    {
      if (LocaleCompare((char *) name,"ellipse") == 0)
        {
          double
            angle;

          angle=svg_info->element.angle;
          (void) fprintf(svg_info->file,"ellipse %g,%g %g,%g 0,360\n",
            svg_info->element.cx,svg_info->element.cy,
            angle == 0.0 ? svg_info->element.major : svg_info->element.minor,
            angle == 0.0 ? svg_info->element.minor : svg_info->element.major);
          break;
        }
      break;
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((char *) name,"image") == 0)
        {
          (void) fprintf(svg_info->file,"image Replace %g,%g %g,%g %s\n",
            svg_info->bounds.x,svg_info->bounds.y,svg_info->bounds.width,
            svg_info->bounds.height,svg_info->url);
          break;
        }
      break;
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((char *) name,"line") == 0)
        {
          (void) fprintf(svg_info->file,"line %g,%g %g,%g\n",
            svg_info->segment.x1,svg_info->segment.y1,svg_info->segment.x2,
            svg_info->segment.y2);
          break;
        }
      break;
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare((char *) name,"path") == 0)
        {
          (void) fprintf(svg_info->file,"path '%s'\n",svg_info->vertices);
          break;
        }
      if (LocaleCompare((char *) name,"polygon") == 0)
        {
          (void) fprintf(svg_info->file,"polygon %s\n",svg_info->vertices);
          break;
        }
      if (LocaleCompare((char *) name,"polyline") == 0)
        {
          (void) fprintf(svg_info->file,"polyline %s\n",svg_info->vertices);
          break;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare((char *) name,"rect") == 0)
        {
          if ((svg_info->radius.x == 0.0) && (svg_info->radius.y == 0.0))
            {
              (void) fprintf(svg_info->file,"rectangle %g,%g %g,%g\n",
                svg_info->bounds.x,svg_info->bounds.y,
                svg_info->bounds.x+svg_info->bounds.width,
                svg_info->bounds.y+svg_info->bounds.height);
              break;
            }
          if (svg_info->radius.x == 0.0)
            svg_info->radius.x=svg_info->radius.y;
          if (svg_info->radius.y == 0.0)
            svg_info->radius.y=svg_info->radius.x;
          (void) fprintf(svg_info->file,"roundRectangle %g,%g %g,%g %g,%g\n",
            svg_info->bounds.x,svg_info->bounds.y,
            svg_info->bounds.x+svg_info->bounds.width,
            svg_info->bounds.y+svg_info->bounds.height,
            svg_info->radius.x,svg_info->radius.y);
          svg_info->radius.x=0.0;
          svg_info->radius.y=0.0;
          break;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((char *) name,"text") == 0)
        {
          if (Extent(svg_info->text) == 0)
            break;
          if (strchr(svg_info->text,'\'') != (char *) NULL)
            {
              (void) fprintf(svg_info->file,"text %g,%g \"%s\"\n",
                svg_info->bounds.x,svg_info->bounds.y,svg_info->text);
              break;
            }
          (void) fprintf(svg_info->file,"text %g,%g '%s'\n",svg_info->bounds.x,
            svg_info->bounds.y,svg_info->text);
          break;
        }
      if (LocaleCompare((char *) name,"title") == 0)
        {
          CloneString(&svg_info->title,svg_info->text);
          break;
        }
      break;
    }
    default:
      break;
  }
  if (svg_info->text != (char *) NULL)
    *svg_info->text='\0';
  if (LocaleCompare((char *) name,"svg") != 0)
    (void) fprintf(svg_info->file,"pop graphic-context\n");
  if (LocaleCompare((char *) name,"clipPath") == 0)
    (void) fprintf(svg_info->file,"pop clip-path\n");
  svg_info->n--;
}

static void SVGCharacters(void *context,const xmlChar *c,int length)
{
  register char
    *p;

  register int
    i;

  SVGInfo
    *svg_info;

  /*
    Receiving some characters from the parser.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.characters(");
      for (i=0; (i < length) && (i < 30); i++)
        (void) fprintf(stdout,"  %c",c[i]);
      (void) fprintf(stdout,"  , %d)\n",length);
    }
  if (svg_info->text != (char *) NULL)
    ReacquireMemory((void **) &svg_info->text,Extent(svg_info->text)+length+1);
  else
    {
      svg_info->text=(char *) AcquireMemory(length+1);
      if (svg_info->text != (char *) NULL)
        *svg_info->text='\0';
    }
  if (svg_info->text == (char *) NULL)
    return;
  p=svg_info->text+Extent(svg_info->text);
  for (i=0; i < length; i++)
    *p++=c[i];
  *p='\0';
}

static void SVGReference(void *context,const xmlChar *name)
{
  SVGInfo
    *svg_info;

  xmlParserCtxtPtr
    parser;

  /*
    Called when an entity reference is detected.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.reference(%s)\n",name);
  parser=svg_info->parser;
  if (*name == '#')
    xmlAddChild(parser->node,xmlNewCharRef(svg_info->document,name));
  else
    xmlAddChild(parser->node,xmlNewReference(svg_info->document,name));
}

static void SVGIgnorableWhitespace(void *context,const xmlChar *c,int length)
{
  SVGInfo
    *svg_info;

  /*
    Receiving some ignorable whitespaces from the parser.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.ignorableWhitespace(%.30s, %d)\n",(char *) c,
      length);
}

static void SVGProcessingInstructions(void *context,const xmlChar *target,
  const xmlChar *data)
{
  SVGInfo
    *svg_info;

  /*
    A processing instruction has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.processingInstruction(%s, %s)\n",(char *) target,
      (char *) data);
}

static void SVGComment(void *context,const xmlChar *value)
{
  SVGInfo
    *svg_info;

  /*
    A comment has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.comment(%s)\n",value);
  CloneString(&svg_info->comment,(char *) value);
}

static void SVGWarning(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /**
    Display and format a warning messages, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.warning: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateWarning;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->reason,reason);
  va_end(operands);
}

static void SVGError(void *context,const char *format,...)
{
  char
    reason[MaxTextExtent];

  SVGInfo
    *svg_info;

  va_list
    operands;

  /*
    Display and format a error formats, gives file, line, position and
    extra parameters.
  */
  va_start(operands,format);
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.error: ");
      vfprintf(stdout,format,operands);
    }
  svg_info->exception->severity=DelegateError;
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(reason,format,operands);
#else
  (void) vsnprintf(reason,MaxTextExtent,format,operands);
#endif
  CloneString(&svg_info->exception->reason,reason);
  va_end(operands);
}

static void SVGCDataBlock(void *context,const xmlChar *value,int length)
{
  SVGInfo
    *svg_info;

   xmlNodePtr
     child;

  xmlParserCtxtPtr
    parser;

  /*
    Called when a pcdata block has been parsed.
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    (void) fprintf(stdout,"  SAX.pcdata(%.1024s, %d)\n",(char *) value,length);
  parser=svg_info->parser;
  child=xmlGetLastChild(parser->node);
  if ((child != (xmlNodePtr) NULL) && (child->type == XML_CDATA_SECTION_NODE))
    {
      xmlTextConcat(child,value,length);
      return;
    }
  xmlAddChild(parser->node,xmlNewCDataBlock(parser->myDoc,value,length));
}

static void SVGExternalSubset(void *context,const xmlChar *name,
  const xmlChar *external_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserCtxt
    parser_context;

  xmlParserCtxtPtr
    parser;

  xmlParserInputPtr
    input;

  /*
    Does this document has an enternal subset?
  */
  svg_info=(SVGInfo *) context;
  if (svg_info->verbose)
    {
      (void) fprintf(stdout,"  SAX.InternalSubset(%s",name);
      if (external_id != NULL)
        (void) fprintf(stdout,"  , %s",external_id);
      if (system_id != NULL)
        (void) fprintf(stdout,"  , %s",system_id);
      (void) fprintf(stdout,"\n");
    }
  parser=svg_info->parser;
  if (((external_id == NULL) && (system_id == NULL)) ||
      (!parser->validate || !parser->wellFormed || !svg_info->document))
    return;
  input=SVGResolveEntity(context,external_id,system_id);
  if (input == NULL)
    return;
  xmlNewDtd(svg_info->document,name,external_id,system_id);
  parser_context=(*parser);
  parser->inputTab=(xmlParserInputPtr *) xmlMalloc(5*sizeof(xmlParserInputPtr));
  if (parser->inputTab == (xmlParserInputPtr *) NULL)
    {
      parser->errNo=XML_ERR_NO_MEMORY;
      parser->input=parser_context.input;
      parser->inputNr=parser_context.inputNr;
      parser->inputMax=parser_context.inputMax;
      parser->inputTab=parser_context.inputTab;
      return;
  }
  parser->inputNr=0;
  parser->inputMax=5;
  parser->input=NULL;
  xmlPushInput(parser,input);
  xmlSwitchEncoding(parser,xmlDetectCharEncoding(parser->input->cur,4));
  if (input->filename == (char *) NULL)
    input->filename=(char *) xmlStrdup(system_id);
  input->line=1;
  input->col=1;
  input->base=parser->input->cur;
  input->cur=parser->input->cur;
  input->free=NULL;
  xmlParseExternalSubset(parser,external_id,system_id);
  while (parser->inputNr > 1)
    xmlPopInput(parser);
  xmlFreeInputStream(parser->input);
  xmlFree(parser->inputTab);
  parser->input=parser_context.input;
  parser->inputNr=parser_context.inputNr;
  parser->inputMax=parser_context.inputMax;
  parser->inputTab=parser_context.inputTab;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  xmlSAXHandler
    SAXHandlerStruct =
    {
      SVGInternalSubset,
      SVGIsStandalone,
      SVGHasInternalSubset,
      SVGHasExternalSubset,
      SVGResolveEntity,
      SVGGetEntity,
      SVGEntityDeclaration,
      SVGNotationDeclaration,
      SVGAttributeDeclaration,
      SVGElementDeclaration,
      SVGUnparsedEntityDeclaration,
      SVGSetDocumentLocator,
      SVGStartDocument,
      SVGEndDocument,
      SVGStartElement,
      SVGEndElement,
      SVGReference,
      SVGCharacters,
      SVGIgnorableWhitespace,
      SVGProcessingInstructions,
      SVGComment,
      SVGWarning,
      SVGError,
      SVGError,
      SVGGetParameterEntity,
      SVGCDataBlock,
      SVGExternalSubset
    };

  char
    buffer[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    n;

  SVGInfo
    svg_info;

  unsigned int
    status;

  xmlSAXHandlerPtr
    SAXHandler;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open draw file.
  */
  TemporaryFilename(filename);
  file=fopen(filename,"w");
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Parse SVG file.
  */
  memset(&svg_info,0,sizeof(SVGInfo));
  svg_info.file=file;
  svg_info.verbose=image_info->verbose;
  svg_info.exception=exception;
  svg_info.scale=(double *) AcquireMemory(sizeof(double));
  if (svg_info.scale == (double *) NULL)
    MagickError(ResourceLimitError,"Unable to convert SVG image",
      "Memory allocation failed");
  svg_info.scale[0]=1.0;
  IdentityAffine(&svg_info.affine);
  svg_info.affine.sx=
    image->x_resolution == 0.0 ? 1.0 : image->x_resolution/72.0;
  svg_info.affine.sy=
    image->y_resolution == 0.0 ? 1.0 : image->y_resolution/72.0;
  svg_info.bounds.width=image->columns;
  svg_info.bounds.height=image->rows;
  if (image_info->size != (char *) NULL)
    CloneString(&svg_info.size,image_info->size);
  if (image_info->page != (char *) NULL)
    CloneString(&svg_info.page,image_info->page);
  if (svg_info.verbose)
    (void) fprintf(stdout,"begin SAX\n");
  xmlSubstituteEntitiesDefault(1);
  SAXHandler=(&SAXHandlerStruct);
  svg_info.parser=xmlCreatePushParserCtxt(SAXHandler,&svg_info,(char *) NULL,0,
    image->filename);
  while (GetStringBlob(image,buffer) != (char *) NULL)
  {
    n=Extent(buffer);
    if (n == 0)
      continue;
    status=xmlParseChunk(svg_info.parser,buffer,n,False);
    if (status != 0)
      break;
    (void) xmlParseChunk(svg_info.parser," ",1,False);
  }
  /* (void) xmlParseChunk(svg_info.parser," ",1,True); */
  xmlFreeParserCtxt(svg_info.parser);
  if (svg_info.verbose)
    (void) fprintf(stdout,"end SAX\n");
  xmlCleanupParser();
  (void) fclose(file);
  CloseBlob(image);
  DestroyImage(image);
  image=(Image *) NULL;
  if (exception->severity == UndefinedException)
    {
      /*
        Draw image.
      */
      clone_info=CloneImageInfo(image_info);
      FormatString(geometry,"%dx%d",svg_info.width,svg_info.height);
      CloneString(&clone_info->size,geometry);
      FormatString(clone_info->filename,"mvg:%.1024s",filename);
      if (clone_info->density != (char *) NULL)
        LiberateMemory((void **) &clone_info->density);
      image=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
      if (image != (Image *) NULL)
        {
          (void) strcpy(image->filename,image_info->filename);
          if (svg_info.comment != (char *) NULL)
            (void) SetImageAttribute(image,"Comment",svg_info.comment);
          if (svg_info.description != (char *) NULL)
            (void) SetImageAttribute(image,"Description",svg_info.description);
          if (svg_info.title != (char *) NULL)
            (void) SetImageAttribute(image,"Title",svg_info.title);
        }
    }
  /*
    Free resources.
  */
  (void) remove(filename);
  if (svg_info.title != (char *) NULL)
    LiberateMemory((void **) &svg_info.title);
  if (svg_info.description != (char *) NULL)
    LiberateMemory((void **) &svg_info.description);
  if (svg_info.comment != (char *) NULL)
    LiberateMemory((void **) &svg_info.comment);
  return(image);
}
#else
static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "SVG library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S V G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSVGImage adds attributes for the SVG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSVGImage method is:
%
%      RegisterSVGImage(void)
%
*/
ModuleExport void RegisterSVGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SVG");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->encoder=WriteSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("XML");
  entry->magick=IsSVG;
  entry->decoder=ReadSVGImage;
  entry->description=AllocateString("Scalable Vector Gaphics");
  entry->module=AllocateString("SVG");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S V G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSVGImage removes format registrations made by the
%  SVG module from the list of supported formats.
%
%  The format of the UnregisterSVGImage method is:
%
%      UnregisterSVGImage(void)
%
*/
ModuleExport void UnregisterSVGImage(void)
{
  UnregisterMagickInfo("SVG");
}

#if defined(HasAUTOTRACE)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S V G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSVGImage writes a image in the SVG - XML based W3C standard
%  format.
%
%  The format of the WriteSVGImage method is:
%
%      unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSVGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
{
  FILE
    *output_file;

  fitting_opts_type
    fit_info;

  image_header_type
    image_header;

  bitmap_type
    bitmap;

  ImageType
    image_type;

  int
    j;

  pixel_outline_list_type
    pixels;

  PixelPacket
    p;

  PixelPacket
    *pixel;

  QuantizeObj
    *quantize_info;

  spline_list_array_type
    splines;

  output_write
    output_writer;

  register int
    i;

  unsigned int
    number_planes,
    point,
    thin;

  thin=False;
  quantize_info=(QuantizeObj *) NULL;
  pixel=(&p);
  fit_info=new_fitting_opts();
  output_writer=output_get_handler("svg");
  if (output_writer == NULL)
    ThrowWriterException(FileOpenWarning,"Unable to write svg format",image);
  image_type=GetImageType(image);
  number_planes=3;
  if ((image_type == BilevelType) || (image_type == GrayscaleType))
    number_planes=1;
  bitmap.np=number_planes;
  bitmap.dimensions.width=image->columns;
  bitmap.dimensions.height=image->rows;
  bitmap.bitmap=(unsigned char *)
    AcquireMemory(number_planes*image->columns*image->rows);
  if (bitmap.bitmap == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
      image);
  point=0;
  for (j=0; j < image->rows; j++)
  {
    for (i=0; i < image->columns; i++)
    {
      p=GetOnePixel(image,i,j);
      bitmap.bitmap[point++]=pixel->red;
      if (number_planes == 3)
        {
          bitmap.bitmap[point++]=pixel->green;
          bitmap.bitmap[point++]=pixel->blue;
        }
    }
  }
  image_header.width=DIMENSIONS_WIDTH(bitmap.dimensions);
  image_header.height=DIMENSIONS_HEIGHT(bitmap.dimensions);
  if ((fit_info.color_count > 0) && (BITMAP_PLANES(bitmap) == 3))
    quantize(bitmap.bitmap,bitmap.bitmap,DIMENSIONS_WIDTH(bitmap.dimensions),
      DIMENSIONS_HEIGHT(bitmap.dimensions),fit_info.color_count,
      fit_info.bgColor,&quantize_info);
  if (thin)
    thin_image(&bitmap);
  pixels=find_outline_pixels (bitmap);
  LiberateMemory((void **) &(bitmap.bitmap));
  splines=fitted_splines(pixels,&fit_info);
  output_file=fopen(image->filename,"w");
  if (output_file == (FILE *) NULL)
    ThrowWriterException(FileOpenWarning,"Unable to open the output file",
      image);
  output_writer(output_file,image->filename,0,0,image_header.width,
    image_header.height,splines);
  return(True);
}
#else
static unsigned int WriteSVGImage(const ImageInfo *image_info,Image *image)
{
#define BezierQuantum  200

  AffineMatrix
    affine,
    current;

  char
    buffer[MaxTextExtent],
    fill[MaxTextExtent],
    keyword[MaxTextExtent],
    *primitive,
    *q,
    stroke[MaxTextExtent],
    value[MaxTextExtent];

  double
    angle;

  DrawInfo
    **graphic_context;

  ImageAttribute
    *attribute;

  int
    j,
    n,
    number_points;

  PointInfo
    point;

  PrimitiveInfo
    *primitive_info;

  PrimitiveType
    primitive_type;

  register char
    *p;

  register int
    i,
    x;

  unsigned int
    length,
    status;

  /*
    Open output image file.
  */
  attribute=GetImageAttribute(image,"[MVG]");
  if (attribute == (ImageAttribute *) NULL)
    ThrowWriterException(DelegateWarning,"no image vector graphics",image);
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  n=0;
  graphic_context=(DrawInfo **) AcquireMemory(sizeof(DrawInfo *));
  if (graphic_context == (DrawInfo **) NULL)
    MagickError(ResourceLimitWarning,"Unable to draw image",
      "Memory allocation failed");
  graphic_context[n]=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  primitive=graphic_context[n]->primitive;
  /*
    Allocate primitive info memory.
  */
  number_points=2047;
  primitive_info=(PrimitiveInfo *)
    AcquireMemory(number_points*sizeof(PrimitiveInfo));
  if (primitive_info == (PrimitiveInfo *) NULL)
    {
      for ( ; n >= 0; n--)
        DestroyDrawInfo(graphic_context[n]);
      LiberateMemory((void **) &graphic_context);
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    }
  (void) WriteBlobString(image,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
  (void) WriteBlobString(image,
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000802//EN\"\n");
  (void) WriteBlobString(image,
    "  \"http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd\">\n");
  status=True;
  if (graphic_context[n]->debug)
    (void) fprintf(stdout,"begin vector-graphics\n");
  for (q=attribute->value; *q != '\0'; )
  {
    /*
      Define primitive.
    */
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    p=q;
    for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
      keyword[x]=(*q++);
    keyword[x]='\0';
    if (*keyword == '\0')
      break;
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    primitive_type=UndefinedPrimitive;
    current=graphic_context[n]->affine;
    IdentityAffine(&affine);
    switch (*keyword)
    {
      case ';':
        break;
      case 'a':
      case 'A':
      {
        if (LocaleCompare("affine",keyword) == 0)
          {
            affine.sx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.rx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ry=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.sy=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.tx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ty=strtod(q,&q);
            break;
          }
        if (LocaleCompare("angle",keyword) == 0)
          {
            (void) strtod(q,&q);
            break;
          }
        if (LocaleCompare("arc",keyword) == 0)
          {
            primitive_type=ArcPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'b':
      case 'B':
      {
        if (LocaleCompare("bezier",keyword) == 0)
          {
            primitive_type=BezierPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'c':
      case 'C':
      {
        if (LocaleCompare("clip-path",keyword) == 0)
          {
            if ((LocaleNCompare(q,"url(",4) == 0))
              for (x=0; (*(q-1) != ')') && (*q != '\0'); x++)
                value[x]=(*q++);
            else
              for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                value[x]=(*q++);
            value[x]='\0';
            break;
          }
        if (LocaleCompare("circle",keyword) == 0)
          {
            primitive_type=CirclePrimitive;
            break;
          }
        if (LocaleCompare("color",keyword) == 0)
          {
            primitive_type=ColorPrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'd':
      case 'D':
      {
        if (LocaleCompare("decorate",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("none",value) == 0)
              graphic_context[n]->decorate=NoDecoration;
            if (LocaleCompare("underline",value) == 0)
              graphic_context[n]->decorate=UnderlineDecoration;
            if (LocaleCompare("overline",value) == 0)
              graphic_context[n]->decorate=OverlineDecoration;
            if (LocaleCompare("line-through",value) == 0)
              graphic_context[n]->decorate=LineThroughDecoration;
            break;
          }
        status=False;
        break;
      }
      case 'e':
      case 'E':
      {
        if (LocaleCompare("ellipse",keyword) == 0)
          {
            primitive_type=EllipsePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'f':
      case 'F':
      {
        if (LocaleCompare("fill",keyword) == 0)
          {
            if ((LocaleNCompare(q,"rgb(",4) == 0))
              for (x=0; (*(q-1) != ')') && (*q != '\0'); x++)
                value[x]=(*q++);
            else
              for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                value[x]=(*q++);
            value[x]='\0';
            (void) QueryColorDatabase(value,&graphic_context[n]->fill);
            if (graphic_context[n]->fill.opacity != TransparentOpacity)
              graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("fill-rule",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("evenodd",value) == 0)
              graphic_context[n]->fill_rule=EvenOddRule;
            if (LocaleCompare("nonzero",value) == 0)
              graphic_context[n]->fill_rule=NonZeroRule;
            break;
          }
        if (LocaleCompare("fill-opacity",keyword) == 0)
          {
            graphic_context[n]->fill.opacity=(Quantum)
              ceil(MaxRGB*strtod(q,&q)/100.0-0.5);
            break;
          }
        if (LocaleCompare("font",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            CloneString(&graphic_context[n]->font,value);
            break;
          }
        status=False;
        break;
      }
      case 'g':
      case 'G':
      {
        if (LocaleCompare("gravity",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("NorthWest",value) == 0)
              graphic_context[n]->gravity=NorthWestGravity;
            if (LocaleCompare("North",value) == 0)
              graphic_context[n]->gravity=NorthGravity;
            if (LocaleCompare("NorthEast",value) == 0)
              graphic_context[n]->gravity=NorthEastGravity;
            if (LocaleCompare("West",value) == 0)
              graphic_context[n]->gravity=WestGravity;
            if (LocaleCompare("Center",value) == 0)
              graphic_context[n]->gravity=CenterGravity;
            if (LocaleCompare("East",value) == 0)
              graphic_context[n]->gravity=EastGravity;
            if (LocaleCompare("SouthWest",value) == 0)
              graphic_context[n]->gravity=SouthWestGravity;
            if (LocaleCompare("South",value) == 0)
              graphic_context[n]->gravity=SouthGravity;
            if (LocaleCompare("SouthEast",value) == 0)
              graphic_context[n]->gravity=SouthEastGravity;
            break;
          }
        status=False;
        break;
      }
      case 'i':
      case 'I':
      {
        if (LocaleCompare("image",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("Over",value) == 0)
              graphic_context[n]->compose=OverCompositeOp;
            if (LocaleCompare("In",value) == 0)
              graphic_context[n]->compose=InCompositeOp;
            if (LocaleCompare("Out",value) == 0)
              graphic_context[n]->compose=OutCompositeOp;
            if (LocaleCompare("Atop",value) == 0)
              graphic_context[n]->compose=AtopCompositeOp;
            if (LocaleCompare("Xor",value) == 0)
              graphic_context[n]->compose=XorCompositeOp;
            if (LocaleCompare("Plus",value) == 0)
              graphic_context[n]->compose=PlusCompositeOp;
            if (LocaleCompare("Minus",value) == 0)
              graphic_context[n]->compose=MinusCompositeOp;
            if (LocaleCompare("Add",value) == 0)
              graphic_context[n]->compose=AddCompositeOp;
            if (LocaleCompare("Subtract",value) == 0)
              graphic_context[n]->compose=SubtractCompositeOp;
            if (LocaleCompare("Difference",value) == 0)
              graphic_context[n]->compose=DifferenceCompositeOp;
            if (LocaleCompare("Multiply",value) == 0)
              graphic_context[n]->compose=MultiplyCompositeOp;
            if (LocaleCompare("Bumpmap",value) == 0)
              graphic_context[n]->compose=BumpmapCompositeOp;
            if (LocaleCompare("Replace",value) == 0)
              graphic_context[n]->compose=ReplaceCompositeOp;
            if (LocaleCompare("ReplaceRed",value) == 0)
              graphic_context[n]->compose=ReplaceRedCompositeOp;
            if (LocaleCompare("ReplaceGreen",value) == 0)
              graphic_context[n]->compose=ReplaceGreenCompositeOp;
            if (LocaleCompare("ReplaceBlue",value) == 0)
              graphic_context[n]->compose=ReplaceBlueCompositeOp;
            if (LocaleCompare("ReplaceMatte",value) == 0)
              graphic_context[n]->compose=ReplaceMatteCompositeOp;
            primitive_type=ImagePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'l':
      case 'L':
      {
        if (LocaleCompare("line",keyword) == 0)
          {
            primitive_type=LinePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'm':
      case 'M':
      {
        if (LocaleCompare("matte",keyword) == 0)
          {
            primitive_type=MattePrimitive;
            break;
          }
        status=False;
        break;
      }
      case 'o':
      case 'O':
      {
        if (LocaleCompare("opacity",keyword) == 0)
          {
            graphic_context[n]->opacity=(Quantum)
              ceil(MaxRGB*strtod(q,&q)/100.0-0.5);
            graphic_context[n]->fill.opacity=graphic_context[n]->opacity;
            graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        status=False;
        break;
      }
      case 'p':
      case 'P':
      {
        if (LocaleCompare("path",keyword) == 0)
          {
            primitive_type=PathPrimitive;
            break;
          }
        if (LocaleCompare("point",keyword) == 0)
          {
            primitive_type=PointPrimitive;
            break;
          }
        if (LocaleCompare("pointsize",keyword) == 0)
          {
            graphic_context[n]->pointsize=strtod(q,&q);
            break;
          }
        if (LocaleCompare("polyline",keyword) == 0)
          {
            primitive_type=PolylinePrimitive;
            break;
          }
        if (LocaleCompare("polygon",keyword) == 0)
          {
            primitive_type=PolygonPrimitive;
            break;
          }
        if (LocaleCompare("pop",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("clip-path",value) == 0)
              break;
            if (LocaleCompare("graphic-context",value) == 0)
              {
                DestroyDrawInfo(graphic_context[n]);
                n--;
                if (n < 0)
                  ThrowWriterException(CorruptImageWarning,
                    "unbalanced graphic context push/pop",image);
              }
            break;
          }
        if (LocaleCompare("push",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleNCompare("clip-path-",value,10) == 0)
              {
                while (isspace((int) (*q)) && (*q != '\0'))
                  q++;
                for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                  value[x]=(*q++);
                value[x]='\0';
                break;
              }
            if (LocaleCompare("graphic-context",value) == 0)
              {
                n++;
                ReacquireMemory((void **) &graphic_context,
                  (n+1)*sizeof(DrawInfo *));
                if (graphic_context == (DrawInfo **) NULL)
                  MagickError(ResourceLimitWarning,"Unable to draw image",
                    "Memory allocation failed");
                graphic_context[n]=
                  CloneDrawInfo((ImageInfo *) NULL,graphic_context[n-1]);
              }
            break;
          }
        status=False;
        break;
      }
      case 'r':
      case 'R':
      {
        if (LocaleNCompare("rect",keyword,4) == 0)
          {
            primitive_type=RectanglePrimitive;
            break;
          }
        if (LocaleCompare("roundRectangle",keyword) == 0)
          {
            primitive_type=RoundRectanglePrimitive;
            break;
          }
        if (LocaleCompare("rotate",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
            affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
            affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
            affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        status=False;
        break;
      }
      case 's':
      case 'S':
      {
        if (LocaleCompare("scale",keyword) == 0)
          {
            affine.sx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.sy=strtod(q,&q);
            break;
          }
        if (LocaleCompare("skewX",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("skewY",keyword) == 0)
          {
            angle=strtod(q,&q);
            affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
            break;
          }
        if (LocaleCompare("stroke",keyword) == 0)
          {
            if ((LocaleNCompare(q,"rgb(",4) == 0))
              for (x=0; (*(q-1) != ')') && (*q != '\0'); x++)
                value[x]=(*q++);
            else
              for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
                value[x]=(*q++);
            value[x]='\0';
            (void) QueryColorDatabase(value,&graphic_context[n]->stroke);
            if (graphic_context[n]->stroke.opacity != TransparentOpacity)
              graphic_context[n]->stroke.opacity=graphic_context[n]->opacity;
            break;
          }
        if (LocaleCompare("stroke-antialias",keyword) == 0)
          {
            graphic_context[n]->stroke_antialias=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-dasharray",keyword) == 0)
          {
            if (IsGeometry(q))
              {
                char
                  *r;

                r=q;
                for (x=0; IsGeometry(r); x++)
                  (void) strtod(r,&r);
                graphic_context[n]->dash_pattern=(unsigned int *)
                  AcquireMemory((x+1)*sizeof(unsigned int));
                if (graphic_context[n]->dash_pattern == (unsigned int *) NULL)
                  {
                    ThrowException(&image->exception,ResourceLimitWarning,
                      "Unable to draw image","Memory allocation failed");
                    break;
                  }
                for (x=0; IsGeometry(q); x++)
                  graphic_context[n]->dash_pattern[x]=
                    (unsigned int) strtod(q,&q);
                graphic_context[n]->dash_pattern[x]=0;
                break;
              }
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare(value,"none") != 0)
              break;
            if (graphic_context[n]->dash_pattern != (unsigned int *) NULL)
              LiberateMemory((void **) &graphic_context[n]->dash_pattern);
            graphic_context[n]->dash_pattern=(unsigned int *) NULL;
            break;
          }
        if (LocaleCompare("stroke-dashoffset",keyword) == 0)
          {
            graphic_context[n]->dash_offset=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-linecap",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("butt",value) == 0)
              graphic_context[n]->linecap=ButtCap;
            if (LocaleCompare("round",value) == 0)
              graphic_context[n]->linecap=RoundCap;
            if (LocaleCompare("square",value) == 0)
              graphic_context[n]->linecap=SquareCap;
            break;
          }
        if (LocaleCompare("stroke-linejoin",keyword) == 0)
          {
            for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
              value[x]=(*q++);
            value[x]='\0';
            if (LocaleCompare("butt",value) == 0)
              graphic_context[n]->linejoin=MiterJoin;
            if (LocaleCompare("round",value) == 0)
              graphic_context[n]->linejoin=RoundJoin;
            if (LocaleCompare("square",value) == 0)
              graphic_context[n]->linejoin=BevelJoin;
            break;
          }
        if (LocaleCompare("stroke-miterlimit",keyword) == 0)
          {
            graphic_context[n]->miterlimit=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("stroke-opacity",keyword) == 0)
          {
            graphic_context[n]->stroke.opacity=(Quantum)
              ceil(MaxRGB*strtod(q,&q)/100.0-0.5);
            break;
          }
        if (LocaleCompare("stroke-width",keyword) == 0)
          {
            graphic_context[n]->stroke_width=strtod(q,&q);
            continue;
          }
        status=False;
        break;
      }
      case 't':
      case 'T':
      {
        if (LocaleCompare("text",keyword) == 0)
          {
            primitive_type=TextPrimitive;
            break;
          }
        if (LocaleCompare("text-antialias",keyword) == 0)
          {
            graphic_context[n]->text_antialias=(unsigned int) strtod(q,&q);
            break;
          }
        if (LocaleCompare("translate",keyword) == 0)
          {
            affine.tx=strtod(q,&q);
            if (*q == ',')
              q++;
            affine.ty=strtod(q,&q);
            break;
          }
        status=False;
        break;
      }
      case 'v':
      case 'V':
      {
        if (LocaleCompare("viewbox",keyword) == 0)
          {
            RectangleInfo
              viewbox;

            viewbox.x=strtod(q,&q);
            if (*q == ',')
              q++;
            viewbox.y=strtod(q,&q);
            if (*q == ',')
              q++;
            viewbox.width=strtod(q,&q);
            if (*q == ',')
              q++;
            viewbox.height=strtod(q,&q);
            (void) FormatString(buffer,
              "<svg width=\"%u\" height=\"%u\" viewBox=\"%d %d %u %u\">\n",
              image->columns,image->rows,viewbox.x,viewbox.y,
              viewbox.width,viewbox.height);
            (void) WriteBlobString(image,buffer);
            break;
          }
        status=False;
        break;
      }
      default:
      {
        status=False;
        break;
      }
    }
    if (status == False)
      break;
    if ((affine.sx != 1.0) || (affine.rx != 0.0) || (affine.ry != 0.0) ||
        (affine.sy != 1.0) || (affine.tx != 0.0) || (affine.ty != 0.0))
      {
        graphic_context[n]->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
        graphic_context[n]->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
        graphic_context[n]->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
        graphic_context[n]->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
        graphic_context[n]->affine.tx=
          current.sx*affine.tx+current.ry*affine.ty+current.tx;
        graphic_context[n]->affine.ty=
          current.rx*affine.tx+current.sy*affine.ty+current.ty;
      }
    if (primitive_type == UndefinedPrimitive)
      {
        if (graphic_context[n]->debug)
          (void) fprintf(stdout,"  %.*s\n",q-p,p);
        continue;
      }
    /*
      Parse the primitive attributes.
    */
    i=0;
    j=0;
    for (x=0; *q != '\0'; x++)
    {
      /*
        Define points.
      */
      while (isspace((int) (*q)) && (*q != '\0'))
        q++;
      if (!IsGeometry(q))
        break;
      point.x=strtod(q,&q);
      if (*q == ',')
        q++;
      point.y=strtod(q,&q);
      if (*q == ',')
        q++;
      primitive_info[i].primitive=primitive_type;
      primitive_info[i].point=point;
      primitive_info[i].coordinates=0;
      primitive_info[i].method=FloodfillMethod;
      while ((isspace((int) (*q)) || (*q == ',')) && (*q != '\0'))
        q++;
      i++;
      if (i < (int) (number_points-6*BezierQuantum-360))
        continue;
      number_points+=6*BezierQuantum+360;
      ReacquireMemory((void **) &primitive_info,
        number_points*sizeof(PrimitiveInfo));
      if (primitive_info == (PrimitiveInfo *) NULL)
        {
          ThrowException(&image->exception,ResourceLimitWarning,
            "Unable to draw image","Memory allocation failed");
          break;
        }
    }
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    primitive_info[j].primitive=primitive_type;
    primitive_info[j].coordinates=x;
    primitive_info[j].method=FloodfillMethod;
    primitive_info[j].text=(char *) NULL;
    QueryColorName(&graphic_context[n]->fill,fill);
    QueryColorName(&graphic_context[n]->stroke,stroke);
    (void) FormatString(buffer,
      "<g style=\"fill:%s; stroke:%s; stroke-width:%g\"\n",
      fill,stroke,graphic_context[n]->stroke_width);
    (void) WriteBlobString(image,buffer);
    (void) FormatString(buffer,"  transform=\"matrix(%g,%g,%g,%g,%g,%g)\">\n",
      graphic_context[n]->affine.sx,graphic_context[n]->affine.rx,
      graphic_context[n]->affine.ry,graphic_context[n]->affine.sy,
      graphic_context[n]->affine.tx,graphic_context[n]->affine.ty);
    (void) WriteBlobString(image,buffer);
    switch (primitive_type)
    {
      case PointPrimitive:
      default:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        break;
      }
      case LinePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        break;
      }
      case RectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        (void) FormatString(buffer,
          "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\"/>\n",
          primitive_info[j].point.x,primitive_info[j].point.y,
          primitive_info[j+1].point.x-primitive_info[j].point.x,
          primitive_info[j+1].point.y-primitive_info[j].point.y);
        (void) WriteBlobString(image,buffer);
        break;
      }
      case RoundRectanglePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        break;
      }
      case ArcPrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        break;
      }
      case EllipsePrimitive:
      {
        if (primitive_info[j].coordinates != 3)
          {
            status=False;
            break;
          }
        break;
      }
      case CirclePrimitive:
      {
        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        break;
      }
      case PolylinePrimitive:
      {
        if (primitive_info[j].coordinates < 2)
          {
            status=False;
            break;
          }
        break;
      }
      case PolygonPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        primitive_info[i]=primitive_info[j];
        primitive_info[i].coordinates=0;
        primitive_info[j].coordinates++;
        i++;
        break;
      }
      case BezierPrimitive:
      {
        if (primitive_info[j].coordinates < 3)
          {
            status=False;
            break;
          }
        break;
      }
      case PathPrimitive:
      {
        char
          *path;

        int
          number_attributes;

        if (*q == '\0')
          break;
        number_attributes=1;
        p=q;
        if (*q == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
            {
              if (isalpha((int) *q))
                number_attributes++;
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
            }
          }
        else
          if (*q == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
              {
                if (isalpha((int) *q))
                  number_attributes++;
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
              }
            }
          else
            for (q++;  *q != '\0'; q++)
            {
              if (isalpha((int) *q))
                number_attributes++;
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
            }
        path=(char *) AcquireMemory(q-p+1);
        if (i > (number_points-6*BezierQuantum*number_attributes-1))
          {
            number_points+=6*BezierQuantum*number_attributes;
            ReacquireMemory((void **) &primitive_info,
              number_points*sizeof(PrimitiveInfo));
          }
        if ((path == (char *) NULL) ||
            (primitive_info == (PrimitiveInfo *) NULL))
          {
            ThrowException(&image->exception,ResourceLimitWarning,
              "Unable to draw image","Memory allocation failed");
            break;
          }
        (void) strncpy(path,p,q-p+1);
        path[q-p]='\0';
        LiberateMemory((void **) &path);
        break;
      }
      case ColorPrimitive:
      case MattePrimitive:
      {
        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        /*
          Define method.
        */
        for (x=0; !isspace((int) (*q)) && (*q != '\0'); x++)
          keyword[x]=(*q++);
        keyword[x]='\0';
        if (*keyword == '\0')
          break;
        if (LocaleCompare("point",keyword) == 0)
          primitive_info[j].method=PointMethod;
        else
          if (LocaleCompare("replace",keyword) == 0)
            primitive_info[j].method=ReplaceMethod;
          else
            if (LocaleCompare("floodfill",keyword) == 0)
              primitive_info[j].method=FloodfillMethod;
            else
              if (LocaleCompare("filltoborder",keyword) == 0)
                primitive_info[j].method=FillToBorderMethod;
              else
                if (LocaleCompare("reset",keyword) == 0)
                  primitive_info[j].method=ResetMethod;
                else
                  status=False;
        break;
      }
      case TextPrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 1)
          {
            status=False;
            break;
          }
        if (*q == '\0')
          break;
        p=q;
        if (*q == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
          }
        else
          if (*q == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
            }
          else
            for (q++;  *q != '\0'; q++)
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
        primitive_info[j].text=(char *) AcquireMemory(q-p+1);
        if (primitive_info[j].text != (char *) NULL)
          {
            (void) strncpy(primitive_info[j].text,p,q-p+1);
            primitive_info[j].text[q-p]='\0';
          }
        break;
      }
      case ImagePrimitive:
      {
        register char
          *p;

        if (primitive_info[j].coordinates != 2)
          {
            status=False;
            break;
          }
        if (*q == '\0')
          break;
        p=q;
        if (*p == '"')
          {
            p++;
            for (q++; *q != '\0'; q++)
              if ((*q == '"') && (*(q-1) != '\\'))
                break;
          }
        else
          if (*p == '\'')
            {
              p++;
              for (q++; *q != '\0'; q++)
                if ((*q == '\'') && (*(q-1) != '\\'))
                  break;
            }
          else
            for (q++;  *q != '\0'; q++)
              if (isspace((int) *q) && (*(q-1) != '\\') && (*q != '\0'))
                break;
        primitive_info[j].text=(char *) AcquireMemory(q-p+1);
        if (primitive_info[j].text != (char *) NULL)
          {
            (void) strncpy(primitive_info[j].text,p,q-p);
            primitive_info[j].text[q-p]='\0';
          }
        break;
      }
    }
    (void) WriteBlobString(image,"</g>\n");
    if (primitive_info == (PrimitiveInfo *) NULL)
      break;
    if (graphic_context[n]->debug)
      (void) fprintf(stdout,"  %.*s\n",q-p,p);
    if (status == False)
      break;
    if ((*q == '"') || (*q == '\''))
      q++;
    while (isspace((int) (*q)) && (*q != '\0'))
      q++;
    primitive_info[i].primitive=UndefinedPrimitive;
    if (primitive_info->text != (char *) NULL)
      LiberateMemory((void **) &primitive_info->text);
    if (status == False)
      break;
  }
  if (graphic_context[n]->debug)
    (void) fprintf(stdout,"end vector-graphics\n");
  (void) WriteBlobString(image,"</svg>\n");
  /*
    Free resources.
  */
  if (primitive_info != (PrimitiveInfo *) NULL)
    LiberateMemory((void **) &primitive_info);
  for ( ; n >= 0; n--)
    DestroyDrawInfo(graphic_context[n]);
  LiberateMemory((void **) &graphic_context);
  CloseBlob(image);
  return(status);
}
#endif
