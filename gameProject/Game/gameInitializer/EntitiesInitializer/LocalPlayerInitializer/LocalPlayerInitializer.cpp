#include "LocalPlayerInitializer.h"


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
		CBaseEntityAnimationType::ATTACKING_RIGHT
	};

	std::optional<CBaseEntityAnimationConstructor> animation = createEntityAnimationConstructor( "localPlayer", requiredAnimations );
	if ( !animation ) {
		return nullptr;
	}

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::PLAYER;
	builder.health = 100;
	builder.movementSpeed = 10;
	builder.Name = "Player";

	return new CPlayerEntity( builder );
}