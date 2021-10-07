#pragma once

#include <common/font.h>

namespace debug
{

  struct ConsoleLine
  {
    TextBox* text;
    ConsoleLine* next;
  };

  struct Console
  {


    ConsoleLine* first;

  };
}