#include "LocalPlayerInitializer.h"

#include "../../../SDK/Events/EventManager.h"
#include "../../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"

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

	std::optional<CBaseEntityAnimationConstructor> animation = createEntityAnimationConstructor( "localPlayer", requiredAnimations );
	if ( !animation ) {
		return nullptr;
	}

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 500 , 500 );
	builder.entityType = CBaseEntityType::PLAYER;
	builder.health = 100;
	builder.movementSpeed = 10;
	builder.Name = animationName;

	return new CPlayerEntity( builder );
}

bool LocalPlayerInitializer::initializeEvents( ) {
	std::string eventName;

	eventName = "localPlayer_hurt";
	EventManager::Get().RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsQueue.addEventToQueue( "localPlayer_hurt" );
		}
	) );

	eventName = "localPlayer_dead";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsQueue.addEventToQueue( "localPlayer_dead" );
		}
	) );

	eventName = "localPlayer_attack_miss";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsQueue.addEventToQueue( "localPlayer_attack_miss" );
		}
	) );

	eventName = "localPlayer_walking";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ ] ( ) {

			gameSoundsQueue.addEventToQueue( "localPlayer_walking" );
		}
	) );

	return true;
}


bool LocalPlayerInitializer::initialize( ) {
	CPlayerEntity * player = generate( "localPlayer" );
	if ( !player ) {
		return false;
	}
	entitiesHandler::Get().setLocalPlayer( player );

	if ( !this->initializeEvents( ) ) {
		return false;
	}

	return true;
}