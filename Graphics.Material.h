
#ifndef MATERIAL_H
#define MATERIAL_H

namespace Graphics
{
  template<typename T = float>
  class Material
  {
  public:
    Material()
    {
      ambient = diffuse = specular = shininess = NULL;
    }

    ~Material()
    {
      if( this->ambient != NULL )
        garbage_collect_array( this->ambient );
      
      if( this->diffuse != NULL )
        garbage_collect_array( this->diffuse );

      if( this->specular != NULL )
        garbage_collect_array( this->specular );

      if( this->shininess != NULL )
        garbage_collect_array( this->shininess );
    }

    void apply()
    {
      if( this->ambient != NULL )
        glMaterialfv( GL_FRONT, GL_AMBIENT, this->ambient );
      
      if( this->diffuse != NULL )
        glMaterialfv( GL_FRONT, GL_DIFFUSE, this->diffuse );

      if( this->specular != NULL )
        glMaterialfv( GL_FRONT, GL_SPECULAR, this->specular );

      if( this->shininess != NULL )
        glMaterialfv( GL_FRONT, GL_SHININESS, this->shininess );

    }

    void set_ambient( const T a[], int n = 4 )
    {
      set_property( this->ambient, a, n );
    }

    void set_diffuse( const T a[], int n = 4 )
    {
      set_property( this->diffuse, a, n );
    }

    void set_specular( const T a[], int n = 4 )
    {
      set_property( this->specular, a, n );
    }

    void set_shininess( const T a[], int n = 1 )
    {
      set_property( this->shininess, a, n );
    }

    // Ugly, but the only way C++ will allow it


    void set_ambient( T a1, T a2, T a3, T a4 )
    {
      if( this->ambient != NULL )
        garbage_collect_array( this->ambient );

      this->ambient = new T[ 4 ];
      this->ambient[0] = a1;
      this->ambient[1] = a2;
      this->ambient[2] = a3;
      this->ambient[3] = a4;
    }

    void set_diffuse( T d1, T d2, T d3, T d4 )
    {
      if( this->diffuse != NULL )
        garbage_collect_array( this->diffuse );

      this->diffuse = new T[ 4 ];
      this->diffuse[0] = d1;
      this->diffuse[1] = d2;
      this->diffuse[2] = d3;
      this->diffuse[3] = d4;
    }

    void set_specular( T s1, T s2, T s3, T s4 )
    {
      if( this->specular != NULL )
        garbage_collect_array( this->specular );

      this->specular = new T[ 4 ];
      this->specular[0] = s1;
      this->specular[1] = s2;
      this->specular[2] = s3;
      this->specular[3] = s4;
    }


    void set_shininess( T s1 )
    {
      if( this->shininess != NULL )
        garbage_collect_array( this->shininess );

      this->shininess = new T[ 1 ];
      this->shininess[0] = s1;
    }

    

  private:      
    T* ambient;
    T* diffuse;
    T* specular;
    T* shininess;

    void set_property( T*& prop, const T a[], int n )
    {
      if( prop != NULL )
        garbage_collect_array( prop );

      prop = new T[ n ];

      for( int i = 0; i < n; i++ )
        prop[i] = a[i];
    }
  };
}

#endif