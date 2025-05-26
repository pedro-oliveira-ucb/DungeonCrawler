#include "CBaseEntityAnimation.h"
#include <mutex>

#include "../../../../Utils/Log/Log.h"

CBaseEntityAnimation::CBaseEntityAnimation( const CBaseEntityAnimation & other ) {
	this->animations = other.animations;
	this->currentAnimationType = other.currentAnimationType;
	this->animationFPS = other.animationFPS;
	this->currentAnimationStep = 0;
	this->timeSinceLastFrame = 0.0f;
	this->animationState = AnimationState::PLAYING;
	this->lastUpdateTime = std::chrono::steady_clock::now( );
	this->lastUpdateTime = std::chrono::steady_clock::now( );
	setCurrentAnimationType( other.currentAnimationType , true );
}

CBaseEntityAnimation::CBaseEntityAnimation( CBaseEntityAnimationConstructor builder ) {
	for ( auto it = builder.animations.begin( ); it != builder.animations.end( ); ++it ) {
		this->animations.emplace( it->first , it->second );
	}

	this->animationFPS = builder.animationFPS;
	this->currentAnimationType = builder.currentAnimationType;
	this->currentAnimationStep = 0;
	this->timeSinceLastFrame = 0.0f;
	this->animationState = AnimationState::PLAYING;
	setCurrentAnimationType( builder.currentAnimationType , true );
}

GVector2D CBaseEntityAnimation::getCurrentTextureSize( ) {
	std::lock_guard<std::mutex> lock( animationMutex );
	return this->spriteSize;
}

int CBaseEntityAnimation::getAnimationFPS( ) {
	return this->animationFPS;
}

CBaseEntityAnimationType CBaseEntityAnimation::getCurrentAnimationType( ) {
	return this->currentAnimationType;
}

void CBaseEntityAnimation::updateAnimation( bool loop , bool reverse ) {
	auto currentTime = std::chrono::steady_clock::now( );
	float deltaTime = std::chrono::duration<float>( currentTime - lastUpdateTime ).count( );
	lastUpdateTime = currentTime;

	// Chama a implementação com deltaTime calculado
	updateAnimationWithDeltaTime( deltaTime , loop , reverse );
}


int CBaseEntityAnimation::getCurrentAnimationStep( ) {
	std::lock_guard<std::mutex> lock( animationMutex );
	return currentAnimationStep;
}

void CBaseEntityAnimation::updateAnimationWithDeltaTime( float deltaTime , bool loop , bool reverse ) {
	/*if ( animationState != AnimationState::PLAYING )
		return;*/

	std::lock_guard<std::mutex> lock( animationMutex );

	this->looping = loop;
	this->playingReverse = reverse;

	timeSinceLastFrame += deltaTime;

	// Tempo necessário para cada frame baseado no FPS
	float frameTime = 1.0f / static_cast< float >( animationFPS );

	if ( timeSinceLastFrame >= frameTime ) {
		int framesToAdvance = static_cast< int >( timeSinceLastFrame / frameTime );
		timeSinceLastFrame -= framesToAdvance * frameTime;

		if ( !currentAnimation ) return;

		int totalFrames = currentAnimation->size( );
		if ( totalFrames <= 0 ) return;

		if ( !reverse ) {
			// Reprodução para frente
			if ( currentAnimationStep + framesToAdvance < totalFrames ) {
				currentAnimationStep += framesToAdvance;
			}
			else if ( loop ) {
				// Completa um ciclo
				animationCycle++;
				currentAnimationStep = ( currentAnimationStep + framesToAdvance ) % totalFrames;
			}
			else {
				// Finaliza no último frame
				currentAnimationStep = totalFrames - 1;
				animationState = AnimationState::FINISHED;
			}
		}
		else {
			// Reprodução para trás
			if ( currentAnimationStep >= framesToAdvance ) {
				currentAnimationStep -= framesToAdvance;
			}
			else if ( loop ) {
				// Completa um ciclo
				animationCycle++;
				currentAnimationStep = totalFrames - ( framesToAdvance - currentAnimationStep ) % totalFrames - 1;
				if ( currentAnimationStep < 0 )
					currentAnimationStep = 0;
			}
			else {
				// Finaliza no primeiro frame
				currentAnimationStep = 0;
				animationState = AnimationState::FINISHED;
			}
		}

		if ( currentAnimation->getFrame( currentAnimationStep ) ) {
			this->spriteSize = currentAnimation->getFrame( currentAnimationStep )->getSpriteSize( );
		}
	}
}

void * CBaseEntityAnimation::getCurrentTexture( ) {
	std::lock_guard<std::mutex> lock( animationMutex );

	if ( this->currentAnimation == nullptr ) {
		return nullptr;
	}

	if ( this->currentAnimation->getFrame( this->currentAnimationStep ) == nullptr ) {
		return nullptr;
	}

	return this->currentAnimation->getFrame( this->currentAnimationStep )->getTexture( );
}

