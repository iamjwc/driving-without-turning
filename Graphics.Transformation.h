#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

namespace Graphics
{
  /*class Transformation
  {
  public:
    Transformation(){}

    float x, y, z;

    Transformation operator+( const Transformation& t )
    {
      return( Transformation( this->x + t.x, this->y + t.y, this->z + t.z ) );
    }

    Transformation operator-( const Transformation& t )
    {
      return( Transformation( this->x - t.x, this->y - t.y, this->z - t.z ) );
    }

    Transformation operator+( Transformation t )
    {
      return( Transformation( this->x + t.x, this->y + t.y, this->z + t.z ) );
    }

    Transformation operator-( Transformation t )
    {
      return( Transformation( this->x - t.x, this->y - t.y, this->z - t.z ) );
    }

  protected:
    Transformation( float x, float y, float z )
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }
  };*/

  class Transformation
  {
  public:
    float x, y, z;
    Axis::Axis axis;
    float theta;

  protected:
    Transformation( float x, float y, float z )
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }

    Transformation( Axis::Axis axis, float theta )
    {
      this->axis  = axis;
      this->theta = theta;
    }
  };

  class Scalation : public Transformation
  {
  public:
    //float x, y, z;

    Scalation( float x = 0.0f, float y = 0.0f, float z = 0.0f ) : Transformation( x, y, z ){}
    
    Scalation operator+( Scalation t )
    {
      return( Scalation( this->x + t.x, this->y + t.y, this->z + t.z ) );
    }

    Scalation operator-( Scalation t )
    {
      return( Scalation( this->x - t.x, this->y - t.y, this->z - t.z ) );
    }
  };

  class Translation : public Transformation
  {
  public:
    //float x, y, z;

    Translation( float x = 0.0f, float y = 0.0f, float z = 0.0f ) : Transformation( x, y, z ){}
    
    Translation operator+( Translation t )
    {
      return( Translation( this->x + t.x, this->y + t.y, this->z + t.z ) );
    }

    Translation operator-( Translation t )
    {
      return( Translation( this->x - t.x, this->y - t.y, this->z - t.z ) );
    }
  };

  class Rotation : public Transformation
  {
  public:
    Rotation( Axis::Axis axis, float theta ) : Transformation( axis, theta ){}
  };


  /*void glTranslateft( const Translation &t )
  {
    glTranslatef( t.x, t.y, t.z );
  }

  void glScaleft( const Scalation &s )
  {
    glScalef( s.x, s.y, s.z );
  }*/
  
  void glTranslateft( Translation t )
  {
    glTranslatef( t.x, t.y, t.z );
  }

  void glScaleft( Scalation s )
  {
    glScalef( s.x, s.y, s.z );
  }

  void glRotateft( Rotation t )
  {
    switch( t.axis )
    {
    case Axis::X:
      glRotatef( t.theta, 1.0f, 0.0f, 0.0f );
      break;

    case Axis::Y:
      glRotatef( t.theta, 0.0f, 1.0f, 0.0f );
      break;

    default:
      glRotatef( t.theta, 0.0f, 0.0f, 1.0f );
    }
  }
}

#endif