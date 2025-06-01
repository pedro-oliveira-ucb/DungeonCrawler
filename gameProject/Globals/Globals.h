#pragma once
#include <chrono>
#include "gameSettings/gameSettings.h"
#include "currentGame/currentGame.h"

#include "../Utils/singleton.h"


class Globals : public CSingleton<Globals>
{
	gameSettings settings;
	currentGame game;

public:

	int screenWidth = 1920;
	int screenHeight = 1080;

	gameSettings * getGameSettings( );
	currentGame * getGame( );

	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	float mousePosX;
	float mousePosY;

	float mousePosWorldX;
	float mousePosWorldY;
};


