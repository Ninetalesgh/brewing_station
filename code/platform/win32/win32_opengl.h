#pragma once
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#include <platform/opengl/bsopengl_ext.h>

#include <core/bs_texture.h>
#include <core/bs_camera.h>
#include <common/bs_color.h>

namespace opengl
{
  struct OpenGlInfo
  {
    u32 extEnabled;
    char const* vendor;
    char const* renderer;
    char const* version;
    char const* shadingLanguageVersion;
    char const* extensions;
    u32 GL_EXT_texture_sRGB;
    u32 GL_EXT_framebuffer_sRGB;
  };

  void swap_buffers();

  u32 init( HDC deviceContext, int2 viewportDimensions );
  void resize_viewport( int2 dimensions );

  HGLRC create_render_context_for_worker_thread();

  void set_worker_thread_render_context( HGLRC renderContext );
  void check_gl_error();

  //all three files in one, mark sections with #h, #vs and #fs.
  //example file content:
  //#h
  //#version 450 core
  //#vs
  //void main() { ... }
  //#fs
  //void main() { ... }
  //
  bs::ShaderProgramID create_shader_program( char const* combinedglslData, s32 size );
};



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////inl/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

namespace opengl_ext
{
  using wglCreateContextAttribsARB = HGLRC WINAPI( HDC hDC, HGLRC hShareContext, const int* attribList );
  using wglChoosePixelFormatARB = BOOL WINAPI( HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats );
  using wglGetPixelFormatAttribivARB = BOOL WINAPI( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, int* piValues );
  using wglGetPixelFormatAttribfvARB = BOOL WINAPI( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int* piAttributes, FLOAT* pfValues );
  using wglSwapIntervalEXT = BOOL WINAPI( int );
};

static opengl_ext::wglChoosePixelFormatARB* wglChoosePixelFormatARB;
static opengl_ext::wglCreateContextAttribsARB* wglCreateContextAttribsARB;
static opengl_ext::wglGetPixelFormatAttribivARB* wglGetPixelFormatAttribivARB;
static opengl_ext::wglGetPixelFormatAttribfvARB* wglGetPixelFormatAttribfvARB;
static opengl_ext::wglSwapIntervalEXT* wglSwapIntervalEXT;

namespace opengl_ext
{
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

namespace opengl
{
  namespace oglglobal
  {
    static OpenGlInfo info;
    static u32 defaultTextureFormat;
    static s32 rcAttributes[] =
    {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 0,
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB /*| WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB*/,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0
    };
    static HGLRC renderContext;
    static HDC   deviceContext;
  };
  void gl_debug_message_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param );

  void init_opengl_info()
  {
    oglglobal::info.vendor = (char const*) glGetString( GL_VENDOR );
    oglglobal::info.renderer = (char const*) glGetString( GL_RENDERER );
    oglglobal::info.version = (char const*) glGetString( GL_VERSION );
    oglglobal::info.shadingLanguageVersion = (char const*) glGetString( GL_SHADING_LANGUAGE_VERSION );
    oglglobal::info.extensions = (char const*) glGetString( GL_EXTENSIONS );
    oglglobal::info.GL_EXT_texture_sRGB = bs::string_contains( oglglobal::info.extensions, "GL_EXT_texture_sRGB" ) != nullptr;
    oglglobal::info.GL_EXT_framebuffer_sRGB = bs::string_contains( oglglobal::info.extensions, "GL_EXT_framebuffer_sRGB" ) != nullptr;
  };

  u32 set_pixel_format_for_dc( HDC deviceContext )
  {
    s32 suggestedPixelFormatIndex = 0;

    if ( wglChoosePixelFormatARB )
    {
      s32 intAttribList[] =
      {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_RED_BITS_ARB, 8,
        WGL_RED_SHIFT_ARB, 16,
        WGL_GREEN_BITS_ARB, 8,
        WGL_GREEN_SHIFT_ARB, 8,
        WGL_BLUE_BITS_ARB, 8,
        WGL_BLUE_SHIFT_ARB, 0,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_ALPHA_SHIFT_ARB, 24,
      0 };
      //TODO this doesn't give me any alpha bits and alpha shift, whereas the non wgl call does?
      float floatAttribList[] = { 0 };
      u32 extendedPick = 0;
      wglChoosePixelFormatARB( deviceContext, intAttribList, floatAttribList, 1, &suggestedPixelFormatIndex, &extendedPick );
    }
    else
    {
      PIXELFORMATDESCRIPTOR desiredPixelFormat {};
      desiredPixelFormat.nSize = sizeof( desiredPixelFormat );
      desiredPixelFormat.nVersion = 1;
      desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
      desiredPixelFormat.cColorBits = 32;
      desiredPixelFormat.cAlphaBits = 8;
      desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
      suggestedPixelFormatIndex = ChoosePixelFormat( deviceContext, &desiredPixelFormat );
    }

    PIXELFORMATDESCRIPTOR suggestedPixelFormat {};
    DescribePixelFormat( deviceContext, suggestedPixelFormatIndex, sizeof( suggestedPixelFormat ), &suggestedPixelFormat );

    u32 result = SetPixelFormat( deviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat );
    return result;
  }

