#ifndef POINT_H
#define POINT_H

#include <cmath>

namespace Graphics
{
  template< typename T = float >
  class Point
  {
  public:
    T x, y;

    Point()
    {
      this->x = this->y = 0.0f;
    }

    Point( T x, T y )
    {
      this->x = x;
      this->y = y;
    }

    T distance_from( Point p )
    {
      return( sqrt( pow( p.x - this->x, 2) + pow( p.y - this->y, 2 ) ) );
    }

    T *to_array()
    {
      T a[] = { this->x, this->y };
      return( a );
    }

	  Point operator+( Point p )
	  {
      return( Point( this->x + p.x, this->y + p.y ) );
    }

	  Point operator-( Point p )
	  {
      return( Point( this->x - p.x, this->y - p.y ) );
    }

	  Point operator*( T scalar )
	  {
      return( Point( this->x * scalar, this->y * scalar ) );
    }
	  
    Point& operator*=( T scalar )
	  { 
      this->x *= scalar;
      this->y *= scalar;
      
      return( *this );
    }

	  Point& operator+=( Point p )
	  { 
      this->x += p.x;
      this->y += p.y;
      
      return( *this );
    }

	  Point& operator-=( Point p )
	  { 
      this->x -= p.x;
      this->y -= p.y;
      
      return( *this );
    }
  };
}

#endif