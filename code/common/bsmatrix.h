#pragma once

#include "bsmath.h"

namespace bs
{
  struct Matrix
  {
    Matrix( float3 const& xAxis, float3 const& yAxis, float3 const& zAxis, float3 const& pos )
      : xAxis( xAxis ), yAxis( yAxis ), zAxis( zAxis ), pos( pos )
    {}
    Matrix( float xAxisx, float xAxisy, float xAxisz,
            float yAxisx, float yAxisy, float yAxisz,
            float zAxisx, float zAxisy, float zAxisz,
            float posx, float posy, float posz )
      : m00( xAxisx ), m01( xAxisy ), m02( xAxisz )
      , m10( yAxisx ), m11( yAxisy ), m12( yAxisz )
      , m20( zAxisx ), m21( zAxisy ), m22( zAxisz )
      , m30( posx ), m31( posy ), m32( posz )
    {}

    union
    {
      struct
      {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
        float m30, m31, m32;
      };
      struct { float3 row[4]; };
      struct
      {
        float3 xAxis;
        float3 yAxis;
        float3 zAxis;
        float3 pos;
      };
    };

    INLINE float3  operator[]( int i ) const { return row[i]; }
    INLINE float3& operator[]( int i ) { return row[i]; }

    INLINE friend float3 operator*( float3 const& v, Matrix const& m )
    {
      return float3
      {
        /////////////
        v.x * m.xAxis.x + v.y * m.yAxis.x + v.z * m.zAxis.x + m.pos.x,
        v.x * m.xAxis.y + v.y * m.yAxis.y + v.z * m.zAxis.y + m.pos.y,
        v.x * m.xAxis.z + v.y * m.yAxis.z + v.z * m.zAxis.z + m.pos.z,
      };
    }

    INLINE friend Matrix operator* ( Matrix const& a, Matrix const& b )
    {
      return Matrix
      {
        ///////////////////
        a.xAxis.x * b.xAxis.x + a.xAxis.y * b.yAxis.x + a.xAxis.z * b.zAxis.x,
        a.xAxis.x * b.xAxis.y + a.xAxis.y * b.yAxis.y + a.xAxis.z * b.zAxis.y,
        a.xAxis.x * b.xAxis.z + a.xAxis.y * b.yAxis.z + a.xAxis.z * b.zAxis.z,

        a.yAxis.x * b.xAxis.x + a.yAxis.y * b.yAxis.x + a.yAxis.z * b.zAxis.x,
        a.yAxis.x * b.xAxis.y + a.yAxis.y * b.yAxis.y + a.yAxis.z * b.zAxis.y,
        a.yAxis.x * b.xAxis.z + a.yAxis.y * b.yAxis.z + a.yAxis.z * b.zAxis.z,

        a.zAxis.x * b.xAxis.x + a.zAxis.y * b.yAxis.x + a.zAxis.z * b.zAxis.x,
        a.zAxis.x * b.xAxis.y + a.zAxis.y * b.yAxis.y + a.zAxis.z * b.zAxis.y,
        a.zAxis.x * b.xAxis.z + a.zAxis.y * b.yAxis.z + a.zAxis.z * b.zAxis.z,

        a.pos.x * b.xAxis.x + a.pos.y * b.yAxis.x + a.pos.z * b.zAxis.x + b.pos.x,
        a.pos.x * b.xAxis.y + a.pos.y * b.yAxis.y + a.pos.z * b.zAxis.y + b.pos.y,
        a.pos.x * b.xAxis.z + a.pos.y * b.yAxis.z + a.pos.z * b.zAxis.z + b.pos.z
      };
    }

    INLINE static Matrix const& identity()
    {
      static Matrix id = Matrix { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
      return id;
    }
  };

};


// if ( m22 < 0 )
// {
//   if ( m00 > m11 )
//   {
//     t = 1 + m00 - m11 - m22;
//     q = quat( t, m01 + m10, m20 + m02, m12 - m21 );
//   }
//   else
//   {
//     t = 1 - m00 + m11 - m22;
//     q = quat( m01 + m10, t, m12 + m21, m20 - m02 );
//   }
// }
// else
// {
//   if ( m00 < -m11 )
//   {
//     t = 1 - m00 - m11 + m22;
//     q = quat( m20 + m02, m12 + m21, t, m01 - m10 );
//   }
//   else
//   {
//     t = 1 + m00 + m11 + m22;
//     q = quat( m12 - m21, m20 - m02, m01 - m10, t );
//   }
// }
// q *= 0.5 / Sqrt( t );

// void quat_to_mat33_std(mat33_t* m, quat_t* q)
// {
//   float x  = q->x, y  = q->y, z  = q->z, w  = q->w;
//   float tx = 2 *x, ty = 2 *y, tz = 2 *z;
//   float xx = tx*x, yy = ty*y, zz = tz*z;
//   float xy = ty*x, xz = tz*x, yz = ty*z;
//   float wx = tx*w, wy = ty*w, wz = tz*w;

//   m->m00 = 1.f-(yy+zz); m->m11 = 1.f-(xx+zz); m->m22 = 1.f-(xx+yy);

//   m->m10 = xy+wz; m->m01 = xy-wz;
//   m->m20 = xz-wy; m->m02 = xz+wy;
//   m->m21 = yz+wx; m->m12 = yz-wx;
// }

  // void rotate_vector_by_quaternion( const Vector3& v, const Quaternion& q, Vector3& vprime )
  // {
  //   // Extract the vector part of the quaternion
  //   Vector3 u( q.x, q.y, q.z );

  //   // Extract the scalar part of the quaternion
  //   float s = q.w;

  //   // Do the math
  //   vprime = 2.0f * dot( u, v ) * u
  //     + (s * s - dot( u, u )) * v
  //     + 2.0f * s * cross( u, v );
  // }


// t = 2 * cross(q.xyz, v)
// v' = v + q.w * t + cross(q.xyz, t)