  u32 load_extensions()
  {
    u32 result = 0;
    HWND dummyWindow = 0;
    WNDCLASSW wndClass {};
    wndClass.lpfnWndProc   = DefWindowProcW;
    wndClass.hInstance     = GetModuleHandle( NULL );
    wndClass.lpszClassName = L"OpenGlExtensionLoaderDummy";

    if ( RegisterClassW( &wndClass ) )
    {
      dummyWindow = CreateWindowExW( 0,
                                     wndClass.lpszClassName, L"dummy",
                                     0,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     0,
                                     0,
                                     wndClass.hInstance,
                                     0 );

      HDC dummyDC = GetDC( dummyWindow );
      set_pixel_format_for_dc( dummyDC );

      HGLRC dummyRC = wglCreateContext( dummyDC );
      if ( !wglMakeCurrent( dummyDC, dummyRC ) )
      {
        BREAK;
      }

      ::wglChoosePixelFormatARB =    (opengl_ext::wglChoosePixelFormatARB*) opengl_ext::get_proc_address( "wglChoosePixelFormatARB" );
      ::wglCreateContextAttribsARB =  (opengl_ext::wglCreateContextAttribsARB*) opengl_ext::get_proc_address( "wglCreateContextAttribsARB" );
      ::wglGetPixelFormatAttribivARB = (opengl_ext::wglGetPixelFormatAttribivARB*) opengl_ext::get_proc_address( "wglGetPixelFormatAttribivARB" );
      ::wglGetPixelFormatAttribfvARB = (opengl_ext::wglGetPixelFormatAttribfvARB*) opengl_ext::get_proc_address( "wglGetPixelFormatAttribfvARB" );
      ::wglSwapIntervalEXT =         (opengl_ext::wglSwapIntervalEXT*) opengl_ext::get_proc_address( "wglSwapIntervalEXT" );

      if ( ::wglChoosePixelFormatARB == nullptr ) BREAK;
      if ( ::wglCreateContextAttribsARB == nullptr ) BREAK;
      if ( ::wglGetPixelFormatAttribivARB == nullptr ) BREAK;
      if ( ::wglGetPixelFormatAttribfvARB == nullptr ) BREAK;
      if ( ::wglSwapIntervalEXT == nullptr ) BREAK;

      opengl_ext::init();

      wglMakeCurrent( 0, 0 );
      wglDeleteContext( dummyRC );
      ReleaseDC( dummyWindow, dummyDC );
      DestroyWindow( dummyWindow );

      result = 1;
    }

    return result;
  }

  u32 init( HDC deviceContext, int2 viewportDimensions )
  {
    if ( !load_extensions() )
    {
      BREAK;
      return 0;
    }

    if ( !set_pixel_format_for_dc( deviceContext ) )
    {
      BREAK;
      return 0;
    }

    oglglobal::deviceContext = deviceContext;
    if ( wglCreateContextAttribsARB )
    {
      oglglobal::renderContext = wglCreateContextAttribsARB( oglglobal::deviceContext, 0, oglglobal::rcAttributes );
      oglglobal::info.extEnabled = 1;
    }

    if ( !oglglobal::renderContext )
    {
      oglglobal::renderContext = wglCreateContext( oglglobal::deviceContext );
      oglglobal::info.extEnabled = 0;
      BREAK;
    }

    if ( !wglMakeCurrent( oglglobal::deviceContext, oglglobal::renderContext ) )
    {
      BREAK;
      return 0;
    }

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( gl_debug_message_callback, nullptr );
    check_gl_error();

    init_opengl_info();

    oglglobal::defaultTextureFormat = oglglobal::info.GL_EXT_texture_sRGB ? GL_RGBA8 : GL_SRGB8_ALPHA8;

    if ( oglglobal::info.GL_EXT_framebuffer_sRGB )
    {
      glEnable( GL_FRAMEBUFFER_SRGB );
    }

    if ( wglSwapIntervalEXT ) //vsync
    {
      wglSwapIntervalEXT( 1 );
    }

    resize_viewport( viewportDimensions );

    // glEnable( GL_CULL_FACE );
    // glCullFace( GL_BACK );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    float4 bg = color::float4_from_rgba( color::rgba( 30, 30, 30, 255 ) );
    glClearColor( bg.x * bg.x, bg.y * bg.y, bg.z * bg.z, bg.w );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    check_gl_error();

    return 1;
  }

