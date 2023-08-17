#include "first_app.h"

float deltaTime = 1.0f / 60.0f;

int2 circlePos;
float2 circleSpeed;
float circleRadius;
void start()
{
  circlePos = { windowWidth / 2, windowHeight / 2 + 50 };
  circleSpeed = { 0, 0 };
  circleRadius = 50.f;
}

float gravity = 9.8f;

void update()
{
  using namespace bs::input;

  s32 speed = 5;
  if ( is_key_held( KEY_LEFT ) )
  {
    circlePos.x -= speed;
  }
  else if ( is_key_held( KEY_RIGHT ) )
  {
    circlePos.x += speed;
  }

  if ( is_key_held( KEY_UP ) )
  {
    circlePos.y -= speed;
  }
  else if ( is_key_held( KEY_DOWN ) )
  {
    circlePos.y += speed;
  }

  circlePos += {(s32) circleSpeed.x, (s32) circleSpeed.y};
  circleSpeed.y += gravity * deltaTime;

  if ( circlePos.y + circleRadius > windowHeight )
  {
    circleSpeed.y *= -1.0f;
  }

  int2 begin = { 20,20 };
  int2 end = { 500,400 };
  draw_rect( begin, end, color::GREEN );

  begin = { 400, 30 };
  end = { 600, 100 };
  draw_rect( begin, end, color::RED );

  draw_circle( circlePos, circleRadius, color::BLUE );
  //log_info( "hello, deltaTime was: ", deltaTime );
}



namespace bs
{
  void app_on_load( bsp::AppData* appData, bsp::PlatformCallbacks* platform )
  {
    if ( appData->userData == nullptr )
    {
      appData->userData = (AppUserData*) allocate( platform->default.allocator, sizeof( AppUserData ) );
      app = (AppUserData*) appData->userData;
      app->bmp.pixel = (u32*) allocate( platform->default.allocator, sizeof( u32 ) * windowWidth * windowHeight );
      app->bmp.height = windowHeight;
      app->bmp.width = windowWidth;
    }

    app = (AppUserData*) appData->userData;
    inputPtr = &appData->input;

    start();
  }

  void app_tick( bsp::AppData* appData, bsp::PlatformCallbacks* platform )
  {
    update();
    // graphics::RenderGroup rg = graphics::render_group_from_custom_bitmap( &app->bmp );
  //  platform::render( nullptr, &rg, nullptr );
    clear( color::BLACK );
  }
};