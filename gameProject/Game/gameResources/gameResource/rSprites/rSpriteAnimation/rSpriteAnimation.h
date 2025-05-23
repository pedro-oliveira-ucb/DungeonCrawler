#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "../rSprite/rSprite.h"

enum SPRITE_TYPE {
    STYLESHEET ,
    SINGULAR
};

class rSpriteAnimation
{
	std::mutex rSpriteAnimationMutex;
	std::vector<std::shared_ptr<rSprite>> sprites;
public:
	rSpriteAnimation( std::vector<std::shared_ptr<rSprite>> frames , int fps  );

	std::shared_ptr<rSprite> getFrame( int idx );
	int size( );
};

