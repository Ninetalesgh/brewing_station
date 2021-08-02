#pragma once
#include "preprocessor.h"

#if 1
using s8  = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
#else
# include <stdint.h>
using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#endif

constexpr u8   U8_MAX = 0xff;
constexpr u16 U16_MAX = 0xffff;
constexpr u32 U32_MAX = 0xffffffff;
constexpr u64 U64_MAX = 0xffffffffffffffff;

constexpr s32 min( s32 a, s32 b ) { return a < b ? a : b; }
constexpr s32 max( s32 a, s32 b ) { return a > b ? a : b; }
constexpr s32 clamp( s32 value, s32 low, s32 high ) { return max( min( value, high ), low ); }

constexpr u32 min( u32 a, u32 b ) { return a < b ? a : b; }
constexpr u32 max( u32 a, u32 b ) { return a > b ? a : b; }
constexpr u32 clamp( u32 value, u32 low, u32 high ) { return max( min( value, high ), low ); }

constexpr float min( float a, float b ) { return a < b ? a : b; }
constexpr float max( float a, float b ) { return a > b ? a : b; }
constexpr float clamp( float value, float low, float high ) { return max( min( value, high ), low ); }

constexpr u64 KiloBytes( u64 kiloBytes ) { return kiloBytes * 1024ULL; }
constexpr u64 MegaBytes( u64 megaBytes ) { return KiloBytes( megaBytes ) * 1024ULL; }
constexpr u64 GigaBytes( u64 gigaBytes ) { return MegaBytes( gigaBytes ) * 1024ULL; }