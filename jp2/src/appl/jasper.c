/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001-2002 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 * 
 * JasPer Software License
 * 
 * IMAGE POWER JPEG-2000 PUBLIC LICENSE
 * ************************************
 * 
 * GRANT:
 * 
 * Permission is hereby granted, free of charge, to any person (the "User")
 * obtaining a copy of this software and associated documentation, to deal
 * in the JasPer Software without restriction, including without limitation
 * the right to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the JasPer Software (in source and binary forms),
 * and to permit persons to whom the JasPer Software is furnished to do so,
 * provided further that the License Conditions below are met.
 * 
 * License Conditions
 * ******************
 * 
 * A.  Redistributions of source code must retain the above copyright notice,
 * and this list of conditions, and the following disclaimer.
 * 
 * B.  Redistributions in binary form must reproduce the above copyright
 * notice, and this list of conditions, and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 * 
 * C.  Neither the name of Image Power, Inc. nor any other contributor
 * (including, but not limited to, the University of British Columbia and
 * Michael David Adams) may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * D.  User agrees that it shall not commence any action against Image Power,
 * Inc., the University of British Columbia, Michael David Adams, or any
 * other contributors (collectively "Licensors") for infringement of any
 * intellectual property rights ("IPR") held by the User in respect of any
 * technology that User owns or has a right to license or sublicense and
 * which is an element required in order to claim compliance with ISO/IEC
 * 15444-1 (i.e., JPEG-2000 Part 1).  "IPR" means all intellectual property
 * rights worldwide arising under statutory or common law, and whether
 * or not perfected, including, without limitation, all (i) patents and
 * patent applications owned or licensable by User; (ii) rights associated
 * with works of authorship including copyrights, copyright applications,
 * copyright registrations, mask work rights, mask work applications,
 * mask work registrations; (iii) rights relating to the protection of
 * trade secrets and confidential information; (iv) any right analogous
 * to those set forth in subsections (i), (ii), or (iii) and any other
 * proprietary rights relating to intangible property (other than trademark,
 * trade dress, or service mark rights); and (v) divisions, continuations,
 * renewals, reissues and extensions of the foregoing (as and to the extent
 * applicable) now existing, hereafter filed, issued or acquired.
 * 
 * E.  If User commences an infringement action against any Licensor(s) then
 * such Licensor(s) shall have the right to terminate User's license and
 * all sublicenses that have been granted hereunder by User to other parties.
 * 
 * F.  This software is for use only in hardware or software products that
 * are compliant with ISO/IEC 15444-1 (i.e., JPEG-2000 Part 1).  No license
 * or right to this Software is granted for products that do not comply
 * with ISO/IEC 15444-1.  The JPEG-2000 Part 1 standard can be purchased
 * from the ISO.
 * 
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE.
 * NO USE OF THE JASPER SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE JASPER SOFTWARE IS PROVIDED BY THE LICENSORS AND
 * CONTRIBUTORS UNDER THIS LICENSE ON AN ``AS-IS'' BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION,
 * WARRANTIES THAT THE JASPER SOFTWARE IS FREE OF DEFECTS, IS MERCHANTABLE,
 * IS FIT FOR A PARTICULAR PURPOSE OR IS NON-INFRINGING.  THOSE INTENDING
 * TO USE THE JASPER SOFTWARE OR MODIFICATIONS THEREOF FOR USE IN HARDWARE
 * OR SOFTWARE PRODUCTS ARE ADVISED THAT THEIR USE MAY INFRINGE EXISTING
 * PATENTS, COPYRIGHTS, TRADEMARKS, OR OTHER INTELLECTUAL PROPERTY RIGHTS.
 * THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE JASPER SOFTWARE
 * IS WITH THE USER.  SHOULD ANY PART OF THE JASPER SOFTWARE PROVE DEFECTIVE
 * IN ANY RESPECT, THE USER (AND NOT THE INITIAL DEVELOPERS, THE UNIVERSITY
 * OF BRITISH COLUMBIA, IMAGE POWER, INC., MICHAEL DAVID ADAMS, OR ANY
 * OTHER CONTRIBUTOR) SHALL ASSUME THE COST OF ANY NECESSARY SERVICING,
 * REPAIR OR CORRECTION.  UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY,
 * WHETHER TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE, SHALL THE
 * INITIAL DEVELOPER, THE UNIVERSITY OF BRITISH COLUMBIA, IMAGE POWER, INC.,
 * MICHAEL DAVID ADAMS, ANY OTHER CONTRIBUTOR, OR ANY DISTRIBUTOR OF THE
 * JASPER SOFTWARE, OR ANY SUPPLIER OF ANY OF SUCH PARTIES, BE LIABLE TO
 * THE USER OR ANY OTHER PERSON FOR ANY INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES OF ANY CHARACTER INCLUDING, WITHOUT LIMITATION,
 * DAMAGES FOR LOSS OF GOODWILL, WORK STOPPAGE, COMPUTER FAILURE OR
 * MALFUNCTION, OR ANY AND ALL OTHER COMMERCIAL DAMAGES OR LOSSES, EVEN IF
 * SUCH PARTY HAD BEEN INFORMED, OR OUGHT TO HAVE KNOWN, OF THE POSSIBILITY
 * OF SUCH DAMAGES.  THE JASPER SOFTWARE AND UNDERLYING TECHNOLOGY ARE NOT
 * FAULT-TOLERANT AND ARE NOT DESIGNED, MANUFACTURED OR INTENDED FOR USE OR
 * RESALE AS ON-LINE CONTROL EQUIPMENT IN HAZARDOUS ENVIRONMENTS REQUIRING
 * FAIL-SAFE PERFORMANCE, SUCH AS IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL, DIRECT
 * LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH THE FAILURE OF THE
 * JASPER SOFTWARE OR UNDERLYING TECHNOLOGY OR PRODUCT COULD LEAD DIRECTLY
 * TO DEATH, PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE
 * ("HIGH RISK ACTIVITIES").  LICENSOR SPECIFICALLY DISCLAIMS ANY EXPRESS
 * OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.  USER WILL NOT
 * KNOWINGLY USE, DISTRIBUTE OR RESELL THE JASPER SOFTWARE OR UNDERLYING
 * TECHNOLOGY OR PRODUCTS FOR HIGH RISK ACTIVITIES AND WILL ENSURE THAT ITS
 * CUSTOMERS AND END-USERS OF ITS PRODUCTS ARE PROVIDED WITH A COPY OF THE
 * NOTICE SPECIFIED IN THIS SECTION.
 * 
 * __END_OF_JASPER_LICENSE__
 */

