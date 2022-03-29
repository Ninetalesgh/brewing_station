#pragma once

#include "bscommon.h"


#if 1
#include <math.h>
#else
float sqrtf( float value )
{
  return 0.0f;
}
float cosf( float value )
{
  return 0.0f;
}
float sinf( float value )
{
  return 0.0f;
}

#endif

//                     |1.5707963|2679..
constexpr float PIx05 = 1.57079637f;
//                     |3.1415926|535897..
constexpr float PI    = 3.14159274f;
//                     |6.283185|30718..
constexpr float PIx2  = 6.28318548f;

struct int2
{
  int2() {}
  int2( s32 xy ) : x( xy ), y( xy ) {}
  int2( s32 x, s32 y ) : x( x ), y( y ) {}
  union
  {
    s32 elements[2];
    struct
    {
      s32 x;
      s32 y;
    };
  };

  INLINE s32& operator[]( int i ) { return elements[i]; }
  INLINE s32  operator[]( int i ) const { return elements[i]; }

  INLINE int2 friend operator +( int2 const& a, int2 const& b ) { return { a.x + b.x, a.y + b.y }; }
  INLINE int2 friend operator -( int2 const& a, int2 const& b ) { return { a.x - b.x, a.y - b.y }; }
  INLINE int2 friend operator *( s32 f, int2 const& v ) { return { f * v.x, f * v.y }; }
  INLINE int2 friend operator *( int2 const& v, s32 f ) { return { f * v.x, f * v.y }; }

  INLINE int2 const& operator+=( int2 const& other ) { return *this = *this + other; }
  INLINE int2 const& operator-=( int2 const& other ) { return *this = *this - other; }
  INLINE int2 const& operator*=( s32 other ) { return *this = *this * other; }
};
struct float2
{
  float2() {}
  float2( float xy ) : x( xy ), y( xy ) {}
  float2( float x, float y ) : x( x ), y( y ) {}
  float2( s32 x, s32 y ) : x( float( x ) ), y( float( y ) ) {}
  float2( int2 _int2 ) : x( float( _int2.x ) ), y( float( _int2.y ) ) {}

  union
  {
    float elements[2];
    struct { float x; float y; };
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float  operator[]( int i ) const { return elements[i]; }

  INLINE float2 friend operator +( float2 const& a, float2 const& b ) { return { a.x + b.x, a.y + b.y }; }
  INLINE float2 friend operator -( float2 const& a, float2 const& b ) { return { a.x - b.x, a.y - b.y }; }
  INLINE float2 friend operator *( float f, float2 const& v ) { return { f * v.x, f * v.y }; }
  INLINE float2 friend operator *( float2 const& v, float f ) { return { f * v.x, f * v.y }; }
  INLINE float2 friend operator /( float2 const& v, float f ) { f = 1.0f / f; return v * f; }

  INLINE float2        operator+=( float2 const& other ) { return *this = *this + other; }
  INLINE float2        operator-=( float2 const& other ) { return *this = *this - other; }
  INLINE float2        operator*=( float other ) { return *this = *this * other; }

  explicit operator int2() { return { s32( x ), s32( y ) }; }
};

struct float3
{
  float3() {}
  float3( float xyz ) : x( xyz ), y( xyz ), z( xyz ) {}
  float3( float x, float y, float z ) : x( x ), y( y ), z( z ) {}
  float3( float3 const& other ) : x( other.x ), y( other.y ), z( other.z ) {}
  union
  {
    float elements[3];
    struct { float x; float y; float z; };
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float  operator[]( int i ) const { return elements[i]; }

  INLINE float3 friend operator +( float3 const& a, float3 const& b ) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
  INLINE float3 friend operator -( float3 const& a, float3 const& b ) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
  INLINE float3 friend operator *( float f, float3 const& v ) { return { f * v.x, f * v.y, f * v.z }; }
  INLINE float3 friend operator *( float3 const& v, float f ) { return { f * v.x, f * v.y, f * v.z }; }
  INLINE float3 friend operator /( float3 const& v, float f ) { f = 1.0f / f; return v * f; }

  INLINE float3 const& operator+=( float3 const& other ) { return *this = *this + other; }
  INLINE float3 const& operator-=( float3 const& other ) { return *this = *this - other; }
  INLINE float3 const& operator*=( float other ) { return *this = *this * other; }
};

struct float4
{
  float4() {}
  float4( float x, float y, float z, float w ) : x( x ), y( y ), z( z ), w( w ) {}
  float4( float xyzw ) : x( xyzw ), y( xyzw ), z( xyzw ), w( xyzw ) {}
  union
  {
    float elements[4];
    struct { float x; float y; float z; float w; };
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float  operator[]( int i ) const { return elements[i]; }

