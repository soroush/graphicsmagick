// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001
//
// Implementation of Drawable (Graphic objects)
//

#define MAGICK_IMPLEMENTATION

#include <math.h>
#include <string>
#include <iostream>
#include <strstream>

#include "Magick++/Drawable.h"
#include "Magick++/Image.h"

using namespace std;

#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)
#define RadiansToDegrees(x) (180.0*(x)/3.14159265358979323846)

//Write Coordinate to stream
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::Coordinate& coordinate_)
{
  stream_ << coordinate_.x() << "," << coordinate_.y();
  return stream_;
}

// Write DrawableBase to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::DrawableBase& drawable_)
{
  const_cast<const Magick::DrawableBase&>(drawable_).print (stream_);
  return stream_;
}

// Write Drawable to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::Drawable& drawable_)
  
{
  if (drawable_.dp != 0)
    stream_ << *drawable_.dp;
  return stream_;
}

// Write VPathBase to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::VPathBase& drawable_)
{
  const_cast<const Magick::VPathBase&>(drawable_).print (stream_);
  return stream_;
}

// Write Path to stream
std::ostream& Magick::operator<< (std::ostream& stream_,
                                  const Magick::VPath& drawable_)
  
{
  if (drawable_.dp != 0)
    stream_ << *drawable_.dp;
  return stream_;
}

//
// Drawable Objects
//

// Affine (scaling, rotation, and translation)
void Magick::DrawableAffine::print (std::ostream& stream_) const
{
  stream_ << "affine "
          << _sx
          << ","
          << _rx
          << ","
          << _ry
          << ","
          << _sy
          << ","
          << _tx
          << ","
          << _ty;
}

// Angle (text drawing angle)
void Magick::DrawableAngle::print (std::ostream& stream_) const
{
  stream_ << "angle "
          << _angle;
}

// Arc
void Magick::DrawableArc::print (std::ostream& stream_) const
{
  stream_ << "arc"
          << " " << Magick::Coordinate(_startX,_startY)
          << " " << Magick::Coordinate(_endX,_endY)
          << " " << Magick::Coordinate(_startDegrees,_endDegrees);
}

// Bezier curve (Coordinate list must contain at least three members)
void Magick::DrawableBezier::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "bezier";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Circle
void Magick::DrawableCircle::print (std::ostream& stream_) const
{
  stream_ << "circle "
          << Magick::Coordinate( _originX, _originY )
          << " "
          << Magick::Coordinate( _perimX, _perimY );
}

// Colorize at point using PaintMethod
void Magick::DrawableColor::print (std::ostream& stream_) const
{
  stream_ << "color "
          << Magick::Coordinate(_x,_y)
          << " ";
  
  switch ( _paintMethod )
    {
    case PointMethod :
      stream_ << "point";
      break;
    case ReplaceMethod :
      stream_ << "replace";
      break;
    case FloodfillMethod :
      stream_ << "floodfill";
      break;
    case FillToBorderMethod :
      stream_ << "filltoborder";
      break;
    case ResetMethod :
      stream_ << "reset";
      break;
    default :
      {
	stream_ << "point";
      }
    }
}

// Drawable Path
void Magick::DrawablePath::print (std::ostream& stream_) const
{
  std::list<Magick::VPath>::const_iterator p = _path.begin();

  stream_ << "path '";
  while ( p != _path.end() )
    {
      stream_ << *p;
      p++;
    }
  stream_ << "'";
}

// Decoration (text decoration)
void Magick::DrawableTextDecoration::print (std::ostream& stream_) const
{
  stream_ << "decorate ";

  switch ( _decoration )
    {
    case NoDecoration:
      stream_ << "none";
      break;
    case UnderlineDecoration:
      stream_ << "underline";
      break;
    case OverlineDecoration:
      stream_ << "overline";
      break;
    case LineThroughDecoration:
      stream_ << "line-through";
      break;
    default :
      {
        stream_ << "none";
      }
    }
}

// Ellipse
void Magick::DrawableEllipse::print (std::ostream& stream_) const
{
  stream_ << "ellipse "
          << Magick::Coordinate(_originX,_originY)
	  << " " << _radiusX << "," << _radiusY
	  << " " << _arcStart << "," << _arcEnd;
}

