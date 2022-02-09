#pragma once

#include <gl/gl.h>

using GLchar = char;
using GLsizeiptr = ptrdiff_t;

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31

#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define ERROR_INVALID_VERSION_ARB               0x2095
#define ERROR_INVALID_PROFILE_ARB               0x2096

namespace opengl_ext
{
  using glShaderSource = void WINAPI( GLuint shader, GLsizei count, const GLchar** string, const GLint* length );
  using glCreateShader = GLuint WINAPI( GLenum type );
  using glCompileShader = void WINAPI( GLuint shader );
  using glAttachShader = void WINAPI( GLuint program, GLuint shader );
  using glDetachShader = void WINAPI( GLuint program, GLuint shader );
  using glDeleteShader = void WINAPI( GLuint shader );
  using glCreateProgram = GLuint WINAPI( void );
  using glLinkProgram = void WINAPI( GLuint program );
  using glDeleteProgram = void WINAPI( GLuint program );
  using glCreateBuffers = void WINAPI( GLsizei n, GLuint* buffers );
  using glBufferData = void WINAPI( GLenum target, GLsizeiptr size, const void* data, GLenum usage );
  using glBindBuffer = void WINAPI( GLenum target, GLuint buffer );
  using glDeleteBuffers = void WINAPI( GLsizei n, const GLuint* buffers );

  using wglSwapIntervalEXT = BOOL WINAPI( int );

  using wglCreateContextAttribsARB = HGLRC WINAPI( HDC hDC, HGLRC hShareContext, const int* attribList );
}

static opengl_ext::glShaderSource* glShaderSource;
static opengl_ext::glCreateShader* glCreateShader;
static opengl_ext::glCompileShader* glCompileShader;
static opengl_ext::glAttachShader* glAttachShader;
static opengl_ext::glDetachShader* glDetachShader;
static opengl_ext::glDeleteShader* glDeleteShader;
static opengl_ext::glCreateProgram* glCreateProgram;
static opengl_ext::glLinkProgram* glLinkProgram;
static opengl_ext::glDeleteProgram* glDeleteProgram;
static opengl_ext::glCreateBuffers* glCreateBuffers;
static opengl_ext::glBufferData* glBufferData;
static opengl_ext::glBindBuffer* glBindBuffer;
static opengl_ext::glDeleteBuffers* glDeleteBuffers;
static opengl_ext::wglSwapIntervalEXT* wglSwapInterval;
static opengl_ext::wglCreateContextAttribsARB* wglCreateContextAttribsARB;

namespace opengl_ext
{
  void* get_proc_address( char const* functionName );

  u32 init()
  {
    ::wglCreateContextAttribsARB =  (opengl_ext::wglCreateContextAttribsARB*) get_proc_address( "wglCreateContextAttribsARB" );
    ::glShaderSource =  (opengl_ext::glShaderSource*) get_proc_address( "glShaderSource" );
    ::glCreateShader =  (opengl_ext::glCreateShader*) get_proc_address( "glCreateShader" );
    ::glCompileShader = (opengl_ext::glCompileShader*) get_proc_address( "glCompileShader" );
    ::glAttachShader =  (opengl_ext::glAttachShader*) get_proc_address( "glAttachShader" );
    ::glDetachShader =  (opengl_ext::glDetachShader*) get_proc_address( "glDetachShader" );
    ::glDeleteShader =  (opengl_ext::glDeleteShader*) get_proc_address( "glDeleteShader" );
    ::glCreateProgram = (opengl_ext::glCreateProgram*) get_proc_address( "glCreateProgram" );
    ::glLinkProgram =   (opengl_ext::glLinkProgram*) get_proc_address( "glLinkProgram" );
    ::glDeleteProgram = (opengl_ext::glDeleteProgram*) get_proc_address( "glDeleteProgram" );
    ::glCreateBuffers = (opengl_ext::glCreateBuffers*) get_proc_address( "glCreateBuffers" );
    ::glBufferData =    (opengl_ext::glBufferData*) get_proc_address( "glBufferData" );
    ::glBindBuffer =    (opengl_ext::glBindBuffer*) get_proc_address( "glBindBuffer" );
    ::glDeleteBuffers = (opengl_ext::glDeleteBuffers*) get_proc_address( "glDeleteBuffers" );
    ::wglSwapInterval = (opengl_ext::wglSwapIntervalEXT*) get_proc_address( "wglSwapIntervalEXT" );

    if ( ::glShaderSource == nullptr ) BREAK;
    if ( ::glCreateShader == nullptr ) BREAK;
    if ( ::glCompileShader == nullptr ) BREAK;
    if ( ::glAttachShader == nullptr ) BREAK;
    if ( ::glDetachShader == nullptr ) BREAK;
    if ( ::glDeleteShader == nullptr ) BREAK;
    if ( ::glCreateProgram == nullptr ) BREAK;
    if ( ::glLinkProgram == nullptr ) BREAK;
    if ( ::glDeleteProgram == nullptr ) BREAK;
    if ( ::glCreateBuffers == nullptr ) BREAK;
    if ( ::glBufferData == nullptr ) BREAK;
    if ( ::glBindBuffer == nullptr ) BREAK;
    if ( ::glDeleteBuffers == nullptr ) BREAK;

    return 1;
  }

  void* get_proc_address( char const* functionName )
  {
    void* p = (void*) wglGetProcAddress( functionName );
    if ( p == 0 ||
      (p == (void*) 0x1) || (p == (void*) 0x2) || (p == (void*) 0x3) ||
      (p == (void*) -1) )
    {
      HMODULE module = LoadLibraryA( "opengl32.dll" );
      p = (void*) GetProcAddress( module, functionName );
    }

    return p;
  }
}