  INLINE float4 friend operator +( float4 const& a, float4 const& b ) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
  INLINE float4 friend operator -( float4 const& a, float4 const& b ) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
  INLINE float4 friend operator *( float f, float4 const& v ) { return { f * v.x, f * v.y, f * v.z, f * v.w }; }
  INLINE float4 friend operator *( float4 const& v, float f ) { return { f * v.x, f * v.y, f * v.z, f * v.w }; }
  INLINE float4 friend operator /( float4 const& v, float f ) { f = 1.0f / f; return v * f; }

  INLINE float4 const& operator+=( float4 const& other ) { return *this = *this + other; }
  INLINE float4 const& operator-=( float4 const& other ) { return *this = *this - other; }
  INLINE float4 const& operator*=( float other ) { return *this = *this * other; }

};

class float2x2
{
public:
  float2x2();
  float2x2( float2 const& a ) : j0( a ), j1( a ) {}
  float2x2( float2 const& j0, float2 const& b ) : j0( j0 ), j1( j1 ) {}
  float2x2( float i0j0, float i1j0, float i0j1, float i1j1 )
    : i0j0( i0j0 ), i1j0( i1j0 )
    , i0j1( i0j1 ), i1j1( i1j1 )
  {}
  union
  {
    struct { float2 j0, j1; };
    struct { float2 col[2]; };
    struct { float data[4]; };
    struct
    {
      float i0j0, i1j0;
      float i0j1, i1j1;
    };
  };

  INLINE float2  operator[]( int i ) const { return col[i]; }
  INLINE float2& operator[]( int i ) { return col[i]; }
  INLINE float2x2	 operator-() { return float2x2 { -i0j0, -i1j0, -i0j1, -i1j1 }; }

  INLINE friend float2x2 operator+ ( float2x2 const& a, float2x2 const& b ) { return float2x2 { a.j0 + b.j0, a.j1 + b.j1 }; }
  INLINE friend float2x2 operator- ( float2x2 const& a, float2x2 const& b ) { return float2x2 { a.j0 - b.j0, a.j1 - b.j1 }; }
  INLINE friend float2x2 operator*( float f, float2x2 const& m ) { return float2x2 { m.j0 * f, m.j1 * f }; }
  INLINE friend float2x2 operator*( float2x2 const& m, float f ) { return float2x2 { m.j0 * f, m.j1 * f }; }
  INLINE friend float2x2 operator/( float2x2 const& m, float f ) { f = 1.0f / f; return m * f; }
  INLINE friend float2x2 operator* ( float2x2 const& a, float2x2 const& b )
  {
    return float2x2 {
      //column 0
      a.data[0] * b.data[0] + a.data[2] * b.data[1],
      a.data[1] * b.data[0] + a.data[3] * b.data[1],
      //column 1
      a.data[0] * b.data[2] + a.data[2] * b.data[3],
      a.data[1] * b.data[2] + a.data[3] * b.data[3] };
  }

  INLINE float2x2 const& operator+=( float2x2 const& a ) { *this = *this + a; return *this; }
  INLINE float2x2 const& operator-=( float2x2 const& a ) { *this = *this - a; return *this; }
  INLINE float2x2 const& operator*=( float2x2 const& a ) { *this = *this * a; return *this; }
  INLINE float2x2 const& operator/=( float const& a ) { *this = *this / a; return *this; }
};

struct float4x4
{
  float4x4()
    : i0j0( 1.f ), i1j0( 0.f ), i2j0( 0.f ), i3j0( 0.f )
    , i0j1( 0.f ), i1j1( 1.f ), i2j1( 0.f ), i3j1( 0.f )
    , i0j2( 0.f ), i1j2( 0.f ), i2j2( 1.f ), i3j2( 0.f )
    , i0j3( 0.f ), i1j3( 0.f ), i2j3( 0.f ), i3j3( 1.f )
  {}
  float4x4( float4x4 const& other )
    : i0j0( other.i0j0 ), i1j0( other.i1j0 ), i2j0( other.i2j0 ), i3j0( other.i3j0 )
    , i0j1( other.i0j1 ), i1j1( other.i1j1 ), i2j1( other.i2j1 ), i3j1( other.i3j1 )
    , i0j2( other.i0j2 ), i1j2( other.i1j2 ), i2j2( other.i2j2 ), i3j2( other.i3j2 )
    , i0j3( other.i0j3 ), i1j3( other.i1j3 ), i2j3( other.i2j3 ), i3j3( other.i3j3 )
  {}
  float4x4( float4 const& j0, float4 const& j1, float4 const& j2, float4 const& j3 )
    : j0( j0 ), j1( j1 ), j2( j2 ), j3( j3 )
  {}

