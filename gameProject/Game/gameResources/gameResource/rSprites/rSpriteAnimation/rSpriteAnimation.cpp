#include "rSpriteAnimation.h"
#include "../../../Utils/Log/Log.h"

#include <filesystem>
#include <regex>
#include <algorithm>

namespace fs = std::filesystem;

rSpriteAnimation::rSpriteAnimation( std::vector<std::shared_ptr<rSprite>> sprites , int fps ) {
	this->sprites = sprites;
}

int rSpriteAnimation::size( ) {
	std::lock_guard<std::mutex> lock( this->rSpriteAnimationMutex );

	if ( this == nullptr ) {
		return 0;
	}

	if ( this->sprites.empty( ) )
		return 0;

	return static_cast< int >( this->sprites.size( ) );
}

std::shared_ptr<rSprite> rSpriteAnimation::getFrame( int idx )  {
	std::lock_guard<std::mutex> lock( this->rSpriteAnimationMutex );

	if ( this == nullptr ) {
		return nullptr;
	}

	if ( this->sprites.empty( ) )
		return nullptr;

	if ( idx < 0 || idx >= static_cast< int >( this->sprites.size( ) ) )
		return nullptr;

	return sprites[ idx ];
}
