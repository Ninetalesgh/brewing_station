#pragma once

#include <common/bscommon.h>

#include <gl/gl.h>

using GLchar = char;
using GLsizeiptr = s64;
using GLintptr = intptr_t;
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
  struct OpenGLExtCallbacks;

  u32 validate_callbacks();

  // OPENGL 2.0 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glShaderSource             = void   WINAPI( GLuint shader, GLsizei count, const GLchar** string, const GLint* length );
  using glCreateShader             = GLuint WINAPI( GLenum type );
  using glCompileShader            = void   WINAPI( GLuint shader );
  using glAttachShader             = void   WINAPI( GLuint program, GLuint shader );
  using glDetachShader             = void   WINAPI( GLuint program, GLuint shader );
  using glDeleteShader             = void   WINAPI( GLuint shader );
  using glCreateProgram            = GLuint WINAPI( void );
  using glLinkProgram              = void   WINAPI( GLuint program );
  using glDeleteProgram            = void   WINAPI( GLuint program );
  using glBufferData               = void   WINAPI( GLenum target, GLsizeiptr size, const void* data, GLenum usage );
  using glBufferSubData            = void   WINAPI( GLenum target, GLintptr offset, GLsizeiptr size, const void* data );
  using glBindBuffer               = void   WINAPI( GLenum target, GLuint buffer );
  using glDeleteBuffers            = void   WINAPI( GLsizei n, const GLuint* buffers );
  using glUseProgram               = void   WINAPI( GLuint program );
  using glGetProgramiv             = void   WINAPI( GLuint program, GLenum pname, GLint* params );
  using glGetProgramInfoLog        = void   WINAPI( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
  using glGetShaderiv              = void   WINAPI( GLuint shader, GLenum pname, GLint* params );
  using glGetShaderInfoLog         = void   WINAPI( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
  using glVertexAttribPointer      = void   WINAPI( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer );
  using glDisableVertexAttribArray = void   WINAPI( GLuint index );
  using glEnableVertexAttribArray  = void   WINAPI( GLuint index );
  using glGenBuffers               = void   WINAPI( GLsizei n, GLuint* buffers );
  using glGetUniformLocation       = GLint  WINAPI( GLuint program, const GLchar* name );
  using glGetUniformfv             = void   WINAPI( GLuint program, GLint location, GLfloat* params );
  using glGetUniformiv             = void   WINAPI( GLuint program, GLint location, GLint* params );
  using glUniformMatrix4fv         = void   WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );

  // OPENGL 3.0 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glGenVertexArrays    = void WINAPI( GLsizei n, GLuint* arrays );
  using glBindVertexArray    = void WINAPI( GLuint array );
  using glDeleteVertexArrays = void WINAPI( GLsizei n, const GLuint* arrays );
  using glBindBufferBase     = void WINAPI( GLenum target, GLuint index, GLuint buffer );
  using glBindBufferRange    = void WINAPI( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size );

  // OPENGL 3.1 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glGetUniformBlockIndex = GLuint WINAPI( GLuint program, const GLchar* uniformBlockName );
  using glUniformBlockBinding  = void   WINAPI( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding );

  // OPENGL 4.5 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glCreateBuffers            = void WINAPI( GLsizei n, GLuint* buffers );
  using glNamedBufferData          = void WINAPI( GLuint buffer, GLsizeiptr size, const void* data, GLenum usage );
  using glNamedBufferSubData       = void WINAPI( GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data );
  using glCreateVertexArrays       = void WINAPI( GLsizei n, GLuint* arrays );
  using glEnableVertexArrayAttrib  = void WINAPI( GLuint vaobj, GLuint index );
  using glVertexArrayElementBuffer = void WINAPI( GLuint vaobj, GLuint buffer );
  using glVertexArrayAttribBinding = void WINAPI( GLuint vaobj, GLuint attribindex, GLuint bindingindex );
  using glVertexArrayAttribFormat  = void WINAPI( GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset );
  using glVertexArrayVertexBuffer  = void WINAPI( GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride );
  //GLAPI void APIENTRY glNamedBufferSubData (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);

};

#define DEFINE_GL(fn) static opengl_ext::fn* fn

