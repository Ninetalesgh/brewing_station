#pragma once

#include "bs_common.h"


#include <math.h>

//                     |1.5707963|2679..
constexpr float PIx05 = 1.57079637f;
//                     |3.1415926|535897..
constexpr float PI    = 3.14159274f;
//                     |6.283185|30718..
constexpr float PIx2  = 6.28318548f;


#define S32_MAX 0x7fffffff
#define U32_MAX 0xffffffff
#define FLOAT_E 0.00000011920928955078125f



INLINE bool is_negative( float x ) { return (*(u32*) &x) & 0x80000000; }
INLINE bool is_negative( s32 x ) { return (*(u32*) &x) & 0x80000000; }

INLINE s32   get_sign( s32 x ) { return is_negative( x ) ? -1 : 1; }
INLINE float get_sign( float x ) { return is_negative( x ) ? -1.0f : 1.0f; }

INLINE bool sign_match( s32 a, s32 b ) { return (a ^ b) >= 0; }
INLINE bool sign_match( float a, float b ) { return ((*(s32*) &a) ^ (*(s32*) &b)) >= 0; }

INLINE float to_radians( float degrees ) { return degrees * 0.01745329251f; }
INLINE float to_degrees( float radians ) { return radians * 57.2957795131f; }

//thanks, quake III
float inv_sqrt( float number )
{
  union {
    float f;
    s32 i;
  } conv;
  conv.f = number;
  conv.i  = 0x5f3759df - (conv.i >> 1);
  conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
  return conv.f;
}


struct int2
{
  int2() {}
  int2( s32 xy ): x( xy ), y( xy ) {}
  int2( s32 x, s32 y ): x( x ), y( y ) {}
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
  float2( float xy ): x( xy ), y( xy ) {}
  float2( float x, float y ): x( x ), y( y ) {}
  float2( s32 x, s32 y ): x( float( x ) ), y( float( y ) ) {}
  float2( int2 const& other ): x( float( other.x ) ), y( float( other.y ) ) {}

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
  float3( float xyz ): x( xyz ), y( xyz ), z( xyz ) {}
  float3( float x, float y, float z ): x( x ), y( y ), z( z ) {}
  union
  {
    float elements[3];
    struct { float x; float y; float z; };
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float  operator[]( int i ) const { return elements[i]; }
  INLINE float3	 operator-() { return float3 { -x, -y, -z }; }

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
  float4( float x, float y, float z, float w ): x( x ), y( y ), z( z ), w( w ) {}
  float4( float3 const& v3, float w ): x( v3.x ), y( v3.y ), z( v3.z ), w( w ) {}
  float4( float xyzw ): x( xyzw ), y( xyzw ), z( xyzw ), w( xyzw ) {}
  union
  {
    float elements[4];
    struct { float x; float y; float z; float w; };
    struct { float r; float g; float b; float a; };
    float3 xyz;
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float  operator[]( int i ) const { return elements[i]; }

  INLINE float4 friend operator +( float4 const& a, float3 const& b ) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w }; }
  INLINE float4 friend operator +( float4 const& a, float4 const& b ) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
  INLINE float4 friend operator -( float4 const& a, float4 const& b ) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
  INLINE float4 friend operator *( float f, float4 const& v ) { return { f * v.x, f * v.y, f * v.z, f * v.w }; }
  INLINE float4 friend operator *( float4 const& v, float f ) { return { f * v.x, f * v.y, f * v.z, f * v.w }; }
  INLINE float4 friend operator /( float4 const& v, float f ) { f = 1.0f / f; return v * f; }

  INLINE float4 const& operator+=( float3 const& other ) { return *this = *this + other; }
  INLINE float4 const& operator+=( float4 const& other ) { return *this = *this + other; }
  INLINE float4 const& operator-=( float4 const& other ) { return *this = *this - other; }
  INLINE float4 const& operator*=( float other ) { return *this = *this * other; }

};

class float2x2
{
public:
  float2x2();
  float2x2( float2 const& a ): j0( a ), j1( a ) {}
  float2x2( float2 const& j0, float2 const& b ): j0( j0 ), j1( j1 ) {}
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
  float4x4() {}
  float4x4( float4 const& xAxis, float4 const& yAxis, float4 const& zAxis, float4 const& pos )
    : xAxis( xAxis ), yAxis( yAxis ), zAxis( zAxis ), pos( pos )
  {}

