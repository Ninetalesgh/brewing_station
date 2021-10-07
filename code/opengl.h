#pragma once

#include <common/basic_math.h>
#include <windows.h>
#include <gl/gl.h>

namespace win32
{
  struct OpenGLThreadParameter
  {
    ThreadInfo threadInfo;
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
    OpenGLThreadParameter& parameter = *(OpenGLThreadParameter*) voidParameter;
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
    while ( 1 )
    {
      glClear( GL_COLOR_BUFFER_BIT );
      SwapBuffers( windowDeviceContext );
    }
    return 0;
  }

};