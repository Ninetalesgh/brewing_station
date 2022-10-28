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

  //make sure you call set_callbacks on any module including this before calling any opengl functions! 
  u32 validate_callbacks();
  u32 set_callbacks( OpenGLExtCallbacks const& callbacks );
  u32 get_callbacks( OpenGLExtCallbacks& out_callbacks );

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

  // OPENGL 4.5 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glCreateBuffers            = void WINAPI( GLsizei n, GLuint* buffers );
  using glNamedBufferData          = void WINAPI( GLuint buffer, GLsizeiptr size, const void* data, GLenum usage );
  using glCreateVertexArrays       = void WINAPI( GLsizei n, GLuint* arrays );
  using glEnableVertexArrayAttrib  = void WINAPI( GLuint vaobj, GLuint index );
  using glVertexArrayElementBuffer = void WINAPI( GLuint vaobj, GLuint buffer );
  using glVertexArrayAttribBinding = void WINAPI( GLuint vaobj, GLuint attribindex, GLuint bindingindex );
  using glVertexArrayAttribFormat  = void WINAPI( GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset );
  using glVertexArrayVertexBuffer  = void WINAPI( GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride );
  //GLAPI void APIENTRY glNamedBufferSubData (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);

  struct OpenGLExtCallbacks
  {
    opengl_ext::glShaderSource* ptr_glShaderSource;
    opengl_ext::glCreateShader* ptr_glCreateShader;
    opengl_ext::glCompileShader* ptr_glCompileShader;
    opengl_ext::glAttachShader* ptr_glAttachShader;
    opengl_ext::glDetachShader* ptr_glDetachShader;
    opengl_ext::glDeleteShader* ptr_glDeleteShader;
    opengl_ext::glCreateProgram* ptr_glCreateProgram;
    opengl_ext::glLinkProgram* ptr_glLinkProgram;
    opengl_ext::glDeleteProgram* ptr_glDeleteProgram;
    opengl_ext::glBufferData* ptr_glBufferData;
    opengl_ext::glBindBuffer* ptr_glBindBuffer;
    opengl_ext::glDeleteBuffers* ptr_glDeleteBuffers;
    opengl_ext::glUseProgram* ptr_glUseProgram;
    opengl_ext::glGetProgramiv* ptr_glGetProgramiv;
    opengl_ext::glGetProgramInfoLog* ptr_glGetProgramInfoLog;
    opengl_ext::glGetShaderiv* ptr_glGetShaderiv;
    opengl_ext::glGetShaderInfoLog* ptr_glGetShaderInfoLog;
    opengl_ext::glVertexAttribPointer* ptr_glVertexAttribPointer;
    opengl_ext::glDisableVertexAttribArray* ptr_glDisableVertexAttribArray;
    opengl_ext::glEnableVertexAttribArray* ptr_glEnableVertexAttribArray;
    opengl_ext::glGenBuffers* ptr_glGenBuffers;
    opengl_ext::glGetUniformLocation* ptr_glGetUniformLocation;
    opengl_ext::glGetUniformfv* ptr_glGetUniformfv;
    opengl_ext::glGetUniformiv* ptr_glGetUniformiv;
    opengl_ext::glUniformMatrix4fv* ptr_glUniformMatrix4fv;
    opengl_ext::glGenVertexArrays* ptr_glGenVertexArrays;
    opengl_ext::glBindVertexArray* ptr_glBindVertexArray;
    opengl_ext::glDeleteVertexArrays* ptr_glDeleteVertexArrays;
    opengl_ext::glCreateBuffers* ptr_glCreateBuffers;
    opengl_ext::glNamedBufferData* ptr_glNamedBufferData;
    opengl_ext::glCreateVertexArrays* ptr_glCreateVertexArrays;
    opengl_ext::glEnableVertexArrayAttrib* ptr_glEnableVertexArrayAttrib;
    opengl_ext::glVertexArrayElementBuffer* ptr_glVertexArrayElementBuffer;
    opengl_ext::glVertexArrayAttribBinding* ptr_glVertexArrayAttribBinding;
    opengl_ext::glVertexArrayAttribFormat* ptr_glVertexArrayAttribFormat;
    opengl_ext::glVertexArrayVertexBuffer* ptr_glVertexArrayVertexBuffer;
  };
};

