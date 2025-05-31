#include "AttacksInitializer.h"
#include <optional>

#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../../../SDK/Events/EventManager.h"

#include "../../../../Utils/Log/Log.h"



std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );


std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );


template <typename T>
T * AttacksInitializer::generateAttack( std::string animationName , CBaseAttackConstructor attackBuilder )
{
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
			CBaseEntityAnimationType::ATTACKING_FORWARD ,
	};

	std::optional<CBaseEntityAnimationConstructor> animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation.has_value( ) ) {
		return nullptr;
	}

	animation.value( ).currentAnimationType = ATTACKING_FORWARD;

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::ATTACK;
	builder.health = 100;
	builder.movementSpeed = 10;
	builder.Name = animationName;

	T * result = new T( builder , attackBuilder );

	return result;
}

bool generateAttackEvents( std::string attackName ) {
	std::string eventName = attackName + "_attackThrow";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ eventName ] ( ) {
			gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
		}
	) );
	eventName = attackName + "_attackLoad";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ eventName ] ( ) {
			gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
		}
	) );
	eventName = attackName + "_attackHit";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ eventName ] ( ) {
			gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
		}
	) );

	return true;
}

template <typename T>
bool AttacksInitializer::generateMobAttack( CBaseAttackConstructor attackBuilder , std::string mobName ) {
	std::string attackName = mobName + "_" + attackBuilder.Name;
	attackBuilder.Name = attackName;

	T * raw = generateAttack<T>( attackName , attackBuilder );
	if ( raw == nullptr ) {
		Log::Print( "[generateMobAttack] Failed to generate %s" , attackName.c_str( ) );
		return false;
	}
	std::shared_ptr<T> newAttack( raw );

	attackHandler::Get( ).addAvailableEnemyAttack( mobName, newAttack );
	Log::Print( "[generateMobAttack] Generated $s" , attackName.c_str( ) );

	if(!generateAttackEvents( attackName )) {
		Log::Print( "[generateMobAttack] Failed to generate attack events for %s" , attackName.c_str( ) );
		return false;
	}

	return true;
}

template <typename T>
bool AttacksInitializer::generateLocalPlayerAttack( CBaseAttackConstructor attackBuilder ) {
	std::string attackName = "localPlayer_" + attackBuilder.Name;
	attackBuilder.Name = attackName;

	T * raw = generateAttack<T>( attackName , attackBuilder );
	if ( raw == nullptr ) {
		Log::Print( "[generateLocalPlayerAttack] Failed to generate %s" , attackName.c_str( ) );
		return false;
	}
	std::shared_ptr<T> newAttack( raw );

	attackHandler::Get( ).addAvailableLocalPlayerAttack( newAttack );
	Log::Print( "[generateLocalPlayerAttack] Generated $s" , attackName.c_str( ) );

	if ( !generateAttackEvents( attackName ) ) {
		Log::Print( "[generateLocalPlayerAttack] Failed to generate attack events for %s" , attackName.c_str( ) );
		return false;
	}

	return true;
}


bool AttacksInitializer::generateLocalPlayerAttacks( ) {

	CBaseAttackConstructor attackBuilder;
	{
		attackBuilder.damage = 20;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 0.2f;
		attackBuilder.range = 30.f;
		attackBuilder.speed = 30;
		attackBuilder.type = CBaseAttackType_Melee;
		attackBuilder.Name = "MeleeAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 5 , 5 );

		if ( !generateLocalPlayerAttack<CMeleeAttack>( attackBuilder ) )
		{
			Log::Print( "[generateLocalPlayerAttacks] Failed to generate MeleeAttack" );
			return false;
		}
	}

	{
		attackBuilder.damage = 100;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 5.0f;
		attackBuilder.range = 1000.f;
		attackBuilder.speed = 200;
		attackBuilder.type = CBaseAttackType_Ranged;
		attackBuilder.Name = "MagicAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 10 , 10 );

		if ( !generateLocalPlayerAttack<CRangedAttack>( attackBuilder ) ) {
			Log::Print( "[generateLocalPlayerAttacks] Failed to generate RangedAttack" );
			return false;
		}
	}

	return true;
}

bool AttacksInitializer::generateEnemiesAttacks( ) {

	CBaseAttackConstructor attackBuilder;
	{
		attackBuilder.damage = 10;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 1.0f;
		attackBuilder.range = 30.f;
		attackBuilder.speed = 10;
		attackBuilder.type = CBaseAttackType_Melee;
		attackBuilder.Name = "MeleeAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 5 , 5 );

		if ( !generateMobAttack<CMeleeAttack>(  attackBuilder, "BasicEnemy" ) )
		{
			Log::Print( "[generateEnemiesAttacks] Failed to generate MeleeAttack" );
			return false;
		}
	}

	{
		attackBuilder.damage = 30;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 1.0f;
		attackBuilder.range = 60.f;
		attackBuilder.speed = 20;
		attackBuilder.type = CBaseAttackType_Melee;
		attackBuilder.Name = "MeleeAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 5 , 5 );

		if ( !generateMobAttack<CMeleeAttack>( attackBuilder , "MeleeEnemy1" ) )
		{
			Log::Print( "[generateEnemiesAttacks] Failed to generate MeleeAttack of MeleeEnemy1" );
			return false;
		}
	}

	return true;
}


bool AttacksInitializer::initializeAttacks( ) {

	if ( !generateLocalPlayerAttacks( ) ) {
		Log::Print( "[initializeAttacks] Failed to generate localPlayerAttacks" );
		return false;
	}

	if ( !generateEnemiesAttacks( ) ) {
		Log::Print( "[initializeAttacks] Failed to generate EnemiesAttacks" );
		return false;
	}

	return true;
}