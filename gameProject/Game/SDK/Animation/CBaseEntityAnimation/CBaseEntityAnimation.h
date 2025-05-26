#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <chrono>

#include "../../../gameResources/gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"
#include "../../math/Vector2D/GVector2D.h"

enum CBaseEntityAnimationType {
	// Idle
	IDLE_FORWARD ,
	IDLE_BACKWARD ,
	IDLE_LEFT ,
	IDLE_RIGHT ,

	// Walking
	WALKING_FORWARD ,
	WALKING_BACKWARD ,
	WALKING_LEFT ,
	WALKING_RIGHT ,

	// Running
	RUNNING_FORWARD ,
	RUNNING_BACKWARD ,
	RUNNING_LEFT ,
	RUNNING_RIGHT ,

	// Attacking (stationary)
	ATTACKING_FORWARD ,
	ATTACKING_BACKWARD ,
	ATTACKING_LEFT ,
	ATTACKING_RIGHT ,

	// Attacking while walking
	ATTACKING_WALKING_FORWARD ,
	ATTACKING_WALKING_BACKWARD ,
	ATTACKING_WALKING_LEFT ,
	ATTACKING_WALKING_RIGHT ,

	// Attacking while running
	ATTACKING_RUNNING_FORWARD ,
	ATTACKING_RUNNING_BACKWARD ,
	ATTACKING_RUNNING_LEFT ,
	ATTACKING_RUNNING_RIGHT ,

	// Hurt
	HURT_FORWARD ,
	HURT_BACKWARD ,
	HURT_LEFT ,
	HURT_RIGHT ,

	// Dead
	DEAD_FORWARD ,
	DEAD_BACKWARD ,
	DEAD_LEFT ,
	DEAD_RIGHT
};

enum class AnimationState {
	PLAYING ,
	PAUSED ,
	FINISHED
};

struct CBaseEntityAnimationConstructor {
	std::unordered_map<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>> animations;
	int animationFPS = 10;
	CBaseEntityAnimationType currentAnimationType;
};

class CBaseEntityAnimation {
	std::mutex animationMutex;
	std::unordered_map<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>> animations;
	std::shared_ptr<rSpriteAnimation> currentAnimation;
	std::chrono::steady_clock::time_point lastUpdateTime;

	GVector2D spriteSize;
	int animationFPS = 10;
	int currentAnimationStep = 0;
	CBaseEntityAnimationType currentAnimationType;
	void * texture;

	int animationCycle = 0;
	float timeSinceLastFrame = 0.0f;
	bool playingReverse = false;
	bool looping = true;
	AnimationState animationState = AnimationState::PLAYING;

public:

	CBaseEntityAnimation & operator=( const CBaseEntityAnimation & other ) {
		if ( this == &other )
			return *this;

		std::lock_guard<std::mutex> lock( this->animationMutex );

		this->animations = other.animations;
		this->spriteSize = other.spriteSize;
		this->animationFPS = other.animationFPS;
		this->currentAnimationStep = other.currentAnimationStep;
		this->currentAnimationType = other.currentAnimationType;
		this->texture = other.texture;
		this->animationCycle = other.animationCycle;

		return *this;
	}

	CBaseEntityAnimation( const CBaseEntityAnimation & other );
	CBaseEntityAnimation( CBaseEntityAnimationConstructor builder );

	static std::string getAnimationTypeName( CBaseEntityAnimationType anim );
	static std::string getAnimationTypePath( CBaseEntityAnimationType anim );
	static CBaseEntityAnimationType getReverseAnimation( CBaseEntityAnimationType anim );
	static bool isDifferentAnimationType( CBaseEntityAnimationType animA , CBaseEntityAnimationType animB );

	void updateAnimation( bool loop = true , bool reverse = false );
	void updateAnimationWithDeltaTime( float deltaTime , bool loop = true, bool reverse = false);
	void * getCurrentTexture( );
	void setCurrentAnimationType( CBaseEntityAnimationType animationType , bool ignoreCheck = false );

	// Novos controles de animação
	void resetAnimation( );
	void pauseAnimation( );
	void resumeAnimation( );
	void stopAnimation( );
	bool isAnimationPlaying( ) const;
	bool isAnimationPaused( ) const;
	bool isAnimationFinished( ) const;

	int getAnimationCycle( ) { return this->animationCycle; }
	int getAnimationFPS( );
	int getCurrentAnimationStep( );

	GVector2D getCurrentTextureSize( );
	CBaseEntityAnimationType getCurrentAnimationType( );
};