#pragma once


#include <common/bitmap.h>
#include <common/basic_types.h>


struct UIState
{
  Bitmap image;
  Bitmap8 fontImage;
};

struct GameState
{
  UIState uiState;
  s32 xOffset;
  s32 yOffset;
  float toneHz;
};