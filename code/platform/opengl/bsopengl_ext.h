#pragma once

#include <common/bs_common.h>

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
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_SHADER_TYPE                    0x8B4F
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#define GL_SAMPLER_1D_SHADOW              0x8B61
#define GL_SAMPLER_2D_SHADOW              0x8B62

#define GL_RGBA8                          0x8058
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_FRAMEBUFFER_SRGB               0x8DB9

#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_UNIFORM_BUFFER                              0x8A11
#define GL_UNIFORM_BUFFER_BINDING                      0x8A28
#define GL_UNIFORM_BUFFER_START                        0x8A29
#define GL_UNIFORM_BUFFER_SIZE                         0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS                   0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS                 0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS                 0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS                 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS                 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE                      0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS      0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS    0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS    0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT             0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH        0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS                       0x8A36
#define GL_UNIFORM_TYPE                                0x8A37
#define GL_UNIFORM_SIZE                                0x8A38
#define GL_UNIFORM_NAME_LENGTH                         0x8A39
#define GL_UNIFORM_BLOCK_INDEX                         0x8A3A
#define GL_UNIFORM_OFFSET                              0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE                        0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE                       0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR                        0x8A3E
#define GL_UNIFORM_BLOCK_BINDING                       0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE                     0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH                   0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS               0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES        0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER   0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX                               0xFFFFFFFFu

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

#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87

