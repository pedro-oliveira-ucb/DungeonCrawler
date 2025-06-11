#pragma once
#include <chrono>
#include "gameSettings/gameSettings.h"
#include "currentGame/currentGame.h"
#include "playerStats/playerStats.h"

#include "../Utils/singleton.h"


class Globals : public CSingleton<Globals>
{
	gameSettings settings;
	currentGame game;
	playerStats stats;
public:

	int screenWidth = 1920;
	int screenHeight = 1080;

	gameSettings * getGameSettings( );
	currentGame * getGame( );
	playerStats * getPlayerStats( );

	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	float mousePosX;
	float mousePosY;

	float mousePosWorldX;
	float mousePosWorldY;
};


