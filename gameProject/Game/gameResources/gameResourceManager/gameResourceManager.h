#pragma once
#include <string>
#include <memory>
#include "../../../Utils/singleton.h"
#include "rSpritesManager/rSpritesManager.h"


class gameResourceManager
{
	std::string basePath;
	std::unique_ptr<rSpritesManager> spritesMananger;

public:

	bool initialize( std::string path );
	rSpritesManager &getSpritesManager( );

};

extern gameResourceManager _gameResourceManager;
