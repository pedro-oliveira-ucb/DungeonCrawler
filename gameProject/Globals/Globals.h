#pragma once
class Globals
{
public:
	bool exitGame = false;
	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	void * resourceManagerPointer = nullptr;
};

extern Globals globals;

