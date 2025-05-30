#pragma once
#include <string>
#include <memory>
#include "../../../Utils/singleton.h"
#include "rSpritesManager/rSpritesManager.h"
#include "rSoundsManager/rSoundsManager.h"
#include "rMusicManager/rMusicManager.h"
#include "rShadersManager/rShadersManager.h"
#include "rMapManager/rMapManager.h"

class gameResourceManager
{
	std::string basePath;
	std::unique_ptr<rSpritesManager> spritesMananger;
	std::unique_ptr<rSoundsManager> soundsMananger;
	std::unique_ptr <rMusicManager> musicMananger;
	std::unique_ptr <rShadersManager> shadersManager;
	std::unique_ptr <rMapManager> mapManagerManager;

public:

	bool initialize( std::string path );
	rSpritesManager * getSpritesManager( );
	rSoundsManager * getSoundManager( );;
	rMusicManager * getMusicManager( );
	rShadersManager * getShadersManager( );
	rMapManager * getMapManager( );
};

extern gameResourceManager _gameResourceManager;
