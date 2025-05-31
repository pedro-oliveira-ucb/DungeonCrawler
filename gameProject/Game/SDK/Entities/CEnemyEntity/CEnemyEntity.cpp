#include "CEnemyEntity.h"

#include "../../Events/EventManager.h"
#include "../CPlayerEntity/CPlayerEntity.h"
#include "../../math/math.h"

#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Managers/collisionManager/collisionManager.h"

#include "../../../../Utils/Log/Log.h"


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



GVector2D CEnemyEntity::findBestDirectionToPlayer( CBaseEntity * entity , GVector2D & toPlayer ) {
	GVector2D dirs[ ] = {
		{ math::sign( toPlayer.x ),  math::sign( toPlayer.y ) },                // Diagonal principal
		{  math::sign( toPlayer.x ), 0 }, { 0,  math::sign( toPlayer.y ) },      // Eixos principais
		{  math::sign( toPlayer.x ),  -math::sign( toPlayer.y ) },               // Diagonal invertida
		{ -math::sign( toPlayer.x ),  math::sign( toPlayer.y ) },               // Outra diagonal invertida
		{  -math::sign( toPlayer.x ), 0 }, { 0, -math::sign( toPlayer.y ) }     // Direções opostas
	};

	for ( const auto & dir : dirs ) {
		GVector2D newPos = entity->getEntityPosition( ) + dir * entity->getMovementSpeed( );
		if ( CollisionManager::Get( ).CanMoveTo( entity , newPos ) ) {
			return dir;
		}
	}

	return { 0, 0 }; // Sem direção válida
}


