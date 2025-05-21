#include "entitiesHandler.h"


void entitiesHandler::setLocalPlayer( CPlayerEntity* local) {
	this->localPlayer = local;
}

CPlayerEntity * entitiesHandler::getLocalPlayer( ) {
	return this->localPlayer;
}