static opengl_ext::glShaderSource* glShaderSource;
static opengl_ext::glCreateShader* glCreateShader;
static opengl_ext::glCompileShader* glCompileShader;
static opengl_ext::glAttachShader* glAttachShader;
static opengl_ext::glDetachShader* glDetachShader;
static opengl_ext::glDeleteShader* glDeleteShader;
static opengl_ext::glCreateProgram* glCreateProgram;
static opengl_ext::glLinkProgram* glLinkProgram;
static opengl_ext::glDeleteProgram* glDeleteProgram;
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
static opengl_ext::glGetUniformLocation* glGetUniformLocation;
static opengl_ext::glGetUniformfv* glGetUniformfv;
static opengl_ext::glGetUniformiv* glGetUniformiv;
static opengl_ext::glUniformMatrix4fv* glUniformMatrix4fv;
static opengl_ext::glGenVertexArrays* glGenVertexArrays;
static opengl_ext::glBindVertexArray* glBindVertexArray;
static opengl_ext::glDeleteVertexArrays* glDeleteVertexArrays;
static opengl_ext::glCreateBuffers* glCreateBuffers;
static opengl_ext::glNamedBufferData* glNamedBufferData;
static opengl_ext::glCreateVertexArrays* glCreateVertexArrays;
static opengl_ext::glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
static opengl_ext::glVertexArrayElementBuffer* glVertexArrayElementBuffer;
static opengl_ext::glVertexArrayAttribBinding* glVertexArrayAttribBinding;
static opengl_ext::glVertexArrayAttribFormat* glVertexArrayAttribFormat;
static opengl_ext::glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;

namespace opengl_ext
{
  u32 validate_callbacks()
  {
    u32 result = 1;

    if ( ::glShaderSource == nullptr ) { result = 0; BREAK; }
    if ( ::glCreateShader == nullptr ) { result = 0; BREAK; }
    if ( ::glCompileShader == nullptr ) { result = 0; BREAK; }
    if ( ::glAttachShader == nullptr ) { result = 0; BREAK; }
    if ( ::glDetachShader == nullptr ) { result = 0; BREAK; }
    if ( ::glDeleteShader == nullptr ) { result = 0; BREAK; }
    if ( ::glCreateProgram == nullptr ) { result = 0; BREAK; }
    if ( ::glLinkProgram == nullptr ) { result = 0; BREAK; }
    if ( ::glDeleteProgram == nullptr ) { result = 0; BREAK; }
    if ( ::glBufferData == nullptr ) { result = 0; BREAK; }
    if ( ::glBindBuffer == nullptr ) { result = 0; BREAK; }
    if ( ::glDeleteBuffers == nullptr ) { result = 0; BREAK; }
    if ( ::glUseProgram == nullptr ) { result = 0; BREAK; }
    if ( ::glGetProgramiv == nullptr ) { result = 0; BREAK; }
    if ( ::glGetProgramInfoLog == nullptr ) { result = 0; BREAK; }
    if ( ::glGetShaderiv == nullptr ) { result = 0; BREAK; }
    if ( ::glGetShaderInfoLog == nullptr ) { result = 0; BREAK; }
    if ( ::glVertexAttribPointer == nullptr ) { result = 0; BREAK; }
    if ( ::glDisableVertexAttribArray == nullptr ) { result = 0; BREAK; }
    if ( ::glEnableVertexAttribArray == nullptr ) { result = 0; BREAK; }
    if ( ::glGenBuffers == nullptr ) { result = 0; BREAK; }
    if ( ::glGetUniformLocation == nullptr ) { result = 0; BREAK; }
    if ( ::glGetUniformfv == nullptr ) { result = 0; BREAK; }
    if ( ::glGetUniformiv == nullptr ) { result = 0; BREAK; }
    if ( ::glUniformMatrix4fv == nullptr ) { result = 0; BREAK; }
    if ( ::glGenVertexArrays == nullptr ) { result = 0; BREAK; }
    if ( ::glBindVertexArray == nullptr ) { result = 0; BREAK; }
    if ( ::glDeleteVertexArrays == nullptr ) { result = 0; BREAK; }
    if ( ::glCreateBuffers == nullptr ) { result = 0; BREAK; }
    if ( ::glNamedBufferData == nullptr ) { result = 0; BREAK; }
    if ( ::glCreateVertexArrays == nullptr ) { result = 0; BREAK; }
    if ( ::glEnableVertexArrayAttrib == nullptr ) { result = 0; BREAK; }
    if ( ::glVertexArrayElementBuffer == nullptr ) { result = 0; BREAK; }
    if ( ::glVertexArrayAttribBinding == nullptr ) { result = 0; BREAK; }
    if ( ::glVertexArrayAttribFormat == nullptr ) { result = 0; BREAK; }
    if ( ::glVertexArrayVertexBuffer == nullptr ) { result = 0; BREAK; }

    return result;
  }

