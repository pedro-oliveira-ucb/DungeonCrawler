#include "Globals.h"

gameSettings * Globals::getGameSettings( )
{
	return &this->settings;
}

currentGame * Globals::getGame( )
{
	return &this->game;
}

playerStats * Globals::getPlayerStats( )
{
	return &this->stats;
}

