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

#pragma warning(disable : 4702)
void test_stuff()
{
  draw_line( { 0, 0 }, { 500,500 }, color::WHITE );
  draw_line( { 0, 500 }, { 500,0 }, color::PINK );

}

//////////////////////////////////////////////////////////////////////////////////////////////
//LINES
void update()
{
  using namespace bs::input;

  test_stuff();
  return;

  // for ( int i = 10; i < 300; ++i )
  // {
  //   plot( { 10,i }, color::PURPLE );
  // }

  // for ( int i = 10; i < 300; ++i )
  // {
  //   plot( { i,60 }, color::TEAL );
  // }

////////////////////////////////////////////////////////////////////////////////////////////////
  //1200 740
  //600 300
  //TRANS FLAG
  {
    int2 begin { 300, 220 };
    int2 end { 700, 520 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::interpolate( 0.5f * ((sinf( app->rotator ) + 1.0f)), color::PURPLE, color::TEAL ) );
      }
  }
  {
    int2 begin { 300, 270 };
    int2 end { 700, 470 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::interpolate( 0.5f * ((sinf( app->rotator ) + 1.0f)), color::BABYPINK, color::HOTPINK ) );
      }
  }
  {
    int2 begin { 300, 320 };
    int2 end { 700, 420 };

    app->rotator += 0.1f;

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        //plot( pos, color::interpolate( 0.5f * ((sinf( app->rotator ) + 1.0f)), color::RED, color::BLUE ) );
        plot( pos, color::WHITE );
      }
  }
  //BLOCK
    // {
    //   int2 begin { 600, 550 };
    //   int2 end { 950, 700 };

    //   app->rotator += 0.1f;

    //   for ( s32 y = begin.y; y < end.y; ++y )
    //     for ( s32 x = begin.x; x < end.x; ++x )
    //     {
    //       int2 pos = { x, y };
    //       plot( pos, color::interpolate( 0.5f * ((sinf( app->rotator ) + 1.0f)), color::BABYPINK, color::HOTPINK ) );
    //     }
    // }

//BACKGROUND
  // {
  //   int2 begin { 0, 0 };
  //   int2 end { windowWidth, windowHeight };


  //   for ( s32 y = begin.y; y < end.y; ++y )
  //     for ( s32 x = begin.x; x < end.x; ++x )
  //     {
  //       plot( { x,y }, color::interpolate( float( x ) / float( windowWidth ), color::BABYPINK, color::LIGHTBLUE ) );
  //     }
  // }


  ////////////////////////////////////////////////////////////////////////////////////
  //RED CIRCLE HALF 4X
  // {
  //   int2 pos { 200, 200 };
  //   s32 radius = 50;
  //   s32 begin = -radius;
  //   s32 end =  radius;

  //   for ( s32 y = begin; y < begin / end; ++y )
  //     for ( s32 x = begin; x < end; ++x )
  //     {
  //       if ( x * x + y * y < radius * radius )
  //       {
  //         int2 tmpPos = pos + int2 { x, y };
  //         plot( tmpPos, color::RED );
  //       }
  //     }
  // }
  // {
  //   int2 pos { 200, 350 };
  //   s32 radius = 50;
  //   s32 begin = -radius;
  //   s32 end =  radius;

  //   for ( s32 y = begin; y < end / end; ++y )
  //     for ( s32 x = begin; x < end; ++x )
  //     {
  //       if ( x * x + y * y < radius * radius )
  //       {
  //         int2 tmpPos = pos - int2 { x, y };
  //         plot( tmpPos, color::RED );
  //       }
  //     }
  // }
  // {
  //   int2 pos { 200, 500 };
  //   s32 radius = 50;
  //   s32 begin = -radius;
  //   s32 end =  radius;

  //   for ( s32 y = begin; y < end; ++y )
  //     for ( s32 x = begin; x < end / end; ++x )
  //     {
  //       if ( x * x + y * y < radius * radius )
  //       {
  //         int2 tmpPos = pos + int2 { x, y };
  //         plot( tmpPos, color::RED );
  //       }
  //     }
  // }
  // {
  //   int2 pos { 200, 650 };
  //   s32 radius = 50;
  //   s32 begin = -radius;
  //   s32 end =  radius;

  //   for ( s32 y = begin; y < end; ++y )
  //     for ( s32 x = begin; x < end / end; ++x )
  //     {
  //       if ( x * x + y * y < radius * radius )
  //       {
  //         int2 tmpPos = pos - int2 { x, y };
  //         plot( tmpPos, color::RED );
  //       }
  //     }
  // }
  ///////////////////////////////////////////////////////////////////////////////////////
  //POKE BALL RAW
  {
    int2 pos { 850, 350 };
    s32 radius = 100;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos + int2 { x, y };
          plot( tmpPos, color::interpolate( float( x + radius ) / float( 2.0f * radius ), color::PINK, color::LIGHTBLUE ) );
        }
      }
  }

  {
    int2 pos { 850, 350 };
    s32 radius = 100;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end / end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos - int2 { x, y };
          plot( tmpPos, color::WHITE );
        }
      }
  }

  {
    int2 begin { 750, 340 };
    int2 end { 950, 360 };

    for ( s32 y = begin.y; y < end.y; ++y )
      for ( s32 x = begin.x; x < end.x; ++x )
      {
        int2 pos = { x, y };
        plot( pos, color::BLACK );
      }
  }

  {
    int2 pos { 850, 350 };
    s32 radius = 30;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos - int2 { x, y };
          plot( tmpPos, color::BLACK );
        }
      }
  }

  {
    int2 pos { 850, 350 };
    s32 radius = 20;
    s32 begin = -radius;
    s32 end =  radius;

    for ( s32 y = begin; y < end; ++y )
      for ( s32 x = begin; x < end; ++x )
      {
        if ( x * x + y * y < radius * radius )
        {
          int2 tmpPos = pos - int2 { x, y };
          plot( tmpPos, color::WHITE );
        }
      }
  }


  ///////////////////////////////////////////////////////////////////////////////////////
//POKEBALL FUNCTION
  {
    s32 radius = 60;
    int2 pos = { 780,600 };
    draw_pokeball( radius, pos );
  }
  {
    s32 radius = 80;
    int2 pos = { 700, 150 };
    draw_pokeball( radius, pos );
  }
  {
    s32 radius = 150;
    int2 pos = { 200,550 };
    draw_pokeball( radius, pos );
  }
  ///////////////////////////////////////////////////////////////////////////////////////
//ELIPSE
  // {
  //   int2 pos { 150, 350 };
  //   s32 radius = 20;
  //   s32 begin = -radius;
  //   s32 end =  radius;

  //   for ( s32 y = begin; y < end; ++y )
  //     for ( s32 x = begin; x < end; ++x )
  //     {
  //       if ( x * x + y * y < radius * radius )
  //       {
  //         int2 tmpPos = pos - int2 { x, y };
  //         plot( tmpPos, color::WHITE );
  //       }
  //     }
  // }

  // {
  // s32 begin = -(s32) radius;
  // s32 end = (s32) radius;

  // for ( s32 y = begin; y < end; ++y )
  //   for ( s32 x = begin; x < end; ++x )
  //   {
  //     if ( x * x + y * y < radius * radius )
  //     {
  //       int2 tmpPos = pos + int2 { x, y };
  //       plot( tmpPos, color );
  //     }
  //   }
  // }
  ///////////////////////////////////////////////////////////////////////////////////////
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


