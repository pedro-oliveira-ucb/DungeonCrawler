#include "EntitiesInitializer.h"

#include <optional>


#include "../../World/World.h"
#include "../../gameWorld/gameWorld.h"
#include "../../gameResources/gameResourceManager/gameResourceManager.h"

#include "LocalPlayerInitializer/LocalPlayerInitializer.h"

#include "../../../Utils/Log/Log.h"


std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>> generateAnimationPair( CBaseEntityAnimationType type , std::string Name ) {
	if ( _gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->find( Name ) !=
		_gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->end( ) )
	{
		Log::Print( "[LocalPlayerInitializer] Found %s animation" , Name.c_str( ) );
		rSpriteAnimation * anim = &_gameResourceManager.getSpritesManager( ).getSpriteAnimations( )->at( Name );

		return std::make_pair(
			type ,
			anim
		);
	}

	Log::Print( "[LocalPlayerInitializer] Can't Found %s animation" , Name.c_str( ) );

	return std::nullopt;
}

std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName, 
	std::vector<CBaseEntityAnimationType> requiredAnimations ) {
	CBaseEntityAnimationConstructor builder;

	std::unordered_map<CBaseEntityAnimationType , rSpriteAnimation * > animations;

	for ( int i = 0; i < requiredAnimations.size( ); i++ ) {
		CBaseEntityAnimationType anim = requiredAnimations.at( i );

		std::string fullAnimationName = animationName;
		std::string animationTypePath = CBaseEntityAnimation::getAnimationTypePath( anim );
		if ( !animationTypePath.empty( ) ) {
			fullAnimationName += "_" + animationTypePath;
		}
		
		auto animation =
			generateAnimationPair( anim , fullAnimationName );
		if ( !animation )
			return std::nullopt;

		animations.emplace( animation.value( ) );
	}

	builder.animations = animations;
	builder.animationFPS = 24;
	builder.currentAnimationType = IDLE_FORWARD;

	return builder;
}

bool EntitiesInitializer::initialize( ) {

	CPlayerEntity * localPlayer = LocalPlayerInitializer::Get( ).generate( "localPlayer" );
	if ( !localPlayer ) {
		Log::Print( "[EntitiesInitializer] Failed to generate localplayer" );
		return false;
	}
	_gameWorld.localplayer = localPlayer;
	printf( "LocalPlayer name: %s\n" , _gameWorld.localplayer->GetEntityName().c_str());

	return true;
}