  float4x4( float xAxisx, float xAxisy, float xAxisz, float xAxisw,
            float yAxisx, float yAxisy, float yAxisz, float yAxisw,
            float zAxisx, float zAxisy, float zAxisz, float zAxisw,
            float posx, float posy, float posz, float posw )
    :
    m00( xAxisx ), m01( xAxisy ), m02( xAxisz ), m03( xAxisw ),
    m10( yAxisx ), m11( yAxisy ), m12( yAxisz ), m13( yAxisw ),
    m20( zAxisx ), m21( zAxisy ), m22( zAxisz ), m23( zAxisw ),
    m30( posx ), m31( posy ), m32( posz ), m33( posw )
  {}
  union
  {
    struct
    {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
      float m30, m31, m32, m33;
    };
    struct { float4 row[4]; };
    struct
    {
      float4 xAxis;
      float4 yAxis;
      float4 zAxis;
      float4 pos;
    };
  };

  INLINE float4  operator[]( int i ) const { return row[i]; }
  INLINE float4& operator[]( int i ) { return row[i]; }

  INLINE float4x4 operator-() { return float4x4 { -m00, -m01, -m02, -m03, -m10, -m11, -m12, -m13, -m20, -m21, -m22, -m23, -m30, -m31, -m32, -m33 }; }
  INLINE friend float4x4 operator+ ( float4x4 const& a, float4x4 const& b ) { return float4x4( a.xAxis + b.xAxis, a.yAxis + b.yAxis, a.zAxis + b.zAxis, a.pos + b.pos ); }
  INLINE friend float4x4 operator- ( float4x4 const& a, float4x4 const& b ) { return float4x4( a.xAxis - b.xAxis, a.yAxis - b.yAxis, a.zAxis - b.zAxis, a.pos - b.pos ); }
  //INLINE friend float4x4 operator*( float f, float4x4 const& m ) { return float4x4 { m.xAxis * f, m.yAxis * f, m.zAxis * f, m.pos * f }; }
  //INLINE friend float4x4 operator/( float4x4 const& m, float f ) { f = 1.0f / f; return m * f; }

  INLINE friend float4 operator*( float4 const& v, float4x4 const& m )
  {
    return float4
    {
      v.x * m.xAxis.x + v.y * m.yAxis.x + v.z * m.zAxis.x + v.w * m.pos.x,
      v.x * m.xAxis.y + v.y * m.yAxis.y + v.z * m.zAxis.y + v.w * m.pos.y,
      v.x * m.xAxis.z + v.y * m.yAxis.z + v.z * m.zAxis.z + v.w * m.pos.z,
      v.x * m.xAxis.w + v.y * m.yAxis.w + v.z * m.zAxis.w + v.w * m.pos.w
    };
  }

  INLINE friend float4x4 operator* ( float4x4 const& a, float4x4 const& b )
  {
    return float4x4
    {
      a.xAxis * b,
      a.yAxis * b,
      a.zAxis * b,
      a.pos * b
    };
  }

  INLINE static float4x4 const& identity()
  {
    static float4x4 id = float4x4 { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    return id;
  }
};

struct quaternion
{
  quaternion(): t( 1.f ), x( 0.f ), y( 0.f ), z( 0.f ) {}
  quaternion( quaternion const& a ): t( a.t ), x( a.x ), y( a.y ), z( a.z ) {}
  quaternion( float real, const float3& imaginary )
    : t( real ), v( imaginary ) {}
  quaternion( float real, float imaginary1, float imaginary2, float imaginary3 )
    : t( real ), x( imaginary1 ), y( imaginary2 ), z( imaginary3 ) {}
  // quaternion( float3 const& euler )
  // {
  //   float3 c { cosf( euler.x * .5f ), cosf( euler.y * .5f ), cosf( euler.z * .5f ) };
  //   float3 s { sinf( euler.x * .5f ), sinf( euler.y * .5f ), sinf( euler.z * .5f ) };
  //   t = c.x * c.y * c.z + s.x * s.y * s.z;
  //   x = s.x * c.y * c.z - c.x * s.y * s.z;
  //   y = c.x * s.y * c.z + s.x * c.y * s.z;
  //   z = c.x * c.y * s.z - s.x * s.y * c.z;
  // }

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

