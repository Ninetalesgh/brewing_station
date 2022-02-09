#pragma once

#if 1
# if defined(_WIN32)
#   define INLINE __forceinline
# else
#   define INLINE inline
# endif
#else
# define INLINE
#endif

//TODO not hardcode these as soon as 32 bit is needed
#define BS_BUILD_64BIT
#define BS_BUILD_32BIT 

#if BS_BUILD_DEBUG
# define BREAK __debugbreak()
# define DEPRECATED_CODE_BREAK __debugbreak()
# define TODO __debugbreak()
#else
# define BREAK (void(0))
# define DEPRECATED_CODE_BREAK (void(0))
# define TODO dontcompilethis
#endif

#if BS_BUILD_DEBUG
//# define assert(expression) { if ( !(expression) ) *(s32*) 0=0; }
#ifdef assert
# undef assert
#endif

# define assert(expression) { if ( !(expression) ) BREAK; }
#else
# define assert(expression)
#endif


#define array_size(array) (sizeof(array) / (sizeof((array)[0])))

#define global_variable            static
#define local_persist              static
#define constexpr_member constexpr static




#define _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, basictype)\
INLINE enumtypename  operator |  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) | basictype( b ) ); }\
INLINE enumtypename& operator |= ( enumtypename& a, enumtypename b ) { a = b | a; return a; }\
INLINE enumtypename  operator &  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) & basictype( b ) ); }\
INLINE enumtypename& operator &= ( enumtypename& a, enumtypename b ) { a = b & a; return a; }\
INLINE enumtypename  operator ~  ( enumtypename a ) { return enumtypename( ~basictype( a ) ); }\
INLINE enumtypename  operator ^  ( enumtypename a, enumtypename b ) { return enumtypename( basictype( a ) ^ basictype( b ) ); }\
INLINE enumtypename& operator ^= ( enumtypename& a, enumtypename b ) { a = b ^ a; return a; }\

#define DEFINE_ENUM_OPERATORS_U8(enumtypename)  _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u8)
#define DEFINE_ENUM_OPERATORS_U16(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u16)
#define DEFINE_ENUM_OPERATORS_U32(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u32)
#define DEFINE_ENUM_OPERATORS_U64(enumtypename) _DEFINE_ENUM_OPERATORS_INTERNAL(enumtypename, u64)


