#pragma once

#include "win32/win32_global.h"
#include <common/basic_math.h>
#include <windows.h>
#include <gl/gl.h>

namespace win32
{
  struct ThreadOpenGLParameter
  {
    thread::ThreadInfo threadInfo;
    HWND window;
    int2 resolution;
  };
  DWORD thread_OpenGL( void* voidParameter );
};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
////////////cpp//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

namespace win32
{

  namespace opengl
  {
    HDC init_device_context( HWND window )
    {
      HDC windowDeviceContext = GetDC( window );

      PIXELFORMATDESCRIPTOR desiredPixelFormat {};
      desiredPixelFormat.nSize = sizeof( desiredPixelFormat );
      desiredPixelFormat.nVersion = 1;
      desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
      desiredPixelFormat.cColorBits = 32;
      desiredPixelFormat.cAlphaBits = 8;
      desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
      s32 suggestedPixelFormatIndex = ChoosePixelFormat( windowDeviceContext, &desiredPixelFormat );
      PIXELFORMATDESCRIPTOR suggestedPixelFormat {};
      DescribePixelFormat( windowDeviceContext, suggestedPixelFormatIndex, sizeof( suggestedPixelFormat ), &suggestedPixelFormat );
      SetPixelFormat( windowDeviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat );

      return windowDeviceContext;
    }
  };


  DWORD thread_OpenGL( void* voidParameter )
  {
    ThreadOpenGLParameter& parameter = *(ThreadOpenGLParameter*) voidParameter;
    //ThreadInfo& threadInfo = parameter.threadInfo;
    int2& resolution = parameter.resolution;

    HDC windowDeviceContext = opengl::init_device_context( parameter.window );
    HGLRC oglRenderContext = wglCreateContext( windowDeviceContext );
    if ( !wglMakeCurrent( windowDeviceContext, oglRenderContext ) )
    {
      BREAK;
    }

    glViewport( 0, 0, resolution.x, resolution.y );
    glClearColor( 1.0f, 0.0f, 1.0f, 0.0f );

    GLuint textureHandle = 0;
    glGenTextures( 1, &textureHandle );

    while ( 1 )
    {
      thread::wait_if_requested( &parameter.threadInfo );

      if ( 1 )
      {
        BackBuffer backBuffer {};
        {
          backBuffer.data          = global::win32Data.backBuffer.bmpBuffer;
          backBuffer.width         = global::win32Data.backBuffer.bmpWidth;
          backBuffer.height        = global::win32Data.backBuffer.bmpHeight;
          backBuffer.pitch         = global::win32Data.backBuffer.pitch;
          backBuffer.bytesPerPixel = global::win32Data.backBuffer.bytesPerPixel;
        }

        {
          AppRenderParameter renderParameter {};
          renderParameter.platformData = &global::win32Data.platformData;
          renderParameter.appData      = &global::win32Data.appData;
          renderParameter.backBuffer   = &backBuffer;
          global::win32Data.app_instances[global::win32Data.guard_currentDllIndex].render( renderParameter );
        }

        GLuint textureHandle = 0;
        glGenTextures( 1, &textureHandle );
        glBindTexture( GL_TEXTURE_2D, textureHandle );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, backBuffer.data );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        glEnable( GL_TEXTURE_2D );

        glClear( GL_COLOR_BUFFER_BIT );
        glBegin( GL_TRIANGLES );

        glMatrixMode( GL_TEXTURE );
        glLoadIdentity();

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        float p = 1.0f;

        glTexCoord2f( 0, 1 );
        glVertex2f( -p, -p );

        glTexCoord2f( 1, 1 );
        glVertex2f( p, -p );

        glTexCoord2f( 1, 0 );
        glVertex2f( p, p );

        glTexCoord2f( 0, 1 );
        glVertex2f( -p, -p );

        glTexCoord2f( 1, 0 );
        glVertex2f( p, p );

        glTexCoord2f( 0, 0 );
        glVertex2f( -p, p );

        // float p = 0.9f;
        // glColor3f( 1, 0, 0 );
        // glVertex2f( -p, -p );
        // glColor3f( 0, 1, 0 );
        // glVertex2f( p, -p );
        // glColor3f( 0, 0, 1 );
        // glVertex2f( p, p );
        // glColor3f( 1, 0, 0 );
        // glVertex2f( -p, -p );
        // glColor3f( 0, 1, 0 );
        // glVertex2f( p, p );
        // glColor3f( 0, 0, 1 );
        // glVertex2f( -p, p );

        glEnd();
        SwapBuffers( windowDeviceContext );
      }
    }
    return 0;
  }

};