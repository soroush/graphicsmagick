/*
 * alternate cjpeg.c
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2009-2020 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains an alternate user interface for the JPEG compressor.
 * One or more input files are named on the command line, and output file
 * names are created by substituting ".jpg" for the input file's extension.
 */

#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jversion.h"		/* for version message */

#ifdef USE_CCOMMAND		/* command-line reader for Macintosh */
#ifdef __MWERKS__
#include <SIOUX.h>              /* Metrowerks needs this */
#include <console.h>		/* ... and this */
#endif
#ifdef THINK_C
#include <console.h>		/* Think declares it here */
#endif
#endif

#ifndef PATH_MAX		/* ANSI maximum-pathname-length constant */
#define PATH_MAX 256
#endif


/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};


/*
 * Automatic determination of available memory.
 */

static long default_maxmem;	/* saves value determined at startup, or 0 */

#ifndef FREE_MEM_ESTIMATE	/* may be defined from command line */

#ifdef MSDOS			/* For MS-DOS (unless flat-memory model) */

#include <dos.h>		/* for access to intdos() call */

LOCAL(long)
unused_dos_memory (void)
/* Obtain total amount of unallocated DOS memory */
{
  union REGS regs;
  long nparas;

  regs.h.ah = 0x48;		/* DOS function Allocate Memory Block */
  regs.x.bx = 0xFFFF;		/* Ask for more memory than DOS can have */
  (void) intdos(&regs, &regs);
  /* DOS will fail and return # of paragraphs actually available in BX. */
  nparas = (unsigned int) regs.x.bx;
  /* Times 16 to convert to bytes. */
  return nparas << 4;
}

/* The default memory setting is 95% of the available space. */
#define FREE_MEM_ESTIMATE  ((unused_dos_memory() * 95L) / 100L)

#endif /* MSDOS */

#ifdef ATARI			/* For Atari ST/STE/TT, Pure C or Turbo C */

#include <ext.h>

/* The default memory setting is 90% of the available space. */
#define FREE_MEM_ESTIMATE  (((long) coreleft() * 90L) / 100L)

#endif /* ATARI */

/* Add memory-estimation procedures for other operating systems here,
 * with appropriate #ifdef's around them.
 */

#endif /* !FREE_MEM_ESTIMATE */


/*
 * This routine determines what format the input file is,
 * and selects the appropriate input-reading module.
 *
 * To determine which family of input formats the file belongs to,
 * we may look only at the first byte of the file, since C does not
 * guarantee that more than one character can be pushed back with ungetc.
 * Looking at additional bytes would require one of these approaches:
 *     1) assume we can fseek() the input file (fails for piped input);
 *     2) assume we can push back more than one character (works in
 *        some C implementations, but unportable);
 *     3) provide our own buffering (breaks input readers that want to use
 *        stdio directly, such as the RLE library);
 * or  4) don't put back the data, and modify the input_init methods to assume
 *        they start reading after the start of file (also breaks RLE library).
 * #1 is attractive for MS-DOS but is untenable on Unix.
 *
 * The most portable solution for file types that can't be identified by their
 * first byte is to make the user tell us what they are.  This is also the
 * only approach for "raw" file types that contain only arbitrary values.
 * We presently apply this method for Targa files.  Most of the time Targa
 * files start with 0x00, so we recognize that case.  Potentially, however,
 * a Targa file could start with any byte value (byte 0 is the length of the
 * seldom-used ID field), so we provide a switch to force Targa input mode.
 */

static boolean is_targa;	/* records user -targa switch */


