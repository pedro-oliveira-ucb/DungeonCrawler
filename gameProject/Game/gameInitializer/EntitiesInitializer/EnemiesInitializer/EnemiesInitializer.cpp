#include "EnemiesInitializer.h"

#include <optional>

#include "../../../../Utils/Log/Log.h"

#include "../../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"
#include "../../../SDK/Events/EventManager.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Handlers/itemsHandler/itemsHandler.h"
#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../../../Utils/utils.h"

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );

std::optional<CBaseEntityAnimationConstructor> createEntityAnimationConstructor(
	std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations
);

std::unique_ptr<CEnemyEntity> generateEnemy( CBaseEntityConstructor & builder , const std::string & animationName , CEnemyType enemyType )
{
	// Exemplo de lista de animações para o inimigo
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		 CBaseEntityAnimationType::IDLE_FORWARD ,
		 CBaseEntityAnimationType::IDLE_BACKWARD ,
		 CBaseEntityAnimationType::IDLE_LEFT ,
		 CBaseEntityAnimationType::IDLE_RIGHT ,
		 CBaseEntityAnimationType::WALKING_FORWARD ,
		 CBaseEntityAnimationType::WALKING_BACKWARD ,
		 CBaseEntityAnimationType::WALKING_LEFT ,
		 CBaseEntityAnimationType::WALKING_RIGHT,
		 CBaseEntityAnimationType::ATTACKING_FORWARD ,
		 CBaseEntityAnimationType::ATTACKING_BACKWARD ,
		 CBaseEntityAnimationType::ATTACKING_LEFT ,
		 CBaseEntityAnimationType::ATTACKING_RIGHT,
		 CBaseEntityAnimationType::HURT_FORWARD ,
		 CBaseEntityAnimationType::HURT_BACKWARD ,
		 CBaseEntityAnimationType::HURT_LEFT ,
		 CBaseEntityAnimationType::HURT_RIGHT,
		 CBaseEntityAnimationType::DEAD_FORWARD ,
		 CBaseEntityAnimationType::DEAD_BACKWARD ,
		 CBaseEntityAnimationType::DEAD_LEFT ,
		 CBaseEntityAnimationType::DEAD_RIGHT
	};

	// Apenas exemplificado, reutilize a mesma lógica de criação utilizada em LocalPlayerInitializer.
	// Caso use 'createEntityAnimationConstructor', aplique o mesmo padrão:
	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) {
		Log::Print( "[Enemy Initializer] %s animation generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	builder.entityAnimations = animation.value( );
	std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks = attackHandler::Get( ).getAvailableEnemyAttack( animationName );

	if ( attacks.empty( ) ) {
		Log::Print( "[Enemy Initializer] not attacks found for %s, generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	// Cria a instância do inimigo
	return std::make_unique<CEnemyEntity>( builder , attacks , enemyType );
}

bool EnemiesInitializer::initializeEvents( std::string enemyName )
{
	std::vector<std::string> eventsNames {
		"hurt"
	};

	for ( std::string event : eventsNames ) {
		std::string eventName = enemyName + "_" + event;
		EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
			eventName ,
			[ eventName ] ( ) {
				gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
			}
		) );
	}

	std::string eventName = enemyName + "_dead";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ eventName ] ( ) {		
			if ( utils::Get().onProbability( 20 ) ) {
				// 20% de chance de spawnar um item de vida
				itemsHandler::Get( ).spawnItem( ItemType::HEALTH_ITEM );
			}
			gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
		}
	) );

	return true;
}

bool EnemiesInitializer::initialize( )
{
	CBaseEntityConstructor builder;
	{	
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.movementSpeed = 30;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.Name = "BasicEnemy";
		auto BasicEnemy = generateEnemy( builder , "BasicEnemy" , CEnemyType::MELEE_ENEMY );
		if ( !BasicEnemy ) {
			Log::Print( "[Enemies Initializer] Failed to create enemy BasicEnemy!" );
			return false;
		}

		if ( !initializeEvents( "BasicEnemy" ) ) {
			Log::Print( "[Enemies Initializer] Failed to initialize enemy BasicEnemy events!" );
			return false;
		}

		entitiesHandler::Get( ).addSpawnableEnemy( CEnemyType::MELEE_ENEMY , std::move( BasicEnemy ) );
	}

	
	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		builder.Name = "MeleeEnemy1";
		auto MeleeEnemy1 = generateEnemy( builder , "MeleeEnemy1" , CEnemyType::MELEE_ENEMY );
		if ( !MeleeEnemy1 ) {
			Log::Print( "[Enemies Initializer] Failed to create enemy MeleeEnemy1!" );
			return false;
		}

		if ( !initializeEvents( "MeleeEnemy1" ) ) {
			Log::Print( "[Enemies Initializer] Failed to initialize enemy MeleeEnemy1 events!" );
			return false;
		}

		entitiesHandler::Get( ).addSpawnableEnemy( CEnemyType::MELEE_ENEMY_MEDIUM , std::move( MeleeEnemy1 ) );
	}



	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		builder.Name = "RangedEnemy1";
		auto RangedEnemy1 = generateEnemy( builder , "RangedEnemy1" , CEnemyType::RANGED_ENEMY );
		if ( !RangedEnemy1 ) {
			Log::Print( "[Enemies Initializer] Failed to create enemy RangedEnemy1!" );
			return false;
		}

		if ( !initializeEvents( "RangedEnemy1" ) ) {
			Log::Print( "[Enemies Initializer] Failed to initialize enemy RangedEnemy1 events!" );
			return false;
		}
		entitiesHandler::Get( ).addSpawnableEnemy( CEnemyType::RANGED_ENEMY , std::move( RangedEnemy1 ) );
	}




	return true;
}