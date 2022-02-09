#pragma once


#include <ui/debugui.h>

#include <common/font.h>
#include <common/bitmap.h>
#include <common/memory.h>
#include <common/basic_types.h>

struct UIState
{
  Bitmap image;
  //  Bitmap8 fontImage;
  font::GlyphTable* glyphTable;

  ui::debug::DebugDisplay* debugDisplay;
};

struct GameState
{
  UIState uiState;
  s32 xOffset;
  s32 yOffset;
  float toneHz;
};
