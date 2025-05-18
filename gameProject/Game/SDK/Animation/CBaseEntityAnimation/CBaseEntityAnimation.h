#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "../../../gameResources/gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"
#include "../../math/Vector2D/GVector2D.h"


enum CBaseEntityAnimationType {
	IDLE_FORWARD,
	IDLE_BACKWARD,
	IDLE_LEFT,
	IDLE_RIGHT,
	WALKING_FORWARD,
	WALKING_BACKWARD,
	WALKING_LEFT,
	WALKING_RIGHT,
	ATTACKING_FORWARD,
	ATTACKING_BACKWARD,
	ATTACKING_LEFT,
	ATTACKING_RIGHT,
};



struct CBaseEntityAnimationConstructor {
	std::unordered_map<CBaseEntityAnimationType , rSpriteAnimation *> animations;

	GVector2D spriteSize;
	int animationFPS = 10;
	CBaseEntityAnimationType currentAnimationType;
};

class CBaseEntityAnimation {

	std::unordered_map<CBaseEntityAnimationType, rSpriteAnimation *> animations;
	rSpriteAnimation * currentAnimation;

	GVector2D spriteSize;
	int animationFPS = 10;
	CBaseEntityAnimationType currentAnimationType;
public:
	
	CBaseEntityAnimation( ){}

	static std::string getAnimationTypeName( CBaseEntityAnimationType anim );
	static std::string getAnimationTypePath( CBaseEntityAnimationType anim );

	CBaseEntityAnimation( CBaseEntityAnimationConstructor builder ) {
		this->animations = builder.animations;
		this->spriteSize = builder.spriteSize;
		this->animationFPS = builder.animationFPS;
		this->currentAnimationType = builder.currentAnimationType;
		SetCurrentAnimation( builder.currentAnimationType );
	}
	rSpriteAnimation * GetCurrentAnimation( );

	void SetCurrentAnimation( CBaseEntityAnimationType animationType);
	GVector2D getSpriteSize( );
	int getAnimationFPS( );
	CBaseEntityAnimationType getCurrentAnimationType( );
};