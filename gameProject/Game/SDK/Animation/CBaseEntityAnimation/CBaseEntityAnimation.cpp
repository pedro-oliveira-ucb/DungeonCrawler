#include "CBaseEntityAnimation.h"
#include <mutex>

#include "../../../../Utils/Log/Log.h"

CBaseEntityAnimation::CBaseEntityAnimation( const  CBaseEntityAnimation & other ) {
	this->animations = other.animations;
	this->currentAnimationType = other.currentAnimationType;
	this->animationFPS = other.animationFPS;
	this->currentAnimationStep = 0;
	setCurrentAnimationType( other.currentAnimationType , true );
}

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

		if ( this->currentAnimationStep == this->currentAnimation->size( ) - 1 ) {
			this->animationCycle++;
		}
	}
	else if ( loop ) {
		this->currentAnimationStep = 0;
	}
	else {
		this->currentAnimationStep = this->currentAnimation->size( ) - 1;
	}

	if ( this->currentAnimation == nullptr || this->currentAnimation->getFrame( this->currentAnimationStep ).get( ) == nullptr ) {
		return;
	}

	this->spriteSize = this->currentAnimation->getFrame( this->currentAnimationStep )->getSpriteSize( );
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

		this->currentAnimation = this->animations.at( this->currentAnimationType );
	}
}

// Mapeamento de animações para caminhos de arquivo
std::unordered_map<CBaseEntityAnimationType , std::string> animationPaths = {
	// Idle
	{ IDLE_FORWARD,             "idle_forward" },
	{ IDLE_BACKWARD,            "idle_backward" },
	{ IDLE_LEFT,                "idle_left" },
	{ IDLE_RIGHT,               "idle_right" },
	// Walking
	{ WALKING_FORWARD,          "walking_forward" },
	{ WALKING_BACKWARD,         "walking_backward" },
	{ WALKING_LEFT,             "walking_left" },
	{ WALKING_RIGHT,            "walking_right" },
	// Running
	{ RUNNING_FORWARD,          "running_forward" },
	{ RUNNING_BACKWARD,         "running_backward" },
	{ RUNNING_LEFT,             "running_left" },
	{ RUNNING_RIGHT,            "running_right" },
	// Attacking stationary
	{ ATTACKING_FORWARD,        "attacking_forward" },
	{ ATTACKING_BACKWARD,       "attacking_backward" },
	{ ATTACKING_LEFT,           "attacking_left" },
	{ ATTACKING_RIGHT,          "attacking_right" },
	// Attacking walking
	{ ATTACKING_WALKING_FORWARD,"attacking_walking_forward" },
	{ ATTACKING_WALKING_BACKWARD,"attacking_walking_backward" },
	{ ATTACKING_WALKING_LEFT,   "attacking_walking_left" },
	{ ATTACKING_WALKING_RIGHT,  "attacking_walking_right" },
	// Attacking running
	{ ATTACKING_RUNNING_FORWARD,"attacking_running_forward" },
	{ ATTACKING_RUNNING_BACKWARD,"attacking_running_backward" },
	{ ATTACKING_RUNNING_LEFT,   "attacking_running_left" },
	{ ATTACKING_RUNNING_RIGHT,  "attacking_running_right" },
	// Hurt
	{ HURT_FORWARD,             "hurt_forward" },
	{ HURT_BACKWARD,            "hurt_backward" },
	{ HURT_LEFT,                "hurt_left" },
	{ HURT_RIGHT,               "hurt_right" },
	// Dead
	{ DEAD_FORWARD,             "dead_forward" },
	{ DEAD_BACKWARD,            "dead_backward" },
	{ DEAD_LEFT,                "dead_left" },
	{ DEAD_RIGHT,               "dead_right" }
};

std::string CBaseEntityAnimation::getAnimationTypeName( CBaseEntityAnimationType anim ) {
	if ( animationPaths.find( anim ) != animationPaths.end( ) ) {
		return animationPaths[ anim ];
	}

	return "";
}

std::string CBaseEntityAnimation::getAnimationTypePath( CBaseEntityAnimationType anim ) {
	if ( animationPaths.find( anim ) != animationPaths.end( ) ) {
		return animationPaths[ anim ];
	}
	return "";
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