/*
 * JasPer Transcoder Program
 *
 * $Id$
 */

/******************************************************************************\
* Includes.
\******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <jasper/jasper.h>

/******************************************************************************\
*
\******************************************************************************/

#define OPTSMAX	4096

/******************************************************************************\
* Types.
\******************************************************************************/

/* Encoder command line options. */

typedef struct {

	char *infile;
	/* The input image file. */

	int infmt;
	/* The input image file format. */

	char *inopts;
	char inoptsbuf[OPTSMAX + 1];

	char *outfile;
	/* The output image file. */

	int outfmt;

	char *outopts;
	char outoptsbuf[OPTSMAX + 1];

	int verbose;
	/* Verbose mode. */

	int debug;

	int version;

	int_fast32_t cmptno;

} cmdopts_t;

/******************************************************************************\
* Local prototypes.
\******************************************************************************/

cmdopts_t *cmdopts_parse(int argc, char **argv);
void cmdopts_destroy(cmdopts_t *cmdopts);
void cmdusage();
void badusage();
void cmdinfo();
int addopt(char *optstr, int maxlen, char *s);

/******************************************************************************\
* Global data.
\******************************************************************************/

char *cmdname = "";

/******************************************************************************\
* Code.
\******************************************************************************/

int main(int argc, char **argv)
{
	jas_image_t *image;
	cmdopts_t *cmdopts;
	jas_stream_t *in;
	jas_stream_t *out;
	clock_t startclk;
	clock_t endclk;
	long dectime;
	long enctime;
	int_fast16_t numcmpts;
	int i;

	/* Determine the base name of this command. */
	if ((cmdname = strrchr(argv[0], '/'))) {
		++cmdname;
	} else {
		cmdname = argv[0];
	}

	if (jas_init()) {
		abort();
	}

	/* Parse the command line options. */
	if (!(cmdopts = cmdopts_parse(argc, argv))) {
		fprintf(stderr, "error: cannot parse command line\n");
		exit(EXIT_FAILURE);
	}

	if (cmdopts->version) {
		printf("%s\n", JAS_VERSION);
		fprintf(stderr, "libjasper %s\n", jas_getversion());
		exit(EXIT_SUCCESS);
	}

	jas_setdbglevel(cmdopts->debug);

	if (cmdopts->verbose) {
		cmdinfo();
	}

	/* Open the input image file. */
	if (cmdopts->infile) {
		/* The input image is to be read from a file. */
		if (!(in = jas_stream_fopen(cmdopts->infile, "rb"))) {
			fprintf(stderr, "error: cannot open input image file %s\n",
			  cmdopts->infile);
			exit(EXIT_FAILURE);
		}
	} else {
		/* The input image is to be read from standard input. */
		if (!(in = jas_stream_fdopen(0, "rb"))) {
			fprintf(stderr, "error: cannot open standard input\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Open the output image file. */
	if (cmdopts->outfile) {
		/* The output image is to be written to a file. */
		if (!(out = jas_stream_fopen(cmdopts->outfile, "w+b"))) {
			fprintf(stderr, "error: cannot open output image file %s\n",
			  cmdopts->outfile);
			exit(EXIT_FAILURE);
		}
	} else {
		/* The output image is to be written to standard output. */
		if (!(out = jas_stream_fdopen(1, "w+b"))) {
			fprintf(stderr, "error: cannot open standard output\n");
			exit(EXIT_FAILURE);
		}
	}

	if (cmdopts->infmt < 0) {
		if ((cmdopts->infmt = jas_image_getfmt(in)) < 0) {
			fprintf(stderr, "error: input image has unknown format\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Get the input image data. */
	startclk = clock();
	if (!(image = jas_image_decode(in, cmdopts->infmt, cmdopts->inopts))) {
		fprintf(stderr, "error: cannot load image data\n");
		exit(EXIT_FAILURE);
	}
	endclk = clock();
	dectime = endclk - startclk;

	/* If requested, throw away all of the components except one.
	  Why might this be desirable?  It is a hack, really.
	  None of the image formats other than the JPEG-2000 ones support
	  images with two, four, five, or more components.  This hack
	  allows such images to be decoded with the non-JPEG-2000 decoders,
	  one component at a time. */
	numcmpts = jas_image_numcmpts(image);
	if (cmdopts->cmptno >= 0 && cmdopts->cmptno < numcmpts) {
		for (i = numcmpts - 1; i >= 0; --i) {
			if (i != cmdopts->cmptno) {
				jas_image_delcmpt(image, i);
			}
		}
	}

	/* Generate the output image data. */
	startclk = clock();
	if (jas_image_encode(image, out, cmdopts->outfmt, cmdopts->outopts)) {
		fprintf(stderr, "error: cannot encode image\n");
		exit(EXIT_FAILURE);
	}
	jas_stream_flush(out);
	endclk = clock();
	enctime = endclk - startclk;

	if (cmdopts->verbose) {
		fprintf(stderr, "decoding time = %f\n", dectime / (double)
		  CLOCKS_PER_SEC);
		fprintf(stderr, "encoding time = %f\n", enctime / (double)
		  CLOCKS_PER_SEC);
	}

	/* If this fails, we don't care. */
	(void) jas_stream_close(in);

	/* Close the output image stream. */
	if (jas_stream_close(out)) {
		fprintf(stderr, "error: cannot close output image file\n");
		exit(EXIT_FAILURE);
	}

	cmdopts_destroy(cmdopts);
	jas_image_destroy(image);
	jas_image_clearfmts();

	/* Success at last! :-) */
	return EXIT_SUCCESS;
}

cmdopts_t *cmdopts_parse(int argc, char **argv)
{

	typedef enum {
		CMDOPT_HELP = 0,
		CMDOPT_VERBOSE,
		CMDOPT_INFILE,
		CMDOPT_INFMT,
		CMDOPT_INOPT,
		CMDOPT_OUTFILE,
		CMDOPT_OUTFMT,
		CMDOPT_OUTOPT,
		CMDOPT_VERSION,
		CMDOPT_DEBUG,
		CMDOPT_CMPTNO
	} cmdoptid_t;

	static jas_opt_t cmdoptions[] = {
		{CMDOPT_HELP, "help", 0},
		{CMDOPT_VERBOSE, "verbose", 0},
		{CMDOPT_INFILE, "input", JAS_OPT_HASARG},
		{CMDOPT_INFILE, "f", JAS_OPT_HASARG},
		{CMDOPT_INFMT, "input-format", JAS_OPT_HASARG},
		{CMDOPT_INFMT, "t", JAS_OPT_HASARG},
		{CMDOPT_INOPT, "input-option", JAS_OPT_HASARG},
		{CMDOPT_INOPT, "o", JAS_OPT_HASARG},
		{CMDOPT_OUTFILE, "output", JAS_OPT_HASARG},
		{CMDOPT_OUTFILE, "F", JAS_OPT_HASARG},
		{CMDOPT_OUTFMT, "output-format", JAS_OPT_HASARG},
		{CMDOPT_OUTFMT, "T", JAS_OPT_HASARG},
		{CMDOPT_OUTOPT, "output-option", JAS_OPT_HASARG},
		{CMDOPT_OUTOPT, "O", JAS_OPT_HASARG},
		{CMDOPT_VERSION, "version", 0},
		{CMDOPT_DEBUG, "debug-level", JAS_OPT_HASARG},
		{CMDOPT_CMPTNO, "cmptno", JAS_OPT_HASARG},
		{-1, 0, 0}
	};

	cmdopts_t *cmdopts;
	int c;

	if (!(cmdopts = malloc(sizeof(cmdopts_t)))) {
		fprintf(stderr, "error: insufficient memory\n");
		exit(EXIT_FAILURE);
	}

	cmdopts->infile = 0;
	cmdopts->infmt = -1;
	cmdopts->inopts = 0;
	cmdopts->inoptsbuf[0] = '\0';
	cmdopts->outfile = 0;
	cmdopts->outfmt = -1;
	cmdopts->outopts = 0;
	cmdopts->outoptsbuf[0] = '\0';
	cmdopts->verbose = 0;
	cmdopts->version = 0;
	cmdopts->cmptno = -1;
	cmdopts->debug = 0;

	while ((c = jas_getopt(argc, argv, cmdoptions)) != EOF) {
		switch (c) {
		case CMDOPT_HELP:
			cmdusage();
			break;
		case CMDOPT_VERBOSE:
			cmdopts->verbose = 1;
			break;
		case CMDOPT_VERSION:
			cmdopts->version = 1;
			break;
		case CMDOPT_DEBUG:
			cmdopts->debug = atoi(jas_optarg);
			break;
		case CMDOPT_INFILE:
			cmdopts->infile = jas_optarg;
			break;
		case CMDOPT_INFMT:
			if ((cmdopts->infmt = jas_image_strtofmt(jas_optarg)) < 0) {
				fprintf(stderr, "warning: ignoring invalid input format %s\n",
				  jas_optarg);
				cmdopts->infmt = -1;
			}
			break;
		case CMDOPT_INOPT:
			addopt(cmdopts->inoptsbuf, OPTSMAX, jas_optarg);
			cmdopts->inopts = cmdopts->inoptsbuf;
			break;
		case CMDOPT_OUTFILE:
			cmdopts->outfile = jas_optarg;
			break;
		case CMDOPT_OUTFMT:
			if ((cmdopts->outfmt = jas_image_strtofmt(jas_optarg)) < 0) {
				fprintf(stderr, "error: invalid output format %s\n", jas_optarg);
				badusage();
			}
			break;
		case CMDOPT_OUTOPT:
			addopt(cmdopts->outoptsbuf, OPTSMAX, jas_optarg);
			cmdopts->outopts = cmdopts->outoptsbuf;
			break;
		case CMDOPT_CMPTNO:
			cmdopts->cmptno = atoi(jas_optarg);
			break;
		default:
			badusage();
			break;
		}
	}

	while (jas_optind < argc) {
		fprintf(stderr,
		  "warning: ignoring bogus command line argument %s\n",
		  argv[jas_optind]);
		++jas_optind;
	}

	if (cmdopts->version) {
		goto done;
	}

	if (cmdopts->outfmt < 0 && cmdopts->outfile) {
		if ((cmdopts->outfmt = jas_image_fmtfromname(cmdopts->outfile)) < 0) {
			fprintf(stderr,
			  "error: cannot guess image format from output file name\n");
		}
	}

	if (cmdopts->outfmt < 0) {
		fprintf(stderr, "error: no output format specified\n");
		badusage();
	}

done:
	return cmdopts;
}

void cmdopts_destroy(cmdopts_t *cmdopts)
{
	free(cmdopts);
}

int addopt(char *optstr, int maxlen, char *s)
{
	int n;
	int m;

	n = strlen(optstr);
	m = n + strlen(s) + 1;
	if (m > maxlen) {
		return 1;
	}
	if (n > 0) {
		strcat(optstr, "\n");
	}
	strcat(optstr, s);
	return 0;
}

void cmdinfo()
{
	fprintf(stderr, "JasPer Transcoder (Version %s).\n",
	  JAS_VERSION);
	fprintf(stderr, "%s\n", JAS_COPYRIGHT);
	fprintf(stderr, "%s\n", JAS_NOTES);
}

static char *helpinfo[] = {
"The following options are supported:\n",
"    --help                  Print this help information and exit.\n",
"    --version               Print version information and exit.\n",
"    --verbose               Enable verbose mode.\n",
"    --debug-level $lev      Set the debug level to $lev.\n",
"    --input $file           Read the input image from the file named $file\n",
"                            instead of standard input.\n",
"    --input-format $fmt     Specify the format of the input image as $fmt.\n",
"                            (See below for the list of supported formats.)\n",
"    --input-option $opt     Provide the option $opt to the decoder.\n",
"    --output $file          Write the output image to the file named $file\n",
"                            instead of standard output.\n",
"    --output-format $fmt    Specify the format of the output image as $fmt.\n",
"                            (See below for the list of supported formats.)\n",
"    --output-option $opt    Provide the option $opt to the encoder.\n",
"Some of the above option names can be abbreviated as follows:\n",
"    --input = -f, --input-format = -t, --input-option = -o,\n",
"    --output = -F, --output-format = -T, --output-option = -O\n",
0
};

void cmdusage()
{
	int fmtid;
	jas_image_fmtinfo_t *fmtinfo;
	char *s;
	int i;
	cmdinfo();
	fprintf(stderr, "usage: %s [options]\n", cmdname);
	for (i = 0, s = helpinfo[i]; s; ++i, s = helpinfo[i]) {
		fprintf(stderr, "%s", s);
	}
	fprintf(stderr, "The following formats are supported:\n");
	for (fmtid = 0;; ++fmtid) {
		if (!(fmtinfo = jas_image_lookupfmtbyid(fmtid))) {
			break;
		}
		fprintf(stderr, "    %-5s    %s\n", fmtinfo->name,
		  fmtinfo->desc);
	}
	exit(EXIT_FAILURE);
}

void badusage()
{
	fprintf(stderr,
	  "For more information on how to use this command, type:\n");
	fprintf(stderr, "    %s --help\n", cmdname);
	exit(EXIT_FAILURE);
}
