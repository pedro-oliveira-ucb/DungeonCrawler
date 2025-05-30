#include "rSprite.h"
#include <raylib/raylib.h>
#include "../../../../Utils/Log/Log.h"

rSprite::rSprite( std::string model )
    : modelPath( model ) , texture( nullptr ) {
    // Lazy loading opcional: 

    initializeTexture( );
}

rSprite::~rSprite( ) {
    if ( texture ) {
        UnloadTexture( *( Texture2D * ) texture );
        delete ( Texture2D * ) texture;
        texture = nullptr;
    }
}

void rSprite::initializeTexture( ) {
    if ( !texture ) {
        Texture2D * tex = new Texture2D( );
        *tex = LoadTexture( modelPath.c_str( ) );
        texture = static_cast< void * >( tex );
        this->spriteSize.x = tex->width;
        this->spriteSize.y = tex->height;
        Log::Print( "Loaded %s, address: 0x%p" , modelPath.c_str( ) , texture );
    }
}

void * rSprite::getTexture( ) {
    if ( !texture ) {
        initializeTexture( );
    }
    return texture;
}