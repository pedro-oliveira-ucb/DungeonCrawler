#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "../../../gameResources/gameResource/rSound/rSound.h"
#include "../../../gameResources/rBaseResource/rBaseResource.h"


class rSoundsManager : public rBaseResource
{
	std::mutex mtx;

	void loadSound( std::string name );

	std::unordered_map<std::string , std::unique_ptr<rSound>> sounds;

public:
	rSoundsManager( std::string path ) :rBaseResource( path ) { }

	bool initialize( ) override;
	bool playSound( std::string soundName );
};

