// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 2001
//
// CoderInfo implementation
//

#define MAGICK_IMPLEMENTATION

#include "Magick++/CoderInfo.h"
#include "Magick++/Exception.h"
#include "Magick++/Include.h"

Magick::CoderInfo::CoderInfo ( const std::string &name_ )
  : _name(),
    _description(),
    _isReadable(false),
    _isWritable(false),
    _isMultiFrame(false)
{
  ExceptionInfo exceptionInfo;
  GetExceptionInfo( &exceptionInfo );
  Magick::MagickInfo *magickInfo = GetMagickInfo( name_.c_str(), &exceptionInfo );
  throwException( exceptionInfo );
  if( !magickInfo )
    throwExceptionExplicit(OptionError,
                           "Coder not found",
                           name_.c_str() );

  _name         = string(magickInfo->name);
  _description  = string(magickInfo->description);
  _isReadable   = static_cast<bool>(magickInfo->decoder);
  _isWritable   = static_cast<bool>(magickInfo->encoder);
  _isMultiFrame = static_cast<bool>(magickInfo->adjoin);
}

Magick::CoderInfo::~CoderInfo ( void )
{
  // Nothing to do
}

// Format name
std::string Magick::CoderInfo::name( void ) const
{
  return _name;
}

// Format description
std::string Magick::CoderInfo::description( void ) const
{
  return _description;
}

// Format is readable
bool Magick::CoderInfo::isReadable( void ) const
{
  return _isReadable;
}

// Format is writeable
bool Magick::CoderInfo::isWritable( void ) const
{
  return _isWritable;
}

// Format supports multiple frames
bool Magick::CoderInfo::isMultiFrame( void ) const
{
  return _isMultiFrame;
}

// Construct from MagickLib::MagickInfo*
Magick::CoderInfo::CoderInfo ( const MagickLib::MagickInfo *magickInfo_ )
  : _name(string(magickInfo_->name)),
    _description(string(magickInfo_->description)),
    _isReadable(static_cast<bool>(magickInfo_->decoder)),
    _isWritable(static_cast<bool>(magickInfo_->encoder)),
    _isMultiFrame(static_cast<bool>(magickInfo_->adjoin))
{
  // Nothing more to do
}

