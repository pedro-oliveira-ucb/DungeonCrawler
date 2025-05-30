#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../../../gameResources/rBaseResource/rBaseResource.h"
#include "../../../gameResources/gameResource/rSprites/rSprite/rSprite.h"

class rMapManager : public rBaseResource
{
	std::unordered_map<std::string, std::shared_ptr<rSprite>> availableImages;
	void loadImage( const std::string & name );
public:
	rMapManager( std::string path ) :rBaseResource( path ) { }
	bool initialize( ) override;
	std::shared_ptr<rSprite> getMapAsset( const std::string & name ) const;
};

