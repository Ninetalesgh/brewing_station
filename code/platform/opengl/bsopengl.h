#pragma once
#include "bsopengl_ext.h"

#include <core/bsfile.h>
#include <common/bscolor.h>
#include <common/bscommon.h>


namespace opengl
{

  using ProgramID = u32;

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData );
  //all three files in one, mark sections with #h, #vs and #fs.
  //example file content:
  //#h
  //#version 450 core
  //#vs
  //void main() { ... }
  //#fs
  //void main() { ... }
  //
  ProgramID create_shader_program( bs::file::Data combinedglsl );

  void check_gl_error();
};


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////inl/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


namespace opengl
{
  using ShaderID = u32;

  u32 validate_shader( ShaderID shaderID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetShaderInfoLog( shaderID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  u32 validate_program( ProgramID programID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetProgramiv( programID, GL_LINK_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetProgramInfoLog( programID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData )
  {
    log_info( "[OGL] Compiling shader." );

    GLint shaderCodeLengths[] = { (s32) headerFileData.size, (s32) vsFileData.size };

    ShaderID vsID = glCreateShader( GL_VERTEX_SHADER );
    GLchar const* vsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) vsFileData.data
    };
    glShaderSource( vsID, array_count( vsCode ), vsCode, shaderCodeLengths );
    glCompileShader( vsID );

    validate_shader( vsID );

    shaderCodeLengths[1] = (s32) fsFileData.size;

    ShaderID fsID = glCreateShader( GL_FRAGMENT_SHADER );
    GLchar const* fsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) fsFileData.data
    };
    glShaderSource( fsID, array_count( fsCode ), fsCode, shaderCodeLengths );
    glCompileShader( fsID );

    validate_shader( fsID );

    log_info( "[OGL] Linking program." );

    GLuint programID = glCreateProgram();
    glAttachShader( programID, vsID );
    glAttachShader( programID, fsID );
    glLinkProgram( programID );

    validate_program( programID );

    glDetachShader( programID, vsID );
    glDetachShader( programID, fsID );

    glDeleteShader( vsID );
    glDeleteShader( fsID );

    return programID;
  }

  ProgramID create_shader_program( bs::file::Data combinedglsl )
  {
    char* reader = (char*) combinedglsl.data;
    char* end = reader + combinedglsl.size;
    bs::file::Data h;
    bs::file::Data vs;
    bs::file::Data fs;

    //header
    char* nextSection = bs::string_contains( reader, "#h" );
    assert( nextSection );
    char* currentSection = nextSection + 2;

    nextSection = bs::string_contains( currentSection, "#vs" );
    assert( nextSection );

    h.data = currentSection;
    h.size = nextSection - currentSection;

    //vertex shader
    currentSection = nextSection + 3;
    nextSection = bs::string_contains( currentSection, "#fs" );
    assert( nextSection );

    vs.data = currentSection;
    vs.size = nextSection - currentSection;

    //fragment shader
    currentSection = nextSection + 3;
    nextSection = end;

    fs.data = currentSection;
    fs.size = nextSection - currentSection;

    return create_shader_program( h, vs, fs );
  }

  void check_gl_error()
  {
    GLenum err;
    while ( (err = glGetError()) != GL_NO_ERROR )
    {
      switch ( err )
      {
        case GL_INVALID_ENUM:
        {
          BREAK;
          break;
        }
        case GL_INVALID_VALUE:
        {
          BREAK;
          break;
        }
        case GL_INVALID_OPERATION:
        {
          BREAK;
          break;
        }
        case GL_STACK_OVERFLOW:
        {
          BREAK;
          break;
        }
        case GL_STACK_UNDERFLOW:
        {
          BREAK;
          break;
        }
        case GL_OUT_OF_MEMORY:
        {
          BREAK;
          break;
        }
        default:
        {
          //??
          BREAK;
        }
      }
    }
  }
};