  u32 set_callbacks( OpenGLExtCallbacks const& callbacks )
  {
    ::glShaderSource = callbacks.ptr_glShaderSource;
    ::glCreateShader = callbacks.ptr_glCreateShader;
    ::glCompileShader = callbacks.ptr_glCompileShader;
    ::glAttachShader = callbacks.ptr_glAttachShader;
    ::glDetachShader = callbacks.ptr_glDetachShader;
    ::glDeleteShader = callbacks.ptr_glDeleteShader;
    ::glCreateProgram = callbacks.ptr_glCreateProgram;
    ::glLinkProgram = callbacks.ptr_glLinkProgram;
    ::glDeleteProgram = callbacks.ptr_glDeleteProgram;
    ::glBufferData = callbacks.ptr_glBufferData;
    ::glBindBuffer = callbacks.ptr_glBindBuffer;
    ::glDeleteBuffers = callbacks.ptr_glDeleteBuffers;
    ::glUseProgram = callbacks.ptr_glUseProgram;
    ::glGetProgramiv = callbacks.ptr_glGetProgramiv;
    ::glGetProgramInfoLog = callbacks.ptr_glGetProgramInfoLog;
    ::glGetShaderiv = callbacks.ptr_glGetShaderiv;
    ::glGetShaderInfoLog = callbacks.ptr_glGetShaderInfoLog;
    ::glVertexAttribPointer = callbacks.ptr_glVertexAttribPointer;
    ::glDisableVertexAttribArray = callbacks.ptr_glDisableVertexAttribArray;
    ::glEnableVertexAttribArray = callbacks.ptr_glEnableVertexAttribArray;
    ::glGenBuffers = callbacks.ptr_glGenBuffers;
    ::glGetUniformLocation = callbacks.ptr_glGetUniformLocation;
    ::glGetUniformfv = callbacks.ptr_glGetUniformfv;
    ::glGetUniformiv = callbacks.ptr_glGetUniformiv;
    ::glUniformMatrix4fv = callbacks.ptr_glUniformMatrix4fv;
    ::glGenVertexArrays = callbacks.ptr_glGenVertexArrays;
    ::glBindVertexArray = callbacks.ptr_glBindVertexArray;
    ::glDeleteVertexArrays = callbacks.ptr_glDeleteVertexArrays;
    ::glCreateBuffers = callbacks.ptr_glCreateBuffers;
    ::glNamedBufferData = callbacks.ptr_glNamedBufferData;
    ::glCreateVertexArrays = callbacks.ptr_glCreateVertexArrays;
    ::glEnableVertexArrayAttrib = callbacks.ptr_glEnableVertexArrayAttrib;
    ::glVertexArrayElementBuffer = callbacks.ptr_glVertexArrayElementBuffer;
    ::glVertexArrayAttribBinding = callbacks.ptr_glVertexArrayAttribBinding;
    ::glVertexArrayAttribFormat = callbacks.ptr_glVertexArrayAttribFormat;
    ::glVertexArrayVertexBuffer = callbacks.ptr_glVertexArrayVertexBuffer;
    return validate_callbacks();
  }

