#include "CBaseEntity.h"

#include "../../../Managers/collisionManager/collisionManager.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"
#include <unordered_map>
#include <set>
#include <cmath>


CBaseEntity::CBaseEntity( const CBaseEntity & other ) :
	entityAnimations( other.entityAnimations )
{
	this->Name = other.Name;
	this->entityPosition = other.entityPosition;
	this->health = other.health;
	this->entityType = other.entityType;
	this->entityMovementDirection = other.entityMovementDirection;
	this->entityState = other.entityState;
	this->movementsRequest = other.movementsRequest;
	this->lookingAngle = other.lookingAngle;
	this->movementAngle = other.movementAngle;
	this->movementSpeed = other.movementSpeed;
	this->entityHitbox = other.entityHitbox;

	Log::Print( "[%s] Copy constructor called" , this->GetEntityName( ).c_str( ) );
}

CBaseEntity::CBaseEntity( CBaseEntityConstructor builder ) :
	entityAnimations( builder.entityAnimations )
{
	this->health = builder.health;
	this->Name = builder.Name;
	this->entityPosition = builder.entityPosition;
	this->entityType = builder.entityType;
	this->entityMovementDirection = builder.entityMovementDirection;
	this->entityAnimations = builder.entityAnimations;
	this->movementSpeed = builder.movementSpeed;
}

std::string CBaseEntity::GetEntityName( ) {
	return this->Name;
}

GVector2D CBaseEntity::getEntityPosition( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return this->entityPosition;
}

int CBaseEntity::getHealth( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return this->health;
}

//CBaseEntityState CBaseEntity::getEntityState( ) {
//	std::lock_guard<std::mutex> lock( this->cBaseMutex );
//	return this->entityState;
//}

CBaseEntityType CBaseEntity::getEntityType( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return this->entityType;
}



float CBaseEntity::getEntityLookingDirectionBaseAngle( ) {
	switch ( getEntityLookingDirection( ) ) {
	case DIRECTION::FORWARD:
		return 270.f;
	case DIRECTION::BACKWARD:
		return 90.f;
	case DIRECTION::LEFT:
		return 180.f;
	case DIRECTION::RIGHT:
		return 0.f;
	}

	return 0.f;
}

float CBaseEntity::getMaxHealth( ) {
	return this->maxHealth;
}

CBaseEntityAnimation * CBaseEntity::getEntityAnimations( ) {
	return &this->entityAnimations;
}

void CBaseEntity::setHealth( int health ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->health = health;
}

void CBaseEntity::setEntityPosition( GVector2D pos ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->entityPosition = pos;
}

void CBaseEntity::Hit( int damage ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->health -= damage;
	this->beingHit = true;

	if ( this->isAlive( ) )
		EventManager::Get( ).Trigger( this->Name + "_hurt" );
	else
		EventManager::Get( ).Trigger( this->Name + "_dead" );
}

bool CBaseEntity::isBeingHit( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return this->beingHit;
}

void CBaseEntity::stopBeingHit( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->beingHit = false;
}

CBaseEntityMovementDirection CBaseEntity::getEntityMovementDirection( ) {
	return this->entityMovementDirection;
}

void CBaseEntity::setEntityMovementDirection( CBaseEntityMovementDirection move ) {
	this->entityMovementDirection = move;
}

//
//void CBaseEntity::setEntityState( CBaseEntityState state ) {
//	this->entityState = state;
//}

void CBaseEntity::addMoveRequest( CBaseEntityMovementDirection movement ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->movementsRequest.emplace_back( movement );
}

float CBaseEntity::getMovementAngle( ) {
	return this->movementAngle;
}

void CBaseEntity::clearMovementRequest( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->movementsRequest.clear( );
}

void CBaseEntity::move( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );

	// Tempo atual
	auto now = std::chrono::high_resolution_clock::now( );

	// Calcular delta time
	float deltaTime = 0.0f;
	if ( this->lastMoveTime.time_since_epoch( ).count( ) > 0 ) {
		std::chrono::duration<float> delta = now - this->lastMoveTime;
		deltaTime = delta.count( );
	}

	// Atualiza o lastMoveTime
	this->lastMoveTime = now;

	// Movimento final
	GVector2D finalMovement( 0.f , 0.f );
	float moveSpeed = this->sprinting ? this->movementSpeed * 2.0f : this->movementSpeed;
	float scaledSpeed = moveSpeed * deltaTime;

	// Ignorar duplicatas
	std::set<CBaseEntityMovementDirection> uniqueMoves( this->movementsRequest.begin( ) , this->movementsRequest.end( ) );

	for ( const auto & move : uniqueMoves ) {
		switch ( move ) {
		case CBaseEntityMovementDirection::MOVEMENT_FORWARD:
			finalMovement.y -= scaledSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_BACKWARD:
			finalMovement.y += scaledSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_LEFT:
			finalMovement.x -= scaledSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_RIGHT:
			finalMovement.x += scaledSpeed;
			break;
		}
	}

	this->movementsRequest.clear( );

	if ( finalMovement.x != 0.0f || finalMovement.y != 0.0f ) {
		GVector2D newPos = this->entityPosition + finalMovement;

		if ( CollisionManager::Get( ).CanMoveTo( this , newPos ) ) {
			float angleRadians = std::atan2( finalMovement.y , finalMovement.x );
			float angleDegrees = angleRadians * ( 180.0f / static_cast< float >( M_PI ) );
			this->movementAngle = angleDegrees;
			this->entityPosition = newPos;
		}
	}
}