  float4x4( float i0j0, float i1j0, float i2j0, float i3j0,
            float i0j1, float i1j1, float i2j1, float i3j1,
            float i0j2, float i1j2, float i2j2, float i3j2,
            float i0j3, float i1j3, float i2j3, float i3j3 )
    : i0j0( i0j0 ), i1j0( i1j0 ), i2j0( i2j0 ), i3j0( i3j0 )
    , i0j1( i0j1 ), i1j1( i1j1 ), i2j1( i2j1 ), i3j1( i3j1 )
    , i0j2( i0j2 ), i1j2( i1j2 ), i2j2( i2j2 ), i3j2( i3j2 )
    , i0j3( i0j3 ), i1j3( i1j3 ), i2j3( i2j3 ), i3j3( i3j3 )
  {}

  union
  {
    struct { float4 j0, j1, j2, j3; };
    struct { float4 col[4]; };
    struct { float data[16]; };
    struct
    {
      float i0j0, i1j0, i2j0, i3j0;
      float i0j1, i1j1, i2j1, i3j1;
      float i0j2, i1j2, i2j2, i3j2;
      float i0j3, i1j3, i2j3, i3j3;
    };
  };

  INLINE float4  operator[]( int i ) const { return col[i]; }
  INLINE float4& operator[]( int i ) { return col[i]; }

  INLINE float4x4 operator-() { return float4x4( -i0j0, -i1j0, -i2j0, -i3j0, -i0j1, -i1j1, -i2j1, -i3j1, -i0j2, -i1j2, -i2j2, -i3j2, -i0j3, -i1j3, -i2j3, -i3j3 ); }
  INLINE friend float4x4 operator+ ( float4x4 const& a, float4x4 const& b ) { return float4x4( a.j0 + b.j0, a.j1 + b.j1, a.j2 + b.j2, a.j3 + b.j3 ); }
  INLINE friend float4x4 operator- ( float4x4 const& a, float4x4 const& b ) { return float4x4( a.j0 - b.j0, a.j1 - b.j1, a.j2 - b.j2, a.j3 - b.j3 ); }

  INLINE friend float4x4 operator*( float f, float4x4 const& m ) { return float4x4 { m.j0 * f, m.j1 * f, m.j2 * f, m.j3 * f }; }
  INLINE friend float4x4 operator*( float4x4 const& m, float f ) { return float4x4 { m.j0 * f, m.j1 * f, m.j2 * f, m.j3 * f }; }
  INLINE friend float4x4 operator/( float4x4 const& m, float f ) { f = 1.0f / f; return m * f; }
  INLINE friend float4x4 operator* ( float4x4 const& a, float4x4 const& b )
  {
    return float4x4( \
      //column 0     
      a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12] * b.data[3], \
      a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13] * b.data[3], \
      a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14] * b.data[3], \
      a.data[3] * b.data[0] + a.data[7] * b.data[1] + a.data[11] * b.data[2] + a.data[15] * b.data[3], \
      //column 1  					    				  	    
      a.data[0] * b.data[4] + a.data[4] * b.data[5] + a.data[8] * b.data[6] + a.data[12] * b.data[7], \
      a.data[1] * b.data[4] + a.data[5] * b.data[5] + a.data[9] * b.data[6] + a.data[13] * b.data[7], \
      a.data[2] * b.data[4] + a.data[6] * b.data[5] + a.data[10] * b.data[6] + a.data[14] * b.data[7], \
      a.data[3] * b.data[4] + a.data[7] * b.data[5] + a.data[11] * b.data[6] + a.data[15] * b.data[7], \
      //column 2	 					    
      a.data[0] * b.data[8] + a.data[4] * b.data[9] + a.data[8] * b.data[10] + a.data[12] * b.data[11], \
      a.data[1] * b.data[8] + a.data[5] * b.data[9] + a.data[9] * b.data[10] + a.data[13] * b.data[11], \
      a.data[2] * b.data[8] + a.data[6] * b.data[9] + a.data[10] * b.data[10] + a.data[14] * b.data[11], \
      a.data[3] * b.data[8] + a.data[7] * b.data[9] + a.data[11] * b.data[10] + a.data[15] * b.data[11], \
      //column 3
      a.data[0] * b.data[12] + a.data[4] * b.data[13] + a.data[8] * b.data[14] + a.data[12] * b.data[15], \
      a.data[1] * b.data[12] + a.data[5] * b.data[13] + a.data[9] * b.data[14] + a.data[13] * b.data[15], \
      a.data[2] * b.data[12] + a.data[6] * b.data[13] + a.data[10] * b.data[14] + a.data[14] * b.data[15], \
      a.data[3] * b.data[12] + a.data[7] * b.data[13] + a.data[11] * b.data[14] + a.data[15] * b.data[15] );
  }

