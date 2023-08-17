#include "first_app.h"

float deltaTime = 1.0f / 60.0f;

//int2 circlePos;
//float2 circleSpeed;
//float circleRadius;
void start()
{
  //circlePos = { windowWidth / 2, windowHeight / 2 + 50 };
 // circleSpeed = { 0, 0 };
 // circleRadius = 50.f;

}

//float gravity = 9.8f;

void update()
{
  using namespace bs::input;

  for ( int i = 10; i < 300; ++i )
  {
    plot( { 10,i }, color::PURPLE );
  }

  for ( int i = 10; i < 300; ++i )
  {
    plot( { i,60 }, color::TEAL );
  }

  for ( int i = 10; i < 200; ++i )
  {
    plot( { 10,i }, color::PURPLE );
  }
  //1200 740
  //600 300
  {
    int2 begin { 300, 220 };
    int2 end { 700, 520 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::TEAL );
      }
  }
  {
    int2 begin { 300, 270 };
    int2 end { 700, 470 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::PURPLE );
      }
  }
  {
    int2 begin { 300, 320 };
    int2 end { 700, 420 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::WHITE );
      }
  }

  {
    int2 pos { windowWidth / 2,windowHeight / 2 };
    s32 radius = 105;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos + int2 { x, y };
          plot( tmpPos, color::BLACK );
        }
      }
  }

  {
    int2 pos { windowWidth / 2,windowHeight / 2 };
    s32 radius = 100;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos + int2 { x, y };
          plot( tmpPos, color::RED );
        }
      }
  }

  {
    int2 begin { 415, 350 };
    int2 end { 609, 390 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::WHITE );
      }
  }

  //int2 begin = { 50,70 };
  //int2 end = { 100,300 };
  //draw_rect( begin, end, color::GREEN );

  // s32 speed = 5;
   // if ( is_key_held( KEY_LEFT ) )
   // {
   //   circlePos.x -= speed;
   // }
   // else if ( is_key_held( KEY_RIGHT ) )
   // {
   //   circlePos.x += speed;
   // }

   // if ( is_key_held( KEY_UP ) )
   // {
   //   circlePos.y -= speed;
   // }
   // else if ( is_key_held( KEY_DOWN ) )
   // {
   //   circlePos.y += speed;
   // }

   // circlePos += {(s32) circleSpeed.x, (s32) circleSpeed.y};
   // circleSpeed.y += gravity * deltaTime;

   // if ( circlePos.y + circleRadius > windowHeight )
   // {
   //   circleSpeed.y *= -1.0f;
   // }

   // int2 begin = { 20,20 };
   // int2 end = { 500,500 };
   //draw_rect( begin, end, color::GREEN );

   // begin = { 400, 30 };
   // end = { 600, 100 };
    //draw_rect( begin, end, color::RED );

    //draw_circle( circlePos, circleRadius, color::BLUE );
   //log_info( "hello, deltaTime was: ", deltaTime );
}


