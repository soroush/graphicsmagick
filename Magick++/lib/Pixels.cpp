// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Pixels Implementation
//

#define MAGICK_IMPLEMENTATION

#include <string> // This is here to compile with Visual C++
#include "Magick++/Thread.h"
#include "Magick++/Exception.h"
#include "Magick++/Pixels.h"

namespace Magick
{


}

// Construct pixel view using specified image.
Magick::Pixels::Pixels( Magick::Image &image_ )
  : _image(image_),
    _view(OpenCacheView(image_.image())),
    _x(0),
    _y(0),
    _columns(0),
    _rows(0)
{
  if (!_view)
    throwExceptionExplicit( ResourceLimitError, "Out of pixel views" );
}

// Destroy pixel view
Magick::Pixels::~Pixels( void )
{
  if ( _view )
    CloseCacheView( _view );
}

// Transfer pixels from the image to the pixel view as defined by
// the specified region. Modified pixels may be subsequently
// transferred back to the image via sync.
    
Magick::PixelPacket* Magick::Pixels::get ( unsigned int x_,
					   unsigned int y_,
					   unsigned int columns_,
					   unsigned int rows_ )
{
  if ( ( x_ + columns_ > _image.columns()) ||
       ( y_ + rows_ > _image.rows()) )
    throwExceptionExplicit( MagickLib::OptionError,
			    "View requested outside of image" );
  _x = x_;
  _y = y_;
  _columns = columns_;
  _rows = rows_;

  PixelPacket* pixels = GetCacheView( _view, x_, y_,
                                      columns_, rows_ );
  if ( pixels == 0 )
    throwExceptionExplicit( OptionError, "Failed to get pixels" );
  
  return pixels;
}

// Transfers the image view pixels to the image.
void Magick::Pixels::sync ( void )
{
  if( !SyncCacheView( _view ) )
    throwExceptionExplicit( OptionError, "Failed to sync pixels" );
}
    
// Allocate a pixel view region to store image pixels as defined
// by the region rectangle.  This area is subsequently transferred
// from the pixel view to the image via 'sync'.
Magick::PixelPacket* Magick::Pixels::set ( unsigned int x_,
					   unsigned int y_,
					   unsigned int columns_,
					   unsigned int rows_ )
{
  if ( ( x_ + columns_ > _image.columns()) ||
       ( y_ + rows_ > _image.rows()) )
    throwExceptionExplicit( OptionError,
			    "View requested outside of image" );

  _x = x_;
  _y = y_;
  _columns = columns_;
  _rows = rows_;

  PixelPacket* pixels = SetCacheView( _view, x_, y_,
                                      columns_, rows_ );
  if ( !pixels )
    throwExceptionExplicit( OptionError, "Failed to set pixels" );
  
  return pixels;
}

// Return pixel colormap index array
Magick::IndexPacket* Magick::Pixels::indexes ( void )
{
  IndexPacket* indexes = GetCacheViewIndexes( _view );

  if ( !indexes )
    throwExceptionExplicit( OptionError,
			    "Image does not contain index channel");

  return indexes;
}
