#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../../../gameResources/rBaseResource/rBaseResource.h"
#include "../../../gameResources/gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"

class rSpritesManager : public rBaseResource
{
	std::unordered_map<std::string, std::vector<std::shared_ptr<rSprite>>> availableSprites;
	std::unordered_map<std::string, std::shared_ptr<rSpriteAnimation>> spriteAnimations;
public:
	rSpritesManager( std::string path ) :rBaseResource( path ) { }
	bool initialize( ) override;

	std::vector<std::shared_ptr<rSprite>> * getSprite( const std::string & name );

	std::shared_ptr<rSpriteAnimation> getClip( const std::string & name ) const;
};

