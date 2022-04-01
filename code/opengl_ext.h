#pragma once

#include <gl/gl.h>
#include <common/bscommon.h>


using GLchar = char;
using GLsizeiptr = s64;

#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

#define GL_RGBA8                          0x8058
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_FRAMEBUFFER_SRGB               0x8DB9

#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB  0x20A9
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_RED_BITS_ARB                  0x2015
#define WGL_RED_SHIFT_ARB                 0x2016
#define WGL_GREEN_BITS_ARB                0x2017
#define WGL_GREEN_SHIFT_ARB               0x2018
#define WGL_BLUE_BITS_ARB                 0x2019
#define WGL_BLUE_SHIFT_ARB                0x201A
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_ALPHA_SHIFT_ARB               0x201C

#define WGL_ACCELERATION_ARB              0x2003

#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_FULL_ACCELERATION_ARB         0x2027

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
  using wglCreateContextAttribsARB = HGLRC WINAPI( HDC hDC, HGLRC hShareContext, const int* attribList );
  using wglChoosePixelFormatARB = BOOL WINAPI( HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats );
  using wglGetPixelFormatAttribivARB = BOOL WINAPI( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, int* piValues );
  using wglGetPixelFormatAttribfvARB = BOOL WINAPI( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, FLOAT* pfValues );
  using wglSwapIntervalEXT = BOOL WINAPI( int );

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
  using glUseProgram = void WINAPI( GLuint program );
  using glGetProgramiv = void WINAPI( GLuint program, GLenum pname, GLint* params );
  using glGetProgramInfoLog = void WINAPI( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
  using glGetShaderiv = void WINAPI( GLuint shader, GLenum pname, GLint* params );
  using glGetShaderInfoLog = void WINAPI( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
  using glVertexAttribPointer = void WINAPI( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer );
  using glDisableVertexAttribArray = void WINAPI( GLuint index );
  using glEnableVertexAttribArray = void WINAPI( GLuint index );
  using glGenBuffers = void WINAPI( GLsizei n, GLuint* buffers );
  using glGenVertexArrays = void WINAPI( GLsizei n, GLuint* arrays );
  using glBindVertexArray = void WINAPI( GLuint array );
  using glDeleteVertexArrays = void WINAPI( GLsizei n, const GLuint* arrays );

  using glGetUniformLocation = GLint WINAPI( GLuint program, const GLchar* name );
  using glGetUniformfv = void WINAPI( GLuint program, GLint location, GLfloat* params );
  using glGetUniformiv = void WINAPI( GLuint program, GLint location, GLint* params );
  using glUniformMatrix4fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
}

static opengl_ext::wglChoosePixelFormatARB* wglChoosePixelFormatARB;
static opengl_ext::wglCreateContextAttribsARB* wglCreateContextAttribsARB;
static opengl_ext::wglGetPixelFormatAttribivARB* wglGetPixelFormatAttribivARB;
static opengl_ext::wglGetPixelFormatAttribfvARB* wglGetPixelFormatAttribfvARB;
static opengl_ext::wglSwapIntervalEXT* wglSwapIntervalEXT;

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
static opengl_ext::glUseProgram* glUseProgram;
static opengl_ext::glGetProgramiv* glGetProgramiv;
static opengl_ext::glGetProgramInfoLog* glGetProgramInfoLog;
static opengl_ext::glGetShaderiv* glGetShaderiv;
static opengl_ext::glGetShaderInfoLog* glGetShaderInfoLog;
static opengl_ext::glVertexAttribPointer* glVertexAttribPointer;
static opengl_ext::glDisableVertexAttribArray* glDisableVertexAttribArray;
static opengl_ext::glEnableVertexAttribArray* glEnableVertexAttribArray;
static opengl_ext::glGenBuffers* glGenBuffers;
static opengl_ext::glGenVertexArrays* glGenVertexArrays;
static opengl_ext::glBindVertexArray* glBindVertexArray;
static opengl_ext::glDeleteVertexArrays* glDeleteVertexArrays;

static opengl_ext::glGetUniformLocation* glGetUniformLocation;
static opengl_ext::glGetUniformfv* glGetUniformfv;
static opengl_ext::glGetUniformiv* glGetUniformiv;
static opengl_ext::glUniformMatrix4fv* glUniformMatrix4fv;

namespace opengl_ext
{
  void* get_proc_address( char const* functionName );
  u32 validate();

  u32 init()
  {
    ::wglChoosePixelFormatARB = (opengl_ext::wglChoosePixelFormatARB*) get_proc_address( "wglChoosePixelFormatARB" );
    ::wglCreateContextAttribsARB =  (opengl_ext::wglCreateContextAttribsARB*) get_proc_address( "wglCreateContextAttribsARB" );
    ::wglGetPixelFormatAttribivARB = (opengl_ext::wglGetPixelFormatAttribivARB*) get_proc_address( "wglGetPixelFormatAttribivARB" );
    ::wglGetPixelFormatAttribfvARB = (opengl_ext::wglGetPixelFormatAttribfvARB*) get_proc_address( "wglGetPixelFormatAttribfvARB" );
    ::wglSwapIntervalEXT = (opengl_ext::wglSwapIntervalEXT*) get_proc_address( "wglSwapIntervalEXT" );
    ::glShaderSource =             (opengl_ext::glShaderSource*) get_proc_address( "glShaderSource" );
    ::glCreateShader =             (opengl_ext::glCreateShader*) get_proc_address( "glCreateShader" );
    ::glCompileShader =            (opengl_ext::glCompileShader*) get_proc_address( "glCompileShader" );
    ::glAttachShader =             (opengl_ext::glAttachShader*) get_proc_address( "glAttachShader" );
    ::glDetachShader =             (opengl_ext::glDetachShader*) get_proc_address( "glDetachShader" );
    ::glDeleteShader =             (opengl_ext::glDeleteShader*) get_proc_address( "glDeleteShader" );
    ::glCreateProgram =            (opengl_ext::glCreateProgram*) get_proc_address( "glCreateProgram" );
    ::glLinkProgram =              (opengl_ext::glLinkProgram*) get_proc_address( "glLinkProgram" );
    ::glDeleteProgram =            (opengl_ext::glDeleteProgram*) get_proc_address( "glDeleteProgram" );
    ::glCreateBuffers =            (opengl_ext::glCreateBuffers*) get_proc_address( "glCreateBuffers" );
    ::glBufferData =               (opengl_ext::glBufferData*) get_proc_address( "glBufferData" );
    ::glBindBuffer =               (opengl_ext::glBindBuffer*) get_proc_address( "glBindBuffer" );
    ::glDeleteBuffers =            (opengl_ext::glDeleteBuffers*) get_proc_address( "glDeleteBuffers" );
    ::glUseProgram =               (opengl_ext::glUseProgram*) get_proc_address( "glUseProgram" );
    ::glGetProgramiv =             (opengl_ext::glGetProgramiv*) get_proc_address( "glGetProgramiv" );
    ::glGetProgramInfoLog =        (opengl_ext::glGetProgramInfoLog*) get_proc_address( "glGetProgramInfoLog" );
    ::glGetShaderiv =              (opengl_ext::glGetShaderiv*) get_proc_address( "glGetShaderiv" );
    ::glGetShaderInfoLog =         (opengl_ext::glGetShaderInfoLog*) get_proc_address( "glGetShaderInfoLog" );
    ::glVertexAttribPointer =      (opengl_ext::glVertexAttribPointer*) get_proc_address( "glVertexAttribPointer" );
    ::glDisableVertexAttribArray = (opengl_ext::glDisableVertexAttribArray*) get_proc_address( "glDisableVertexAttribArray" );
    ::glEnableVertexAttribArray =  (opengl_ext::glEnableVertexAttribArray*) get_proc_address( "glEnableVertexAttribArray" );
    ::glGenBuffers =               (opengl_ext::glGenBuffers*) get_proc_address( "glGenBuffers" );
    ::glGenVertexArrays =          (opengl_ext::glGenVertexArrays*) get_proc_address( "glGenVertexArrays" );
    ::glBindVertexArray =          (opengl_ext::glBindVertexArray*) get_proc_address( "glBindVertexArray" );
    ::glDeleteVertexArrays =       (opengl_ext::glDeleteVertexArrays*) get_proc_address( "glDeleteVertexArrays" );

    ::glGetUniformLocation = (opengl_ext::glGetUniformLocation*) get_proc_address( "glGetUniformLocation" );
    ::glGetUniformfv = (opengl_ext::glGetUniformfv*) get_proc_address( "glGetUniformfv" );
    ::glGetUniformiv = (opengl_ext::glGetUniformiv*) get_proc_address( "glGetUniformiv" );
    ::glUniformMatrix4fv = (opengl_ext::glUniformMatrix4fv*) get_proc_address( "glUniformMatrix4fv" );

    return validate();
  }

  u32 validate()
  {
    if ( ::wglChoosePixelFormatARB == nullptr ) BREAK;
    if ( ::wglCreateContextAttribsARB == nullptr ) BREAK;
    if ( ::wglGetPixelFormatAttribivARB == nullptr ) BREAK;
    if ( ::wglGetPixelFormatAttribfvARB == nullptr ) BREAK;
    if ( ::wglSwapIntervalEXT == nullptr ) BREAK;
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
    if ( ::glUseProgram == nullptr ) BREAK;
    if ( ::glGetProgramiv == nullptr ) BREAK;
    if ( ::glGetProgramInfoLog == nullptr ) BREAK;
    if ( ::glGetShaderiv == nullptr ) BREAK;
    if ( ::glGetShaderInfoLog == nullptr ) BREAK;
    if ( ::glVertexAttribPointer == nullptr ) BREAK;
    if ( ::glDisableVertexAttribArray == nullptr ) BREAK;
    if ( ::glEnableVertexAttribArray == nullptr ) BREAK;
    if ( ::glGenBuffers == nullptr ) BREAK;
    if ( ::glGenVertexArrays == nullptr ) BREAK;
    if ( ::glBindVertexArray == nullptr ) BREAK;
    if ( ::glDeleteVertexArrays == nullptr ) BREAK;

    if ( ::glGetUniformLocation == nullptr ) BREAK;
    if ( ::glGetUniformfv == nullptr ) BREAK;
    if ( ::glGetUniformiv == nullptr ) BREAK;
    if ( ::glUniformMatrix4fv == nullptr ) BREAK;

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

