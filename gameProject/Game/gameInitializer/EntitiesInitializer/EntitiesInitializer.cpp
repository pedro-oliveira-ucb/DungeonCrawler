#include "EntitiesInitializer.h"
#include "../../World/World.h"
#include "../../gameWorld/gameWorld.h"
#include "../../gameResources/gameResourceManager/gameResourceManager.h"

#include "LocalPlayerInitializer/LocalPlayerInitializer.h"

#include "../../../Utils/Log/Log.h"



bool EntitiesInitializer::initialize( ) {

	CPlayerEntity * localPlayer = LocalPlayerInitializer::Get( ).generate( "localPlayer" );
	if ( !localPlayer ) {
		Log::Print( "[EntitiesInitializer] Failed to generate localplayer" );
		return false;
	}
	_gameWorld.localplayer = localPlayer;
	printf( "LocalPlayer name: %s\n" , _gameWorld.localplayer->GetEntityName().c_str());

	return true;
}