bool CBaseEntity::hasMovementRequest( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return !this->movementsRequest.empty( );
}

void CBaseEntity::updateEntity( ) {

}

CBaseEntityAnimationType CBaseEntity::getAnimationTypeBasedOnStateAndDirection( uint32_t states , DIRECTION direction ) {
	// Converte direção em índice (0 = forward, 1 = backward, 2 = left, 3 = right)
	int dirIndex = 0;
	switch ( direction ) {
	case FORWARD:  dirIndex = 0; break;
	case BACKWARD: dirIndex = 1; break;
	case LEFT:     dirIndex = 2; break;
	case RIGHT:    dirIndex = 3; break;
	}

	// PRIORIDADE: DEAD > HURT > ATTACKING > RUNNING > MOVING > STOPPED

	// Dead
	if ( states & DEAD ) {
		int idx = static_cast< int >( DEAD_FORWARD ) + dirIndex;
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Hurt
	if ( states & HURT ) {
		int idx = static_cast< int >( HURT_FORWARD ) + dirIndex;
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Attacking
	if ( states & ATTACKING ) {
		bool running = states & RUNNING;
		bool walking = states & MOVING;
		int baseAnim = static_cast< int >( ATTACKING_FORWARD );
		if ( running ) {
			baseAnim = static_cast< int >( ATTACKING_RUNNING_FORWARD );
		}
		else if ( walking ) {
			baseAnim = static_cast< int >( ATTACKING_WALKING_FORWARD );
		}
		int idx = baseAnim + dirIndex;
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Running
	if ( states & RUNNING ) {
		int idx = static_cast< int >( RUNNING_FORWARD ) + dirIndex;
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Moving (walking)
	if ( states & MOVING ) {
		int idx = static_cast< int >( WALKING_FORWARD ) + dirIndex;
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Stopped (idle)
	int idx = static_cast< int >( IDLE_FORWARD ) + dirIndex;
	return static_cast< CBaseEntityAnimationType >( idx );
}


bool CBaseEntity::isAlive( ) {
	return this->health > 0;
}

bool CBaseEntity::deathAnimationFinished( )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return this->finishedDeathAnimation;
}

void CBaseEntity::setDeathAnimationFinished( bool finished )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->finishedDeathAnimation = finished;
}

void CBaseEntity::setLookingAngle( float degress )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	if ( !this->isAlive( ) )
		return;

	if ( degress > -45 && degress <= 45 )
		this->entityLookingDirection = ( DIRECTION::RIGHT );
	else if ( degress > 45 && degress <= 135 )
		this->entityLookingDirection = ( DIRECTION::BACKWARD );
	else if ( degress > 135 || degress <= -135 )
		this->entityLookingDirection = ( DIRECTION::LEFT );
	else
		this->entityLookingDirection = ( DIRECTION::FORWARD );

	this->lookingAngle.setDegrees( degress );
}

GAngle CBaseEntity::getLookingAngle( )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );

	return this->lookingAngle;
}

CBaseEntityHitbox CBaseEntity::getHitbox( )
{
	return this->entityHitbox;
}
DIRECTION CBaseEntity::getEntityLookingDirection( )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );

	return this->entityLookingDirection;
}

void CBaseEntity::setEntityLookingDirection( DIRECTION direction )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );

	this->entityLookingDirection = direction;
}

std::string CBaseEntity::getEntityStateAsString() const {
    std::string result;
    
    if (entityState == 0) {
        return "Undefined";
    }
    
    if (entityState & STOPPED) {
        result += "STOPPED";
    }
    
    if (entityState & MOVING) {
        if (!result.empty()) result += " | ";
        result += "MOVING";
    }
    
    if (entityState & RUNNING) {
        if (!result.empty()) result += " | ";
        result += "RUNNING";
    }
    
    if (entityState & ATTACKING) {
        if (!result.empty()) result += " | ";
        result += "ATTACKING";
    }
    
    if (entityState & HURT) {
        if (!result.empty()) result += " | ";
        result += "HURT";
    }
    
    if (entityState & DEAD) {
        if (!result.empty()) result += " | ";
        result += "DEAD";
    }
    
    return result;
}

