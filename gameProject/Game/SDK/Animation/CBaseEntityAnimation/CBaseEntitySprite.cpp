#include "CBaseEntitySprite.h"

std::unordered_map<CBaseEntitySpriteAnimationType , rSpriteAnimation *> CBaseEntitySprite::getAnimations( ) {
	return this->animations;
}

GVector2D CBaseEntitySprite::getSpriteSize( ) {
	return this->spriteSize;
}

int CBaseEntitySprite::getAnimationFPS( ) {
	return this->animationFPS;
}

CBaseEntitySpriteAnimationType CBaseEntitySprite::getCurrentAnimationType( ) {
	return this->currentAnimationType;
}