  u32 get_callbacks( OpenGLExtCallbacks& out_callbacks )
  {
    out_callbacks.ptr_glShaderSource = ::glShaderSource;
    out_callbacks.ptr_glCreateShader = ::glCreateShader;
    out_callbacks.ptr_glCompileShader = ::glCompileShader;
    out_callbacks.ptr_glAttachShader = ::glAttachShader;
    out_callbacks.ptr_glDetachShader = ::glDetachShader;
    out_callbacks.ptr_glDeleteShader = ::glDeleteShader;
    out_callbacks.ptr_glCreateProgram = ::glCreateProgram;
    out_callbacks.ptr_glLinkProgram = ::glLinkProgram;
    out_callbacks.ptr_glDeleteProgram = ::glDeleteProgram;
    out_callbacks.ptr_glBufferData = ::glBufferData;
    out_callbacks.ptr_glBindBuffer = ::glBindBuffer;
    out_callbacks.ptr_glDeleteBuffers = ::glDeleteBuffers;
    out_callbacks.ptr_glUseProgram = ::glUseProgram;
    out_callbacks.ptr_glGetProgramiv = ::glGetProgramiv;
    out_callbacks.ptr_glGetProgramInfoLog = ::glGetProgramInfoLog;
    out_callbacks.ptr_glGetShaderiv = ::glGetShaderiv;
    out_callbacks.ptr_glGetShaderInfoLog = ::glGetShaderInfoLog;
    out_callbacks.ptr_glVertexAttribPointer = ::glVertexAttribPointer;
    out_callbacks.ptr_glDisableVertexAttribArray = ::glDisableVertexAttribArray;
    out_callbacks.ptr_glEnableVertexAttribArray = ::glEnableVertexAttribArray;
    out_callbacks.ptr_glGenBuffers = ::glGenBuffers;
    out_callbacks.ptr_glGetUniformLocation = ::glGetUniformLocation;
    out_callbacks.ptr_glGetUniformfv = ::glGetUniformfv;
    out_callbacks.ptr_glGetUniformiv = ::glGetUniformiv;
    out_callbacks.ptr_glUniformMatrix4fv = ::glUniformMatrix4fv;
    out_callbacks.ptr_glGenVertexArrays = ::glGenVertexArrays;
    out_callbacks.ptr_glBindVertexArray = ::glBindVertexArray;
    out_callbacks.ptr_glDeleteVertexArrays = ::glDeleteVertexArrays;
    out_callbacks.ptr_glCreateBuffers = ::glCreateBuffers;
    out_callbacks.ptr_glNamedBufferData = ::glNamedBufferData;
    out_callbacks.ptr_glCreateVertexArrays = ::glCreateVertexArrays;
    out_callbacks.ptr_glEnableVertexArrayAttrib = ::glEnableVertexArrayAttrib;
    out_callbacks.ptr_glVertexArrayElementBuffer = ::glVertexArrayElementBuffer;
    out_callbacks.ptr_glVertexArrayAttribBinding = ::glVertexArrayAttribBinding;
    out_callbacks.ptr_glVertexArrayAttribFormat = ::glVertexArrayAttribFormat;
    out_callbacks.ptr_glVertexArrayVertexBuffer = ::glVertexArrayVertexBuffer;
    return validate_callbacks();
  }

};