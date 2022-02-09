#pragma once

#include "bscommon.h"


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
  INLINE int2        operator =( int2 const& other ) { x = other.x; y = other.y; return other; }
  INLINE int2        operator+=( int2 const& other ) { return *this = *this + other; }
  INLINE int2        operator-=( int2 const& other ) { return *this = *this - other; }
  INLINE int2        operator*=( s32 other ) { return *this = *this * other; }
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

  INLINE float2        operator =( float2 const& other ) { x = other.x; y = other.y; return other; }
  INLINE float2        operator+=( float2 const& other ) { return *this = *this + other; }
  INLINE float2        operator-=( float2 const& other ) { return *this = *this - other; }
  INLINE float2        operator*=( float other ) { return *this = *this * other; }

  explicit operator int2() { return { s32( x ), s32( y ) }; }
};

struct float3
{
  float3() {}
  float3( float x, float y, float z ) : x( x ), y( y ), z( z ) {}
  float3( float xyz ) : x( xyz ), y( xyz ), z( xyz ) {}
  union
  {
    float elements[3];
    struct { float x; float y; float z; };
  };

  INLINE float& operator[]( int i ) { return elements[i]; }
  INLINE float         operator[]( int i ) const { return elements[i]; }
  INLINE float3 friend operator +( float3 const& a, float3 const& b ) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
  INLINE float3 friend operator -( float3 const& a, float3 const& b ) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
  INLINE float3 friend operator *( float f, float3 const& v ) { return { f * v.x, f * v.y, f * v.z }; }
  INLINE float3 friend operator *( float3 const& v, float f ) { return { f * v.x, f * v.y, f * v.z }; }
  INLINE float3        operator =( float3 const& other ) { x = other.x; y = other.y; z = other.z; return other; }
  INLINE float3        operator+=( float3 const& other ) { return *this = *this + other; }
  INLINE float3        operator-=( float3 const& other ) { return *this = *this - other; }
  INLINE float3        operator*=( float other ) { return *this = *this * other; }
};

INLINE int2   min( int2 const& a, int2 const& b ) { return { a.x < b.x ? a.x : b.x , a.y < b.y ? a.y : b.y }; }
INLINE int2   max( int2 const& a, int2 const& b ) { return { a.x > b.x ? a.x : b.x , a.y > b.y ? a.y : b.y }; }
INLINE int2 clamp( int2 value, int2 low, int2 high ) { return max( min( value, high ), low ); }

INLINE float2   min( float2 const& a, float2 const& b ) { return { a.x < b.x ? a.x : b.x , a.y < b.y ? a.y : b.y }; }
INLINE float2   max( float2 const& a, float2 const& b ) { return { a.x > b.x ? a.x : b.x , a.y > b.y ? a.y : b.y }; }
INLINE float2 clamp( float2 value, float2 low, float2 high ) { return max( min( value, high ), low ); }

INLINE float    dot( float2 const& a, float2 const& b ) { return a.x * b.x + a.y * b.y; }
INLINE float    dot( float3 const& a, float3 const& b ) { return a.x * b.x + a.y * b.y + a.z * b.z; }
INLINE float3 cross( float3 const& a, float3 const& b ) {
  return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
