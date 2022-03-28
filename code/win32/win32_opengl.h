#pragma once

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#include <ui/bstextarea.h>
#include <core/bsgraphics.h>

#include <opengl_ext.h>
#include <core/bsfont.h>
#include <core/bsmemory.h>
#include <common/bscolor.h>
#include <common/bscommon.h>
#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif


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
  void resize_viewport();
  void tick();

  OpenGlInfo const* get_info();
  void create_opengl_context_for_worker_thread();

  bs::graphics::TextureID allocate_texture( u32 const* pixel, s32 width, s32 height );
  void free_texture( bs::graphics::TextureID texture );
  void render( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );
};


class Shader
{
  void create( char const* headerSource, char const* vertexSource, char const* fragmentSource )
  {
    GLint shaderCodeLengths[] = { -1,-1 };

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


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////cpp/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    global::info.GL_EXT_texture_sRGB = bs::string::contains( global::info.extensions, "GL_EXT_texture_sRGB" ) != nullptr;
    global::info.GL_EXT_framebuffer_sRGB = bs::string::contains( global::info.extensions, "GL_EXT_framebuffer_sRGB" ) != nullptr;
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

    resize_viewport();
    glClearColor( 1.0f, 0.0f, 1.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    return 1;
  }

  void create_opengl_context_for_worker_thread()
  {
    //needs to be called before worker threads hop online
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

  void resize_viewport()
  {
    int2 size = ::global::mainWindow.size;
    glViewport( 0, 0, size.x, size.y );
  }

  void tick()
  {
    //  render( nullptr, nullptr, nullptr );

    SwapBuffers( global::deviceContext );
    glClear( GL_COLOR_BUFFER_BIT );
  }

  bs::graphics::TextureID allocate_texture( u32 const* pixel, s32 width, s32 height )
  {
    //TODO use PBO to skip one copy step
    GLuint textureHandle = 0;
    glGenTextures( 1, &textureHandle );
    glBindTexture( GL_TEXTURE_2D, textureHandle );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixel );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return (bs::graphics::TextureID) textureHandle;
  }

  void free_texture( bs::graphics::TextureID texture )
  {
    GLuint handle = (GLuint) texture;
    glDeleteTextures( 1, &handle );
  }

  void render_text_area( bs::graphics::RenderTarget* target, bs::ui::TextArea* textArea )
  {
    bs::font::GlyphSheet* glyphSheet = ::global::defaultGlyphSheet;
    bs::font::GlyphTable* glyphTable = ::global::defaultGlyphTable;

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, glyphSheet->textureID );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef( 1.0f / (float) glyphSheet->width, 1.0f / (float) glyphSheet->height, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    float matrix[] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1,
    };
    glLoadMatrixf( matrix );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBegin( GL_TRIANGLES );
    //render here


    float2 invScreenSize = ::global::mainWindow.size;
    invScreenSize = { 1.0f / invScreenSize.x, 1.0f / invScreenSize.y };

    float2 textAreaOrigin = { 0, 0 };
    float2 currentPos = { 0, 0 };

    for ( s32 lineIndex = 0; lineIndex < textArea->lineCount; ++lineIndex )
    {
      bs::ui::TextLine& textLine = textArea->lines[lineIndex];

      s32 const end = lineIndex == textArea->lineCount - 1 ? (s32) '\0' : (s32) '\n';
      s32 const* reader = textLine.codepoints;
      while ( *reader != end )
      {
        bs::font::Glyph* glyph = get_glyph_for_codepoint( glyphSheet, *reader );
        ++reader;

        // float2 rectMin = currentPos;
        // float2 rectMax = rectMin + glyph->uvSize;

        {
          // float2 min = currentPos;

          float scale = glyphTable->scale;
          float2 min = textAreaOrigin + float2 { currentPos.x + glyph->offsetX, currentPos.y + glyph->offsetY };
          float2 max = { min.x + (float) glyph->uvSize.x, (float) glyph->uvSize.y };

          min = float2 { min.x * invScreenSize.x, min.y * invScreenSize.y };
          max = float2 { max.x * invScreenSize.x, max.y * invScreenSize.y };

          currentPos.x += glyph->advance;
          //float2 minuv = { glyph->uvBegin.x / glyphSheet->width, glyph->uvBegin.y / glyphSheet->height };
          //float2 maxuv = minuv + float2 { glyph->uvSize.x / glyphSheet->width, glyph->uvSize.y / glyphSheet->height };
          float2 minuv = glyph->uvBegin;
          float2 maxuv = glyph->uvBegin + glyph->uvSize;

          // min = { 0,0 };
          // max = { 1,1 };
          // minuv = { 0, 0 };
         // maxuv = { glyphSheet->width, glyphSheet->height };

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, min.y );

          glTexCoord2f( maxuv.x, maxuv.y );
          glVertex2f( max.x, min.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, max.y );

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, min.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, max.y );

          glTexCoord2f( minuv.x, minuv.y );
          glVertex2f( min.x, max.y );
        }
      }
    }



    glEnd();
  }

  void render( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    switch ( group->type )
    {
      case bs::graphics::RenderGroup::TEXT_AREA:
      {
        render_text_area( target, (bs::ui::TextArea*) group->renderObject );
        break;
      }
      default:
      {
        BREAK;
        break;
      }
    }

    // static bs::graphics::TextureID f = ::global::defaultGlyphSheet->textureID;
    // static bs::font::Glyph* test2 = ::global::defaultGlyphSheet->glyphs + 1;
    // static s32 counter = 0;
    // bs::font::Glyph* test = test2 + counter++;
    // if ( counter > 90 ) counter = 0;






    // //rendertarget stuff:
    // glViewport( , , , );

    // //rendergroup stuff:
    // glBindTexture( GL_TEXTURE_2D, f );
    // glEnable( GL_TEXTURE_2D );

    // glClear( GL_COLOR_BUFFER_BIT );
    // glBegin( GL_TRIANGLES );

    // glMatrixMode( GL_TEXTURE );
    // glLoadIdentity();

    // glMatrixMode( GL_MODELVIEW );
    // glLoadIdentity();

    // glMatrixMode( GL_PROJECTION );
    // glLoadIdentity();

    // glEnable( GL_BLEND );
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // {
    //   float2 min = test->uvBegin;
    //   float2 max = test->uvBegin + test->uvSize;

    //   glTexCoord2f( min.x, max.y );
    //   glVertex2f( -p, -p );

    //   glTexCoord2f( max.x, max.y );
    //   glVertex2f( p, -p );

    //   glTexCoord2f( max.x, min.y );
    //   glVertex2f( p, p );

    //   glTexCoord2f( min.x, max.y );
    //   glVertex2f( -p, -p );

    //   glTexCoord2f( max.x, min.y );
    //   glVertex2f( p, p );

    //   glTexCoord2f( min.x, min.y );
    //   glVertex2f( -p, p );
    // }

  }
};


