#pragma once


#include <common/font.h>
#include <common/bitmap.h>
#include <common/basic_types.h>

struct UIState
{
  Bitmap image;
  //  Bitmap8 fontImage;
  font::GlyphTable* glyphTable;
  int currentWriter;
};

struct GameState
{
  UIState uiState;
  s32 xOffset;
  s32 yOffset;
  float toneHz;
};