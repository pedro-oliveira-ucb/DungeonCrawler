#include "rSprite.h"
#include "../../../Utils/Log/Log.h"

rSprite::rSprite( std::string model ) : modelPath( model ) {
    initializeTexture( );
}

rSprite::~rSprite( ) {
    if ( texture ) {
        UnloadTexture( *texture );
        // unique_ptr será automaticamente deletado
    }
}
void rSprite::initializeTexture( ) {
    // NOTE: Textures MUST be loaded after Window initialization( OpenGL context is required )
    texture = std::make_unique<Texture2D>( );
    *texture = LoadTexture( modelPath.c_str( ) );       // Image converted to texture, GPU memory (RAM -> VRAM)
    Log::Print( "Loaded %s" , modelPath.c_str() );
}

Texture2D * rSprite::getTexture( ) {
    return texture.get();
}