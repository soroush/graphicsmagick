// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Implementation of Montage
//

#define MAGICK_IMPLEMENTATION

#include <string>

#include "Magick++/Montage.h"
#include "Magick++/Functions.h"


void Magick::Montage::updateMontageInfo ( MontageInfo &montageInfo_ ) const
{
  if ( _backgroundColor.isValid() )
    montageInfo_.background_color = _backgroundColor;

  if ( _compose != Magick::UndefinedCompositeOp )
    montageInfo_.compose = _compose;

  if ( _fileName.length() != 0 )
    {
      _fileName.copy( montageInfo_.filename, MaxTextExtent - 1 );
      montageInfo_.filename[ _fileName.length() ] = 0; // null terminate
    }

  if ( _font.length() != 0 )
    Magick::CloneString( &montageInfo_.font, _font );

  if ( _geometry.isValid() )
    Magick::CloneString( &montageInfo_.geometry, _geometry );

  montageInfo_.gravity = _gravity;

  if ( _pen.isValid() )
    {
      montageInfo_.stroke = _pen;
      montageInfo_.fill = _pen;
    }

  if ( _pointSize != 0 )
    montageInfo_.pointsize = _pointSize;

  if ( _shadow == true )
    montageInfo_.shadow = static_cast<int>(true);

  if ( _texture.length() != 0 )
    Magick::CloneString( &montageInfo_.texture, _texture );

  if ( _tile.isValid() )
    Magick::CloneString( &montageInfo_.tile, _tile );

  if ( _title.length() != 0 )
    Magick::CloneString( &montageInfo_.title, _title );
}


//
// Implementation of MontageFramed
//


/* virtual */ void
Magick::MontageFramed::updateMontageInfo ( MontageInfo &montageInfo_ ) const
{
  // Do base updates
  Montage::updateMontageInfo ( montageInfo_ );

  if ( _borderColor.isValid() )
    montageInfo_.border_color = _borderColor;

  if ( _borderWidth != 0 )
    montageInfo_.border_width = _borderWidth;

  if ( _frame.isValid() )
    Magick::CloneString( &montageInfo_.frame, _frame );

  if ( _matteColor.isValid() )
    montageInfo_.matte_color = _matteColor;
}
