
#include <common/bsstring.h>
#include <corecrt_memory.h>

//bufferSize has to be at least 29 bytes large or it will fail 
//you CAN use the same buffer for in and output
static bool sha1_to_base64( char const* input, s32 inputSize, char* out_hash_base64, s32 bufferSize );

//bufferSize has to be at least 41 bytes large or it will fail 
static bool sha1_to_hex( char const* input, s32 inputSize, char* out_hash_hex, s32 bufferSize );



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *
 * TinySHA1 - a header only implementation of the SHA1 algorithm in C++. Based
 * on the implementation in boost::uuid::details.
 *
 * SHA1 Wikipedia Page: http://en.wikipedia.org/wiki/SHA-1
 *
 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


namespace sha1
{
  class SHA1
  {
  public:
    typedef u32 digest32_t[5];
    typedef u8 digest8_t[20];
    inline static u32 LeftRotate( u32 value, size_t count ) {
      return (value << count) ^ (value >> (32 - count));
    }
    SHA1() { reset(); }
    virtual ~SHA1() {}
    SHA1( const SHA1& s ) { *this = s; }
    const SHA1& operator = ( const SHA1& s ) {
      memcpy( m_digest, s.m_digest, 5 * sizeof( u32 ) );
      memcpy( m_block, s.m_block, 64 );
      m_blockByteIndex = s.m_blockByteIndex;
      m_byteCount = s.m_byteCount;
      return *this;
    }
    SHA1& reset() {
      m_digest[0] = 0x67452301;
      m_digest[1] = 0xEFCDAB89;
      m_digest[2] = 0x98BADCFE;
      m_digest[3] = 0x10325476;
      m_digest[4] = 0xC3D2E1F0;
      m_blockByteIndex = 0;
      m_byteCount = 0;
      return *this;
    }
    SHA1& processByte( u8 octet ) {
      this->m_block[this->m_blockByteIndex++] = octet;
      ++this->m_byteCount;
      if ( m_blockByteIndex == 64 ) {
        this->m_blockByteIndex = 0;
        processBlock();
      }
      return *this;
    }
    SHA1& processBlock( const void* const start, const void* const end ) {
      const u8* begin = static_cast<const u8*>(start);
      const u8* finish = static_cast<const u8*>(end);
      while ( begin != finish ) {
        processByte( *begin );
        begin++;
      }
      return *this;
    }
    SHA1& processBytes( const void* const data, size_t len ) {
      const u8* block = static_cast<const u8*>(data);
      processBlock( block, block + len );
      return *this;
    }
    const u32* getDigest( digest32_t digest ) {
      size_t bitCount = this->m_byteCount * 8;
      processByte( 0x80 );
      if ( this->m_blockByteIndex > 56 ) {
        while ( m_blockByteIndex != 0 ) {
          processByte( 0 );
        }
        while ( m_blockByteIndex < 56 ) {
          processByte( 0 );
        }
      }
      else {
        while ( m_blockByteIndex < 56 ) {
          processByte( 0 );
        }
      }
      processByte( 0 );
      processByte( 0 );
      processByte( 0 );
      processByte( 0 );
      processByte( static_cast<unsigned char>((bitCount >> 24) & 0xFF) );
      processByte( static_cast<unsigned char>((bitCount >> 16) & 0xFF) );
      processByte( static_cast<unsigned char>((bitCount >> 8) & 0xFF) );
      processByte( static_cast<unsigned char>((bitCount) & 0xFF) );

      memcpy( digest, m_digest, 5 * sizeof( u32 ) );
      return digest;
    }
    const u8* getDigestBytes( digest8_t digest ) {
      digest32_t d32;
      getDigest( d32 );
      size_t di = 0;
      digest[di++] = ((d32[0] >> 24) & 0xFF);
      digest[di++] = ((d32[0] >> 16) & 0xFF);
      digest[di++] = ((d32[0] >> 8) & 0xFF);
      digest[di++] = ((d32[0]) & 0xFF);

      digest[di++] = ((d32[1] >> 24) & 0xFF);
      digest[di++] = ((d32[1] >> 16) & 0xFF);
      digest[di++] = ((d32[1] >> 8) & 0xFF);
      digest[di++] = ((d32[1]) & 0xFF);

      digest[di++] = ((d32[2] >> 24) & 0xFF);
      digest[di++] = ((d32[2] >> 16) & 0xFF);
      digest[di++] = ((d32[2] >> 8) & 0xFF);
      digest[di++] = ((d32[2]) & 0xFF);

      digest[di++] = ((d32[3] >> 24) & 0xFF);
      digest[di++] = ((d32[3] >> 16) & 0xFF);
      digest[di++] = ((d32[3] >> 8) & 0xFF);
      digest[di++] = ((d32[3]) & 0xFF);

      digest[di++] = ((d32[4] >> 24) & 0xFF);
      digest[di++] = ((d32[4] >> 16) & 0xFF);
      digest[di++] = ((d32[4] >> 8) & 0xFF);
      digest[di++] = ((d32[4]) & 0xFF);
      return digest;
    }

  protected:
    void processBlock() {
      u32 w[80];
      for ( size_t i = 0; i < 16; i++ ) {
        w[i]  = (m_block[i * 4 + 0] << 24);
        w[i] |= (m_block[i * 4 + 1] << 16);
        w[i] |= (m_block[i * 4 + 2] << 8);
        w[i] |= (m_block[i * 4 + 3]);
      }
      for ( size_t i = 16; i < 80; i++ ) {
        w[i] = LeftRotate( (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]), 1 );
      }

