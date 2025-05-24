#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

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


struct CBaseEntityAnimationConstructor {
	std::unordered_map<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>> animations;

	int animationFPS = 10;
	CBaseEntityAnimationType currentAnimationType;
};

class CBaseEntityAnimation {
	std::mutex animationMutex;
	std::unordered_map<CBaseEntityAnimationType, std::shared_ptr<rSpriteAnimation> > animations;
	std::shared_ptr<rSpriteAnimation> currentAnimation;

	GVector2D spriteSize;
	int animationFPS = 10;
	int currentAnimationStep = 0;
	CBaseEntityAnimationType currentAnimationType;
	void * texture;

	int animationCycle = 0;


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

	void updateAnimation( bool loop = true, bool reverse = false );
	void * getCurrentTexture( );
	void setCurrentAnimationType( CBaseEntityAnimationType animationType , bool ignoreCheck = false );

	int getAnimationCycle( ) { return this->animationCycle; }
	int getAnimationFPS( );

	GVector2D getCurrentTextureSize( );
	CBaseEntityAnimationType getCurrentAnimationType( );
};