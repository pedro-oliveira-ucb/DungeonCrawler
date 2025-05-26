#include "enemiesController.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../gameObjects/attackHandler/attackHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void enemiesController::threadFunction( ) {

	Log::Print( "[enemiesController] Hello world!" );

	CBaseEntity * player = entitiesHandler::Get( ).getLocalPlayer( );

	while ( true ) {

		if ( player == nullptr ) {
			player = entitiesHandler::Get( ).getLocalPlayer( );
			continue;
		}

		std::vector<CEnemyEntity *> entities = entitiesHandler::Get( ).getEnemies( );
		GVector2D playerPos = player->getEntityPosition( );


		for ( int i = 0; i < entities.size( ); i++ ) {
			CEnemyEntity * entity =  entities[ i ];

			if ( !entity )
				continue;

			if ( !entity->isAlive( ) )
				continue;

			GVector2D myPos = entity->getEntityPosition( );
			GVector2D toPlayer = playerPos - myPos;
			float angle = atan2( toPlayer.y , toPlayer.x ) * 180.0f / 3.14159265f;
			entity->setLookingAngle( angle );
			float minimumDistanceToAttack = entity->getMinimumDistanceToAttack( );

			if ( toPlayer.length( ) > minimumDistanceToAttack ) {

				GVector2D bestDir = CEnemyEntity::findBestDirectionToPlayer( entity , toPlayer );
				if ( bestDir.x != 0 ) {
					entity->addMoveRequest( bestDir.x > 0 ? MOVEMENT_RIGHT : MOVEMENT_LEFT );
				}

				if ( bestDir.y != 0 ) {
					entity->addMoveRequest( bestDir.y > 0 ? MOVEMENT_BACKWARD : MOVEMENT_FORWARD );
				}
			}
			else {
				std::vector<CBaseAttackType> availableAttacks = entity->getAvailableAttacks( );
				if ( availableAttacks.empty( ) )
					continue;
				entity->UseAttack( availableAttacks.at( 0 ) );
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}
}