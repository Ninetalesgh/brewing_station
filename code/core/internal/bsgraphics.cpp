

#include <platform/platform_callbacks.h>
//#include <scene/bssceneobject.h>
#include <core/bsgraphics.h>
#include <core/bsmemory.h>


namespace bs
{
  namespace graphics
  {

    RenderObject* create_render_object( MeshData const* meshData, TextureData const* diffuseTextureData, TextureData const* normalMapData )
    {
      RenderObject* resultObject = (RenderObject*) memory::allocate_to_zero( sizeof( RenderObject ) );

      resultObject->mesh = platform::allocate_mesh( meshData );

      if ( diffuseTextureData )
      {
        resultObject->diffuseTexture = platform::allocate_texture( diffuseTextureData );
      }
      if ( normalMapData )
      {
        resultObject->normalMap = platform::allocate_texture( normalMapData );
      }

      return resultObject;
    }
  };
};