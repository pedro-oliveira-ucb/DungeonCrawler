#include "CEnemyEntity.h"
#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"
#include "../CPlayerEntity/CPlayerEntity.h"
#include "../../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../../Managers/collisionManager/collisionManager.h"
#include "../../Events/EventManager.h"



CEnemyEntity::CEnemyEntity( CBaseEntityConstructor builder ,
	std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks ,
	CEnemyType enemyType
)
	: CPlayerEntity( builder , attacks )
{
	this->enemyType = enemyType;
}

CEnemyEntity::CEnemyEntity( const CEnemyEntity & other ) :
	CPlayerEntity( other )
{
	this->enemyType = other.enemyType;
}

CEnemyType CEnemyEntity::getEnemyType( ) {
	return this->enemyType;
}

std::shared_ptr<CEnemyEntity> CEnemyEntity::sharedClone( ) {
	std::shared_ptr<CEnemyEntity> clone = std::make_shared<CEnemyEntity>( *this ); // Usa cópia
	return clone;
}

std::unique_ptr<CEnemyEntity> CEnemyEntity::uniqueClone( ) {
	std::unique_ptr<CEnemyEntity> clone = std::make_unique<CEnemyEntity>( *this ); // Usa cópia
	return clone;
}

void CEnemyEntity::initialize( ) {
	this->initializePlayerAttacks( );
	this->initialized = true;
}


int sign( float x ) {
	return ( x > 0 ) - ( x < 0 );
}

GVector2D CEnemyEntity::findBestDirectionToPlayer( CBaseEntity * entity , GVector2D & toPlayer ) {
	GVector2D dirs[ ] = {
		{ sign( toPlayer.x ), sign( toPlayer.y ) },                // Diagonal principal
		{ sign( toPlayer.x ), 0 }, { 0, sign( toPlayer.y ) },      // Eixos principais
		{ sign( toPlayer.x ), -sign( toPlayer.y ) },               // Diagonal invertida
		{ -sign( toPlayer.x ), sign( toPlayer.y ) },               // Outra diagonal invertida
		{ -sign( toPlayer.x ), 0 }, { 0, -sign( toPlayer.y ) }     // Direções opostas
	};

	for ( const auto & dir : dirs ) {
		GVector2D newPos = entity->getEntityPosition( ) + dir * entity->getMovementSpeed( );
		if ( CollisionManager::Get( ).CanMoveTo( entity , newPos ) ) {
			return dir;
		}
	}

	return { 0, 0 }; // Sem direção válida
}


