#include <cmath>

#include "Graphics.Range.h"

using namespace std;



namespace Graphics
{
  class Animation
  {
  public:

    float   start, end;
    float*  value;
    int     i, n;

    Animation( float& value )
    {
      this->value = &value;
      this->reset( value );
    }

    Animation( float& value, float end, int times )
    {
      this->value = &value;
      this->reset( end, times );
    }

    void reset( float end = 0.0f, int times = 50 )
    {
      this->i = 0;
      this->n = times;

      this->start = (*this->value);
      this->end   = end;
    }

    void redirect_to( float end, int times = 25 )
    {
      this->reset( end, times );
    }

    void animate( float (*f)( float, float, int, int ) )
    {
      if( this->i < this->n )
        (*this->value) = f( this->start, this->end - this->start, ++this->i, this->n );
      else if( this->i == this->n )
        (*this->value) = this->end;
      else
        return;
    }

    bool is_animating()
    {
      return( this->i < this->n );
    }

    void animate_range( Range<>* r, float (*f)( float, float, int, int ) )
    {
      if( !this->is_animating() )
      {
        if( (*this->value) == r->min )
          this->redirect_to( r->max, this->n );
        else if( (*this->value) == r->max )
          this->redirect_to( r->min, this->n );
      }

      this->animate( f );
    }

  };

  namespace AnimationLibrary
  {
    namespace Linear
    {
      float tween( float start, float change, int i, int n )
      {
        return( change * ( float(i)/n ) + start );
      }
    }

    namespace Quadratic
    {
      float ease_in( float start, float change, int i, int n )
      {
        return( change * ( float(i)/n ) * ( float(i)/n ) + start );
      }

      float ease_out( float start, float change, int i, int n )
      {
        return( -change * ( float(i)/n ) * ( float(i)/n - 2 ) + start );
      }

      float ease_in_and_out( float start, float change, int i, int n )
      {
        float half_change = change/2;
        int   half_n      = n/2;

        if( i < half_n )
          return( ease_in( start, half_change, i, half_n ) );
        else
          return( ease_out( start+half_change, change-half_change, i-half_n, n-half_n ) );
      }
    }

    namespace Cubic
    {
      float ease_in( float start, float change, int i, int n )
      {
        return( change * ( float(i)/n ) * ( float(i)/n ) * ( float(i)/n ) + start );
      }

      float ease_out( float start, float change, int i, int n )
      {
        return( -change * ( float(i)/n ) * ( float(i)/n ) * ( float(i)/n - 2 ) + start );
      }

      float ease_in_and_out( float start, float change, int i, int n )
      {
        float half_change = change/2;
        int   half_n      = n/2;

        if( i < half_n )
          return( ease_in( start, half_change, i, half_n ) );
        else
          return( ease_out( start+half_change, change-half_change, i-half_n, n-half_n ) );
      }
    }

  };
}