// Specify drawing fill color
void Magick::DrawableFillColor::print (std::ostream& stream_) const
{
  stream_ << "fill "
          << string(_color);
}

// Specify drawing fill fule
void Magick::DrawableFillRule::print (std::ostream& stream_) const
{
  stream_ << "fill-rule ";

  switch (_fillRule)
    {
    case EvenOddRule :
      {
        stream_ << "evenodd";
        break;
      }
    case NonZeroRule :
      {
        stream_ << "nonzero";
        break;
      }
    default :
      {
        stream_ << "none";
      }
    }
}

// Specify drawing fill opacity
void Magick::DrawableFillOpacity::print (std::ostream& stream_) const
{
  stream_ << "fill-opacity "
          << _opacity;
}

// Specify text font
void Magick::DrawableFont::print (std::ostream& stream_) const
{
  stream_ << "font '";

  for ( unsigned int i = 0; i < _font.length(); ++i )
    {
      if ( _font[i] == '\'' )
	stream_ << "\\";
      stream_ << _font[i];
    }
  stream_  << "'";
}

// Specify text positioning gravity
void Magick::DrawableGravity::print (std::ostream& stream_) const
{
  stream_ << "gravity ";

  switch ( _gravity )
    {
    case ForgetGravity :
      {
        stream_ << "Forget";
        break;
      }
    case NorthWestGravity :
      {
        stream_ << "NorthWest";
        break;
      }
    case NorthGravity :
      {
        stream_ << "North";
        break;
      }
    case NorthEastGravity :
      {
        stream_ << "NorthEast";
        break;
      }
    case WestGravity :
      {
        stream_ << "West";
        break;
      }
    case CenterGravity :
      {
        stream_ << "Center";
        break;
      }
    case EastGravity :
      {
        stream_ << "East";
        break;
      }
    case SouthWestGravity :
      {
        stream_ << "SouthWest";
        break;
      }
    case SouthGravity :
      {
        stream_ << "South";
        break;
      }
    case SouthEastGravity :
      {
        stream_ << "SouthEast";
        break;
      }
    case StaticGravity :
      {
        stream_ << "Static";
        break;
      }

    default :
      {
        stream_ << "Forget";
      }
    }
}

