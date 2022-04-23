#pragma once

#include <platform/opengl/bsopengl_ext.h>

static opengl_ext::wglChoosePixelFormatARB* wglChoosePixelFormatARB;
static opengl_ext::wglCreateContextAttribsARB* wglCreateContextAttribsARB;
static opengl_ext::wglGetPixelFormatAttribivARB* wglGetPixelFormatAttribivARB;
static opengl_ext::wglGetPixelFormatAttribfvARB* wglGetPixelFormatAttribfvARB;
static opengl_ext::wglSwapIntervalEXT* wglSwapIntervalEXT;

static opengl_ext::glShaderSource* glShaderSource;
static opengl_ext::glCreateShader* glCreateShader;
static opengl_ext::glCompileShader* glCompileShader;
static opengl_ext::glAttachShader* glAttachShader;
static opengl_ext::glDetachShader* glDetachShader;
static opengl_ext::glDeleteShader* glDeleteShader;
static opengl_ext::glCreateProgram* glCreateProgram;
static opengl_ext::glLinkProgram* glLinkProgram;
static opengl_ext::glDeleteProgram* glDeleteProgram;
static opengl_ext::glCreateBuffers* glCreateBuffers;
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
static opengl_ext::glGenVertexArrays* glGenVertexArrays;
static opengl_ext::glBindVertexArray* glBindVertexArray;
static opengl_ext::glDeleteVertexArrays* glDeleteVertexArrays;

static opengl_ext::glGetUniformLocation* glGetUniformLocation;
static opengl_ext::glGetUniformfv* glGetUniformfv;
static opengl_ext::glGetUniformiv* glGetUniformiv;
static opengl_ext::glUniformMatrix4fv* glUniformMatrix4fv;

static opengl_ext::glNamedBufferData* glNamedBufferData;
static opengl_ext::glCreateVertexArrays* glCreateVertexArrays;

static opengl_ext::glEnableVertexArrayAttrib* glEnableVertexArrayAttrib;
static opengl_ext::glVertexArrayElementBuffer* glVertexArrayElementBuffer;
static opengl_ext::glVertexArrayAttribBinding* glVertexArrayAttribBinding;
static opengl_ext::glVertexArrayAttribFormat* glVertexArrayAttribFormat;
static opengl_ext::glVertexArrayVertexBuffer* glVertexArrayVertexBuffer;


namespace opengl_ext
{
  void* get_proc_address( char const* functionName );
  u32 validate();

  u32 init()
  {
    ::wglChoosePixelFormatARB = (opengl_ext::wglChoosePixelFormatARB*) get_proc_address( "wglChoosePixelFormatARB" );
    ::wglCreateContextAttribsARB =  (opengl_ext::wglCreateContextAttribsARB*) get_proc_address( "wglCreateContextAttribsARB" );
    ::wglGetPixelFormatAttribivARB = (opengl_ext::wglGetPixelFormatAttribivARB*) get_proc_address( "wglGetPixelFormatAttribivARB" );
    ::wglGetPixelFormatAttribfvARB = (opengl_ext::wglGetPixelFormatAttribfvARB*) get_proc_address( "wglGetPixelFormatAttribfvARB" );
    ::wglSwapIntervalEXT = (opengl_ext::wglSwapIntervalEXT*) get_proc_address( "wglSwapIntervalEXT" );
    ::glShaderSource =             (opengl_ext::glShaderSource*) get_proc_address( "glShaderSource" );
    ::glCreateShader =             (opengl_ext::glCreateShader*) get_proc_address( "glCreateShader" );
    ::glCompileShader =            (opengl_ext::glCompileShader*) get_proc_address( "glCompileShader" );
    ::glAttachShader =             (opengl_ext::glAttachShader*) get_proc_address( "glAttachShader" );
    ::glDetachShader =             (opengl_ext::glDetachShader*) get_proc_address( "glDetachShader" );
    ::glDeleteShader =             (opengl_ext::glDeleteShader*) get_proc_address( "glDeleteShader" );
    ::glCreateProgram =            (opengl_ext::glCreateProgram*) get_proc_address( "glCreateProgram" );
    ::glLinkProgram =              (opengl_ext::glLinkProgram*) get_proc_address( "glLinkProgram" );
    ::glDeleteProgram =            (opengl_ext::glDeleteProgram*) get_proc_address( "glDeleteProgram" );
    ::glCreateBuffers =            (opengl_ext::glCreateBuffers*) get_proc_address( "glCreateBuffers" );
    ::glBufferData =               (opengl_ext::glBufferData*) get_proc_address( "glBufferData" );
    ::glBindBuffer =               (opengl_ext::glBindBuffer*) get_proc_address( "glBindBuffer" );
    ::glDeleteBuffers =            (opengl_ext::glDeleteBuffers*) get_proc_address( "glDeleteBuffers" );
    ::glUseProgram =               (opengl_ext::glUseProgram*) get_proc_address( "glUseProgram" );
    ::glGetProgramiv =             (opengl_ext::glGetProgramiv*) get_proc_address( "glGetProgramiv" );
    ::glGetProgramInfoLog =        (opengl_ext::glGetProgramInfoLog*) get_proc_address( "glGetProgramInfoLog" );
    ::glGetShaderiv =              (opengl_ext::glGetShaderiv*) get_proc_address( "glGetShaderiv" );
    ::glGetShaderInfoLog =         (opengl_ext::glGetShaderInfoLog*) get_proc_address( "glGetShaderInfoLog" );
    ::glVertexAttribPointer =      (opengl_ext::glVertexAttribPointer*) get_proc_address( "glVertexAttribPointer" );
    ::glDisableVertexAttribArray = (opengl_ext::glDisableVertexAttribArray*) get_proc_address( "glDisableVertexAttribArray" );
    ::glEnableVertexAttribArray =  (opengl_ext::glEnableVertexAttribArray*) get_proc_address( "glEnableVertexAttribArray" );
    ::glGenBuffers =               (opengl_ext::glGenBuffers*) get_proc_address( "glGenBuffers" );
    ::glGenVertexArrays =          (opengl_ext::glGenVertexArrays*) get_proc_address( "glGenVertexArrays" );
    ::glBindVertexArray =          (opengl_ext::glBindVertexArray*) get_proc_address( "glBindVertexArray" );
    ::glDeleteVertexArrays =       (opengl_ext::glDeleteVertexArrays*) get_proc_address( "glDeleteVertexArrays" );
    ::glGetUniformLocation =       (opengl_ext::glGetUniformLocation*) get_proc_address( "glGetUniformLocation" );
    ::glGetUniformfv =             (opengl_ext::glGetUniformfv*) get_proc_address( "glGetUniformfv" );
    ::glGetUniformiv =             (opengl_ext::glGetUniformiv*) get_proc_address( "glGetUniformiv" );
    ::glUniformMatrix4fv =         (opengl_ext::glUniformMatrix4fv*) get_proc_address( "glUniformMatrix4fv" );

    ::glNamedBufferData = (opengl_ext::glNamedBufferData*) get_proc_address( "glNamedBufferData" );
    ::glCreateVertexArrays = (opengl_ext::glCreateVertexArrays*) get_proc_address( "glCreateVertexArrays" );

    ::glEnableVertexArrayAttrib = (opengl_ext::glEnableVertexArrayAttrib*) get_proc_address( "glEnableVertexArrayAttrib" );
    ::glVertexArrayElementBuffer = (opengl_ext::glVertexArrayElementBuffer*) get_proc_address( "glVertexArrayElementBuffer" );
    ::glVertexArrayAttribBinding = (opengl_ext::glVertexArrayAttribBinding*) get_proc_address( "glVertexArrayAttribBinding" );
    ::glVertexArrayAttribFormat = (opengl_ext::glVertexArrayAttribFormat*) get_proc_address( "glVertexArrayAttribFormat" );
    ::glVertexArrayVertexBuffer = (opengl_ext::glVertexArrayVertexBuffer*) get_proc_address( "glVertexArrayVertexBuffer" );


    return validate();
  }

