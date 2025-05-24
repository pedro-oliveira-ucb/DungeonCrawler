#pragma once
#include <string>
#include <memory>
#include "../../../Utils/singleton.h"
#include "rSpritesManager/rSpritesManager.h"
#include "rSoundsManager/rSoundsManager.h"

class gameResourceManager
{
	
	std::string basePath;
	std::unique_ptr<rSpritesManager> spritesMananger;
	std::unique_ptr<rSoundsManager> soundsMananger;

public:

	bool initialize( std::string path );
	rSpritesManager * getSpritesManager( );
	rSoundsManager * getSoundManager( );;

};

extern gameResourceManager _gameResourceManager;
