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
  Image *original = (Image *)NULL;
  Image *final = (Image *)NULL;
  char * blob = NULL;
  char infile[MaxTextExtent];
  char format[MaxTextExtent];
  size_t blob_length = 0;
  char *size = NULL;
  int rows, columns = 0;
  double fuzz_factor = 0;
  ImageInfo imageInfo;
  ExceptionInfo exception;

  if ( argc != 3 )
    {
      printf ( "Usage: %s infile format\n", argv[0] );
      exit( 1 );
    }

  if (LocaleNCompare("rwblob",argv[0],7) == 0)
    InitializeMagick(GetExecutionPath(argv[0]));
  else
    InitializeMagick(*argv);

  strncpy( infile, argv[1], MaxTextExtent-1 );
  strncpy( format, argv[2], MaxTextExtent-1 );

  printf("%s %s %s\n",
	  argv[0],infile, format );
  fflush(stdout);

  /*
   * Read original image
   */
  GetImageInfo( &imageInfo );
  imageInfo.dither = 0;
  strncpy( imageInfo.filename, infile, MaxTextExtent-1 );

  original = ReadImage ( &imageInfo, &exception );
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read original image %s\n", imageInfo.filename );
      MagickError(exception.severity,exception.reason,exception.description);
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
  blob = (char *) ImageToBlob ( &imageInfo, original, &blob_length, &exception );
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      MagickError(exception.severity,exception.reason,exception.description);
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
  if ( original == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n",imageInfo.magick );
      MagickError(exception.severity,exception.reason,exception.description);
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
  imageInfo.depth=original->depth;
  if ( blob == NULL )
    {
      printf ( "Failed to write BLOB in format %s\n", imageInfo.magick );
      MagickError(exception.severity,exception.reason,exception.description);
    }

  /*
   * Read image back from BLOB
   */
  strncpy( imageInfo.magick, format, MaxTextExtent-1 );
  strcpy( imageInfo.filename, "" );
  if ( size != NULL )
    CloneString( &imageInfo.size, size );
  final = BlobToImage( &imageInfo, blob, blob_length, &exception );
  if ( final == (Image *)NULL )
    {
      printf ( "Failed to read image from BLOB in format %s\n",imageInfo.magick );
      MagickError(exception.severity,exception.reason,exception.description);
    }
  LiberateMemory( (void**)&blob );

  /*
   * Check final output
   */

  if ( !strcmp( "JPEG", format ) ||
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
  
  if ( !IsImagesEqual(original, final ) &&
       (original->normalized_mean_error > fuzz_factor) )
    {
      printf( "R/W file check for format \"%s\" failed: %u/%.6f/%.6fe\n",
        format,(unsigned int) original->mean_error_per_pixel,original->normalized_mean_error,
        original->normalized_maximum_error);
      fflush(stdout);
    }

  DestroyImage( original );
  original = (Image*)NULL;
  DestroyImage( final );
  final = (Image*)NULL;

  return 0;
}
