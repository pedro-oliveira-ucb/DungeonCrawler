#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "../../../gameResources/gameResource/rSprites/rSpriteAnimation/rSpriteAnimation.h"
#include "../../math/Vector2D/GVector2D.h"


enum CBaseEntitySpriteAnimationType {
	IDLE,
	WALKING_FORWARD,
	WALKING_BACKWARD,
	WALKING_LEFT,
	WALKING_RIGHT,
	ATTACKING_FORWARD,
	ATTACKING_BACKWARD,
	ATTACKING_LEFT,
	ATTACKING_RIGHT,
};

class CBaseEntitySprite {

	std::unordered_map<CBaseEntitySpriteAnimationType, rSpriteAnimation *> animations;

	GVector2D spriteSize;
	int animationFPS = 10;
	CBaseEntitySpriteAnimationType currentAnimationType;
public:
	CBaseEntitySprite( std::unordered_map<CBaseEntitySpriteAnimationType , rSpriteAnimation *> anims, 
		GVector2D vec, 
		int animfps, 
		CBaseEntitySpriteAnimationType animtype )
		:animations(anims), 
		spriteSize(vec),
		animationFPS(animfps), 
		currentAnimationType(animtype) {}

	std::unordered_map<CBaseEntitySpriteAnimationType , rSpriteAnimation *> getAnimations( );
	GVector2D getSpriteSize( );
	int getAnimationFPS( );
	CBaseEntitySpriteAnimationType getCurrentAnimationType( );


};