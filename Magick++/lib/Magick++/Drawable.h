// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Definition of Drawable (Graphic objects)
//

#if !defined(Drawable_header)
#define Drawable_header

#include "Magick++/Include.h"

#include <string>
#include <list>
#include <utility>
#include "Magick++/Color.h"
#include "Magick++/Geometry.h"

namespace Magick
{
  //
  // Representation of an x,y coordinate
  //
  class Coordinate
  {
    friend std::ostream& operator<<( std::ostream& stream_,
                                     const Coordinate& coordinate_ );
  public:
    Coordinate ( void );
    Coordinate ( double x_, double y_ );
    ~Coordinate ();

    void   x ( double x_ );
    double x ( void ) const;

    void   y ( double y_ );
    double y ( void ) const;

  private:
    double _x;
    double _y;
  };
//   typedef std::pair<double,double> Coordinate;

  // Print coordinate to stream.
//   std::ostream& operator<<( std::ostream& stream_,
//                             const Coordinate& coordinate_ );

  //
  // Representation of a drawable object
  //
  class Drawable
  {
  public:
    // Constructor
    Drawable ( void );
    
    // Destructor
    virtual ~Drawable ( void );

    //
    // Following accessors are for internal use only
    //
    void        primitive ( const std::string &primitive_ );
    std::string primitive ( void ) const;

    // Copy construtor
    Drawable( const Drawable& drawable_ );

    // Assignment operator
    const Drawable& operator=( const Drawable& drawable_ );

  protected:

    // Common implementation of methods which take one Coordinate argument
    void one_arg_impl ( const char* command_,
			const Coordinate &coordinate_ );

    // Common implementation of methods which take two Coordinate arguments
    void two_arg_impl ( const char* command_,
			const Coordinate &startCoordinate_,
			const Coordinate &endCoordinate_ );

    // Common implementation of methods which take a list of Coordinates
    void list_arg_impl ( const char* command_,
			 const std::list<Magick::Coordinate> &coordinates_ );
    
    // This is the string normally passed by -draw, e.g. "circle
    // +100+100 +200+200"
    std::string _primitive;
  };

  //
  // Drawable Objects
  //

  // Arc
  class DrawableArc : public Drawable
  {
  public:
    DrawableArc ( double startX_, double startY_,
                  double endX_, double endY_,
                  double startDegrees_, double endDegrees_ );
  };

  // Bezier curve (Coordinate list must contain at least three members)
  class DrawableBezier : public Drawable
  {
  public:
    DrawableBezier ( const std::list<Magick::Coordinate> &coordinates_ );
  };

  // Circle
  class DrawableCircle : public Drawable
  {
  public:
    DrawableCircle ( double originX_, double originY_,
                     double perimX_, double perimY_ );
  };

  // Colorize at point using PaintMethod
  class DrawableColor : public Drawable
  {
  public:
    DrawableColor ( double x_, double y_,
                    PaintMethod paintMethod_ );
  };

  // Ellipse
  class DrawableEllipse : public Drawable
  {
  public:
    DrawableEllipse ( double originX_, double originY_, 
                      double width_, double height_,
                      double arcStart_, double arcEnd_ );
  };

  // Draw image at point
  class DrawableImage : public Drawable
  {
  public:
    DrawableImage ( double x_, double y_,
                    const std::string &image_ );
  };

  // Line
  class DrawableLine : public Drawable
  {
  public:
    DrawableLine ( double startX_, double startY_,
                   double endX_, double endY_ );
  };

  // Change pixel matte value to transparent using PaintMethod
  class DrawableMatte : public Drawable
  {
  public:
    DrawableMatte ( double x_, double y_,
                    PaintMethod paintMethod_ );
  };

  // path

  // Point
  class DrawablePoint : public Drawable
  {
  public:
    DrawablePoint ( double x_, double y_ );
  };

  // Polygon (Coordinate list must contain at least three members)
  class DrawablePolygon : public Drawable
  {
  public:
    DrawablePolygon ( const std::list<Magick::Coordinate> &coordinates_ );
  };

  // Polyline (Coordinate list must contain at least three members)
  class DrawablePolyline : public Drawable
  {
  public:
    DrawablePolyline ( const std::list<Magick::Coordinate> &coordinates_ );
  };

  // Rectangle
  class DrawableRectangle : public Drawable
  {
  public:
    DrawableRectangle ( double upperLeftX_, double upperLeftY_,
                        double lowerRightX_, double lowerRightY );
  };

  // RoundRectangle
  class DrawableRoundRectangle : public Drawable
  {
  public:
    DrawableRoundRectangle ( double centerX_, double centerY_,
                             double width_, double hight_,
                             double cornerWidth_, double cornerHeight_ );
  };

  // Draw text at point
  class DrawableText : public Drawable
  {
  public:
    DrawableText ( double x_, double y_,
                   std::string text_ );
  };

} // namespace Magick


// Print coordinate to stream.
// std::ostream& operator<<( std::ostream& stream_,
// 			  const Magick::Coordinate& coordinate_ );

//
// Inlines
//

//
// Coordinate class
//
inline Magick::Coordinate::Coordinate ( void )
  : _x(0),
    _y(0)
{
}

inline Magick::Coordinate::Coordinate ( double x_, double y_ )
  : _x(x_),
    _y(y_)
{
}

inline Magick::Coordinate::~Coordinate ()
{
  // Nothing to do
}

inline void Magick::Coordinate::x ( double x_ )
{
  _x = x_;
}

inline double Magick::Coordinate::x ( void ) const
{
  return _x;
}

inline void Magick::Coordinate::y ( double y_ )
{
  _y = y_;
}

inline double Magick::Coordinate::y ( void ) const
{
  return _y;
}

//
// Drawable class
//

// Completed draw primitive (for ImageMagick)
inline void Magick::Drawable::primitive ( const std::string &primitive_ )
{
  _primitive = primitive_;
}
inline std::string Magick::Drawable::primitive ( void ) const
{
  // cout << _primitive << endl;
  return _primitive;
}

#endif // Drawable_header
