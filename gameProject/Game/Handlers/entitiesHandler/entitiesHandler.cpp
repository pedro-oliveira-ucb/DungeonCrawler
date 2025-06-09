#include "entitiesHandler.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Globals/Globals.h"

#include "../../../Utils/Log/Log.h"
#include "../../../Utils/utils.h"

void entitiesHandler::setLocalPlayer( CPlayerEntity * player ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	localPlayer = player;
}

CPlayerEntity * entitiesHandler::getLocalPlayer( ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	return localPlayer;
}

void entitiesHandler::addSpawnableEnemy( CEnemyType type , std::unique_ptr<CEnemyEntity> enemy ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnableEnemies.emplace( type , std::move( enemy ) );
}

void entitiesHandler::addSpawnedEnemy( std::unique_ptr<CEnemyEntity> enemy ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnedEnemies.push_back( std::move( enemy ) );
}
std::unordered_map<CEnemyType , std::unique_ptr<CEnemyEntity>> * entitiesHandler::getSpawnableEnemies( ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	return &spawnableEnemies;
}

void entitiesHandler::clearSpawnedEntities( ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnedEnemies.clear( );
}


std::vector<std::unique_ptr<CEnemyEntity>> * entitiesHandler::getSpawnedEnemies( ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	return &spawnedEnemies;
}

void entitiesHandler::addSpawnedEntity( std::unique_ptr<CBaseEntity> * entity ) {
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnedEntities.push_back( std::move( *entity ) );
}

std::vector<std::unique_ptr<CBaseEntity>> * entitiesHandler::getSpawnedEntities( ) {
	std::lock_guard<std::mutex> lock( handlerMutex );

	spawnedEntities.erase(
		std::remove_if(
			spawnedEntities.begin( ) ,
			spawnedEntities.end( ) ,
			[ ] ( const std::unique_ptr<CBaseEntity> & entity ) {
				return entity == nullptr;
			}
		) ,
		spawnedEntities.end( )
	);

	return &spawnedEntities;
}

float calcularDistancia( GVector2D a , GVector2D b ) {
	return std::sqrt( ( b.x - a.x ) * ( b.x - a.x ) + ( b.y - a.y ) * ( b.y - a.y ) );
}

void entitiesHandler::updateSpawnedEnemies( int index , bool followPlayer ) {
	std::lock_guard<std::mutex> lock( handlerMutex );

	if ( index < 0 || index >= spawnedEnemies.size( ) ) {
		Log::Print( "Invalid index for spawned enemies update." );
		return;
	}

	CEnemyEntity * entity = spawnedEnemies.at( index ).get( );

	GVector2D playerPos = localPlayer->getEntityPosition( );

	if ( !entity )
		return;

	if ( !entity->isInitialized( ) )
		entity->initialize( );

	GVector2D myPos = entity->getEntityPosition( );
	GVector2D toPlayer = playerPos - myPos;
	int EntityRoom = gameMap::Get( ).getRoomIdAtPosition( myPos );

	float angle = atan2( toPlayer.y , toPlayer.x ) * 180.0f / 3.14159265f;
	float distance = calcularDistancia( myPos , playerPos );
	entity->setLookingAngle( angle );

	if ( localPlayer->isAlive( ) && followPlayer ) {
		float minimumDistanceToAttack = 0.0f;
		entity->getMinimumDistanceToAttack( &minimumDistanceToAttack );

		if ( distance > minimumDistanceToAttack + 10 ) {
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
			if ( !availableAttacks.empty( ) ) {
				entity->UseAttack( availableAttacks.at( 0 ) );
			}
		}
	}

	entity->updateEntity( );
}

struct movementUpdater {
	keybind_identifier key;
	CBaseEntityMovementDirection movementDirection;
};

double calcularAnguloRad( GVector2D origem , GVector2D destino ) {
	GVector2D dAngle = destino - origem;
	return atan2( dAngle.y , dAngle.x );
}

double radParaGraus( double rad ) {
	return rad * 180.0 / M_PI;
}

std::vector<movementUpdater> data = {
	{ MOVE_FORWARD,   MOVEMENT_FORWARD },
	{ MOVE_BACKWARD,  MOVEMENT_BACKWARD },
	{ MOVE_RIGHT,     MOVEMENT_RIGHT },
	{ MOVE_LEFT,      MOVEMENT_LEFT }
};

void entitiesHandler::updateLocalPlayer( ) {

	std::lock_guard<std::mutex> lock( handlerMutex );

	if ( keybindHandler::Get( ).isPressed( keybind_identifier::SPRINT ) ) {
		localPlayer->setSprinting( true );
	}
	else
		localPlayer->setSprinting( false );

	GVector2D localPos = localPlayer->getEntityPosition( );
	GVector2D mouseWorldPos = { Globals::Get( ).mousePosWorldX, Globals::Get( ).mousePosWorldY };
	// Calcula ângulo com base nas posições no mundo
	float newLookingAngle = radParaGraus( calcularAnguloRad( localPos , mouseWorldPos ) );

	// Ataques do jogador
	if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_SIMPLE ) ) {
		localPlayer->UseAttack( CBaseAttackType_Melee );
	}
	if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_HEAVY ) ) {
		localPlayer->UseAttack( CBaseAttackType_Ranged );
	}
	localPlayer->setLookingAngle( newLookingAngle );

	// Movimento do player
	for ( auto move : data ) {
		if ( keybindHandler::Get( ).isPressed( move.key ) ) {
			localPlayer->addMoveRequest( move.movementDirection );
		}
	}
	// Atualiza player
	localPlayer->updateEntity( );

	Globals::Get( ).getGame( )->setCurrentLocalPlayerPosition( localPos );
}

GVector2D entitiesHandler::getRandomPlaceAroundPlayer( float radius )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	GVector2D playerPos = localPlayer->getEntityPosition( );
	int currentLocalRoom = gameMap::Get( ).getRoomIdAtPosition( playerPos );

	if ( !currentLocalRoom ) {
		Log::Print( "No room found for player position" );
		return GVector2D( 0 , 0 );
	}

	bool okayPlace = false;
	GVector2D deriserdSpawnPlace = gameMap::Get( ).getRandomPlace( currentLocalRoom );
	do {
		deriserdSpawnPlace = gameMap::Get( ).getRandomPlace( currentLocalRoom );

		if ( CollisionManager::Get( ).isSpaceFree( deriserdSpawnPlace , GVector2D( 150 , 150 ) ) ) {
			okayPlace = true;
		}
	} while ( !okayPlace );

	return deriserdSpawnPlace;
}

void entitiesHandler::updateEnemiesCollision( )
{
	std::lock_guard<std::mutex> lock( handlerMutex );

	CPlayerEntity * localPlayer = this->localPlayer;

	std::vector< CBaseEntity *> allEnemies { localPlayer };

	for ( int i = 0; i < spawnedEnemies.size( ); i++ ) {
		CBaseEntity * entity = spawnedEnemies.at( i ).get( );
		allEnemies.emplace_back( entity );
	}

	CollisionManager::Get( ).UpdateEntities( allEnemies );
	//CollisionManager::Get( ).ProcessCollisions( );
}