  void resize_viewport( int2 dimensions )
  {
    glViewport( 0, 0, dimensions.x, dimensions.y );
  }

  void swap_buffers()
  {
    SwapBuffers( oglglobal::deviceContext );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  HGLRC create_render_context_for_worker_thread()
  {
    check_gl_error();
    HGLRC rc = wglCreateContextAttribsARB( oglglobal::deviceContext, oglglobal::renderContext, oglglobal::rcAttributes );
    check_gl_error();
    return rc;
  }

  void set_worker_thread_render_context( HGLRC renderContext )
  {
    if ( !wglMakeCurrent( oglglobal::deviceContext, renderContext ) )
    {
      BREAK;
    }
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

////////////////////////////////////////////////////////////////////////////////////////////////////
// callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <map>
#include <string>

namespace opengl
{
  using ShaderID = u32;
  using UniformID = u32;
  using UniformBufferID = u32;
  struct OGLFileData
  {
    char const* data;
    s64 size;
  };

  bs::Mesh            allocate_mesh( bs::MeshData const* raw );
  void                free_mesh( bs::Mesh const& mesh );
  bs::TextureID       allocate_texture( bs::TextureData const* textureData );
  void                free_texture( bs::TextureID texture );
  bool                validate_shader( ShaderID shaderID );
  bool                validate_program( bs::ShaderProgramID programID );
  bs::ShaderProgramID create_shader_program( OGLFileData headerFileData, OGLFileData vsFileData, OGLFileData fsFileData );
  bs::ShaderProgramID create_shader_program( char const* combinedglslData, s32 size );
  void                init_mesh_vao( bs::Mesh* mesh );


  INLINE u32 get_size_for_index_format( bs::IndexFormat indexFormat ) { return indexFormat == bs::IndexFormat::U16 ? sizeof( u16 ) : sizeof( u32 ); }
  INLINE GLenum get_gl_index_format( bs::IndexFormat indexFormat ) { return indexFormat == bs::IndexFormat::U16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT; }

  void gl_debug_message_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param )
  {
    char const* sourceString = nullptr;
    char const* typeString = nullptr;
    char const* severityString = nullptr;
    BREAK;

    switch ( source )
    {
      case GL_DEBUG_SOURCE_API: sourceString = "API - "; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceString = "WINDOW SYSTEM - "; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceString = "SHADER COMPILER - "; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY: sourceString = "THIRD PARTY - "; break;
      case GL_DEBUG_SOURCE_APPLICATION: sourceString = "APPLICATION - "; break;
      case GL_DEBUG_SOURCE_OTHER: sourceString = "OTHER - "; break;
    }
    switch ( type )
    {
      case GL_DEBUG_TYPE_ERROR: typeString = "ERROR - "; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "DEPRECATED_BEHAVIOR - "; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeString = "UNDEFINED_BEHAVIOR - "; break;
      case GL_DEBUG_TYPE_PORTABILITY: typeString = "PORTABILITY - "; break;
      case GL_DEBUG_TYPE_PERFORMANCE: typeString = "PERFORMANCE - "; break;
      case GL_DEBUG_TYPE_MARKER: typeString = "MARKER - "; break;
      case GL_DEBUG_TYPE_OTHER: typeString = "OTHER - "; break;
    }
    switch ( severity )
    {
      case GL_DEBUG_SEVERITY_NOTIFICATION: severityString = "NOTIFICATION - "; break;
      case GL_DEBUG_SEVERITY_LOW: severityString = "LOW - "; break;
      case GL_DEBUG_SEVERITY_MEDIUM: severityString = "MEDIUM - "; break;
      case GL_DEBUG_SEVERITY_HIGH: severityString = "HIGH - "; break;
    }

    log_error( sourceString, typeString, severityString, message );
  }


  bs::Mesh allocate_mesh( bs::MeshData const* raw )
  {
    bs::Mesh resultMesh {};

    u32 const vertexCount = raw->vertexCount;
    u32 const indexCount = raw->indexCount;
    bs::IndexFormat const indexFormat = raw->indexFormat;

    if ( indexFormat == bs::IndexFormat::INVALID ) BREAK;

    glCreateBuffers( 1, &resultMesh.vertexBuffer );
    glNamedBufferData( resultMesh.vertexBuffer, vertexCount * sizeof( float3 ), raw->vertices, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.uvBuffer );
    glNamedBufferData( resultMesh.uvBuffer, vertexCount * sizeof( float2 ), raw->uvs, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.indexBuffer );
    glNamedBufferData( resultMesh.indexBuffer, indexCount * get_size_for_index_format( indexFormat ), raw->indices, GL_STATIC_DRAW );

    resultMesh.indexCount = indexCount;
    resultMesh.indexFormat = indexFormat;
    return resultMesh;
  }

  void free_mesh( bs::Mesh const& mesh )
  {
    if ( mesh.id )
    {
      glDeleteVertexArrays( 1, &mesh.id );
      BREAK;
    }

    u32 buffers[] = { mesh.vertexBuffer, mesh.uvBuffer, mesh.indexBuffer };
    glDeleteBuffers( 3, buffers );
  }

  bs::TextureID allocate_texture( bs::TextureData const* textureData )
  {
    u32 const* pixel = (u32 const*) textureData->pixel;
    //TODO use PBO to skip one copy step ?
    GLuint textureHandle = 0;
    glGenTextures( 1, &textureHandle );
    glBindTexture( GL_TEXTURE_2D, textureHandle );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, textureData->width, textureData->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixel );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    //glGenerateMipmap(GL_TEXTURE_2D); set filter to mipmap if want
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return (bs::TextureID) textureHandle;
  }

  void free_texture( bs::TextureID texture )
  {
    GLuint handle = (GLuint) texture;
    glDeleteTextures( 1, &handle );
  }


  bool validate_shader( ShaderID shaderID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 8192;
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

  bool validate_program( bs::ShaderProgramID programID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 8192;
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

  void get_uniforms( bs::ShaderProgramID program );

  bs::ShaderProgramID create_shader_program( OGLFileData headerFileData, OGLFileData vsFileData, OGLFileData fsFileData )
  {
    // log_info( "[OGL] Compiling shader." );

    GLint shaderCodeLengths[] = { (s32) headerFileData.size, (s32) vsFileData.size };

    ShaderID vsID = glCreateShader( GL_VERTEX_SHADER );
    GLchar const* vsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) vsFileData.data
    };
    glShaderSource( vsID, array_count( vsCode ), vsCode, shaderCodeLengths );
    glCompileShader( vsID );

    if ( !validate_shader( vsID ) )
    {
      BREAK;
    }

    shaderCodeLengths[1] = (s32) fsFileData.size;

    ShaderID fsID = glCreateShader( GL_FRAGMENT_SHADER );
    GLchar const* fsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) fsFileData.data
    };
    glShaderSource( fsID, array_count( fsCode ), fsCode, shaderCodeLengths );
    glCompileShader( fsID );

