/*
 *
 * Test BLOB operations via write/read/write/read sequence to detect
 * any data corruption problems. This does not verify that the image is
 * correct, only that encode/decode process is repeatable.
 *
 * Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 *
 * The image returned by both reads must be identical in order for the
 * test to pass.
 *
 */

#if !defined(_VISUALC_)
#include <magick_config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>

int main ( int argc, char **argv )
{
  Image
    *final = (Image *) NULL,
    *original = (Image *) NULL;

  size_t
    blob_length = 0;

  char
    *blob = NULL,
    infile[MaxTextExtent],
    format[MaxTextExtent],
    *size = NULL;

  int
    arg = 1,
    rows = 0,
    columns = 0;

  double
    fuzz_factor = 0;

	ErrorInfo
	  error_info;

  ImageInfo
    imageInfo;

  ExceptionInfo
    exception;

  if (LocaleNCompare("rwblob",argv[0],7) == 0)
    InitializeMagick((char *) NULL);
  else
    InitializeMagick(*argv);

  GetImageInfo( &imageInfo );
  GetExceptionInfo( &exception );

  for (arg=1; arg < argc; arg++)
    {
      char
        *option = argv[arg];
    
      if (*option == '-')
        {
          if (LocaleCompare("compress",option+1) == 0)
            {
              arg++;
              option=argv[arg];
              imageInfo.compression=UndefinedCompression;
              if (LocaleCompare("None",option) == 0)
                imageInfo.compression=NoCompression;
              if (LocaleCompare("BZip",option) == 0)
                imageInfo.compression=BZipCompression;
              if (LocaleCompare("Fax",option) == 0)
                imageInfo.compression=FaxCompression;
              if (LocaleCompare("Group4",option) == 0)
                imageInfo.compression=Group4Compression;
              if (LocaleCompare("JPEG",option) == 0)
                imageInfo.compression=JPEGCompression;
              if (LocaleCompare("Lossless",option) == 0)
                imageInfo.compression=LosslessJPEGCompression;
              if (LocaleCompare("LZW",option) == 0)
                imageInfo.compression=LZWCompression;
              if (LocaleCompare("RLE",option) == 0)
                imageInfo.compression=RunlengthEncodedCompression;
              if (LocaleCompare("Zip",option) == 0)
                imageInfo.compression=ZipCompression;
            }
          else if (LocaleCompare("depth",option+1) == 0)
            {
              imageInfo.depth=QuantumDepth;
              arg++;
              if ((arg == argc) || !sscanf(argv[arg],"%ld",&imageInfo.depth))
                {
                  printf("-depth argument missing or not integer\n");
                  exit (1);
                }
              if(imageInfo.depth != 8 && imageInfo.depth != 16 && imageInfo.depth != 32)
                {
                  printf("-depth (%ld) not 8, 16, or 32\n", imageInfo.depth);
                  exit (1);
                }
            }
          else if (LocaleCompare("size",option+1) == 0)
            {
              arg++;
              if ((arg == argc) || !IsGeometry(argv[arg]))
                {
                  printf("-size argument missing or not geometry\n");
                  exit (1);
                }
              (void) CloneString(&imageInfo.size,argv[arg]);
            }
        }
      else
        break;
    }
  if (arg != argc-2)
    {
      printf("arg=%d, argc=%d\n", arg, argc);
      printf ( "Usage: %s [-compress algorithm -depth integer -size geometry] infile format\n", argv[0] );
      exit( 1 );
    }

  strncpy(infile, argv[arg], MaxTextExtent-1 );
  arg++;
  strncpy( format, argv[arg], MaxTextExtent-1 );

  for (arg=0; arg < argc; arg++)
    printf("%s ", argv[arg]);
  printf("\n");
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  GetExceptionInfo( &exception );
  imageInfo.dither = 0;
  strncpy( imageInfo.filename, infile, MaxTextExtent-1 );

  original = ReadImage ( &imageInfo, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read original image %s\n", imageInfo.filename );
      exit(1);
    }

  /*
   * Obtain original image size if format requires it
   */
  rows    = original->rows;
  columns = original->columns;
  size = (char *) AcquireMemory( 40 );
  sprintf( size, "%dx%d", columns, rows );

  /*
   * Save image to BLOB
   */
  blob_length = 8192;
  strncpy( original->magick, format, MaxTextExtent-1 );
  strcpy( imageInfo.filename, "" );
  original->delay = 10;
  blob =(char *) ImageToBlob ( &imageInfo, original, &blob_length, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      exit(1);
    }
  imageInfo.depth=original->depth;
  DestroyImage( original );
  original = (Image*)NULL;

  /*
   * Read image back from BLOB
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strcpy( imageInfo.filename, "" );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  original = BlobToImage( &imageInfo, blob, blob_length, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n",imageInfo.magick );
      exit(1);
    }
  LiberateMemory( (void**)&blob );

  /*
   * Save image to BLOB
   */
  blob_length = 8192;
  strncpy( original->magick, format, MaxTextExtent-1 );
  strcpy( imageInfo.filename, "" );
  original->delay = 10;
  blob = (char *) ImageToBlob ( &imageInfo, original, &blob_length, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  imageInfo.depth=original->depth;
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      exit(1);
    }

  /*
   * Read image back from BLOB
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strcpy( imageInfo.filename, "" );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  final = BlobToImage( &imageInfo, blob, blob_length, &exception );
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n",imageInfo.magick );
      exit(1);
    }
  LiberateMemory( (void**)&blob );

  /*
   * Check final output
   */

  if ( !strcmp( "DPX", format ) ||
       !strcmp( "JPEG", format ) ||
       !strcmp( "JPG", format ) ||
       !strcmp( "JPG24", format ) ||
       !strcmp( "JP2", format ) ||
       !strcmp( "GRAY", format ) ||
       !strcmp( "CMYK", format ) ||
       !strcmp( "PAL", format ) ||
       !strcmp( "PCD", format ) ||
       !strcmp( "PCDS", format ) ||
       !strcmp( "UYVY", format ) ||
       !strcmp( "YUV", format ) )
    fuzz_factor = 0.008;
  
  if ( !IsImagesEqual(original, final, &error_info, &exception ) &&
       (error_info.normalized_mean_error > fuzz_factor) )
    {
      printf( "R/W file check for format \"%s\" failed: %u/%.6f/%.6fe\n",
        format,(unsigned int) error_info.mean_error_per_pixel,error_info.normalized_mean_error,
        error_info.normalized_maximum_error);
      fflush(stdout);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  DestroyMagick();

  return 0;
}
