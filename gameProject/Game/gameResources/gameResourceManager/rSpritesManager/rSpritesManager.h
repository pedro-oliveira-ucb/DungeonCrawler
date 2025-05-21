#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../../rBaseResource/rBaseResource.h"
#include "../../gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"

class rSpritesManager : public rBaseResource
{
	std::unordered_map<std::string, std::shared_ptr<rSpriteAnimation>> spriteAnimations;
public:
	rSpritesManager( std::string path ) :rBaseResource( path ) { }
	bool initialize( ) override;

	std::shared_ptr<rSpriteAnimation> loadClip( const std::string & name , SPRITE_TYPE type );
	std::shared_ptr<rSpriteAnimation> getClip( const std::string & name ) const;
};