  u32 validate()
  {
    if ( ::wglChoosePixelFormatARB == nullptr ) BREAK;
    if ( ::wglCreateContextAttribsARB == nullptr ) BREAK;
    if ( ::wglGetPixelFormatAttribivARB == nullptr ) BREAK;
    if ( ::wglGetPixelFormatAttribfvARB == nullptr ) BREAK;
    if ( ::wglSwapIntervalEXT == nullptr ) BREAK;
    if ( ::glShaderSource == nullptr ) BREAK;
    if ( ::glCreateShader == nullptr ) BREAK;
    if ( ::glCompileShader == nullptr ) BREAK;
    if ( ::glAttachShader == nullptr ) BREAK;
    if ( ::glDetachShader == nullptr ) BREAK;
    if ( ::glDeleteShader == nullptr ) BREAK;
    if ( ::glCreateProgram == nullptr ) BREAK;
    if ( ::glLinkProgram == nullptr ) BREAK;
    if ( ::glDeleteProgram == nullptr ) BREAK;
    if ( ::glCreateBuffers == nullptr ) BREAK;
    if ( ::glBufferData == nullptr ) BREAK;
    if ( ::glBindBuffer == nullptr ) BREAK;
    if ( ::glDeleteBuffers == nullptr ) BREAK;
    if ( ::glUseProgram == nullptr ) BREAK;
    if ( ::glGetProgramiv == nullptr ) BREAK;
    if ( ::glGetProgramInfoLog == nullptr ) BREAK;
    if ( ::glGetShaderiv == nullptr ) BREAK;
    if ( ::glGetShaderInfoLog == nullptr ) BREAK;
    if ( ::glVertexAttribPointer == nullptr ) BREAK;
    if ( ::glDisableVertexAttribArray == nullptr ) BREAK;
    if ( ::glEnableVertexAttribArray == nullptr ) BREAK;
    if ( ::glGenBuffers == nullptr ) BREAK;
    if ( ::glGenVertexArrays == nullptr ) BREAK;
    if ( ::glBindVertexArray == nullptr ) BREAK;
    if ( ::glDeleteVertexArrays == nullptr ) BREAK;
    if ( ::glGetUniformLocation == nullptr ) BREAK;
    if ( ::glGetUniformfv == nullptr ) BREAK;
    if ( ::glGetUniformiv == nullptr ) BREAK;
    if ( ::glUniformMatrix4fv == nullptr ) BREAK;

    if ( ::glNamedBufferData == nullptr ) BREAK;
    if ( ::glCreateVertexArrays == nullptr ) BREAK;

    if ( ::glEnableVertexArrayAttrib == nullptr ) BREAK;
    if ( ::glVertexArrayElementBuffer == nullptr ) BREAK;
    if ( ::glVertexArrayAttribBinding == nullptr ) BREAK;
    if ( ::glVertexArrayAttribFormat == nullptr ) BREAK;
    if ( ::glVertexArrayVertexBuffer == nullptr ) BREAK;

    return 1;
  }

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

