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
	std::unordered_map<CBaseEntityAnimationType , std::shared_ptr<rSpriteAnimation>> animations;

	int animationFPS = 10;
	CBaseEntityAnimationType currentAnimationType;
};

class CBaseEntityAnimation {

	std::unordered_map<CBaseEntityAnimationType, std::shared_ptr<rSpriteAnimation> > animations;
	rSpriteAnimation * currentAnimation;

	GVector2D spriteSize;
	int animationFPS = 10;
	int currentAnimationStep = 0;
	CBaseEntityAnimationType currentAnimationType;
	void * texture;

	int animationCycle = 0;

public:
	
	int getAnimationCycle( ) { return this->animationCycle; }

	CBaseEntityAnimation( ){}

	static std::string getAnimationTypeName( CBaseEntityAnimationType anim );
	static std::string getAnimationTypePath( CBaseEntityAnimationType anim );
	static CBaseEntityAnimationType getReverseAnimation( CBaseEntityAnimationType anim );

	CBaseEntityAnimation( CBaseEntityAnimationConstructor builder );

	void updateAnimation( bool loop = true, bool reverse = false );
	
	void * getCurrentTexture( );
	GVector2D getCurrentTextureSize( );

	void setCurrentAnimationType( CBaseEntityAnimationType animationType, bool ignoreCheck = false );

	int getAnimationFPS( );

	CBaseEntityAnimationType getCurrentAnimationType( );
};