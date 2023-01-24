#pragma once

DEPRECATED

#include <common/bs_common.h>

namespace bs { namespace font { struct GlyphTable; struct GlyphSheet; }; };
namespace bs
{
  namespace ui
  {
    struct TextArea;

    //font can be nullptr
    //dimensions relative to glyph scale
    TextArea* create_text_area_from_text( char const* text, font::GlyphTable* font, float2 dimensions );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


namespace bs
{
  namespace ui
  {
    struct TextLine
    {
      s32* codepoints;
      s32 capacity;
    };
    struct TextArea
    {
      font::GlyphTable* glyphTable;
      font::GlyphSheet* glyphSheet; //TODO
      TextLine* lines;
      s32 lineCount;
    };

    TextArea* create_text_area_from_text( char const* text, font::GlyphTable* glyphTable, float2 dimensions )
    {
      s32 lineCount = string_line_count( text );
      s32 totalTextLength = string_length_utf8( text ) + 1;

      u8* allocation = (u8*) memory::allocate( sizeof( TextArea ) + sizeof( TextLine ) * lineCount + sizeof( s32 ) * totalTextLength );
      TextArea* resultArea = (TextArea*) allocation;
      resultArea->glyphTable = glyphTable;
      resultArea->lines = (TextLine*) (allocation + sizeof( TextArea ));
      resultArea->lineCount = lineCount;

      char const* reader = text;
      s32* writer = (s32*) (allocation + sizeof( TextArea ) + sizeof( TextLine ) * lineCount);

      s32 bytesWritten = 0;
      for ( s32 i = 0; i < lineCount; ++i )
      {
        TextLine& textLine = resultArea->lines[i];
        textLine.codepoints = writer;
        textLine.capacity = 0;

        s32 const end = i == lineCount - 1 ? (s32) '\0' : (s32) '\n';
        s32 codepoint = 1;
        while ( codepoint != end )
        {
          reader = string_parse_utf8( reader, &codepoint );
          *writer++ = codepoint;
          ++textLine.capacity;
        }

        bytesWritten += textLine.capacity;
      }
      assert( bytesWritten == totalTextLength );

      return resultArea;
    }

  };
};