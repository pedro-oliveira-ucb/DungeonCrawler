#include "Globals.h"

gameSettings * Globals::getGameSettings( )
{
	return &this->settings;
}

currentGame * Globals::getGame( )
{
	return &this->game;
}