  friend INLINE quaternion operator * ( quaternion const& a, float b ) { return quaternion { a.t * b, a.x * b, a.y * b, a.z * b }; }
  friend INLINE quaternion operator / ( quaternion const& a, float b ) { float inv = 1.0f / b; return quaternion { a.t * inv, a.x * inv, a.y * inv, a.z * inv }; }
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
INLINE quaternion	normalize( quaternion const& a ) { float invm = 1.0f / magnitude( a ); return a * invm; }

INLINE float    dot( float2 const& a, float2 const& b ) { return a.x * b.x + a.y * b.y; }
INLINE float    dot( float3 const& a, float3 const& b ) { return a.x * b.x + a.y * b.y + a.z * b.z; }
INLINE float    dot( float4 const& a, float4 const& b ) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
INLINE float3 cross( float3 const& a, float3 const& b ) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }


INLINE float4x4 matrix_transpose( float4x4 const& m )
{
  return float4x4 { m.xAxis.x, m.yAxis.x, m.zAxis.x, m.pos.x,
                    m.xAxis.y, m.yAxis.y, m.zAxis.y, m.pos.y,
                    m.xAxis.z, m.yAxis.z, m.zAxis.z, m.pos.z,
                    m.xAxis.w, m.yAxis.w, m.zAxis.w, m.pos.w };
}

INLINE float4x4 matrix_inverse_orthonormal( float4x4 const& m )
{
  return float4x4 { m.xAxis.x, m.yAxis.x, m.zAxis.x, 0.0f,
                    m.xAxis.y, m.yAxis.y, m.zAxis.y, 0.0f,
                    m.xAxis.z, m.yAxis.z, m.zAxis.z, 0.0f,
                    -dot( m.pos, m.xAxis ), -dot( m.pos, m.yAxis ), -dot( m.pos, m.zAxis ), 1.0f };
}

float4x4 matrix_from_euler_x( float pitch );
float4x4 matrix_from_euler_y( float yaw );
float4x4 matrix_from_euler_z( float roll );
float4x4 matrix_from_euler( float yaw, float pitch );
float4x4 matrix_from_euler( float yaw, float pitch, float roll );


INLINE quaternion	quaternion_conjugate( quaternion const& a ) { return quaternion { a.t, -a.x, -a.y, -a.z }; }
INLINE quaternion	quaternion_inverse( quaternion const& a ) { return quaternion { quaternion_conjugate( a ) / quadrant( a ) }; }


// quaternion( b * a * quaternion_inverse( b ) ); }

INLINE float3	    quaternion_rotate( float3 const& v, quaternion const& q )
{
  return 2.0f * dot( q.v, v ) * q.v
    + (q.t * q.t - dot( q.v, q.v )) * v
    + 2.0f * q.t * cross( q.v, v );

}
INLINE float3 quaternion_rotate( quaternion const& q, float3 const& v ) { return quaternion_rotate( v, q ); }

INLINE quaternion	quaternion_from_axis_angle( float3 const& axis, float angle )
{
  float tmp = sinf( angle / 2.0f );
  float x = axis.x * tmp;
  float y = axis.y * tmp;
  float z = axis.z * tmp;
  float w = cosf( angle / 2.0f );

  return normalize( quaternion( w, x, y, z ) );
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

INLINE float4x4 matrix_from_euler_x( float angle )
{
  float4x4 res = float4x4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, cosf( angle ), sinf( angle ), 0.0f,
    0.0f, -sinf( angle ), cosf( angle ), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}

INLINE float4x4 matrix_from_euler_y( float angle )
{
  float4x4 res = float4x4(
    cosf( angle ), 0.0f, -sinf( angle ), 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    sinf( angle ), 0.0f, cosf( angle ), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}

INLINE float4x4 matrix_from_euler_z( float angle )
{
  float4x4 res = float4x4(
    cosf( angle ), sinf( angle ), 0.0f, 0.0f,
    -sinf( angle ), cosf( angle ), 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );
  return res;
}

INLINE float4x4 matrix_from_euler( float yaw, float pitch, float roll )
{
  return matrix_from_euler_z( roll ) * matrix_from_euler( yaw, pitch );
}

float4x4 matrix_from_euler( float yaw, float pitch )
{
  float cx = cosf( pitch );
  float sx = sinf( pitch );

  float cy = cosf( yaw );
  float sy = sinf( yaw );

  return float4x4
  {
         cy, .0f,     -sy, 0.0f,
    sx * sy,  cx, sx * cy, 0.0f,
    cx * sy, -sx, cx * cy, 0.0f,
       0.0f, .0f,    0.0f, 1.0f
  };
}