    if ( !validate_shader( fsID ) )
    {
      BREAK;
    }

    //  log_info( "[OGL] Linking program." );

    GLuint programID = glCreateProgram();
    glAttachShader( programID, vsID );
    glAttachShader( programID, fsID );
    glLinkProgram( programID );

    if ( !validate_program( programID ) )
    {
      BREAK;
    }

    glDetachShader( programID, vsID );
    glDetachShader( programID, fsID );

    glDeleteShader( vsID );
    glDeleteShader( fsID );

    //get_uniforms( programID );
    return programID;
  }


  bs::ShaderProgramID create_shader_program( char const* combinedglslData, s32 size )
  {
    char const* reader = (char const*) combinedglslData;
    char const* end = reader + size;
    OGLFileData h;
    OGLFileData vs;
    OGLFileData fs;

    //header
    char const* nextSection = bs::string_contains( reader, "#h" );
    assert( nextSection );
    char const* currentSection = nextSection + 2;

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

  void delete_shader_prorgam( bs::ShaderProgramID program )
  {
    glDeleteProgram( program );
  }

  void init_mesh_vao( bs::Mesh* mesh )
  {
    check_gl_error();

    glGenVertexArrays( 1, &mesh->id );
    glBindVertexArray( mesh->id );

    glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ARRAY_BUFFER, mesh->uvBuffer );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer );

    check_gl_error();

    glBindVertexArray( 0 );
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    check_gl_error();
  }

  //TODO
  //UNFINISHED SECTION



  void update_uniform_buffer( UniformBufferID buffer, s64 offset, s64 size, void* data )
  {
    glBindBuffer( GL_UNIFORM_BUFFER, buffer );
    glBufferSubData( GL_UNIFORM_BUFFER, (GLintptr) offset, (GLintptr) size, data );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
  }

  UniformBufferID allocate_uniform_buffer( s64 size, void* data )
  {
    UniformBufferID bufferID;
    glGenBuffers( 1, &bufferID );
    glBindBuffer( GL_UNIFORM_BUFFER, bufferID );
    glBufferData( GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    return bufferID;
  }

  void free_uniform_buffer( UniformBufferID buffer )
  {
    glDeleteBuffers( 1, &buffer );
  }

  void set_uniform( bs::ShaderProgramID program, char const* name, void* data )
  {
    //NOT DONE
    UniformID location = glGetUniformLocation( program, name );
    check_gl_error();

    //?
    glUniformMatrix4fv( location, 1, GL_FALSE, (const GLfloat*) data );
  }


  struct DrawCall
  {
    bs::Mesh* meshes;
    u32 meshCount;
    bs::ShaderProgramID programID;
  };

  void draw( bs::Mesh* mesh, bs::Camera* camera, bs::ShaderProgramID programID, bs::TextureID textureID )
  {
    //per batch
    glUseProgram( programID );

    //per mesh
    {
      glBindTexture( GL_TEXTURE_2D, textureID );

      if ( !mesh->id )
      {
        init_mesh_vao( mesh );
      }

      glBindVertexArray( mesh->id );

      //UNIFORMS HERE TODO


      GLenum glIndexFormat = get_gl_index_format( mesh->indexFormat );
      glDrawElements( GL_TRIANGLES, mesh->indexCount, glIndexFormat, (void*) 0 );

      glBindVertexArray( 0 );
      //glBindTexture( 0 );
    }

    glUseProgram( 0 );
  }



  struct UniformInfo
  {
    GLint location;
    GLsizei count;
  };
  void get_uniforms( bs::ShaderProgramID program )
  {


    GLint numBlocks;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks );

    //std::vector<std::string> nameList;
    //nameList.reserve( numBlocks );
    for ( int blockIx = 0; blockIx < numBlocks; ++blockIx )
    {
      GLint nameLen;
      glGetActiveUniformBlockiv( program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen );

      char name[32] = {};
      glGetActiveUniformBlockName( program, blockIx, nameLen, NULL, name );
      name[0] = name[0];
      // nameList.push_back( std::string() );
      // nameList.back().assign( name.begin(), name.end() - 1 ); //Remove the null terminator.
    }



    ///////////////////////////////////////////////


    GLint uniformCount = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &uniformCount );

    if ( uniformCount != 0 )
    {
      char uniformName[1024] = {};

      GLint 	max_name_len = 0;
      GLsizei length = 0;
      GLsizei count = 0;
      GLenum 	type = GL_NONE;
      glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len );

      std::map<std::string, UniformInfo> uniforms;

      for ( GLint i = 0; i < uniformCount; ++i )
      {
        glGetActiveUniform( program, i, max_name_len, &length, &count, &type, uniformName );

        switch ( type )
        {
          case GL_FLOAT_VEC2: BREAK;break;
          case GL_FLOAT_VEC3: BREAK;break;
          case GL_FLOAT_VEC4: BREAK;break;
          case GL_INT_VEC2: BREAK;break;
          case GL_INT_VEC3: BREAK;break;
          case GL_INT_VEC4: BREAK;break;
          case GL_BOOL: BREAK;break;
          case GL_BOOL_VEC2: BREAK;break;
          case GL_BOOL_VEC3: BREAK;break;
          case GL_BOOL_VEC4: BREAK;break;
          case GL_FLOAT_MAT2: BREAK;break;
          case GL_FLOAT_MAT3: BREAK;break;
          case GL_FLOAT_MAT4: BREAK;break;
          case GL_FLOAT_MAT2x3: BREAK;break;
          case GL_FLOAT_MAT2x4: BREAK;break;
          case GL_FLOAT_MAT3x2: BREAK;break;
          case GL_FLOAT_MAT3x4: BREAK;break;
          case GL_FLOAT_MAT4x2: BREAK;break;
          case GL_FLOAT_MAT4x3: BREAK;break;
          case GL_SAMPLER_1D: BREAK;break;
          case GL_SAMPLER_2D: BREAK;break;
          case GL_SAMPLER_3D: BREAK;break;
          case GL_SAMPLER_CUBE: BREAK;break;
          case GL_SAMPLER_1D_SHADOW: BREAK;break;
          case GL_SAMPLER_2D_SHADOW: BREAK;break;
          default:
            BREAK;
        };

        UniformInfo uniformInfo = {};
        uniformInfo.location = glGetUniformLocation( program, uniformName );
        uniformInfo.count = count;

        uniforms.emplace( std::make_pair( std::string( uniformName, length ), uniformInfo ) );
      }
    }
  }
};

