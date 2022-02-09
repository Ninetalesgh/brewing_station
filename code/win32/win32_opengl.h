#pragma once


#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")


#include <opengl_ext.h>

#include <common/bscommon.h>
#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif



namespace bs
{
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
    };

    namespace global
    {
      static OpenGlInfo info;
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
    }

    OpenGlInfo get_info()
    {
      OpenGlInfo result {};
      result.vendor = (char const*) glGetString( GL_VENDOR );
      result.renderer = (char const*) glGetString( GL_RENDERER );
      result.version = (char const*) glGetString( GL_VERSION );
      result.shadingLanguageVersion = (char const*) glGetString( GL_SHADING_LANGUAGE_VERSION );
      result.extensions = (char const*) glGetString( GL_EXTENSIONS );
      return result;
    };

    void init_device_context( HWND window )
    {
      global::deviceContext = GetDC( window );

      PIXELFORMATDESCRIPTOR desiredPixelFormat {};
      desiredPixelFormat.nSize = sizeof( desiredPixelFormat );
      desiredPixelFormat.nVersion = 1;
      desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
      desiredPixelFormat.cColorBits = 32;
      desiredPixelFormat.cAlphaBits = 8;
      desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
      s32 suggestedPixelFormatIndex = ChoosePixelFormat( global::deviceContext, &desiredPixelFormat );
      PIXELFORMATDESCRIPTOR suggestedPixelFormat {};
      DescribePixelFormat( global::deviceContext, suggestedPixelFormatIndex, sizeof( suggestedPixelFormat ), &suggestedPixelFormat );
      SetPixelFormat( global::deviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat );
    }

    void init( HWND window )
    {
      init_device_context( window );
      global::renderContext = wglCreateContext( global::deviceContext );
      if ( !wglMakeCurrent( global::deviceContext, global::renderContext ) )
      {
        BREAK;
      }

      global::info = get_info();

      opengl_ext::init();

      //make proper context

      {
        HGLRC renderContext2 = wglCreateContextAttribsARB( global::deviceContext, 0, global::rcAttributes );
        //replace dummy rc
        if ( renderContext2 )
        {
          global::info.extEnabled = 1;
          wglMakeCurrent( global::deviceContext, renderContext2 );
          wglDeleteContext( global::renderContext );
          global::renderContext = renderContext2;
        }
      }

      //vsync on
      wglSwapInterval( 1 );

      //check if 
      //textureformat = GL_SRGB8_ALPHA8
      glEnable( GL_FRAMEBUFFER_SRGB );


    }

    void tick()
    {

    }



    void win32CreateOpenGlContextForWorkerThread()
    {
      HGLRC workerRenderContext = wglCreateContextAttribsARB( global::deviceContext, global::renderContext, global::rcAttributes );
      if ( workerRenderContext )
      {
        wglMakeCurrent( global::deviceContext, workerRenderContext );
      }
    }

    using TextureID = u64;

    TextureID allocate_texture()
    {
      GLuint textureHandle = 0;
      // glGenTextures( 1, &textureHandle );
      // glBindTexture( GL_TEXTURE_2D, textureHandle );
      // glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, backBuffer.data );
      // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
      // glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      // glBindTexture( GL_TEXTURE_2D, 0 );

      return (TextureID) textureHandle;
    }

    void deallocate_texture( TextureID texture )
    {
      GLuint handle = (GLuint) texture;
      glDeleteTextures( 1, &handle );
    }


  };



  class Shader
  {
    void create( char const* headerSource, char const* vertexSource, char const* fragmentSource )
    {

      GLint shaderCodeLengths[] = { -1,-1,-1,-1,-1,-1,-1,-1 };

      GLuint vsID = glCreateShader( GL_VERTEX_SHADER );
      GLchar const* vsCode[] =
      {
        headerSource,
        vertexSource
      };
      glShaderSource( vsID, array_count( vsCode ), vsCode, shaderCodeLengths );
      glCompileShader( vsID );

      GLuint fsID = glCreateShader( GL_FRAGMENT_SHADER );
      GLchar const* fsCode[] =
      {
        headerSource,
        fragmentSource
      };
      glShaderSource( fsID, array_count( fsCode ), fsCode, shaderCodeLengths );
      glCompileShader( fsID );

      GLuint programID = glCreateProgram();
      glAttachShader( programID, vsID );
      glAttachShader( programID, fsID );
      glLinkProgram( programID );

      //GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

       //glCompileShader
       //glLinkProgram
       //glUniform
       //glShaderSource

    }

    void bind();

    u32 rendererID;
  };

  class VertexBuffer
  {
    void Bind() const;
    void Unbind() const;
    void count();
  };

  class IndexBuffer
  {
    void Bind() const;
    void Unbind() const;
    void count();

  };



  class OpenGlVertexBuffer
  {

    void create( float* vertices, u32 count )
    {
      glCreateBuffers( 1, &rendererID );
      glBufferData( GL_ARRAY_BUFFER, count, vertices, GL_STATIC_DRAW );

      //GL_ELEMENT_ARRAY_BUFFER for the index buffer

      //in bind
      glBindBuffer( GL_ARRAY_BUFFER, rendererID );

      //in unbind
      glBindBuffer( GL_ARRAY_BUFFER, 0 );


      //in delete
      glDeleteBuffers( 1, &rendererID );

    }

    u32 rendererID;
  };
}

