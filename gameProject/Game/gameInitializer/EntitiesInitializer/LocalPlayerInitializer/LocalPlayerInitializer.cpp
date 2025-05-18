#include <optional>

#include "LocalPlayerInitializer.h"

#include "../../../World/World.h"
#include "../../../gameWorld/gameWorld.h"
#include "../../../gameResources/gameResourceManager/gameResourceManager.h"

#include "../../../Utils/Log/Log.h"


std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>> generateLocalPlayerAnimationPair( CBaseEntityAnimationType type , std::string Name ) {
	if ( _gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->find( Name ) !=
		_gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->end( ) )
	{
		Log::Print( "[LocalPlayerInitializer] Found %s animation" , Name.c_str() );
		rSpriteAnimation * anim = &_gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->at( Name );

		return std::make_pair(
			type ,
			anim
		);
	}

	Log::Print( "[LocalPlayerInitializer] Can't Found %s animation" , Name.c_str() );

	return std::nullopt;
}


std::optional < CBaseEntityAnimationConstructor > createLocalPlayerAnimationEntity( std::string animationName ) {
	CBaseEntityAnimationConstructor builder;


	std::unordered_map<CBaseEntityAnimationType , rSpriteAnimation * > animations;

	std::vector<CBaseEntityAnimationType> data = {
		CBaseEntityAnimationType::IDLE_FORWARD ,
		CBaseEntityAnimationType::IDLE_BACKWARD ,
		CBaseEntityAnimationType::IDLE_LEFT ,
		CBaseEntityAnimationType::IDLE_RIGHT ,
		CBaseEntityAnimationType::WALKING_FORWARD ,
		CBaseEntityAnimationType::WALKING_BACKWARD ,
		CBaseEntityAnimationType::WALKING_LEFT ,
		CBaseEntityAnimationType::WALKING_RIGHT
	};

	for ( int i = 0; i < data.size( ); i++ ) {
		CBaseEntityAnimationType anim = data.at( i );

		auto animation =
			generateLocalPlayerAnimationPair( anim , animationName + "_" + CBaseEntityAnimation::getAnimationTypePath( anim ) );
		if ( !animation )
			return std::nullopt;

		animations.emplace( animation.value( ) );
	}

	builder.animations = animations;
	builder.animationFPS = 24;
	builder.currentAnimationType = IDLE_FORWARD;
	builder.spriteSize = GVector2D( 23 , 29 );

	return builder;
}

CPlayerEntity * LocalPlayerInitializer::generate( std::string animationName ) {

	std::optional<CBaseEntityAnimationConstructor> animation = createLocalPlayerAnimationEntity( "localPlayer" );
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