LOCAL(cjpeg_source_ptr)
select_file_type (j_compress_ptr cinfo, FILE * infile)
{
  int c;

  if (is_targa) {
#ifdef TARGA_SUPPORTED
    return jinit_read_targa(cinfo);
#else
    ERREXIT(cinfo, JERR_TGA_NOTCOMP);
#endif
  }

  if ((c = getc(infile)) == EOF)
    ERREXIT(cinfo, JERR_INPUT_EMPTY);
  if (ungetc(c, infile) == EOF)
    ERREXIT(cinfo, JERR_UNGETC_FAILED);

  switch (c) {
#ifdef BMP_SUPPORTED
  case 'B':
    return jinit_read_bmp(cinfo);
#endif
#ifdef GIF_SUPPORTED
  case 'G':
    return jinit_read_gif(cinfo);
#endif
#ifdef PPM_SUPPORTED
  case 'P':
    return jinit_read_ppm(cinfo);
#endif
#ifdef RLE_SUPPORTED
  case 'R':
    return jinit_read_rle(cinfo);
#endif
#ifdef TARGA_SUPPORTED
  case 0x00:
    return jinit_read_targa(cinfo);
#endif
  default:
    ERREXIT(cinfo, JERR_UNKNOWN_FORMAT);
    break;
  }

  return NULL;			/* suppress compiler warnings */
}


/*
 * Argument-parsing code.
 * The switch parser is designed to be useful with DOS-style command line
 * syntax, ie, intermixed switches and file names, where only the switches
 * to the left of a given file name affect processing of that file.
 */


static const char * progname;	/* program name for error messages */
static char * outfilename;	/* for -outfile switch */


LOCAL(void)
usage (void)
/* complain about bad command line */
{
  fprintf(stderr, "usage: %s [switches] inputfile(s)\n", progname);
  fprintf(stderr, "List of input files may use wildcards (* and ?)\n");
  fprintf(stderr, "Output filename is same as input filename, but extension .jpg\n");

  fprintf(stderr, "Switches (names may be abbreviated):\n");
  fprintf(stderr, "  -quality N[,...]   Compression quality (0..100; 5-95 is useful range)\n");
  fprintf(stderr, "  -grayscale     Create monochrome JPEG file\n");
  fprintf(stderr, "  -rgb           Create RGB JPEG file\n");
#ifdef ENTROPY_OPT_SUPPORTED
  fprintf(stderr, "  -optimize      Optimize Huffman table (smaller file, but slow compression)\n");
#endif
#ifdef C_PROGRESSIVE_SUPPORTED
  fprintf(stderr, "  -progressive   Create progressive JPEG file\n");
#endif
#ifdef DCT_SCALING_SUPPORTED
  fprintf(stderr, "  -scale M/N     Scale image by fraction M/N, eg, 1/2\n");
#endif
#ifdef TARGA_SUPPORTED
  fprintf(stderr, "  -targa         Input file is Targa format (usually not needed)\n");
#endif
  fprintf(stderr, "Switches for advanced users:\n");
#ifdef C_ARITH_CODING_SUPPORTED
  fprintf(stderr, "  -arithmetic    Use arithmetic coding\n");
#endif
#ifdef DCT_SCALING_SUPPORTED
  fprintf(stderr, "  -block N       DCT block size (1..16; default is 8)\n");
#endif
#if JPEG_LIB_VERSION_MAJOR >= 9
  fprintf(stderr, "  -rgb1          Create RGB JPEG file with reversible color transform\n");
  fprintf(stderr, "  -bgycc         Create big gamut YCC JPEG file\n");
#endif
#ifdef DCT_ISLOW_SUPPORTED
  fprintf(stderr, "  -dct int       Use integer DCT method%s\n",
	  (JDCT_DEFAULT == JDCT_ISLOW ? " (default)" : ""));
#endif
#ifdef DCT_IFAST_SUPPORTED
  fprintf(stderr, "  -dct fast      Use fast integer DCT (less accurate)%s\n",
	  (JDCT_DEFAULT == JDCT_IFAST ? " (default)" : ""));
#endif
#ifdef DCT_FLOAT_SUPPORTED
  fprintf(stderr, "  -dct float     Use floating-point DCT method%s\n",
	  (JDCT_DEFAULT == JDCT_FLOAT ? " (default)" : ""));
#endif
  fprintf(stderr, "  -nosmooth      Don't use high-quality downsampling\n");
  fprintf(stderr, "  -restart N     Set restart interval in rows, or in blocks with B\n");
#ifdef INPUT_SMOOTHING_SUPPORTED
  fprintf(stderr, "  -smooth N      Smooth dithered input (N=1..100 is strength)\n");
#endif
#ifndef FREE_MEM_ESTIMATE
  fprintf(stderr, "  -maxmemory N   Maximum memory to use (in kbytes)\n");
#endif
  fprintf(stderr, "  -outfile name  Specify name for output file\n");
  fprintf(stderr, "  -verbose  or  -debug   Emit debug output\n");
  fprintf(stderr, "Switches for wizards:\n");
  fprintf(stderr, "  -baseline      Force baseline quantization tables\n");
  fprintf(stderr, "  -qtables file  Use quantization tables given in file\n");
  fprintf(stderr, "  -qslots N[,...]    Set component quantization tables\n");
  fprintf(stderr, "  -sample HxV[,...]  Set component sampling factors\n");
#ifdef C_MULTISCAN_FILES_SUPPORTED
  fprintf(stderr, "  -scans file    Create multi-scan JPEG per script file\n");
#endif
  exit(EXIT_FAILURE);
}