DEFINE_GL( glShaderSource );
DEFINE_GL( glCreateShader );
DEFINE_GL( glCompileShader );
DEFINE_GL( glAttachShader );
DEFINE_GL( glDetachShader );
DEFINE_GL( glDeleteShader );
DEFINE_GL( glCreateProgram );
DEFINE_GL( glLinkProgram );
DEFINE_GL( glDeleteProgram );
DEFINE_GL( glBufferData );
DEFINE_GL( glBufferSubData );
DEFINE_GL( glBindBuffer );
DEFINE_GL( glDeleteBuffers );
DEFINE_GL( glUseProgram );
DEFINE_GL( glGetProgramiv );
DEFINE_GL( glGetProgramInfoLog );
DEFINE_GL( glGetShaderiv );
DEFINE_GL( glGetShaderInfoLog );
DEFINE_GL( glVertexAttribPointer );
DEFINE_GL( glDisableVertexAttribArray );
DEFINE_GL( glEnableVertexAttribArray );
DEFINE_GL( glGenBuffers );
DEFINE_GL( glGetUniformLocation );
DEFINE_GL( glGetUniformfv );
DEFINE_GL( glGetUniformiv );
DEFINE_GL( glUniformMatrix4fv );
DEFINE_GL( glGenVertexArrays );
DEFINE_GL( glBindVertexArray );
DEFINE_GL( glDeleteVertexArrays );
DEFINE_GL( glBindBufferBase );
DEFINE_GL( glBindBufferRange );
DEFINE_GL( glGetUniformBlockIndex );
DEFINE_GL( glUniformBlockBinding );
DEFINE_GL( glCreateBuffers );
DEFINE_GL( glNamedBufferData );
DEFINE_GL( glNamedBufferSubData );
DEFINE_GL( glCreateVertexArrays );
DEFINE_GL( glEnableVertexArrayAttrib );
DEFINE_GL( glVertexArrayElementBuffer );
DEFINE_GL( glVertexArrayAttribBinding );
DEFINE_GL( glVertexArrayAttribFormat );
DEFINE_GL( glVertexArrayVertexBuffer );

namespace opengl_ext
{
  void* get_proc_address( char const* functionName );

  #define INIT_GL(fn) ::fn = (opengl_ext::fn*) get_proc_address(#fn); if (::fn == nullptr) { result = 0; BREAK; }
  u32 init()
  {
    u32 result = 1;
    INIT_GL( glShaderSource );
    INIT_GL( glCreateShader );
    INIT_GL( glCompileShader );
    INIT_GL( glAttachShader );
    INIT_GL( glDetachShader );
    INIT_GL( glDeleteShader );
    INIT_GL( glCreateProgram );
    INIT_GL( glLinkProgram );
    INIT_GL( glDeleteProgram );
    INIT_GL( glBufferData );
    INIT_GL( glBufferSubData );
    INIT_GL( glBindBuffer );
    INIT_GL( glDeleteBuffers );
    INIT_GL( glUseProgram );
    INIT_GL( glGetProgramiv );
    INIT_GL( glGetProgramInfoLog );
    INIT_GL( glGetShaderiv );
    INIT_GL( glGetShaderInfoLog );
    INIT_GL( glVertexAttribPointer );
    INIT_GL( glDisableVertexAttribArray );
    INIT_GL( glEnableVertexAttribArray );
    INIT_GL( glGenBuffers );
    INIT_GL( glGetUniformLocation );
    INIT_GL( glGetUniformfv );
    INIT_GL( glGetUniformiv );
    INIT_GL( glUniformMatrix4fv );
    INIT_GL( glGenVertexArrays );
    INIT_GL( glBindVertexArray );
    INIT_GL( glDeleteVertexArrays );
    INIT_GL( glBindBufferBase );
    INIT_GL( glBindBufferRange );
    INIT_GL( glGetUniformBlockIndex );
    INIT_GL( glUniformBlockBinding );
    INIT_GL( glCreateBuffers );
    INIT_GL( glNamedBufferData );
    INIT_GL( glNamedBufferSubData );
    INIT_GL( glCreateVertexArrays );
    INIT_GL( glEnableVertexArrayAttrib );
    INIT_GL( glVertexArrayElementBuffer );
    INIT_GL( glVertexArrayAttribBinding );
    INIT_GL( glVertexArrayAttribFormat );
    INIT_GL( glVertexArrayVertexBuffer );

    return result;
  }
};


