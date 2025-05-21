#include "CBaseEntityAnimation.h"
#include <mutex>

std::mutex animationMutex;

CBaseEntityAnimation::CBaseEntityAnimation( CBaseEntityAnimationConstructor builder ) {
	this->animations = builder.animations;
	this->animationFPS = builder.animationFPS;
	this->currentAnimationType = builder.currentAnimationType;
	this->currentAnimationStep = 0;
	setCurrentAnimationType( builder.currentAnimationType , true );
}

GVector2D CBaseEntityAnimation::getCurrentTextureSize( ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety
	return this->spriteSize;
}

int CBaseEntityAnimation::getAnimationFPS( ) {
	return this->animationFPS;
}

CBaseEntityAnimationType CBaseEntityAnimation::getCurrentAnimationType( ) {
	return this->currentAnimationType;
}

void CBaseEntityAnimation::updateAnimation( bool loop , bool reverse ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety

	if ( this->currentAnimationStep < this->currentAnimation->size( ) - 1 ) {
		this->currentAnimationStep++;
	}
	else {
		this->animationCycle++;

		if ( loop ) {
			this->currentAnimationStep = 0;
		}
	}

	if ( this->currentAnimation != nullptr ) {
		this->spriteSize = this->currentAnimation->getFrame( this->currentAnimationStep )->getSpriteSize( );
	}
}

void * CBaseEntityAnimation::getCurrentTexture( ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety

	if ( this->currentAnimation == nullptr ) {
		return nullptr;
	}

	if ( this->currentAnimation->getFrame( this->currentAnimationStep ) == nullptr ) {
		return nullptr;
	}

	return this->currentAnimation->getFrame( this->currentAnimationStep )->getTexture( );
}


bool CBaseEntityAnimation::isDifferentAnimationType( CBaseEntityAnimationType animA , CBaseEntityAnimationType animB ) {
	// check if it is different type walking, idle or attacking
	return ( animA / 4 ) != ( animB / 4 );
}

void CBaseEntityAnimation::setCurrentAnimationType( CBaseEntityAnimationType animationType , bool ignoreCheck ) {
	std::lock_guard<std::mutex> lock( animationMutex ); // Lock the mutex to ensure thread safety

	if ( animationType == this->currentAnimationType && !ignoreCheck ) {
		return;
	}

	if ( animations.find( animationType ) != animations.end( ) ) {
		this->currentAnimationType = animationType;
		if ( this->isDifferentAnimationType( this->currentAnimationType , animationType ) ) {
			this->currentAnimationStep = 0;
		}

		std::shared_ptr<rSpriteAnimation> animation = this->animations.at( this->currentAnimationType );

		if ( animation.get( ) == nullptr ) {
			return;
		}

		this->currentAnimation = this->animations.at( this->currentAnimationType ).get( );
	}
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
		return "";
	}
}

CBaseEntityAnimationType CBaseEntityAnimation::getReverseAnimation( CBaseEntityAnimationType anim ) {
	switch ( anim ) {
	case IDLE_FORWARD:
		return IDLE_BACKWARD;
	case IDLE_BACKWARD:
		return IDLE_FORWARD;
	case IDLE_LEFT:
		return IDLE_RIGHT;
	case IDLE_RIGHT:
		return IDLE_LEFT;
	case WALKING_FORWARD:
		return WALKING_BACKWARD;
	case WALKING_BACKWARD:
		return WALKING_FORWARD;
	case WALKING_LEFT:
		return WALKING_RIGHT;
	case WALKING_RIGHT:
		return WALKING_LEFT;
	case ATTACKING_FORWARD:
		return ATTACKING_BACKWARD;
	case ATTACKING_BACKWARD:
		return ATTACKING_FORWARD;
	case ATTACKING_LEFT:
		return ATTACKING_RIGHT;
	case ATTACKING_RIGHT:
		return ATTACKING_LEFT;
	default:
		return anim; // Return the same animation if no reverse is found
	}
}