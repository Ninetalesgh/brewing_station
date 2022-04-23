#pragma once

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#include <ui/bstextarea.h>
#include <scene/bssceneobject.h>

#include <core/bsgraphics.h>
#include <core/bsfile.h>
#include <core/bsfont.h>
#include <core/bsmemory.h>
#include <common/bscolor.h>
#include <common/bscommon.h>

#include <opengl_ext.h>
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
  //void create_opengl_context_for_worker_thread();

  bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* );
  void free_texture( bs::graphics::TextureID texture );

  bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* );
  void free_mesh( bs::graphics::Mesh );

  using VertexArrayObjectID = u32;
  using ProgramID = u32;
  using ShaderID = u32;
  using uniformID = u32;

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData );

  //all three files in one, mark sections with #h, #vs and #fs.
  ProgramID create_shader_program( bs::file::Data combinedglsl );

  void render( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );
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
  void check_gl_error();

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

  HGLRC create_render_context_for_worker_thread()
  {
    check_gl_error();
    HGLRC rc = wglCreateContextAttribsARB( global::deviceContext, global::renderContext, global::rcAttributes );
    check_gl_error();
    return rc;
  }

  void set_worker_thread_render_context( HGLRC renderContext )
  {
    if ( !wglMakeCurrent( global::deviceContext, renderContext ) )
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
    SwapBuffers( global::deviceContext );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  INLINE u32 get_size_for_index_format( bs::graphics::IndexFormat indexFormat ) { return indexFormat == bs::graphics::IndexFormat::U16 ? sizeof( u16 ) : sizeof( u32 ); }

  INLINE GLenum get_gl_index_format( bs::graphics::IndexFormat indexFormat ) { return indexFormat == bs::graphics::IndexFormat::U16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT; }

  VertexArrayObjectID init_vao_for_mesh( bs::graphics::Mesh const* mesh )
  {
    VertexArrayObjectID vao;
    BREAK;
    //not working yet
    check_gl_error();
    glCreateVertexArrays( 1, &vao );

    glVertexArrayVertexBuffer( vao, 0, mesh->vb, 0, 0 );
    glVertexArrayVertexBuffer( vao, 1, mesh->uvb, 0, 0 );

    glEnableVertexArrayAttrib( vao, 0 );
    glVertexArrayAttribBinding( vao, 0, 0 );
    glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );

    glEnableVertexArrayAttrib( vao, 1 );
    glVertexArrayAttribBinding( vao, 1, 1 );
    glVertexArrayAttribFormat( vao, 1, 2, GL_FLOAT, GL_FALSE, 0 );

    glVertexArrayElementBuffer( vao, mesh->ib );

    check_gl_error();
    return vao;
  }

  void init_vao( bs::graphics::RenderObject* renderObject )
  {
    check_gl_error();

    glGenVertexArrays( 1, &renderObject->id );
    glBindVertexArray( renderObject->id );

    glBindBuffer( GL_ARRAY_BUFFER, renderObject->mesh.vb );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ARRAY_BUFFER, renderObject->mesh.uvb );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, renderObject->mesh.ib );

    check_gl_error();

    glBindVertexArray( 0 );
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    check_gl_error();
  }

  void render_object( bs::graphics::RenderObject* renderObject, bs::graphics::ShaderProgram shaderProgram )
  {
    if ( !renderObject->id )
    {
      init_vao( renderObject );
    }

    glBindVertexArray( renderObject->id );

    glUseProgram( shaderProgram );

    u32 indexSize = get_size_for_index_format( renderObject->mesh.indexFormat );
    GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
    glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount * indexSize, glIndexFormat, (void*) 0 );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
  }

  bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* raw )
  {
    bs::graphics::Mesh resultMesh {};

    u32 const vertexCount = raw->vertexCount;
    u32 const indexCount = raw->indexCount;
    bs::graphics::IndexFormat const indexFormat = raw->indexFormat;

    if ( indexFormat == bs::graphics::IndexFormat::INVALID ) BREAK;

    check_gl_error();

    glCreateBuffers( 1, &resultMesh.vb );
    glNamedBufferData( resultMesh.vb, vertexCount * sizeof( float3 ), raw->vertices, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.uvb );
    glNamedBufferData( resultMesh.uvb, vertexCount * sizeof( float2 ), raw->uvs, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.ib );
    glNamedBufferData( resultMesh.ib, indexCount * get_size_for_index_format( indexFormat ), raw->indices, GL_STATIC_DRAW );

    check_gl_error();

    resultMesh.indexCount = indexCount;
    resultMesh.indexFormat = indexFormat;
    return resultMesh;
  }

  void free_mesh( bs::graphics::Mesh mesh )
  {
    u32 buffers[] = { mesh.vb, mesh.uvb, mesh.ib };
    glDeleteBuffers( 3, buffers );
  }

  bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* textureData )
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

    return (bs::graphics::TextureID) textureHandle;
  }

  void free_texture( bs::graphics::TextureID texture )
  {
    GLuint handle = (GLuint) texture;
    glDeleteTextures( 1, &handle );
  }

  void allocate_render_object()
  {

    //diffuse texture
    //normal texture

    //mesh

    //animation data?
  }

  void free_render_object()
  {

    //free_texture();
  }



  void render_text_area_fixed_pipeline( bs::graphics::RenderTarget* target, bs::ui::TextArea* textArea )
  {
    bs::font::GlyphSheet* glyphSheet = ::global::defaultGlyphSheet;
    bs::font::GlyphTable* glyphTable = ::global::defaultGlyphTable;
    int2 screenSize = ::global::mainWindow.size;
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, glyphSheet->textureID );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef( 1.0f / (float) glyphSheet->width, 1.0f / (float) glyphSheet->height, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    float mvMat[] = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f,
    };
    glLoadMatrixf( mvMat );
    glScalef( DEBUG::scale, DEBUG::scale, 1.0f );


    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.f, screenSize.x, screenSize.y, 0.f, 0.f, 1.f );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBegin( GL_TRIANGLES );
    //render here


    float2 invScreenSize = ::global::mainWindow.size;
    invScreenSize = { 1.0f / invScreenSize.x, 1.0f / invScreenSize.y };

    float2 textAreaOrigin = { 0.0f, 0.0f };
    float2 lineOrigin = textAreaOrigin;

    for ( s32 lineIndex = 0; lineIndex < textArea->lineCount; ++lineIndex )
    {
      lineOrigin.y += glyphTable->scale;
      float2 currentPos = lineOrigin;

      bs::ui::TextLine& textLine = textArea->lines[lineIndex];

      s32 const end = lineIndex == textArea->lineCount - 1 ? (s32) '\0' : (s32) '\n';
      s32 const* reader = textLine.codepoints;
      while ( *reader != end )
      {
        bs::font::Glyph* glyph = get_glyph_for_codepoint( glyphSheet, *reader );
        ++reader;

        {
          float2 min = textAreaOrigin + float2 { currentPos.x + glyph->offsetX, currentPos.y + glyph->offsetY };
          float2 max = { min.x + (float) glyph->uvSize.x,  min.y + (float) glyph->uvSize.y };

          currentPos.x += glyph->advance;
          float2 minuv = glyph->uvBegin;
          float2 maxuv = glyph->uvBegin + glyph->uvSize;

          // minuv = { 0, 0 };
          // maxuv = { 1,1 };
          // min = { 0,0 };
          // max = { 1, 1 };

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, max.y );

          glTexCoord2f( maxuv.x, maxuv.y );
          glVertex2f( max.x, max.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, min.y );

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, max.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, min.y );

          glTexCoord2f( minuv.x, minuv.y );
          glVertex2f( min.x, min.y );
        }
      }

    }

    glEnd();
  }


  void render_testDEBUG( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end

        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end

        2.0f,-1.0f, 2.0f,
        -1.0f,-1.0f,-1.0f,
        2.0f,-1.0f,-1.0f,

        2.0f, 1.0f,-1.0f,
        2.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,
        -1.0f, 2.0f, 1.0f,
        -1.0f, 2.0f,-1.0f,

        2.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 2.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 2.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 2.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    static GLuint vertexbuffer;
    static GLuint colorbuffer;

    static int firsttime = 1;
    if ( firsttime-- )
    {
      GLuint VertexArrayID;
      glGenVertexArrays( 1, &VertexArrayID );
      glBindVertexArray( VertexArrayID );

      glGenBuffers( 1, &vertexbuffer );
      glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
      glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );

      GLuint colorArrayID;
      glBindVertexArray( 0 );
      glGenVertexArrays( 1, &colorArrayID );
      glBindVertexArray( colorArrayID );

      glGenBuffers( 1, &colorbuffer );
      glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
      glBufferData( GL_ARRAY_BUFFER, sizeof( g_color_buffer_data ), g_color_buffer_data, GL_STATIC_DRAW );
      glBindVertexArray( 0 );
    }

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    float4x4 model = float4x4::identity();
    model.pos = float4 { 0,2.0f * sinf( modelposy ), 0, 1 };

    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );
    float4x4 mvp = model * vp;
    uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
    glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
    glVertexAttribPointer(
       0,                  // attribute 0. match shader
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*) 0           // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays( GL_TRIANGLES, 0, 3 * 12 ); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glEnableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
    glVertexAttribPointer(
        1,                                // attribute. match shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*) 0                         // array buffer offset
    );

    // glDisableVertexAttribArray( 0 );
    // glDisableVertexAttribArray( 1 );
  }

  void render_custom_bitmap( bs::graphics::RenderTarget* target, bs::graphics::Bitmap* bmp )
  {
    int2 screenSize = ::global::mainWindow.size;

    bs::graphics::TextureData texData {};
    texData.pixel = bmp->pixel;
    texData.width = bmp->width;
    texData.height = bmp->height;
    texData.format = bs::graphics::TextureFormat::RGBA8;

    bs::graphics::TextureID id = allocate_texture( &texData );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, id );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.f, screenSize.x, screenSize.y, 0.f, 0.f, 10.f );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBegin( GL_TRIANGLES );

    float2 min = { -1, -1 };
    float2 max = { (float) bmp->width, (float) bmp->height };
    glTexCoord2f( 0.0f, 1.0f );
    glVertex2f( min.x, max.y );

    glTexCoord2f( 1.0f, 1.0f );
    glVertex2f( max.x, max.y );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex2f( max.x, min.y );

    glTexCoord2f( 0.0f, 1.0f );
    glVertex2f( min.x, max.y );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex2f( max.x, min.y );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex2f( min.x, min.y );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glEnd();

    //free_texture( id );
  }





  struct PrmTest
  {
    thread::ThreadInfo* threadInfo;
    bs::graphics::Mesh mesh;
    HGLRC renderContext;
  };
  DWORD WINAPI thread_bullcrap( void* void_parameter )
  {
    PrmTest* parameter = (PrmTest*) void_parameter;
    opengl::set_worker_thread_render_context( parameter->renderContext );

    static float3 vertices[] = {
              float3{-1.0f,-1.0f, -1.0f},
              float3{ 1.0f,-1.0f, -1.0f},
              float3{ 1.0f, 1.0f, -1.0f},
              float3{-1.0f, 1.0f, -1.0f},
    };

    static  float2 uvs[] = {
    float2{0.0f, 1.0f},
    float2{1.0f, 1.0f},
    float2{1.0f, 0.0f},
    float2{0.0f, 0.0f},
    };

    static  u16 indices[] =
    {
      0,1,2,
      0,2,3
    };
    bs::graphics::MeshData raw {};
    raw.vertices = vertices;
    raw.vertexCount = array_count( vertices );
    raw.indices = indices;
    raw.indexCount = array_count( indices );
    raw.uvs = uvs;
    raw.indexFormat = bs::graphics::IndexFormat::U16;
    parameter->mesh = allocate_mesh( &raw );

    check_gl_error();

    thread::write_barrier();
    parameter->threadInfo = nullptr;
    return 0;
  }

  void render_testDEBUG2( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f, -1.0f,
         1.0f,-1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
    };
    static const GLfloat g_uv_buffer_data[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    };

    static const u16 g_index_buffer_data[] =
    {
      0,1,2,
      0,2,3
    };
    // static const GLfloat g_uv_buffer_data[] = {
    // 0.0f, 0.2f,
    // 0.2f, 0.2f,
    // 0.2f, 0.0f,
    // 0.0f, 0.2f,
    // 0.2f, 0.0f,
    // 0.0f, 0.0f,
    // };
    check_gl_error();

    static bs::graphics::Mesh meshFromThisThread {};
    static bs::graphics::RenderObject* renderObject;

    static GLuint vertexbuffer;
    static GLuint uvBuffer;
    static GLuint indexBuffer;
    static GLuint VertexArrayID;

    static int firsttime = 0;
    if ( !firsttime-- )
    {
      static float3 vertices[] = {
           float3{-1.0f,-1.0f, -1.0f},
           float3{ 1.0f,-1.0f, -1.0f},
           float3{ 1.0f, 1.0f, -1.0f},
           float3{-1.0f, 1.0f, -1.0f},
      };

      static  float2 uvs[] = {
      float2{0.0f, 1.0f},
      float2{1.0f, 1.0f},
      float2{1.0f, 0.0f},
      float2{0.0f, 0.0f},
      };

      static  u16 indices[] =
      {
        0,1,2,
        0,2,3
      };

      bs::graphics::MeshData raw {};
      raw.vertices = vertices;
      raw.vertexCount = array_count( vertices );
      raw.indices = indices;
      raw.indexCount = array_count( indices );
      raw.uvs = uvs;
      raw.indexFormat = bs::graphics::IndexFormat::U16;


      // meshFromThisThread = allocate_mesh( &raw );
       //   glGenVertexArrays( 1, &VertexArrayID );
        //  glBindVertexArray( VertexArrayID );
      check_gl_error();

      renderObject = (bs::graphics::RenderObject*) bs::memory::allocate_to_zero( sizeof( bs::graphics::RenderObject ) );
      //  renderObject->mesh = allocate_mesh( &raw );


      PrmTest poop = {};
      thread::ThreadInfo dumbStandaloneThreadTest {};
      poop.threadInfo = &dumbStandaloneThreadTest;
      poop.renderContext = opengl::create_render_context_for_worker_thread();
      check_gl_error();
      CloseHandle( CreateThread( 0, 0, thread_bullcrap, &poop, 0, (LPDWORD) &poop.threadInfo->id ) );

      //wait for the thread to shutdown
      while ( poop.threadInfo != nullptr ) {}

      renderObject->mesh = poop.mesh;
    }

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    float4x4 model = float4x4::identity();
    model.pos = float4 { 0,2.0f * sinf( modelposy ), 0, 1 };

    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );
    float4x4 mvp = model * vp;

    glBindVertexArray( 0 );
    check_gl_error();

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );
    uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
    glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

    check_gl_error();

    GLuint textureID = ::global::defaultGlyphSheet->textureID;
    glBindTexture( GL_TEXTURE_2D, textureID );
    if ( !renderObject->id )
    {
      //  renderObject->id = init_vao_for_mesh( &renderObject->mesh );

      init_vao( renderObject );
    }
    check_gl_error();


    glBindVertexArray( renderObject->id );

    check_gl_error();

    GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
    glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount, glIndexFormat, (void*) 0 );

    glBindVertexArray( 0 );
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );

    // glUseProgram( 0 );
    // glBindVertexArray( 0 );
    // glBindBuffer( GL_ARRAY_BUFFER, 0 );
    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // glEnableVertexAttribArray( 0 );
    // glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
    // glVertexAttribPointer(
    //    0,                  // attribute 0. match shader
    //    3,                  // size
    //    GL_FLOAT,           // type
    //    GL_FALSE,           // normalized?
    //    0,                  // stride
    //    (void*) 0           // array buffer offset
    // );
    // // Draw the triangle !

    // glEnableVertexAttribArray( 1 );
    // glBindBuffer( GL_ARRAY_BUFFER, uvBuffer );
    // glVertexAttribPointer(
    //     1,                                // attribute. match shader.
    //     2,                                // size
    //     GL_FLOAT,                         // type
    //     GL_FALSE,                         // normalized?
    //     0,                                // stride
    //     (void*) 0                         // array buffer offset
    // );

    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

   // glDrawElements( GL_TRIANGLES, sizeof( g_index_buffer_data ), GL_UNSIGNED_SHORT, (void*) 0 );

  }

  void render_testSCENE( bs::graphics::RenderTarget* target, bs::graphics::Camera* camera, bs::scene::Object* sceneObjects, u32 sceneObjectCount )
  {
    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );

    GLuint id = ::global::defaultGlyphSheet->textureID;
    glBindTexture( GL_TEXTURE_2D, id );

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );

    for ( u32 i = 0; i < sceneObjectCount; ++i )
    {
      bs::graphics::RenderObject* renderObject = sceneObjects[i].renderObject;

      if ( !renderObject->id )
      {
        init_vao( renderObject );
      }
      glBindVertexArray( renderObject->id );

      float4x4 mvp = sceneObjects[i].transform * vp;
      uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
      glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

      //  u32 indexSize = get_size_for_index_format( renderObject->mesh.indexFormat );
      GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
      glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount, glIndexFormat, (void*) 0 );

      glBindVertexArray( 0 );
    }
    glUseProgram( 0 );
  }

  void render( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    render_testDEBUG2( target, group, camera );

    switch ( group->type )
    {
      case bs::graphics::RenderGroup::TEXT_AREA:
      {
        //render_text_area_fixed_pipeline( target, (bs::ui::TextArea*) group->renderObject );
        break;
      }
      case bs::graphics::RenderGroup::CUSTOM_BITMAP:
      {
        render_custom_bitmap( target, (bs::graphics::Bitmap*) group->renderObject );
        break;
      }
      case bs::graphics::RenderGroup::SCENE_OBJECTS:
      {
        render_testSCENE( target, camera, (bs::scene::Object*) group->renderObject, group->count );
        break;
      }
      default:
      {
        //  render_testDEBUG( target, group, camera );
        break;
      }
    }
  }

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
    char* nextSection = bs::string::contains( reader, "#h" );
    assert( nextSection );
    char* currentSection = nextSection + 2;

    nextSection = bs::string::contains( currentSection, "#vs" );
    assert( nextSection );

    h.data = currentSection;
    h.size = nextSection - currentSection;

    //vertex shader
    currentSection = nextSection + 3;
    nextSection = bs::string::contains( currentSection, "#fs" );
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
};