      u32 a = m_digest[0];
      u32 b = m_digest[1];
      u32 c = m_digest[2];
      u32 d = m_digest[3];
      u32 e = m_digest[4];

      for ( size_t i=0; i < 80; ++i ) {
        u32 f = 0;
        u32 k = 0;

        if ( i < 20 ) {
          f = (b & c) | (~b & d);
          k = 0x5A827999;
        }
        else if ( i < 40 ) {
          f = b ^ c ^ d;
          k = 0x6ED9EBA1;
        }
        else if ( i < 60 ) {
          f = (b & c) | (b & d) | (c & d);
          k = 0x8F1BBCDC;
        }
        else {
          f = b ^ c ^ d;
          k = 0xCA62C1D6;
        }
        u32 temp = LeftRotate( a, 5 ) + f + e + k + w[i];
        e = d;
        d = c;
        c = LeftRotate( b, 30 );
        b = a;
        a = temp;
      }

      m_digest[0] += a;
      m_digest[1] += b;
      m_digest[2] += c;
      m_digest[3] += d;
      m_digest[4] += e;
    }
  private:
    digest32_t m_digest;
    u8 m_block[64];
    size_t m_blockByteIndex;
    size_t m_byteCount;
  };


  static bool raw( char const* input, s32 inputSize, char* out_hash, s32 bufferSize )
  {
    u32 digest[5];
    s32 inputLeft = inputSize == 0 ? bs::string_length( input ) : inputSize;

    sha1::SHA1 s;
    s.processBytes( input, inputLeft );
    s.getDigest( digest );

    char* t = (char*) digest;

    if ( out_hash && bufferSize >= 20 )
    {

      for ( s32 i = 0; i < 20; i+=4 )
      {
        out_hash[i]     = t[i + 3];
        out_hash[i + 1] = t[i + 2];
        out_hash[i + 2] = t[i + 1];
        out_hash[i + 3] = t[i];
      }
      return true;
    }

    return false;
  }

  char encode_value_in_base64( s32 value )
  {
    char result = -1;

    if ( value >= 0 )
    {
      if ( value < 26 )
      {
        result = 'A' + (char) value;
      }
      else if ( value < 52 )
      {
        result = 'a' + (char) value - (char) 26;
      }
      else if ( value < 62 )
      {
        result = '0' + (char) value - (char) 52;
      }
      else if ( value == 62 )
      {
        result = '+';
      }
      else if ( value == 63 )
      {
        result = '/';
      }
    }

    return result;
  }

  char* encode_batch_to_base64( char in0, char in1, char in2, char* writer )
  {
    char const mask2bit = 0b00000011;
    char const mask4bit = 0b00001111;
    char const mask6bit = 0b00111111;
    *writer++ = encode_value_in_base64( (in0 >> 2) & mask6bit );
    *writer++ = encode_value_in_base64( ((in0 & mask2bit) << 4) | ((in1 >> 4) & mask4bit) );
    *writer++ = encode_value_in_base64( ((in1 & mask4bit) << 2) | ((in2 >> 6) & mask2bit) );
    *writer++ = encode_value_in_base64( in2 & mask6bit );
    return writer;
  }

  bool to_base64( char const* input, s32 inputSize, char* out_base64, s32 bufferSize )
  {
    if ( out_base64 && bufferSize >= (4 * ((inputSize + 2) / 3)) + 1 )
    {
      char* writer = out_base64;
      s32 i = 0;
      for ( ; i < inputSize - 2; i+=3 )
      {
        writer = encode_batch_to_base64( input[i], input[i + 1], input[i + 2], writer );
      }

      if ( i < inputSize )
      {
        if ( inputSize - i == 1 )
        {
          writer = encode_batch_to_base64( input[i], 0, 0, writer );
          writer[-1] = '=';
          writer[-2] = '=';
        }
        else if ( inputSize - i == 2 )
        {
          writer = encode_batch_to_base64( input[i], input[i + 1], 0, writer );
          writer[-1] = '=';
        }
        else
        {
          assert( 0 );
        }
      }
      *writer = '\0';
    }
    else
    {
      //write buffer not large enough
      return false;
    }


    return true;
  }

  bool to_hex( char const* input, s32 inputSize, char* out_hex, s32 bufferSize )
  {
    if ( out_hex && bufferSize >= (inputSize * 2) + 1 )
    {
      char* writer = out_hex;

      for ( s32 i = 0; i < inputSize; ++i )
      {
        u8 tmp = (input[i] >> 4) & 0x0f;
        tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
        *writer++ = (char) tmp;

        tmp = input[i] & 0x0f;

        tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
        *writer++ = (char) tmp;
      }

      *writer = '\0';

      return true;
    }

    return false;
  }
}

static bool sha1_to_base64( char const* input, s32 inputSize, char* out_hash_base64, s32 bufferSize )
{
  char hash[20];

  if ( sha1::raw( input, inputSize, hash, 20 ) )
  {
    return sha1::to_base64( hash, 20, out_hash_base64, bufferSize );
  }
  else
  {
    assert( 0 );
  }
  return false;
}




static bool sha1_to_hex( char const* input, s32 inputSize, char* out_hash_hex, s32 bufferSize )
{
  char hash[20];

  if ( sha1::raw( input, inputSize, hash, 20 ) )
  {
    return sha1::to_hex( hash, 20, out_hash_hex, bufferSize );
  }
  else
  {
    assert( 0 );
  }
  return false;
}







