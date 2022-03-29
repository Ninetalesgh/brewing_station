#pragma once


namespace bs
{
  namespace input
  {
    enum : u8
    {
      MOUSE_PRIMARY            = 1,
      MOUSE_SECONDARY          = 2,
      MOUSE_3                  = 4,
      MOUSE_4                  = 5,
      MOUSE_5                  = 6,
      KEY_BACK                 = 8,
      KEY_TAB                  = 9,
      KEY_ENTER                = 13,
      KEY_SHIFT                = 16,
      KEY_CTRL                 = 17,
      KEY_ALT                  = 18,
      KEY_PAUSE                = 19,
      KEY_CAPSLOCK             = 20,
      KEY_ESCAPE               = 27,
      KEY_SPACE                = 32,
      KEY_PGUP                 = 33,
      KEY_PGDOWN               = 34,
      KEY_END                  = 35,
      KEY_HOME                 = 36,
      KEY_LEFT                 = 37,
      KEY_UP                   = 38,
      KEY_RIGHT                = 39,
      KEY_DOWN                 = 40,
      KEY_DELETE               = 46,
      KEY_0                    = 48,
      KEY_1                    = 49,
      KEY_2                    = 50,
      KEY_3                    = 51,
      KEY_4                    = 52,
      KEY_5                    = 53,
      KEY_6                    = 54,
      KEY_7                    = 55,
      KEY_8                    = 56,
      KEY_9                    = 57,
      KEY_A                    = 65,
      KEY_B                    = 66,
      KEY_C                    = 67,
      KEY_D                    = 68,
      KEY_E                    = 69,
      KEY_F                    = 70,
      KEY_G                    = 71,
      KEY_H                    = 72,
      KEY_I                    = 73,
      KEY_J                    = 74,
      KEY_K                    = 75,
      KEY_L                    = 76,
      KEY_M                    = 77,
      KEY_N                    = 78,
      KEY_O                    = 79,
      KEY_P                    = 80,
      KEY_Q                    = 81,
      KEY_R                    = 82,
      KEY_S                    = 83,
      KEY_T                    = 84,
      KEY_U                    = 85,
      KEY_V                    = 86,
      KEY_W                    = 87,
      KEY_X                    = 88,
      KEY_Y                    = 89,
      KEY_Z                    = 90,
      KEY_WINDOWS              = 91,
      KEY_MENU                 = 93,
      KEY_INSERT               = 45,
      KEY_F1                   = 112,
      KEY_F2                   = 113,
      KEY_F3                   = 114,
      KEY_F4                   = 115,
      KEY_F5                   = 116,
      KEY_F6                   = 117,
      KEY_F7                   = 118,
      KEY_F8                   = 119,
      KEY_F9                   = 120,
      KEY_F10                  = 121,
      KEY_F11                  = 122,
      KEY_F12                  = 123,
      KEY_SEMICOLON            = 186,
      KEY_EQUALS               = 187,
      KEY_COMMA                = 188,
      KEY_MINUS                = 189,
      KEY_PERIOD               = 190,
      KEY_SLASH                = 191,
      KEY_APOSTROPHE           = 192,
      KEY_SQUARE_BRACKET_OPEN  = 219,
      KEY_SQUARE_BRACKET_CLOSE = 221,
      KEY_BACKSLASH            = 220,
      KEY_QUOTE                = 222,
      STATE_COUNT              = 0xff
    };

    struct ControllerButton
    {
      s32 halfTransitionCount;
      u32 endedDown;
    };

    struct Controller
    {
      float2 start;
      float2 min;
      float2 max;
      float2 end;

      union
      {
        ControllerButton button[6];
        struct
        {
          ControllerButton up;
          ControllerButton down;
          ControllerButton left;
          ControllerButton right;
          ControllerButton leftShoulder;
          ControllerButton rightShoulder;
        };
      };
    };

    struct MousePos
    {
      int2 start;
      int2 end;
      int2 min;
      int2 max;
    };

    struct State
    {
      Controller controller[4];
      MousePos mousePos[1];
      s32 mouseWheelDelta;

      //mouse & keyboard
      u8 down[STATE_COUNT];
      u8 held[STATE_COUNT];
    };
  };
};