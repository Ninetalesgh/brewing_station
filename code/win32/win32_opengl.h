#pragma once

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#include <platform/platform_renderer.h>

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
      u32 GL_EXT_texture_sRGB;
      u32 GL_EXT_framebuffer_sRGB;
    };

    u32 init( HDC deviceContext );
    void tick();

    OpenGlInfo const* get_info();
    void win32CreateOpenGlContextForWorkerThread();

    TextureID allocate_texture( Texture const& texture );
    void deallocate_texture( TextureID texture );
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


};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////cpp/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


namespace bs
{
  namespace opengl
  {
    namespace global
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
    }

    OpenGlInfo const* get_info() { return &global::info; }

    void init_opengl_info()
    {
      global::info.vendor = (char const*) glGetString( GL_VENDOR );
      global::info.renderer = (char const*) glGetString( GL_RENDERER );
      global::info.version = (char const*) glGetString( GL_VERSION );
      global::info.shadingLanguageVersion = (char const*) glGetString( GL_SHADING_LANGUAGE_VERSION );
      global::info.extensions = (char const*) glGetString( GL_EXTENSIONS );
      global::info.GL_EXT_texture_sRGB = bs::string_contains( global::info.extensions, "GL_EXT_texture_sRGB" );
      global::info.GL_EXT_framebuffer_sRGB = bs::string_contains( global::info.extensions, "GL_EXT_framebuffer_sRGB" );
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
        opengl_ext::init();

        wglMakeCurrent( 0, 0 );
        wglDeleteContext( dummyRC );
        ReleaseDC( dummyWindow, dummyDC );
        DestroyWindow( dummyWindow );

        result = 1;
      }

      return result;
    }

    u32 init( HDC deviceContext )
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

      global::deviceContext = deviceContext;
      if ( wglCreateContextAttribsARB )
      {
        global::renderContext = wglCreateContextAttribsARB( global::deviceContext, 0, global::rcAttributes );
        global::info.extEnabled = 1;
      }

      if ( !global::renderContext )
      {
        global::renderContext = wglCreateContext( global::deviceContext );
        global::info.extEnabled = 0;
      }

      if ( !wglMakeCurrent( global::deviceContext, global::renderContext ) )
      {
        BREAK;
        return 0;
      }

      init_opengl_info();

      global::defaultTextureFormat = global::info.GL_EXT_texture_sRGB ? GL_RGBA8 : GL_SRGB8_ALPHA8;

      if ( global::info.GL_EXT_framebuffer_sRGB )
      {
        glEnable( GL_FRAMEBUFFER_SRGB );
      }

      if ( wglSwapIntervalEXT ) //vsync
      {
        wglSwapIntervalEXT( 1 );
      }

      return 1;
    }

    void win32CreateOpenGlContextForWorkerThread()
    {
      HGLRC workerRenderContext = wglCreateContextAttribsARB( global::deviceContext, global::renderContext, global::rcAttributes );
      if ( workerRenderContext )
      {
        wglMakeCurrent( global::deviceContext, workerRenderContext );
      }
      else
      {
        BREAK;
      }
    }

    TextureID allocate_texture( Texture const& texture )
    {
      //TODO use PBO to skip one copy step
      GLuint textureHandle = 0;
      glGenTextures( 1, &textureHandle );
      glBindTexture( GL_TEXTURE_2D, textureHandle );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, texture.width, texture.height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texture.pixel );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glBindTexture( GL_TEXTURE_2D, 0 );

      return (TextureID) textureHandle;
    }

    void deallocate_texture( TextureID texture )
    {
      GLuint handle = (GLuint) texture;
      glDeleteTextures( 1, &handle );
    }

    void tick()
    {
      //  thread::wait_if_requested( &parameter.threadInfo );
      glViewport( 0, 0, 600, 600 );
      glClearColor( 1.0f, 0.0f, 1.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT );
      glBegin( GL_TRIANGLES );

      glMatrixMode( GL_TEXTURE );
      glLoadIdentity();

      float p = 0.9f;
      glColor3f( 1, 0, 0 );
      glVertex2f( -p, -p );
      glColor3f( 0, 1, 0 );
      glVertex2f( p, -p );
      glColor3f( 0, 0, 1 );
      glVertex2f( p, p );
      glColor3f( 1, 0, 0 );
      glVertex2f( -p, -p );
      glColor3f( 0, 1, 0 );
      glVertex2f( p, p );
      glColor3f( 0, 0, 1 );
      glVertex2f( -p, p );

      glEnd();
      SwapBuffers( global::deviceContext );


    }


  };
}