  INLINE static float4x4 const& identity()
  {
    static float4x4 id = float4x4 { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    return id;
  }
};

struct quaternion
{
  quaternion() : t( 1.f ), x( 0.f ), y( 0.f ), z( 0.f ) {}
  quaternion( quaternion const& a ) : t( a.t ), x( a.x ), y( a.y ), z( a.z ) {}
  quaternion( float real, const float3& imaginary )
    : t( real ), v( imaginary ) {}
  quaternion( float real, float imaginary1, float imaginary2, float imaginary3 )
    : t( real ), x( imaginary1 ), y( imaginary2 ), z( imaginary3 ) {}
  quaternion( float3 const& euler )
  {
    float3 c { cosf( euler.x * .5f ), cosf( euler.y * .5f ), cosf( euler.z * .5f ) };
    float3 s { sinf( euler.x * .5f ), sinf( euler.y * .5f ), sinf( euler.z * .5f ) };
    t = c.x * c.y * c.z + s.x * s.y * s.z;
    x = s.x * c.y * c.z - c.x * s.y * s.z;
    y = c.x * s.y * c.z + s.x * c.y * s.z;
    z = c.x * c.y * s.z - s.x * s.y * c.z;
  }

  union
  {
    struct
    {
      float t;
      union
      {
        struct { float3 v; };
        struct { float x, y, z; };
      };
    };
    struct { float data[4]; };
  };

  friend INLINE quaternion operator / ( quaternion const& a, float b ) { return quaternion( a.t / b, a.x / b, a.y / b, a.z / b ); }

  friend INLINE quaternion operator* ( quaternion const& a, quaternion const& b )
  { 						  //Euler's Four Square Identity            (practically)
    return quaternion( (a.t * b.t) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z),
                       (a.t * b.x) + (a.x * b.t) + (a.y * b.z) - (a.z * b.y),
                       (a.t * b.y) + (a.y * b.t) + (a.z * b.x) - (a.x * b.z),
                       (a.t * b.z) + (a.z * b.t) + (a.x * b.y) - (a.y * b.x) );
  }
};


INLINE int2   min( int2 const& a, int2 const& b ) { return { a.x < b.x ? a.x : b.x , a.y < b.y ? a.y : b.y }; }
INLINE int2   max( int2 const& a, int2 const& b ) { return { a.x > b.x ? a.x : b.x , a.y > b.y ? a.y : b.y }; }
INLINE int2 clamp( int2 value, int2 low, int2 high ) { return max( min( value, high ), low ); }

INLINE float2   min( float2 const& a, float2 const& b ) { return { a.x < b.x ? a.x : b.x , a.y < b.y ? a.y : b.y }; }
INLINE float2   max( float2 const& a, float2 const& b ) { return { a.x > b.x ? a.x : b.x , a.y > b.y ? a.y : b.y }; }
INLINE float2 clamp( float2 value, float2 low, float2 high ) { return max( min( value, high ), low ); }


INLINE float		  quadrant( float2 const& a ) { return (a.x * a.x + a.y * a.y); }
INLINE float		  quadrant( float3 const& a ) { return (a.x * a.x + a.y * a.y + a.z * a.z); }
INLINE float		  quadrant( float4 const& a ) { return (a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w); }
INLINE float			quadrant( quaternion const& a ) { return (a.t * a.t + a.x * a.x + a.y * a.y + a.z * a.z); }

INLINE float		  magnitude( float2 const& a ) { return sqrtf( quadrant( a ) ); }
INLINE float		  magnitude( float3 const& a ) { return sqrtf( quadrant( a ) ); }
INLINE float		  magnitude( float4 const& a ) { return sqrtf( quadrant( a ) ); }
INLINE float      magnitude( quaternion const& a ) { return sqrtf( a.t * a.t + a.x * a.x * a.y * a.y * a.z * a.z ); }

