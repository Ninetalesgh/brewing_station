
#include <common/bsstring.h>

//source: https://github.com/vog/sha1/blob/master/sha1.hpp

static s64 const BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
static s64 const BLOCK_BYTES = BLOCK_INTS * 4;

inline static u32 rol( u32 const value, s64 const bits )
{
  return (value << bits) | (value >> (32 - bits));
}

inline static u32 blk( u32 const block[BLOCK_INTS], s64 const i )
{
  return rol( block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^ block[i], 1 );
}

/*
 * (R0+R1), R2, R3, R4 are the different operations used in SHA1
 */

inline static void R0( u32 const block[BLOCK_INTS], u32 const v, u32& w, u32 const x, u32 const y, u32& z, s64 const i )
{
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol( v, 5 );
  w = rol( w, 30 );
}


inline static void R1( u32 block[BLOCK_INTS], u32 const v, u32& w, u32 const x, u32 const y, u32& z, s64 const i )
{
  block[i] = blk( block, i );
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol( v, 5 );
  w = rol( w, 30 );
}


inline static void R2( u32 block[BLOCK_INTS], u32 const v, u32& w, u32 const x, u32 const y, u32& z, s64 const i )
{
  block[i] = blk( block, i );
  z += (w ^ x ^ y) + block[i] + 0x6ed9eba1 + rol( v, 5 );
  w = rol( w, 30 );
}


inline static void R3( u32 block[BLOCK_INTS], u32 const v, u32& w, u32 const x, u32 const y, u32& z, s64 const i )
{
  block[i] = blk( block, i );
  z += (((w | x) & y) | (w & x)) + block[i] + 0x8f1bbcdc + rol( v, 5 );
  w = rol( w, 30 );
}


inline static void R4( u32 block[BLOCK_INTS], u32 const v, u32& w, u32 const x, u32 const y, u32& z, s64 const i )
{
  block[i] = blk( block, i );
  z += (w ^ x ^ y) + block[i] + 0xca62c1d6 + rol( v, 5 );
  w = rol( w, 30 );
}


/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */

inline static void transform( u32 digest[], u32 block[BLOCK_INTS], u64& transforms )
{
  /* Copy digest[] to working vars */
  u32 a = digest[0];
  u32 b = digest[1];
  u32 c = digest[2];
  u32 d = digest[3];
  u32 e = digest[4];

  /* 4 rounds of 20 operations each. Loop unrolled. */
  R0( block, a, b, c, d, e, 0 );
  R0( block, e, a, b, c, d, 1 );
  R0( block, d, e, a, b, c, 2 );
  R0( block, c, d, e, a, b, 3 );
  R0( block, b, c, d, e, a, 4 );
  R0( block, a, b, c, d, e, 5 );
  R0( block, e, a, b, c, d, 6 );
  R0( block, d, e, a, b, c, 7 );
  R0( block, c, d, e, a, b, 8 );
  R0( block, b, c, d, e, a, 9 );
  R0( block, a, b, c, d, e, 10 );
  R0( block, e, a, b, c, d, 11 );
  R0( block, d, e, a, b, c, 12 );
  R0( block, c, d, e, a, b, 13 );
  R0( block, b, c, d, e, a, 14 );
  R0( block, a, b, c, d, e, 15 );
  R1( block, e, a, b, c, d, 0 );
  R1( block, d, e, a, b, c, 1 );
  R1( block, c, d, e, a, b, 2 );
  R1( block, b, c, d, e, a, 3 );
  R2( block, a, b, c, d, e, 4 );
  R2( block, e, a, b, c, d, 5 );
  R2( block, d, e, a, b, c, 6 );
  R2( block, c, d, e, a, b, 7 );
  R2( block, b, c, d, e, a, 8 );
  R2( block, a, b, c, d, e, 9 );
  R2( block, e, a, b, c, d, 10 );
  R2( block, d, e, a, b, c, 11 );
  R2( block, c, d, e, a, b, 12 );
  R2( block, b, c, d, e, a, 13 );
  R2( block, a, b, c, d, e, 14 );
  R2( block, e, a, b, c, d, 15 );
  R2( block, d, e, a, b, c, 0 );
  R2( block, c, d, e, a, b, 1 );
  R2( block, b, c, d, e, a, 2 );
  R2( block, a, b, c, d, e, 3 );
  R2( block, e, a, b, c, d, 4 );
  R2( block, d, e, a, b, c, 5 );
  R2( block, c, d, e, a, b, 6 );
  R2( block, b, c, d, e, a, 7 );
  R3( block, a, b, c, d, e, 8 );
  R3( block, e, a, b, c, d, 9 );
  R3( block, d, e, a, b, c, 10 );
  R3( block, c, d, e, a, b, 11 );
  R3( block, b, c, d, e, a, 12 );
  R3( block, a, b, c, d, e, 13 );
  R3( block, e, a, b, c, d, 14 );
  R3( block, d, e, a, b, c, 15 );
  R3( block, c, d, e, a, b, 0 );
  R3( block, b, c, d, e, a, 1 );
  R3( block, a, b, c, d, e, 2 );
  R3( block, e, a, b, c, d, 3 );
  R3( block, d, e, a, b, c, 4 );
  R3( block, c, d, e, a, b, 5 );
  R3( block, b, c, d, e, a, 6 );
  R3( block, a, b, c, d, e, 7 );
  R3( block, e, a, b, c, d, 8 );
  R3( block, d, e, a, b, c, 9 );
  R3( block, c, d, e, a, b, 10 );
  R3( block, b, c, d, e, a, 11 );
  R4( block, a, b, c, d, e, 12 );
  R4( block, e, a, b, c, d, 13 );
  R4( block, d, e, a, b, c, 14 );
  R4( block, c, d, e, a, b, 15 );
  R4( block, b, c, d, e, a, 0 );
  R4( block, a, b, c, d, e, 1 );
  R4( block, e, a, b, c, d, 2 );
  R4( block, d, e, a, b, c, 3 );
  R4( block, c, d, e, a, b, 4 );
  R4( block, b, c, d, e, a, 5 );
  R4( block, a, b, c, d, e, 6 );
  R4( block, e, a, b, c, d, 7 );
  R4( block, d, e, a, b, c, 8 );
  R4( block, c, d, e, a, b, 9 );
  R4( block, b, c, d, e, a, 10 );
  R4( block, a, b, c, d, e, 11 );
  R4( block, e, a, b, c, d, 12 );
  R4( block, d, e, a, b, c, 13 );
  R4( block, c, d, e, a, b, 14 );
  R4( block, b, c, d, e, a, 15 );

  /* Add the working vars back into digest[] */
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;

  /* Count the number of transformations */
  transforms++;
}


