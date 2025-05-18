#pragma once
class Globals
{
public:
	int screenWidth = 1920;
	int screenHeight = 1080;


	bool exitGame = false;
	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	void * resourceManagerPointer = nullptr;
};

extern Globals globals;

