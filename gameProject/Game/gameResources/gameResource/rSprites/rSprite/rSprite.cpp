#include "rSprite.h"
#include "../../../Utils/Log/Log.h"

#include <SFML/Graphics.hpp>

rSprite::rSprite( std::string model )
    : modelPath( model ) , texture( nullptr ) {
    // Lazy loading opcional: 
     
    initializeTexture();
}

rSprite::~rSprite( ) {
    if ( texture ) {
		delete static_cast< sf::Texture * >( texture );
		this->texture = nullptr;
    }
}

void rSprite::initializeTexture( ) {
    if ( !texture ) {
        sf::Texture * tex = new sf::Texture( );
        if ( !tex->loadFromFile( modelPath )) {
			Log::Print( "failed to load %s" , modelPath.c_str( ) );
			throw std::runtime_error( "failed to load texture" );
            return;
        }
        texture = static_cast< void * >( tex );
		this->spriteSize.x = tex->getSize().x;
		this->spriteSize.y = tex->getSize().y;
    }
}

void * rSprite::getTexture( ) {
    if ( !texture ) {
        initializeTexture( );
    }
    return texture;
}
