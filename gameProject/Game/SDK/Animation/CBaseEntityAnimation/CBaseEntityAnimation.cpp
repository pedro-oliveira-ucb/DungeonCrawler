#include "CBaseEntityAnimation.h"
#include <mutex>

std::mutex animationMutex;

GVector2D CBaseEntityAnimation::getSpriteSize( ) {
	return this->spriteSize;
}

int CBaseEntityAnimation::getAnimationFPS( ) {
	return this->animationFPS;
}

CBaseEntityAnimationType CBaseEntityAnimation::getCurrentAnimationType( ) {
	return this->currentAnimationType;
}

void CBaseEntityAnimation::SetCurrentAnimation( CBaseEntityAnimationType animationType ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety
	if ( animations.find( animationType ) != animations.end( ) ) {
		this->currentAnimationType = animationType;
		this->currentAnimation= animations.at(animationType);
	}
}

rSpriteAnimation * CBaseEntityAnimation::GetCurrentAnimation( ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety
	return this->currentAnimation;
}

std::string CBaseEntityAnimation::getAnimationTypeName( CBaseEntityAnimationType anim ) {
	switch ( anim ) {
	case IDLE_FORWARD:
		return "IDLE_FORWARD";
	case IDLE_BACKWARD:
		return "IDLE_BACKWARD";
	case IDLE_LEFT:
		return "IDLE_LEFT";
	case IDLE_RIGHT:
		return "IDLE_RIGHT";
	case WALKING_FORWARD:
		return "WALKING_FORWARD";
	case WALKING_BACKWARD:
		return "WALKING_BACKWARD";
	case WALKING_LEFT:
		return "WALKING_LEFT";
	case WALKING_RIGHT:
		return "WALKING_RIGHT";
	case ATTACKING_FORWARD:
		return "ATTACKING_FORWARD";
	case ATTACKING_BACKWARD:
		return "ATTACKING_BACKWARD";
	case ATTACKING_LEFT:
		return "ATTACKING_LEFT";
	case ATTACKING_RIGHT:
		return "ATTACKING_RIGHT";
	default:
		return "UNKNOWN_ANIMATION_TYPE";
	}
}

std::string CBaseEntityAnimation::getAnimationTypePath( CBaseEntityAnimationType anim ) {
	switch ( anim ) {
	case IDLE_FORWARD:
		return "idle_forward";
	case IDLE_BACKWARD:
		return "idle_backward";
	case IDLE_LEFT:
		return "idle_left";
	case IDLE_RIGHT:
		return "idle_right";
	case WALKING_FORWARD:
		return "walking_forward";
	case WALKING_BACKWARD:
		return "walking_backward";
	case WALKING_LEFT:
		return "walking_left";
	case WALKING_RIGHT:
		return "walking_right";
	case ATTACKING_FORWARD:
		return "attacking_forward";
	case ATTACKING_BACKWARD:
		return "attacking_backward";
	case ATTACKING_LEFT:
		return "attacking_left";
	case ATTACKING_RIGHT:
		return "attacking_right";
	default:
		return "UNKNOWN_ANIMATION_PATH";
	}
}