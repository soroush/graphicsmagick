/*
 * testXPath.c : a small tester program for XPath.
 *
 * See Copyright for the status of this software.
 *
 * Daniel.Veillard@w3.org
 */

#ifdef WIN32
#include "win32config.h"
#else
#include "config.h"
#endif

#include <libxml/xmlversion.h>
#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_DEBUG_ENABLED)

#include <stdio.h>
#include <string.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif


#include <libxml/xpath.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/debugXML.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlerror.h>
#if defined(LIBXML_XPTR_ENABLED)
#include <libxml/xpointer.h>
static int xptr = 0;
#endif
static int debug = 0;
static int valid = 0;
static int expr = 0;
static xmlDocPtr document = NULL;

/*
 * Default document
 */
static xmlChar buffer[] = 
"<?xml version=\"1.0\"?>\n\
<EXAMPLE prop1=\"gnome is great\" prop2=\"&amp; linux too\">\n\
  <head>\n\
   <title>Welcome to Gnome</title>\n\
  </head>\n\
  <chapter>\n\
   <title>The Linux adventure</title>\n\
   <p>bla bla bla ...</p>\n\
   <image href=\"linus.gif\"/>\n\
   <p>...</p>\n\
  </chapter>\n\
  <chapter>\n\
   <title>Chapter 2</title>\n\
   <p>this is chapter 2 ...</p>\n\
  </chapter>\n\
  <chapter>\n\
   <title>Chapter 3</title>\n\
   <p>this is chapter 3 ...</p>\n\
  </chapter>\n\
  <chapter>\n\
   <title>Chapter 4</title>\n\
   <p>this is chapter 4 ...</p>\n\
  </chapter>\n\
  <chapter>\n\
   <title>Chapter 5</title>\n\
   <p>this is chapter 5 ...</p>\n\
  </chapter>\n\
</EXAMPLE>\n\
";


void testXPath(const char *str) {
    xmlXPathObjectPtr res;
    xmlXPathContextPtr ctxt;
    
#if defined(LIBXML_XPTR_ENABLED)
    if (xptr) {
	ctxt = xmlXPtrNewContext(document, NULL, NULL);
	res = xmlXPtrEval(BAD_CAST str, ctxt);
    } else {
#endif
	ctxt = xmlXPathNewContext(document);
	if (expr)
	    res = xmlXPathEvalExpression(BAD_CAST str, ctxt);
	else
	    res = xmlXPathEval(BAD_CAST str, ctxt);
#if defined(LIBXML_XPTR_ENABLED)
    }
#endif
    xmlXPathDebugDumpObject(stdout, res, 0);
    xmlXPathFreeObject(res);
    xmlXPathFreeContext(ctxt);
}

void testXPathFile(const char *filename) {
    FILE *input;
    char expr[5000];
    int len;

    input = fopen(filename, "r");
    if (input == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"Cannot open %s for reading\n", filename);
	return;
    }
    while (fgets(expr, 4500, input) != NULL) {
	len = strlen(expr);
	len--;
	while ((len >= 0) && 
	       ((expr[len] == '\n') || (expr[len] == '\t') ||
		(expr[len] == '\r') || (expr[len] == ' '))) len--;
	expr[len + 1] = 0;      
	if (len >= 0) {
	    printf("\n========================\nExpression: %s\n", expr) ;
	    testXPath(expr);
	}
    }

    fclose(input);
}

int main(int argc, char **argv) {
    int i;
    int strings = 0;
    int usefile = 0;
    char *filename = NULL;

    for (i = 1; i < argc ; i++) {
#if defined(LIBXML_XPTR_ENABLED)
	if ((!strcmp(argv[i], "-xptr")) || (!strcmp(argv[i], "--xptr")))
	    xptr++;
#endif
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    debug++;
	if ((!strcmp(argv[i], "-valid")) || (!strcmp(argv[i], "--valid")))
	    valid++;
	if ((!strcmp(argv[i], "-expr")) || (!strcmp(argv[i], "--expr")))
	    expr++;
	if ((!strcmp(argv[i], "-i")) || (!strcmp(argv[i], "--input")))
	    filename = argv[++i];
	if ((!strcmp(argv[i], "-f")) || (!strcmp(argv[i], "--file")))
	    usefile++;
    }
    if (valid != 0) xmlDoValidityCheckingDefaultValue = 1;
    if (document == NULL) {
        if (filename == NULL)
	    document = xmlParseDoc(buffer);
	else
	    document = xmlParseFile(filename);
    }
    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-i")) || (!strcmp(argv[i], "--input"))) {
	    i++; continue;
	}
	if (argv[i][0] != '-') {
	    if (usefile)
	        testXPathFile(argv[i]);
	    else
		testXPath(argv[i]);
	    strings ++;
	}
    }
    if (strings == 0) {
	printf("Usage : %s [--debug] [--copy] stringsorfiles ...\n",
	       argv[0]);
	printf("\tParse the XPath strings and output the result of the parsing\n");
	printf("\t--debug : dump a debug version of the result\n");
	printf("\t--valid : switch on DTD support in the parser\n");
#if defined(LIBXML_XPTR_ENABLED)
	printf("\t--xptr : expressions are XPointer expressions\n");
#endif
	printf("\t--expr : debug XPath expressions only\n");
	printf("\t--input filename : or\n");
	printf("\t-i filename      : read the document from filename\n");
	printf("\t--file : or\n");
	printf("\t-f     : read queries from files, args\n");
    }
    if (document != NULL) 
	xmlFreeDoc(document);
    xmlCleanupParser();
    xmlMemoryDump();

    return(0);
}
#else
#include <stdio.h>
int main(int argc, char **argv) {
    printf("%s : XPath/Debug support not compiled in\n", argv[0]);
    return(0);
}
#endif /* LIBXML_XPATH_ENABLED */