void CBaseEntityAnimation::resetAnimation( ) {
	std::lock_guard<std::mutex> lock( animationMutex );

	if ( !currentAnimation ) return;

	currentAnimationStep = playingReverse ? ( currentAnimation->size( ) - 1 ) : 0;
	timeSinceLastFrame = 0.0f;
	animationCycle = 0;
	animationState = AnimationState::PLAYING;
}

void CBaseEntityAnimation::pauseAnimation( ) {
	std::lock_guard<std::mutex> lock( animationMutex );
	if ( animationState == AnimationState::PLAYING )
		animationState = AnimationState::PAUSED;
}

void CBaseEntityAnimation::resumeAnimation( ) {
	std::lock_guard<std::mutex> lock( animationMutex );
	if ( animationState == AnimationState::PAUSED )
		animationState = AnimationState::PLAYING;
}

void CBaseEntityAnimation::stopAnimation( ) {
	std::lock_guard<std::mutex> lock( animationMutex );

	if ( !currentAnimation ) return;

	currentAnimationStep = playingReverse ? ( currentAnimation->size( ) - 1 ) : 0;
	timeSinceLastFrame = 0.0f;
	animationState = AnimationState::FINISHED;
}

bool CBaseEntityAnimation::isAnimationPlaying( ) const {
	return animationState == AnimationState::PLAYING;
}

bool CBaseEntityAnimation::isAnimationPaused( ) const {
	return animationState == AnimationState::PAUSED;
}

bool CBaseEntityAnimation::isAnimationFinished( ) const {
	return animationState == AnimationState::FINISHED;
}

enum CBaseAnimationGroupType {
	GROUP_IDLE ,
	GROUP_WALKING ,
	GROUP_RUNNING ,
	GROUP_ATTACKING ,
	GROUP_HURT ,
	GROUP_DEAD ,
	GROUP_UNKNOWN
};

CBaseAnimationGroupType getAnimationGroup( CBaseEntityAnimationType anim ) {
	switch ( anim ) {
	case IDLE_FORWARD: case IDLE_BACKWARD: case IDLE_LEFT: case IDLE_RIGHT:
		return GROUP_IDLE;

	case WALKING_FORWARD: case WALKING_BACKWARD: case WALKING_LEFT: case WALKING_RIGHT:
		return GROUP_WALKING;

	case RUNNING_FORWARD: case RUNNING_BACKWARD: case RUNNING_LEFT: case RUNNING_RIGHT:
		return GROUP_RUNNING;

	case ATTACKING_FORWARD: case ATTACKING_BACKWARD: case ATTACKING_LEFT: case ATTACKING_RIGHT:	
	case ATTACKING_WALKING_FORWARD: case ATTACKING_WALKING_BACKWARD: case ATTACKING_WALKING_LEFT: case ATTACKING_WALKING_RIGHT: 
	case ATTACKING_RUNNING_FORWARD: case ATTACKING_RUNNING_BACKWARD: case ATTACKING_RUNNING_LEFT: case ATTACKING_RUNNING_RIGHT:
		return GROUP_ATTACKING;

	case HURT_FORWARD: case HURT_BACKWARD: case HURT_LEFT: case HURT_RIGHT:
		return GROUP_HURT;

	case DEAD_FORWARD: case DEAD_BACKWARD: case DEAD_LEFT: case DEAD_RIGHT:
		return GROUP_DEAD;

	default:
		return GROUP_UNKNOWN;
	}
}

// Implementação dos métodos estáticos e de configuração de animação permanecem os mesmos
bool CBaseEntityAnimation::isDifferentAnimationType( CBaseEntityAnimationType animA , CBaseEntityAnimationType animB ) {
	return getAnimationGroup( animA ) != getAnimationGroup( animB );
}

void CBaseEntityAnimation::setCurrentAnimationType( CBaseEntityAnimationType animationType , bool ignoreCheck ) {
	std::lock_guard<std::mutex> lock( animationMutex );

	if ( animationType == this->currentAnimationType && !ignoreCheck ) {
		return;
	}

	auto it = animations.find( animationType );
	if ( it != animations.end( ) && it->second.get() != nullptr ) {
		this->currentAnimationType = animationType;
		if ( this->isDifferentAnimationType( this->currentAnimationType , animationType ) ) {
			this->currentAnimationStep = 0;
			this->timeSinceLastFrame = 0.0f;
			this->animationState = AnimationState::PLAYING;
		}
		else if ( this->currentAnimation != nullptr && this->currentAnimation->size() != it->second->size() ) {
			//update frame step on equal animations
			float CurrentFramePercentage = ( this->currentAnimationStep / this->currentAnimation->size( ) );
			int correctedFrameStep = static_cast< int >( CurrentFramePercentage * it->second->size( ) );
			this->currentAnimationStep = correctedFrameStep;
		}

		this->currentAnimation = it->second;
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