INLINE float2	  	normalize( float2 const& a ) { float invm = 1.0f / magnitude( a ); return a * invm; }
INLINE float3		  normalize( float3 const& a ) { float invm = 1.0f / magnitude( a ); return a * invm; }
INLINE float4		  normalize( float4 const& a ) { float invm = 1.0f / magnitude( a ); return a * invm; }
INLINE quaternion	normalize( quaternion const& a ) { return (a / magnitude( a )); }

INLINE float    dot( float2 const& a, float2 const& b ) { return a.x * b.x + a.y * b.y; }
INLINE float    dot( float3 const& a, float3 const& b ) { return a.x * b.x + a.y * b.y + a.z * b.z; }
INLINE float3 cross( float3 const& a, float3 const& b ) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }


INLINE quaternion	quaternion_conjugate( quaternion const& a ) { return quaternion { a.t, -a.x, -a.y, -a.z }; }
INLINE quaternion	quaternion_inverse( quaternion const& a ) { return quaternion { quaternion_conjugate( a ) / quadrant( a ) }; }
//Rotate a around b						
INLINE quaternion	quaternion_rotate( quaternion const& a, quaternion const& b ) { return quaternion( b * a * quaternion_inverse( b ) ); }
//rotate a around b
INLINE float3			quaternion_rotate( float3 const& a, quaternion const& b )
{
  quaternion c { 0, a };
  c = b * c * quaternion_inverse( b );
  return float3( c.x, c.y, c.z );
}

INLINE quaternion	quaternion_from_axis_angle( float3 const& a, float angle )
{
  float tmp = sinf( angle / 2.0f );
  float x = a.x * tmp;
  float y = a.y * tmp;
  float z = a.z * tmp;
  float w = cosf( angle / 2.0f );

  return normalize( quaternion( w, x, y, z ) );
}

float4x4 look_at_matrix( float3 eye, float3 target, float3 up );

float4x4 projection_matrix( float windowWidth, float windowHeight, float fovRadians, float zNear, float zFar );

float4x4 rotation_matrix_x( float x );

float4x4 rotation_matrix_y( float y );

float4x4 rotation_matrix_z( float z );


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


float4x4 look_at_matrix( float3 eye, float3 target, float3 up )
{
  const float3 f = normalize( target - eye );
  const float3 s = normalize( cross( up, f ) );
  const float3 u = cross( f, s );
  float4x4 result;
  result[0][0] = s.x;
  result[1][0] = s.y;
  result[2][0] = s.z;
  result[3][0] = -dot( s, eye );
  result[0][1] = u.x;
  result[1][1] = u.y;
  result[2][1] = u.z;
  result[3][1] = -dot( u, eye );
  result[0][2] = f.x;
  result[1][2] = f.y;
  result[2][2] = f.z;
  result[3][2] = -dot( f, eye );

  result[0][3] = 0;
  result[1][3] = 0;
  result[2][3] = 0;
  result[3][3] = 1;

  return result;
}

float4x4 projection_matrix( float windowWidth, float windowHeight, float fovRadians, float zNear, float zFar )
{
  float tanHalfFovy = tanf( fovRadians * 0.5f );
  float aspectRatio = windowWidth / windowHeight;

  float4x4 m = float4x4( 1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f );

  m[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
  m[1][1] = 1.0f / (tanHalfFovy);
  m[2][2] = (zFar) / (zFar - zNear);
  m[2][3] = 1.0f;//copy z value to w for perspective divide
  m[3][2] = -(zFar * zNear) / (zFar - zNear);
  m[3][3] = 0.0f;
  return m;
}

float4x4 rotation_matrix_x( float angle )
{
  float4x4 res = float4x4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, cosf( angle ), -sinf( angle ), 0.0f,
    0.0f, sinf( angle ), cosf( angle ), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}

float4x4 rotation_matrix_y( float angle )
{
  float4x4 res = float4x4(
    cosf( angle ), 0.0f, sinf( angle ), 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    -sinf( angle ), 0.0f, cosf( angle ), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}

float4x4 rotation_matrix_z( float angle )
{
  float4x4 res = float4x4(
    cosf( angle ), -sinf( angle ), 0.0f, 0.0f,
    sinf( angle ), cosf( angle ), 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}




