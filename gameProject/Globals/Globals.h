#pragma once
#include <chrono>
#include <mutex>
#include "gameSettings/gameSettings.h"
#include "currentGame/currentGame.h"
#include "playerStats/playerStats.h"

#include "../Utils/singleton.h"


class Globals : public CSingleton<Globals>
{
	mutable std::mutex globalsMutex;

	gameSettings settings;
	currentGame game;
	playerStats stats;
	void * defaultFont = nullptr;
	float frameTime;
public:

	int screenWidth = 1920;
	int screenHeight = 1080;

	gameSettings * getGameSettings( );
	currentGame * getGame( );
	playerStats * getPlayerStats( );
	float getFrameTime( ) const;
	void setFrameTime( float frameTime );

	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	float mousePosX;
	float mousePosY;

	float mousePosWorldX;
	float mousePosWorldY;

	void * getDefaultFont( );
	void setDefaultFont( void * font );
};


