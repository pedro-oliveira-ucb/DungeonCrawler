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

#include "../../../../Globals/Globals.h"

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );

std::optional<CBaseEntityAnimationConstructor> createEntityAnimationConstructor(
	std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations
);

std::unique_ptr<CEnemyEntity> generateEnemy( CBaseEntityConstructor & builder , std::string animationName , CEnemyType enemyType )
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
		 CBaseEntityAnimationType::MAGICSPELLING_FORWARD ,
		 CBaseEntityAnimationType::MAGICSPELLING_BACKWARD ,
		 CBaseEntityAnimationType::MAGICSPELLING_LEFT ,
		 CBaseEntityAnimationType::MAGICSPELLING_RIGHT,
		 CBaseEntityAnimationType::HURT_FORWARD ,
		 CBaseEntityAnimationType::HURT_BACKWARD ,
		 CBaseEntityAnimationType::HURT_LEFT ,
		 CBaseEntityAnimationType::HURT_RIGHT,
		 CBaseEntityAnimationType::DEAD_FORWARD ,
		 CBaseEntityAnimationType::DEAD_BACKWARD ,
		 CBaseEntityAnimationType::DEAD_LEFT ,
		 CBaseEntityAnimationType::DEAD_RIGHT
	};

	std::string entityName = builder.Name;
	animationName = std::string("enemies_") + animationName;


	// Apenas exemplificado, reutilize a mesma lógica de criação utilizada em LocalPlayerInitializer.
	// Caso use 'createEntityAnimationConstructor', aplique o mesmo padrão:
	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) {
		Log::Print( "[Enemy Initializer] %s animation generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	builder.entityAnimations = animation.value( );
	std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks = attackHandler::Get( ).getAvailableEnemyAttack( entityName );

	if ( attacks.empty( ) ) {
		Log::Print( "[Enemy Initializer] not attacks found for %s, generation failed!" , entityName.c_str( ) );
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
		std::string eventName =  enemyName + "_" + event;
		EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
			eventName ,
			[ eventName ] ( ) {
				gameSoundsEventHandler::Get( ).addEventToQueue( "enemies_" + eventName );
			}
		) );
	}
	 
	std::string eventName = enemyName + "_dead";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ eventName ] ( ) {		
			float HealthDropChance = Globals::Get().getPlayerStats()->getHealthDropProbability();

			if ( utils::Get().onProbability( HealthDropChance ) ) {
				itemsHandler::Get( ).spawnItem( ItemType::HEALTH_ITEM );
			}

			float ManaDropChance = Globals::Get( ).getPlayerStats( )->getManaDropProbability( );
			if ( utils::Get( ).onProbability( ManaDropChance ) ) {
				itemsHandler::Get( ).spawnItem( ItemType::MANA_ITEM );
			}

			gameSoundsEventHandler::Get( ).addEventToQueue( "enemies_" + eventName );
		}
	) );

	return true;
}

bool createEnemy( CBaseEntityConstructor builder, std::string name , CEnemyType type ) {

	builder.Name = name;

	auto BasicEnemy = generateEnemy( builder , name , type );
	if ( !BasicEnemy ) {
		Log::Print( "[Enemies Initializer] Failed to create enemy %s!", name.c_str() );
		return false;
	}

	if ( !EnemiesInitializer::initializeEvents( name ) ) {
		Log::Print( "[Enemies Initializer] Failed to initialize enemy %s events!" , name.c_str( ) );
		return false;
	}

	entitiesHandler::Get( ).addSpawnableEnemy( type , std::move( BasicEnemy ) );

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
		if ( !createEnemy( builder , "basicMeleeEnemy" , CEnemyType::MELEE_ENEMY ) )
			return false;
	}
	
	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		if ( !createEnemy( builder , "mediumMeleeEnemy" , CEnemyType::MELEE_ENEMY_MEDIUM ) )
			return false;
	}

	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		if ( !createEnemy( builder , "advancedMeleeEnemy" , CEnemyType::MELEE_ENEMY_ADVANCED ) )
			return false;
	}



	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		if ( !createEnemy( builder , "basicRangeEnemy" , CEnemyType::RANGED_ENEMY ) )
			return false;
	}

	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		if ( !createEnemy( builder , "mediumRangeEnemy" , CEnemyType::RANGED_ENEMY_MEDIUM ) )
			return false;
	}

	{
		builder.entityPosition = GVector2D( 0 , 0 );
		builder.entityType = CBaseEntityType::MOB;
		builder.health = 100;
		builder.entityHitbox = GVector2D( 25 , 25 );
		builder.movementSpeed = 50;
		if ( !createEnemy( builder , "advancedRangeEnemy" , CEnemyType::RANGED_ENEMY_ADVANCED ) )
			return false;
	}


	return true;
}