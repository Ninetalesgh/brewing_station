#pragma once

# ifdef _WIN32
#   define INLINE __forceinline
# else
#   define INLINE inline
# endif

#ifdef assert
# undef assert
#endif

#ifdef BS_DEBUG
# define BREAK __debugbreak()
# define assert(expression) { if ( !(expression) ) BREAK; }
#else
# define BREAK
# define assert(expression)
#endif

#define array_count(array) (sizeof(array) / (sizeof((array)[0])))

#define _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, basictype)\
INLINE enumtypename  operator |  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) | basictype( b ) ); }\
INLINE enumtypename& operator |= ( enumtypename& a, enumtypename b ) { a = b | a; return a; }\
INLINE enumtypename  operator &  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) & basictype( b ) ); }\
INLINE enumtypename& operator &= ( enumtypename& a, enumtypename b ) { a = b & a; return a; }\
INLINE enumtypename  operator ~  ( enumtypename a ) { return enumtypename( ~basictype( a ) ); }\
INLINE enumtypename  operator ^  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) ^ basictype( b ) ); }\
INLINE enumtypename& operator ^= ( enumtypename& a, enumtypename b ) { a = b ^ a; return a; }

#define DEFINE_ENUM_OPERATORS_U8(enumtypename)  _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u8)
#define DEFINE_ENUM_OPERATORS_U16(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u16)
#define DEFINE_ENUM_OPERATORS_U32(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u32)
#define DEFINE_ENUM_OPERATORS_U64(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u64)

using s8  = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using voidfn = void();

constexpr u8   U8_MAX = 0xff;
constexpr u16 U16_MAX = 0xffff;
constexpr u32 U32_MAX = 0xffffffff;
constexpr u64 U64_MAX = 0xffffffffffffffff;

constexpr s8   S8_MAX = 0x7f;
constexpr s16 S16_MAX = 0x7fff;
constexpr s32 S32_MAX = 0x7fffffff;
constexpr s64 S64_MAX = 0x7fffffffffffffff;

constexpr INLINE s32 min( s32 a, s32 b ) { return a < b ? a : b; }
constexpr INLINE s32 max( s32 a, s32 b ) { return a > b ? a : b; }
constexpr INLINE s32 clamp( s32 value, s32 low, s32 high ) { return max( min( value, high ), low ); }

constexpr INLINE u32 min( u32 a, u32 b ) { return a < b ? a : b; }
constexpr INLINE u32 max( u32 a, u32 b ) { return a > b ? a : b; }
constexpr INLINE u32 clamp( u32 value, u32 low, u32 high ) { return max( min( value, high ), low ); }

constexpr INLINE float min( float a, float b ) { return a < b ? a : b; }
constexpr INLINE float max( float a, float b ) { return a > b ? a : b; }
constexpr INLINE float clamp( float value, float low, float high ) { return max( min( value, high ), low ); }

constexpr INLINE u64 KiloBytes( u64 kiloBytes ) { return kiloBytes * 1024ULL; }
constexpr INLINE u64 MegaBytes( u64 megaBytes ) { return KiloBytes( megaBytes ) * 1024ULL; }
constexpr INLINE u64 GigaBytes( u64 gigaBytes ) { return MegaBytes( gigaBytes ) * 1024ULL; }


namespace bs
{
  //PROBABLY REMOVING THIS POOP TODO
  namespace internal { template<class T> struct rem_ref { using type = T; };template<class T> struct rem_ref<T&> { using type = T; };template<class T> struct rem_ref<T&&> { using type = T; }; };

  template <class T> using remove_reference = typename internal::rem_ref<T>::type;
  template <class T> constexpr remove_reference<T>&& move( T&& object ) { return static_cast<remove_reference<T>&&>(object); }

  template <class T>
  class ownptr
  {
  public:
    ownptr( ownptr<T>&& _movedPtr )
      : ptr( _movedPtr )
    {
      _movedPtr.ptr = nullptr;
    }
    operator T* () { return ptr; }
  private:
    ownptr() = delete;
    T* ptr;
  };

  template <class T>
  class refptr
  {
  public:
    refptr( ownptr<T> const& _ptr )
      : ptr( _ptr )
    {}

    operator T* () { return ptr; }

  private:
    refptr() = delete;
    T* ptr;
  };
};