#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B

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
  using glGetActiveUniform         = void   WINAPI( GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name );
  using glGetUniformLocation       = GLint  WINAPI( GLuint program, const GLchar* name );
  using glGetUniformfv             = void   WINAPI( GLuint program, GLint location, GLfloat* params );
  using glGetUniformiv             = void   WINAPI( GLuint program, GLint location, GLint* params );
  using glUniform1f                = void   WINAPI( GLint location, GLfloat v0 );
  using glUniform2f                = void   WINAPI( GLint location, GLfloat v0, GLfloat v1 );
  using glUniform3f                = void   WINAPI( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 );
  using glUniform4f                = void   WINAPI( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 );
  using glUniform1i                = void   WINAPI( GLint location, GLint v0 );
  using glUniform2i                = void   WINAPI( GLint location, GLint v0, GLint v1 );
  using glUniform3i                = void   WINAPI( GLint location, GLint v0, GLint v1, GLint v2 );
  using glUniform4i                = void   WINAPI( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 );
  using glUniform1fv               = void   WINAPI( GLint location, GLsizei count, const GLfloat* value );
  using glUniform2fv               = void   WINAPI( GLint location, GLsizei count, const GLfloat* value );
  using glUniform3fv               = void   WINAPI( GLint location, GLsizei count, const GLfloat* value );
  using glUniform4fv               = void   WINAPI( GLint location, GLsizei count, const GLfloat* value );
  using glUniform1iv               = void   WINAPI( GLint location, GLsizei count, const GLint* value );
  using glUniform2iv               = void   WINAPI( GLint location, GLsizei count, const GLint* value );
  using glUniform3iv               = void   WINAPI( GLint location, GLsizei count, const GLint* value );
  using glUniform4iv               = void   WINAPI( GLint location, GLsizei count, const GLint* value );
  using glUniformMatrix2fv         = void   WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix3fv         = void   WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix4fv         = void   WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );

  // OPENGL 2.1 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glUniformMatrix2x3fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix3x2fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix2x4fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix4x2fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix3x4fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
  using glUniformMatrix4x3fv = void WINAPI( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );

  // OPENGL 3.0 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glGenVertexArrays    = void WINAPI( GLsizei n, GLuint* arrays );
  using glBindVertexArray    = void WINAPI( GLuint array );
  using glDeleteVertexArrays = void WINAPI( GLsizei n, const GLuint* arrays );
  using glBindBufferBase     = void WINAPI( GLenum target, GLuint index, GLuint buffer );
  using glBindBufferRange    = void WINAPI( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size );
  using glUniform1ui         = void WINAPI( GLint location, GLuint v0 );
  using glUniform2ui         = void WINAPI( GLint location, GLuint v0, GLuint v1 );
  using glUniform3ui         = void WINAPI( GLint location, GLuint v0, GLuint v1, GLuint v2 );
  using glUniform4ui         = void WINAPI( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 );
  using glUniform1uiv        = void WINAPI( GLint location, GLsizei count, const GLuint* value );
  using glUniform2uiv        = void WINAPI( GLint location, GLsizei count, const GLuint* value );
  using glUniform3uiv        = void WINAPI( GLint location, GLsizei count, const GLuint* value );
  using glUniform4uiv        = void WINAPI( GLint location, GLsizei count, const GLuint* value );

  // OPENGL 3.1 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using glGetUniformBlockIndex      = GLuint WINAPI( GLuint program, const GLchar* uniformBlockName );
  using glUniformBlockBinding       = void   WINAPI( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding );
  using glGetActiveUniformBlockiv   = void   WINAPI( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params );
  using glGetActiveUniformBlockName = void   WINAPI( GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName );

  // OPENGL 4.3 ///////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  using DEBUG_MESSAGE_CALLBACK_SIGNATURE = void WINAPI( GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* msg, void const* user_param );
  using glDebugMessageCallback           = void WINAPI( DEBUG_MESSAGE_CALLBACK_SIGNATURE callback, const void* userParam );

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
DEFINE_GL( glGetActiveUniform );
DEFINE_GL( glGetUniformLocation );
DEFINE_GL( glGetUniformfv );
DEFINE_GL( glGetUniformiv );
DEFINE_GL( glUniform1f );
DEFINE_GL( glUniform2f );
DEFINE_GL( glUniform3f );
DEFINE_GL( glUniform4f );
DEFINE_GL( glUniform1i );
DEFINE_GL( glUniform2i );
DEFINE_GL( glUniform3i );
DEFINE_GL( glUniform4i );
DEFINE_GL( glUniform1ui );
DEFINE_GL( glUniform2ui );
DEFINE_GL( glUniform3ui );
DEFINE_GL( glUniform4ui );
DEFINE_GL( glUniform1fv );
DEFINE_GL( glUniform2fv );
DEFINE_GL( glUniform3fv );
DEFINE_GL( glUniform4fv );
DEFINE_GL( glUniform1iv );
DEFINE_GL( glUniform2iv );
DEFINE_GL( glUniform3iv );
DEFINE_GL( glUniform4iv );
DEFINE_GL( glUniform1uiv );
DEFINE_GL( glUniform2uiv );
DEFINE_GL( glUniform3uiv );
DEFINE_GL( glUniform4uiv );
DEFINE_GL( glUniformMatrix2fv );
DEFINE_GL( glUniformMatrix3fv );
DEFINE_GL( glUniformMatrix4fv );
DEFINE_GL( glUniformMatrix2x3fv );
DEFINE_GL( glUniformMatrix3x2fv );
DEFINE_GL( glUniformMatrix2x4fv );
DEFINE_GL( glUniformMatrix4x2fv );
DEFINE_GL( glUniformMatrix3x4fv );
DEFINE_GL( glUniformMatrix4x3fv );
DEFINE_GL( glGenVertexArrays );
DEFINE_GL( glBindVertexArray );
DEFINE_GL( glDeleteVertexArrays );
DEFINE_GL( glBindBufferBase );
DEFINE_GL( glBindBufferRange );
DEFINE_GL( glGetUniformBlockIndex );
DEFINE_GL( glUniformBlockBinding );
DEFINE_GL( glGetActiveUniformBlockiv );
DEFINE_GL( glGetActiveUniformBlockName );
DEFINE_GL( glDebugMessageCallback );
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
    INIT_GL( glGetActiveUniform );
    INIT_GL( glGetUniformLocation );
    INIT_GL( glGetUniformfv );
    INIT_GL( glGetUniformiv );
    INIT_GL( glUniform1f );
    INIT_GL( glUniform2f );
    INIT_GL( glUniform3f );
    INIT_GL( glUniform4f );
    INIT_GL( glUniform1i );
    INIT_GL( glUniform2i );
    INIT_GL( glUniform3i );
    INIT_GL( glUniform4i );
    INIT_GL( glUniform1ui );
    INIT_GL( glUniform2ui );
    INIT_GL( glUniform3ui );
    INIT_GL( glUniform4ui );
    INIT_GL( glUniform1fv );
    INIT_GL( glUniform2fv );
    INIT_GL( glUniform3fv );
    INIT_GL( glUniform4fv );
    INIT_GL( glUniform1iv );
    INIT_GL( glUniform2iv );
    INIT_GL( glUniform3iv );
    INIT_GL( glUniform4iv );
    INIT_GL( glUniform1uiv );
    INIT_GL( glUniform2uiv );
    INIT_GL( glUniform3uiv );
    INIT_GL( glUniform4uiv );
    INIT_GL( glUniformMatrix2fv );
    INIT_GL( glUniformMatrix3fv );
    INIT_GL( glUniformMatrix4fv );
    INIT_GL( glUniformMatrix2x3fv );
    INIT_GL( glUniformMatrix3x2fv );
    INIT_GL( glUniformMatrix2x4fv );
    INIT_GL( glUniformMatrix4x2fv );
    INIT_GL( glUniformMatrix3x4fv );
    INIT_GL( glUniformMatrix4x3fv );
    INIT_GL( glGenVertexArrays );
    INIT_GL( glBindVertexArray );
    INIT_GL( glDeleteVertexArrays );
    INIT_GL( glBindBufferBase );
    INIT_GL( glBindBufferRange );
    INIT_GL( glGetUniformBlockIndex );
    INIT_GL( glUniformBlockBinding );
    INIT_GL( glGetActiveUniformBlockiv );
    INIT_GL( glGetActiveUniformBlockName );
    INIT_GL( glCreateBuffers );
    INIT_GL( glDebugMessageCallback );
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