inline static void buffer_to_block( char const* buffer, u32 block[BLOCK_INTS] )
{
  for ( s64 i = 0; i < BLOCK_INTS; i++ )
  {
    block[i] = (buffer[4 * i + 3] & 0xff)
      | (buffer[4 * i + 2] & 0xff) << 8
      | (buffer[4 * i + 1] & 0xff) << 16
      | (buffer[4 * i + 0] & 0xff) << 24;
  }
}

static void sha1( char const* input, s32 inputSize, char* out_hash, s32 bufferSize )
{
  //init
  u32 digest[5];
  u64 transforms = 0;

  digest[0] = 0x67452301;
  digest[1] = 0xefcdab89;
  digest[2] = 0x98badcfe;
  digest[3] = 0x10325476;
  digest[4] = 0xc3d2e1f0;

  u32 block[BLOCK_INTS];

  char const* reader = input;

  s32 inputLeft = inputSize == 0 ? bs::string_length( input ) : inputSize;

  while ( inputLeft > BLOCK_BYTES )
  {
    buffer_to_block( reader, block );
    transform( digest, block, transforms );
    reader += BLOCK_BYTES;
    inputLeft -= BLOCK_BYTES;
  }

  if ( inputLeft )
  {
    u64 totalBits = (transforms * BLOCK_BYTES + inputLeft) * 8;

    s32 i = 0;
    while ( inputLeft > 3 )
    {
      block[i] = (reader[4 * i + 3] & 0xff) | (reader[4 * i + 2] & 0xff) << 8 | (reader[4 * i + 1] & 0xff) << 16 | (reader[4 * i + 0] & 0xff) << 24;
      ++i;
      reader += 4;
      inputLeft -= 4;
    }

    u8 c[4];
    for ( s32 j = 0; j < 4; ++j )
    {
      if ( j > inputLeft )  c[j] = 0;
      else if ( j == inputLeft ) c[j] = 0x80;
      else c[j] = reader[j];
    }

    block[i] = (c[3] & 0xff) | (c[2] & 0xff) << 8 | (c[1] & 0xff) << 16 | (c[0] & 0xff) << 24;
    i++;

    for ( s32 j = i; j < BLOCK_INTS; ++j )
    {
      block[j] = 0;
    }

    if ( i + 3 > BLOCK_INTS )
    {
      transform( digest, block, transforms );
      for ( s32 j = 0; j < BLOCK_INTS; ++j )
      {
        block[j] = 0;
      }
    }

    block[BLOCK_INTS - 1] = (u32) totalBits;
    block[BLOCK_INTS - 2] = (u32) (totalBits >> 32);
    transform( digest, block, transforms );
  }

  int test = 0;
  test =3;
}