LOCAL(int)
parse_switches (j_compress_ptr cinfo, int argc, char **argv,
		int last_file_arg_seen, boolean for_real)
/* Parse optional switches.
 * Returns argv[] index of first file-name argument (== argc if none).
 * Any file names with indexes <= last_file_arg_seen are ignored;
 * they have presumably been processed in a previous iteration.
 * (Pass 0 for last_file_arg_seen on the first or only iteration.)
 * for_real is FALSE on the first (dummy) pass; we may skip any expensive
 * processing.
 */
{
  int argn;
  char * arg;
  boolean force_baseline;
  boolean simple_progressive;
  char * qualityarg = NULL;	/* saves -quality parm if any */
  char * qtablefile = NULL;	/* saves -qtables filename if any */
  char * qslotsarg = NULL;	/* saves -qslots parm if any */
  char * samplearg = NULL;	/* saves -sample parm if any */
  char * scansarg = NULL;	/* saves -scans parm if any */

  /* Set up default JPEG parameters. */

  force_baseline = FALSE;	/* by default, allow 16-bit quantizers */
  simple_progressive = FALSE;
  is_targa = FALSE;
  outfilename = NULL;
  cinfo->err->trace_level = 0;
  if (default_maxmem > 0)	/* override library's default value */
    cinfo->mem->max_memory_to_use = default_maxmem;

  /* Scan command line options, adjust parameters */

  for (argn = 1; argn < argc; argn++) {
    arg = argv[argn];
    if (*arg != '-') {
      /* Not a switch, must be a file name argument */
      if (argn <= last_file_arg_seen) {
	outfilename = NULL;	/* -outfile applies to just one input file */
	continue;		/* ignore this name if previously processed */
      }
      break;			/* else done parsing switches */
    }
    arg++;			/* advance past switch marker character */

    if (keymatch(arg, "arithmetic", 1)) {
      /* Use arithmetic coding. */
#ifdef C_ARITH_CODING_SUPPORTED
      cinfo->arith_code = TRUE;
#else
      fprintf(stderr, "%s: sorry, arithmetic coding not supported\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "baseline", 2)) {
      /* Force baseline-compatible output (8-bit quantizer values). */
      force_baseline = TRUE;

    } else if (keymatch(arg, "block", 2)) {
      /* Set DCT block size. */
#if defined DCT_SCALING_SUPPORTED && JPEG_LIB_VERSION_MAJOR >= 8 && \
      (JPEG_LIB_VERSION_MAJOR > 8 || JPEG_LIB_VERSION_MINOR >= 3)
      int val;

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%d", &val) != 1)
	usage();
      if (val < 1 || val > 16)
	usage();
      cinfo->block_size = val;
#else
      fprintf(stderr, "%s: sorry, block size setting not supported\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "dct", 2)) {
      /* Select DCT algorithm. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (keymatch(argv[argn], "int", 1)) {
	cinfo->dct_method = JDCT_ISLOW;
      } else if (keymatch(argv[argn], "fast", 2)) {
	cinfo->dct_method = JDCT_IFAST;
      } else if (keymatch(argv[argn], "float", 2)) {
	cinfo->dct_method = JDCT_FLOAT;
      } else
	usage();

    } else if (keymatch(arg, "debug", 1) || keymatch(arg, "verbose", 1)) {
      /* Enable debug printouts. */
      /* On first -d, print version identification */
      static boolean printed_version = FALSE;

      if (! printed_version) {
	fprintf(stderr, "Independent JPEG Group's CJPEG, version %s\n%s\n",
		JVERSION, JCOPYRIGHT);
	printed_version = TRUE;
      }
      cinfo->err->trace_level++;

    } else if (keymatch(arg, "grayscale", 2) || keymatch(arg, "greyscale",2)) {
      /* Force a monochrome JPEG file to be generated. */
      jpeg_set_colorspace(cinfo, JCS_GRAYSCALE);

    } else if (keymatch(arg, "rgb", 3) || keymatch(arg, "rgb1", 4)) {
      /* Force an RGB JPEG file to be generated. */
#if JPEG_LIB_VERSION_MAJOR >= 9
      /* Note: Entropy table assignment in jpeg_set_colorspace depends
       * on color_transform.
       */
      cinfo->color_transform = arg[3] ? JCT_SUBTRACT_GREEN : JCT_NONE;
#endif
      jpeg_set_colorspace(cinfo, JCS_RGB);

    } else if (keymatch(arg, "bgycc", 5)) {
      /* Force a big gamut YCC JPEG file to be generated. */
#if JPEG_LIB_VERSION_MAJOR >= 9 && \
      (JPEG_LIB_VERSION_MAJOR > 9 || JPEG_LIB_VERSION_MINOR >= 1)
      jpeg_set_colorspace(cinfo, JCS_BG_YCC);
#else
      fprintf(stderr, "%s: sorry, BG_YCC colorspace not supported\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "maxmemory", 3)) {
      /* Maximum memory in Kb (or Mb with 'm'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
	usage();
      if (ch == 'm' || ch == 'M')
	lval *= 1000L;
      cinfo->mem->max_memory_to_use = lval * 1000L;

    } else if (keymatch(arg, "nosmooth", 3)) {
      /* Suppress fancy downsampling. */
      cinfo->do_fancy_downsampling = FALSE;

    } else if (keymatch(arg, "optimize", 1) || keymatch(arg, "optimise", 1)) {
      /* Enable entropy parm optimization. */
#ifdef ENTROPY_OPT_SUPPORTED
      cinfo->optimize_coding = TRUE;
#else
      fprintf(stderr, "%s: sorry, entropy optimization was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "outfile", 4)) {
      /* Set output file name. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      outfilename = argv[argn];	/* save it away for later use */

    } else if (keymatch(arg, "progressive", 1)) {
      /* Select simple progressive mode. */
#ifdef C_PROGRESSIVE_SUPPORTED
      simple_progressive = TRUE;
      /* We must postpone execution until num_components is known. */
#else
      fprintf(stderr, "%s: sorry, progressive output was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "quality", 1)) {
      /* Quality ratings (quantization table scaling factors). */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      qualityarg = argv[argn];

    } else if (keymatch(arg, "qslots", 2)) {
      /* Quantization table slot numbers. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      qslotsarg = argv[argn];
      /* Must delay setting qslots until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default quant table numbers.
       */

    } else if (keymatch(arg, "qtables", 2)) {
      /* Quantization tables fetched from file. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      qtablefile = argv[argn];
      /* We postpone actually reading the file in case -quality comes later. */

    } else if (keymatch(arg, "restart", 1)) {
      /* Restart interval in MCU rows (or in MCUs with 'b'). */
      long lval;
      char ch = 'x';

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%ld%c", &lval, &ch) < 1)
	usage();
      if (lval < 0 || lval > 65535L)
	usage();
      if (ch == 'b' || ch == 'B') {
	cinfo->restart_interval = (unsigned int) lval;
	cinfo->restart_in_rows = 0; /* else prior '-restart n' overrides me */
      } else {
	cinfo->restart_in_rows = (int) lval;
	/* restart_interval will be computed during startup */
      }

    } else if (keymatch(arg, "sample", 2)) {
      /* Set sampling factors. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      samplearg = argv[argn];
      /* Must delay setting sample factors until after we have processed any
       * colorspace-determining switches, since jpeg_set_colorspace sets
       * default sampling factors.
       */

    } else if (keymatch(arg, "scale", 4)) {
      /* Scale the image by a fraction M/N. */
      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%d/%d",
		 &cinfo->scale_num, &cinfo->scale_denom) != 2)
	usage();

    } else if (keymatch(arg, "scans", 4)) {
      /* Set scan script. */
#ifdef C_MULTISCAN_FILES_SUPPORTED
      if (++argn >= argc)	/* advance to next argument */
	usage();
      scansarg = argv[argn];
      /* We must postpone reading the file in case -progressive appears. */
#else
      fprintf(stderr, "%s: sorry, multi-scan output was not compiled\n",
	      progname);
      exit(EXIT_FAILURE);
#endif

    } else if (keymatch(arg, "smooth", 2)) {
      /* Set input smoothing factor. */
      int val;

      if (++argn >= argc)	/* advance to next argument */
	usage();
      if (sscanf(argv[argn], "%d", &val) != 1)
	usage();
      if (val < 0 || val > 100)
	usage();
      cinfo->smoothing_factor = val;

    } else if (keymatch(arg, "targa", 1)) {
      /* Input file is Targa format. */
      is_targa = TRUE;

    } else {
      usage();			/* bogus switch */
    }
  }

  /* Post-switch-scanning cleanup */

  if (for_real) {

    /* Set quantization tables for selected quality. */
    /* Some or all may be overridden if -qtables is present. */
    if (qualityarg != NULL)	/* process -quality if it was present */
      if (! set_quality_ratings(cinfo, qualityarg, force_baseline))
	usage();

    if (qtablefile != NULL)	/* process -qtables if it was present */
      if (! read_quant_tables(cinfo, qtablefile, force_baseline))
	usage();

    if (qslotsarg != NULL)	/* process -qslots if it was present */
      if (! set_quant_slots(cinfo, qslotsarg))
	usage();

    if (samplearg != NULL)	/* process -sample if it was present */
      if (! set_sample_factors(cinfo, samplearg))
	usage();

#ifdef C_PROGRESSIVE_SUPPORTED
    if (simple_progressive)	/* process -progressive; -scans can override */
      jpeg_simple_progression(cinfo);
#endif

#ifdef C_MULTISCAN_FILES_SUPPORTED
    if (scansarg != NULL)	/* process -scans if it was present */
      if (! read_scan_script(cinfo, scansarg))
	usage();
#endif
  }

  return argn;			/* return index of next arg (file name) */
}


/*
 * Check for overwrite of an existing file; clear it with user
 */

#ifndef NO_OVERWRITE_CHECK

LOCAL(boolean)
is_write_ok (char * outfname)
{
  FILE * ofile;
  int ch;

  ofile = fopen(outfname, READ_BINARY);
  if (ofile == NULL)
    return TRUE;		/* not present */
  fclose(ofile);		/* oops, it is present */

  for (;;) {
    fprintf(stderr, "%s already exists, overwrite it? [y/n] ",
	    outfname);
    fflush(stderr);
    ch = getc(stdin);
    if (ch != '\n')		/* flush rest of line */
      while (getc(stdin) != '\n')
	/* nothing */;

    switch (ch) {
    case 'Y':
    case 'y':
      return TRUE;
    case 'N':
    case 'n':
      return FALSE;
    /* otherwise, ask again */
    }
  }
}

#endif


/*
 * Process a single input file name, and return its index in argv[].
 * File names at or to left of old_file_index have been processed already.
 */

LOCAL(int)
process_one_file (int argc, char **argv, int old_file_index)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  char *infilename;
  char workfilename[PATH_MAX];
#ifdef PROGRESS_REPORT
  struct cdjpeg_progress_mgr progress;
#endif
  int file_index;
  cjpeg_source_ptr src_mgr;
  FILE * input_file = NULL;
  FILE * output_file = NULL;
  JDIMENSION num_scanlines;

  /* Initialize the JPEG compression object with default error handling. */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message = JMSG_LASTADDONCODE;

  /* Now safe to enable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
  enable_signal_catcher((j_common_ptr) &cinfo);
#endif

  /* Initialize JPEG parameters.
   * Much of this may be overridden later.
   * In particular, we don't yet know the input file's color space,
   * but we need to provide some value for jpeg_set_defaults() to work.
   */

  cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
  jpeg_set_defaults(&cinfo);

  /* Scan command line to find next file name.
   * It is convenient to use just one switch-parsing routine, but the switch
   * values read here are ignored; we will rescan the switches after opening
   * the input file.
   */

  file_index = parse_switches(&cinfo, argc, argv, old_file_index, FALSE);
  if (file_index >= argc) {
    fprintf(stderr, "%s: missing input file name\n", progname);
    usage();
  }

  /* Open the input file. */
  infilename = argv[file_index];
  if ((input_file = fopen(infilename, READ_BINARY)) == NULL) {
    fprintf(stderr, "%s: can't open %s\n", progname, infilename);
    goto fail;
  }

#ifdef PROGRESS_REPORT
  start_progress_monitor((j_common_ptr) &cinfo, &progress);
#endif

  /* Figure out the input file format, and set up to read it. */
  src_mgr = select_file_type(&cinfo, input_file);
  src_mgr->input_file = input_file;

  /* Read the input file header to obtain file size & colorspace. */
  (*src_mgr->start_input) (&cinfo, src_mgr);

  /* Now that we know input colorspace, fix colorspace-dependent defaults */
  jpeg_default_colorspace(&cinfo);

  /* Adjust default compression parameters by re-parsing the options */
  file_index = parse_switches(&cinfo, argc, argv, old_file_index, TRUE);

  /* If user didn't supply -outfile switch, select output file name. */
  if (outfilename == NULL) {
    int i;

    outfilename = workfilename;
    /* Make outfilename be infilename with .jpg substituted for extension */
    strcpy(outfilename, infilename);
    for (i = (int)strlen(outfilename)-1; i >= 0; i--) {
      switch (outfilename[i]) {
      case ':':
      case '/':
      case '\\':
	i = 0;			/* stop scanning */
	break;
      case '.':
	outfilename[i] = '\0';	/* lop off existing extension */
	i = 0;			/* stop scanning */
	break;
      default:
	break;			/* keep scanning */
      }
    }
    strcat(outfilename, ".jpg");
  }

  fprintf(stderr, "Compressing %s => %s\n", infilename, outfilename);
#ifndef NO_OVERWRITE_CHECK
  if (! is_write_ok(outfilename))
    goto fail;
#endif

  /* Open the output file. */
  if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL) {
    fprintf(stderr, "%s: can't create %s\n", progname, outfilename);
    goto fail;
  }

  /* Specify data destination for compression */
  jpeg_stdio_dest(&cinfo, output_file);

  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

  /* Process data */
  while (cinfo.next_scanline < cinfo.image_height) {
    num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
    (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
  }

  /* Finish compression and release memory */
  (*src_mgr->finish_input) (&cinfo, src_mgr);
  jpeg_finish_compress(&cinfo);

  /* Clean up and exit */
fail:
  jpeg_destroy_compress(&cinfo);

  if (input_file != NULL) fclose(input_file);
  if (output_file != NULL) fclose(output_file);

#ifdef PROGRESS_REPORT
  end_progress_monitor((j_common_ptr) &cinfo);
#endif

  /* Disable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
  enable_signal_catcher((j_common_ptr) NULL);
#endif

  return file_index;
}


/*
 * The main program.
 */

int
main (int argc, char **argv)
{
  int file_index;

  /* On Mac, fetch a command line. */
#ifdef USE_CCOMMAND
  argc = ccommand(&argv);
#endif

#ifdef MSDOS
  progname = "cjpeg";		/* DOS tends to be too verbose about argv[0] */
#else
  progname = argv[0];
  if (progname == NULL || progname[0] == 0)
    progname = "cjpeg";		/* in case C library doesn't provide it */
#endif

  /* The default maxmem must be computed only once at program startup,
   * since releasing memory with free() won't give it back to the OS.
   */
#ifdef FREE_MEM_ESTIMATE
  default_maxmem = FREE_MEM_ESTIMATE;
#else
  default_maxmem = 0;
#endif

  /* Scan command line, parse switches and locate input file names */

  if (argc < 2)
    usage();			/* nothing on the command line?? */

  file_index = 0;

  while (file_index < argc-1)
    file_index = process_one_file(argc, argv, file_index);

  /* All done. */
  exit(EXIT_SUCCESS);
  return 0;			/* suppress no-return-value warnings */
}
