#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../../rBaseResource/rBaseResource.h"
#include "../../gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"

class rSpritesManager : public rBaseResource
{
	std::unordered_map<std::string, rSpriteAnimation> spriteAnimations;

public:
	rSpritesManager( std::string path ) :rBaseResource( path ) { }
	bool initialize( );
	std::unordered_map<std::string , rSpriteAnimation> * getSpriteAnimations( );
};