// Draw image at point
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const std::string &filename_,
                                                         Magick::CompositeOperator composition_ )
  : _composition(composition_),
    _x(x_),
    _y(y_),
    _width(width_),
    _height(height_),
    _image(new Image(filename_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const Image &image_,
                                                         CompositeOperator composition_ )
  : _composition(composition_),
    _x(x_),
    _y(y_),
    _width(width_),
    _height(height_),
    _image(new Image(image_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const std::string &filename_ )
  :_composition(Magick::CopyCompositeOp),
   _x(x_),
   _y(y_),
   _width(width_),
   _height(height_),
   _image(new Image(filename_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         double width_, double height_,
                                                         const Image &image_ )
  :_composition(Magick::CopyCompositeOp),
   _x(x_),
   _y(y_),
   _width(width_),
   _height(height_),
   _image(new Image(image_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         const std::string &filename_ )
  : _composition(Magick::CopyCompositeOp),
    _x(x_),
    _y(y_),
    _width(0),
    _height(0),
    _image(new Image(filename_))
{
}
Magick::DrawableCompositeImage::DrawableCompositeImage ( double x_, double y_,
                                                         const Image &image_ )
  : _composition(Magick::CopyCompositeOp),
    _x(x_),
    _y(y_),
    _width(0),
    _height(0),
    _image(new Image(image_))
{
}
Magick::DrawableCompositeImage::~DrawableCompositeImage( void )
{
  delete _image;
}
void Magick::DrawableCompositeImage::image( const std::string &image_ )
{
  _image = new Image(image_);
}
std::string Magick::DrawableCompositeImage::image( void )
{
  return _image->fileName();
}

void Magick::DrawableCompositeImage::image( const Image &image_ )
{
  _image = new Image(image_);
}
Magick::Image Magick::DrawableCompositeImage::image( void ) const
{
  return *_image;
}
void Magick::DrawableCompositeImage::print (std::ostream& stream_) const
{
  stream_ << "image ";

  switch ( _composition )
    {
    case OverCompositeOp:
      stream_ << "Over ";
      break;
    case InCompositeOp:
      stream_ << "In ";
      break;
    case OutCompositeOp:
      stream_ << "Out ";
      break;
    case AtopCompositeOp:
      stream_ << "Atop ";
      break;
    case XorCompositeOp:
      stream_ << "Xor ";
      break;
    case PlusCompositeOp:
      stream_ << "Plus ";
      break;
    case MinusCompositeOp:
      stream_ << "Minus ";
      break;
    case AddCompositeOp:
      stream_ << "Add ";
      break;
    case SubtractCompositeOp:
      stream_ << "Subtract ";
      break;
    case DifferenceCompositeOp:
      stream_ << "Difference ";
      break;
    case MultiplyCompositeOp:
      stream_ << "Multiply ";
      break;
    case BumpmapCompositeOp:
      stream_ << "Bumpmap ";
      break;
    case CopyCompositeOp:
      stream_ << "Copy ";
      break;
    case CopyRedCompositeOp:
      stream_ << "CopyRed ";
      break;
    case CopyGreenCompositeOp:
      stream_ << "CopyGreen ";
      break;
    case CopyBlueCompositeOp:
      stream_ << "CopyBlue ";
      break;
    case CopyOpacityCompositeOp:
      stream_ << "CopyOpacity ";
      break;
    default:
        {
          stream_ << "Copy ";
        }
    }

  stream_ << Magick::Coordinate( _x, _y)
          << " "
          << Magick::Coordinate( _width, _height)
          << " 'mpr:" << _image->registerId() << "'";
}

// Line
void Magick::DrawableLine::print (std::ostream& stream_) const
{
  stream_ << "line "
          << Magick::Coordinate( _startX, _startY )
          << " "
          << Magick::Coordinate( _endX, _endY );
}

// Change pixel matte value to transparent using PaintMethod
void Magick::DrawableMatte::print (std::ostream& stream_) const
{
  stream_ << "matte "
          << Magick::Coordinate( _x, _y )
          << " ";
  
  switch ( _paintMethod )
    {
    case PointMethod :
      stream_ << "point";
      break;
    case ReplaceMethod :
      stream_ << "replace";
      break;
    case FloodfillMethod :
      stream_ << "floodfill";
      break;
    case FillToBorderMethod :
      stream_ << "filltoborder";
      break;
    case ResetMethod :
      stream_ << "reset";
      break;
    default :
      {
	stream_ << "point";
      }
    }
}

// Point
void Magick::DrawablePoint::print (std::ostream& stream_) const
{
  stream_ << "point "
          << Magick::Coordinate( _x, _y );
}

// Text pointsize
void Magick::DrawablePointSize::print (std::ostream& stream_) const
{
  stream_ << "font-size "
          << _pointSize;
}

// Polygon (Coordinate list must contain at least three members)
void Magick::DrawablePolygon::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "polygon";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Polyline (Coordinate list must contain at least three members)
void Magick::DrawablePolyline::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "polyline";
  while ( p != _coordinates.end() )
    {
      stream_ << " "
	      << *p;
      p++;
    }
}

// Pop Graphic Context
void Magick::DrawablePopGraphicContext::print (std::ostream& stream_) const
{
  stream_ << "pop graphic-context";
}

// Push Graphic Context
void Magick::DrawablePushGraphicContext::print (std::ostream& stream_) const
{
  stream_ << "push graphic-context";
}

// Pop (terminate) Pattern definition
void Magick::DrawablePopPattern::print (std::ostream& stream_) const
{
  stream_ << "pop pattern";
}

// Push Pattern definition
void Magick::DrawablePushPattern::print (std::ostream& stream_) const
{
  stream_ << "push pattern "
          << _id
          << " "
          << _x << "," << _y
          << " "
          << _width << "," << _height;
}

// Rectangle
void Magick::DrawableRectangle::print (std::ostream& stream_) const
{
  stream_ << "rectangle "
          << Magick::Coordinate( _upperLeftX, _upperLeftY )
          << " "
          << Magick::Coordinate( _lowerRightX, _lowerRightY );
}

// Apply Rotation
void Magick::DrawableRotation::print (std::ostream& stream_) const
{
  stream_ << "rotate " << _angle;
}

// Round Rectangle
void Magick::DrawableRoundRectangle::print (std::ostream& stream_) const
{
  stream_ << "roundRectangle"
          << " " << Magick::Coordinate(_centerX,_centerY)
          << " " << Magick::Coordinate(_width,_hight)
          << " " << Magick::Coordinate(_cornerWidth,_cornerHeight);
}

// Apply Scaling
void Magick::DrawableScaling::print (std::ostream& stream_) const
{
  stream_ << "scale "  << _x << " " << _y;
}

// Apply Skew in the X direction
void Magick::DrawableSkewX::print (std::ostream& stream_) const
{
  stream_ << "skewX " << _angle;
}

// Apply Skew in the Y direction
void Magick::DrawableSkewY::print (std::ostream& stream_) const
{
  stream_ << "skewY " << _angle;
}

// Stroke dasharray
Magick::DrawableDashArray::DrawableDashArray( const unsigned int* dasharray_ )
  : _dasharray(0)
{
  dasharray( dasharray_ );
}
void Magick::DrawableDashArray::dasharray( const unsigned int* dasharray_ )
{
  LiberateMemory(reinterpret_cast<void**>(&_dasharray));

  if(dasharray_)
    {
      // Count elements in dash array
      int x;
      for (x=0; dasharray_[x]; x++);
      // Allocate elements
      _dasharray=static_cast<unsigned int*>(AcquireMemory((x+1)*sizeof(unsigned int)));
      // Copy elements
      memcpy(_dasharray,dasharray_,
             (x+1)*sizeof(unsigned int));
    }
}
void Magick::DrawableDashArray::print (std::ostream& stream_) const
{
  stream_ << "stroke-dasharray ";
  if( _dasharray )
    {
      int x;
      for (x=0; _dasharray[x]; x++)
        {
          if(x)
            stream_ << ",";
          stream_ << _dasharray[x];
        }
    }
  else
    {
      stream_ << "none";
    }
}

// Stroke dashoffset
void Magick::DrawableDashOffset::print (std::ostream& stream_) const
{
  stream_ << "stroke-dashoffset " << _offset;
}

// Stroke linecap
void Magick::DrawableStrokeLineCap::print (std::ostream& stream_) const
{
  stream_ << "stroke-linecap ";

  switch( _linecap )
    {
    case ButtCap :
      stream_ << "butt";
      break;
    case RoundCap :
      stream_ << "round";
      break;
    case SquareCap :
      stream_ << "square";
      break;
    case UndefinedCap :
    default :
      {
        stream_ << "none";
      }
    }
}

// Stroke linejoin
void Magick::DrawableStrokeLineJoin::print (std::ostream& stream_) const
{
  stream_ << "stroke-linejoin ";

  switch( _linejoin )
    {
    case MiterJoin :
      stream_ << "miter";
      break;
    case RoundJoin :
      stream_ << "round";
      break;
    case BevelJoin :
      stream_ << "bevel";
      break;
    case UndefinedJoin :
    default :
      {
        stream_ << "none";
      }
    }
}

// Stroke miterlimit
void Magick::DrawableMiterLimit::print (std::ostream& stream_) const
{
  stream_ << "stroke-miterlimit " << _miterlimit;
}

// Stroke color
void Magick::DrawableStrokeColor::print (std::ostream& stream_) const
{
  stream_ << "stroke "
          << string(_color);
}

// Stroke antialias
void Magick::DrawableStrokeAntialias::print (std::ostream& stream_) const
{
  stream_ << "stroke-antialias ";
  if ( _flag )
    stream_ << "1";
  else
    stream_ << "0";
}

// Stroke opacity
void Magick::DrawableStrokeOpacity::print (std::ostream& stream_) const
{
  stream_ << "stroke-opacity "
          << _opacity;
}

// Stroke width
void Magick::DrawableStrokeWidth::print (std::ostream& stream_) const
{
  stream_ << "stroke-width "
          << _width;
}

// Draw text at point
void Magick::DrawableText::print (std::ostream& stream_) const
{
  stream_ << "text "
          << Magick::Coordinate( _x, _y)
	  << " '";
  for ( unsigned int i = 0; i < _text.length(); ++i )
    {
      if ( _text[i] == '\'' )
	stream_ << "\\";
      stream_ << _text[i];
    }
  stream_ << "\'";
}

// Apply Translation
void Magick::DrawableTranslation::print (std::ostream& stream_) const
{
  stream_ << "translate " << _x << "," << _y;
}

// Text antialias
void Magick::DrawableTextAntialias::print (std::ostream& stream_) const
{
  stream_ << "text-antialias ";
  if ( _flag )
    stream_ << "1";
  else
    stream_ << "0";
}

//
// Path Classes
//

//
// PathArcAbs
//

// Write PathArcArgs to stream
std::ostream& Magick::operator<<( std::ostream& stream_, const Magick::PathArcArgs& args_ )
{
  stream_ << args_.radiusX()
          << ","
          << args_.radiusY()
          << " "
          << args_.xAxisRotation()
          << " ";

  if ( args_.largeArcFlag() )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  if ( args_.sweepFlag() )
    stream_ << "1 ";
  else
    stream_ << "0 ";

  stream_ << args_.x()
          << ","
          << args_.y();

  return stream_;
}
// Path Arc
void Magick::PathArcAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();

  stream_ << "A";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathArcRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathArcArgs>::const_iterator p = _coordinates.begin();

  stream_ << "a";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Closepath
//
// Support a polymorphic print-to-stream operator
void Magick::PathClosePath::print (std::ostream& stream_) const
{
  stream_ << "z ";
}

//
// Path Curveto (Cubic Bezier)
//
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::PathCurvetoArgs& args_ )
{
  stream_ 
    << args_.x1()
    << " "
    << args_.y1()
    << " "
    << args_.x2()
    << " "
    << args_.y2()
    << " "
    << args_.x()
    << " "
    << args_.y();
  return stream_;
}
// Support a polymorphic print-to-stream operator
void Magick::PathCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "C";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "c";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
// Support a polymorphic print-to-stream operator
void Magick::PathSmoothCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "S";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "s";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}


//
// Quadratic Curveto (Quadratic Bezier)
//
std::ostream& Magick::operator<<( std::ostream& stream_,
                                  const Magick::PathQuadraticCurvetoArgs& args_)
{
  stream_ 
    << args_.x1()
    << " "
    << args_.y1()
    << " "
    << args_.x()
    << " "
    << args_.y();
  return stream_;
}
// Support a polymorphic print-to-stream operator
void Magick::PathQuadraticCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "Q";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathQuadraticCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::PathQuadraticCurvetoArgs>::const_iterator p = _args.begin();

  stream_ << "q";
  while ( p != _args.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothQuadraticCurvetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "T";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathSmoothQuadraticCurvetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();
  
  stream_ << "t";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "L";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathLinetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "l";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}

//
// Path Horizontal Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoHorizontalAbs::print (std::ostream& stream_) const
{
  stream_ << "H" << _x << " ";
}
void Magick::PathLinetoHorizontalRel::print (std::ostream& stream_) const
{
  stream_ << "h" << _x << " ";
}

//
// Path Vertical Lineto
//

// Support a polymorphic print-to-stream operator
void Magick::PathLinetoVerticalAbs::print (std::ostream& stream_) const
{
  stream_ << "V" << _y << " ";
}
void Magick::PathLinetoVerticalRel::print (std::ostream& stream_) const
{
  stream_ << "v" << _y << " ";
}

//
// Path Moveto
//

// Support a polymorphic print-to-stream operator
void Magick::PathMovetoAbs::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "M";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
void Magick::PathMovetoRel::print (std::ostream& stream_) const
{
  std::list<Magick::Coordinate>::const_iterator p = _coordinates.begin();

  stream_ << "m";
  while ( p != _coordinates.end() )
    {
      stream_ << *p << " ";
      p++;
    }
}
