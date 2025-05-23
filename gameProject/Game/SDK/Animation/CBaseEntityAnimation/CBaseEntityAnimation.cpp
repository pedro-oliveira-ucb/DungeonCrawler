#include "CBaseEntityAnimation.h"
#include <mutex>




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
	}
	else {
		this->animationCycle++;

		if ( loop ) {
			this->currentAnimationStep = 0;
		}
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

		this->currentAnimation = this->animations.at( this->currentAnimationType ).get( );
	}
}

static const std::string_view animationPaths[ ] = {
	"idle_forward",     // 0
	"idle_backward",    // 1
	"idle_left",        // 2
	"idle_right",       // 3
	"walking_forward",  // 4
	"walking_backward", // 5
	"walking_left",     // 6
	"walking_right",    // 7
	"attacking_forward",// 8
	"attacking_backward",//9
	"attacking_left",   //10
	"attacking_right",  //11
	"hurt_backward",    //12
	"hurt_forward",     //13
	"hurt_left",        //14
	"hurt_right",        //15
	"dead_backward",    //12
	"dead_forward",     //13
	"dead_left",        //14
	"dead_right"        //15
};

std::string CBaseEntityAnimation::getAnimationTypeName( CBaseEntityAnimationType anim ) {
	int index = static_cast< int >( anim );
	if ( index >= 0 && index < static_cast< int >( std::size( animationPaths ) ) ) {
		return std::string( animationPaths[ index ] );
	}
	return "";
}



std::string CBaseEntityAnimation::getAnimationTypePath( CBaseEntityAnimationType anim ) {
	int index = static_cast< int >( anim );
	if ( index >= 0 && index < static_cast< int >( std::size( animationPaths ) ) ) {
		return std::string( animationPaths[ index ] );
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