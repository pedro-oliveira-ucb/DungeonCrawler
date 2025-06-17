#include "LocalPlayerInitializer.h"

#include "../../../SDK/Events/EventManager.h"
#include "../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include <optional>

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );


std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );

CPlayerEntity * LocalPlayerInitializer::generate( std::string animationName ) {

	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		CBaseEntityAnimationType::IDLE_FORWARD ,
		CBaseEntityAnimationType::IDLE_BACKWARD ,
		CBaseEntityAnimationType::IDLE_LEFT ,
		CBaseEntityAnimationType::IDLE_RIGHT ,

		// Walking
		CBaseEntityAnimationType::WALKING_FORWARD ,
		CBaseEntityAnimationType::WALKING_BACKWARD ,
		CBaseEntityAnimationType::WALKING_LEFT ,
		CBaseEntityAnimationType::WALKING_RIGHT ,

		// Running
		CBaseEntityAnimationType::RUNNING_FORWARD ,
		CBaseEntityAnimationType::RUNNING_BACKWARD ,
		CBaseEntityAnimationType::RUNNING_LEFT ,
		CBaseEntityAnimationType::RUNNING_RIGHT ,

		// Attacking (stationary)
		CBaseEntityAnimationType::ATTACKING_FORWARD ,
		CBaseEntityAnimationType::ATTACKING_BACKWARD ,
		CBaseEntityAnimationType::ATTACKING_LEFT ,
		CBaseEntityAnimationType::ATTACKING_RIGHT ,

		// Attacking while walking
		CBaseEntityAnimationType::ATTACKING_WALKING_FORWARD ,
		CBaseEntityAnimationType::ATTACKING_WALKING_BACKWARD ,
		CBaseEntityAnimationType::ATTACKING_WALKING_LEFT ,
		CBaseEntityAnimationType::ATTACKING_WALKING_RIGHT ,

		// Attacking while running
		CBaseEntityAnimationType::ATTACKING_RUNNING_FORWARD ,
		CBaseEntityAnimationType::ATTACKING_RUNNING_BACKWARD ,
		CBaseEntityAnimationType::ATTACKING_RUNNING_LEFT ,
		CBaseEntityAnimationType::ATTACKING_RUNNING_RIGHT ,

		// Hurt
		CBaseEntityAnimationType::HURT_FORWARD ,
		CBaseEntityAnimationType::HURT_BACKWARD ,
		CBaseEntityAnimationType::HURT_LEFT ,
		CBaseEntityAnimationType::HURT_RIGHT ,

		// Dead
		CBaseEntityAnimationType::DEAD_FORWARD ,
		CBaseEntityAnimationType::DEAD_BACKWARD ,
		CBaseEntityAnimationType::DEAD_LEFT ,
		CBaseEntityAnimationType::DEAD_RIGHT
	};

	std::optional<CBaseEntityAnimationConstructor> animation = createEntityAnimationConstructor( "localPlayer" , requiredAnimations );
	if ( !animation.has_value( ) || !animation ) {
		return nullptr;
	}

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 500 , 500 );
	builder.entityType = CBaseEntityType::PLAYER;
	builder.health = 100;
	builder.movementSpeed = 100;
	builder.entityHitbox = GVector2D( 25 , 25 );
	builder.Name = animationName;

	std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> * attacks = attackHandler::Get( ).getAvailableLocalPlayerAttack( );

	return new CPlayerEntity( builder, *attacks );
}

bool LocalPlayerInitializer::initializeEvents( ) {
	std::string eventName;

	eventName = "localPlayer_hurt";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsEventHandler::Get( ).addEventToQueue( "localPlayer_hurt" );
		}
	) );

	eventName = "localPlayer_dead";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsEventHandler::Get( ).addEventToQueue( "localPlayer_dead" );
		}
	) );

	eventName = "localPlayer_attack_miss";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsEventHandler::Get( ).addEventToQueue( "localPlayer_attack_miss" );
		}
	) );

	eventName = "localPlayer_walking";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsEventHandler::Get( ).addEventToQueue( "localPlayer_walking" );
		}
	) );

	return true;
}


bool LocalPlayerInitializer::initialize( ) {
	CPlayerEntity * player = generate( "localPlayer" );
	if ( !player ) {
		return false;
	}
	entitiesHandler::Get( ).setLocalPlayer( player );

	if ( !this->initializeEvents( ) ) {
		return false;
	}

	return true;
}