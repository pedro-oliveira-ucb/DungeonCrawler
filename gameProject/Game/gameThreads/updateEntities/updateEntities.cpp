#include "updateEntities.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../gameObjects/attackHandler/attackHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void updateEntities::threadFunction( ) {

	Log::Print( "[updateEntities] Hello world!" );

	while ( true ) {
		attackHandler::Get( ).updateAttacks( );

		std::vector<CBaseEntity *> allEntities = entitiesHandler::Get( ).getSpawnedEntities( );
		CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );

		if ( localPlayer != nullptr ) {
			allEntities.emplace_back( entitiesHandler::Get( ).getLocalPlayer( ) );
		}
	
		CollisionManager::Get( ).UpdateEntities( allEntities );
		CollisionManager::Get( ).ProcessCollisions( );

		levelManager.updateLevel( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}
}