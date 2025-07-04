#include "EntitiesInitializer.h"
#include <optional>

#include "LocalPlayerInitializer/LocalPlayerInitializer.h"
#include "AttacksInitializer/AttacksInitializer.h"
#include "EnemiesInitializer/EnemiesInitializer.h"
#include "ItemsInitializer/ItemsInitializer.h"
#include "TrapsInitializer/TrapsInitializer.h"
#include "ShopInitializer/ShopInitializer.h" // ADICIONAR ESTA LINHA

#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../../Utils/Log/Log.h"

std::optional<std::pair<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>>> generateAnimationPair( CBaseEntityAnimationType type , std::string Name ) {

	std::shared_ptr< rSpriteAnimation> clip = _gameResourceManager.getSpritesManager( )->getClip( Name );

	if ( clip.get( ) != nullptr )
	{
		Log::Print( "[EntitiesInitializer] Found %s animation" , Name.c_str( ) );

		return std::make_pair(
			type ,
			clip
		);
	}

	Log::Print( "[EntitiesInitializer] Can't Found %s animation" , Name.c_str( ) );

	return std::nullopt;
}

std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations ) {
	CBaseEntityAnimationConstructor builder;

	for ( int i = 0; i < requiredAnimations.size( ); i++ ) {
		CBaseEntityAnimationType anim = requiredAnimations.at( i );

		std::string fullAnimationName = animationName;
		std::string animationTypePath = CBaseEntityAnimation::getAnimationTypePath( anim );
		if ( !animationTypePath.empty( ) ) {
			fullAnimationName += "_" + animationTypePath;
		}

		std::optional<std::pair<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>>> animation =
			generateAnimationPair( anim , fullAnimationName );
		if ( !animation.has_value() ) {
			Log::Print( "[EntitiesInitializer] Failed to initialize %s" , animationName.c_str( ) );
			return std::nullopt;
		}

		builder.animations.emplace( animation.value( ) );
	}

	if ( builder.animations.empty( ) ) {
		Log::Print( "[createEntityAnimationConstructor] animation generation failed" );
		return std::nullopt;
	}

	builder.animationFPS = 10;
	builder.currentAnimationType = IDLE_FORWARD;
	return builder;
}

bool EntitiesInitializer::initialize( ) {

	if ( !AttacksInitializer::Get( ).initializeAttacks( ) ) {
		Log::Print( "[EntitiesInitializer] Failed to initialize attacks" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] attacks initialized" );

	if ( !LocalPlayerInitializer::Get( ).initialize( ) ) {
		Log::Print( "[EntitiesInitializer] Failed to initialize localplayer" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] localPlayer initialized" );

	if ( !EnemiesInitializer::Get( ).initialize( ) ) {
		Log::Print( "[EntitiesInitializer] Failed to initialize enemies" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] enemies initialized" );

	if ( !ItemsInitializer::Get( ).initialize( ) ) {
		Log::Print( "[EnemiesInitializer] Failed to initialize items" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] items initialized" );

	if ( !TrapsInitializer::Get( ).initialize( ) ) {
		Log::Print( "[EnemiesInitializer] Failed to initialize traps" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] traps initialized" );

	if ( !ShopInitializer::Get( ).initialize( ) ) {
		Log::Print( "[EntitiesInitializer] Failed to initialize shop system" );
		return false;
	}

	Log::Print( "[EntitiesInitializer] shop system initialized" );

	return true;
}