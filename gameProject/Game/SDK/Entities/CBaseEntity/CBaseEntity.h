#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"
#include "../../Animation/CBaseEntityAnimation/CBaseEntityAnimation.h"
#include "../../math/gAngle/GAngle.h"

#include <mutex>

enum CBaseEntityType {
	ITEM,
	PLAYER,
	MOB,
	ATTACK
};

enum CBaseEntityState {
	STOPPED = 1 << 0 , // 00001
	MOVING = 1 << 1 , // 00010
	RUNNING = 1 << 2 , // 00100
	ATTACKING = 1 << 3 , // 00100
	HURT = 1 << 4 , // 01000
	DEAD = 1 << 5  // 10000
};

enum CBaseEntityMovementDirection {
	MOVEMENT_FORWARD,
	MOVEMENT_BACKWARD,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT
};

class CBaseEntityMovement {
public:
	int dx;
	int dy;
};

struct CBaseEntityConstructor {
	std::string Name;

	GVector2D entityPosition;
	int health;
	int movementSpeed;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
	CBaseEntityAnimationConstructor entityAnimations;
};


class CBaseEntityHitbox {
	GVector2D hitboxSize;
public:
	CBaseEntityHitbox( ) = default;
	CBaseEntityHitbox( GVector2D size ) {
		this->hitboxSize = size;
	}

	GVector2D getHitboxSize( ) {
		return this->hitboxSize;
	}
};
enum DIRECTION {
	LEFT ,
	RIGHT ,
	FORWARD ,
	BACKWARD
};

class CBaseEntity
{
	std::mutex cBaseMutex;
	std::string Name;

	GVector2D entityPosition;
	int health;
	int movementSpeed;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
	DIRECTION entityLookingDirection;
	std::uint32_t entityState = STOPPED;
	CBaseEntityAnimation entityAnimations;
	std::vector<CBaseEntityMovementDirection> movementsRequest;
	GAngle lookingAngle;
	float movementAngle;
	CBaseEntityHitbox entityHitbox;
	float maxHealth = 100;
	bool beingHit = false;
	bool finishedDeathAnimation = false;
	bool sprinting = false;
public:

	virtual void updateEntity( );

	static CBaseEntityAnimationType getAnimationTypeBasedOnStateAndDirection( std::uint32_t states , DIRECTION entityDirection );

	CBaseEntity( const CBaseEntity & other );
	CBaseEntity( CBaseEntityConstructor builder );

	float getEntityLookingDirectionBaseAngle( );

	bool hasMovementRequest( );

	void addMoveRequest( CBaseEntityMovementDirection movement );
	void clearMovementRequest( );
	void move( );

	bool isAlive( );

	bool deathAnimationFinished( );
	void setDeathAnimationFinished( bool finished );

	float getMovementAngle( );

	std::string GetEntityName( );
	GVector2D getEntityPosition( );
	CBaseEntityType getEntityType( );
	CBaseEntityMovementDirection getEntityMovementDirection();
	CBaseEntityAnimation * getEntityAnimations( );

	bool isSprinting( ) {
		std::scoped_lock lock( cBaseMutex );
		return this->sprinting;
	}

	void setSprinting( bool sprinting ) {
		std::scoped_lock lock( cBaseMutex );
		this->sprinting = sprinting;
	}

	void addEntityState( CBaseEntityState state ) {
		std::scoped_lock lock( cBaseMutex );
		entityState |= state;
	}

	void removeEntityState( CBaseEntityState state ) {
		std::scoped_lock lock( cBaseMutex );
		entityState &= ~state;
	}

	bool hasEntityState( CBaseEntityState state ) const {
		return ( entityState & state ) != 0;
	}

	void clearEntityStates( ) {
		std::scoped_lock lock( cBaseMutex );
		entityState = 0;
	}

	std::uint32_t getEntityStates( ) const {
		return entityState;
	}

	void setEntityStates( std::uint32_t states ) {
		std::scoped_lock lock( cBaseMutex );
		entityState = states;
	}

	DIRECTION getEntityLookingDirection( );
	GAngle getLookingAngle( );
	float getMovementSpeed( ) { return this->movementSpeed; }
	int getHealth( );
	CBaseEntityHitbox getHitbox( );

	void setEntityLookingDirection( DIRECTION direction );
	void setLookingAngle( float degrees );
	
	void setEntityMovementDirection( CBaseEntityMovementDirection move );
	void setHealth( int health );
	void setEntityPosition( GVector2D pos );
	float getMaxHealth( );
	void Hit( int damage );
	bool isBeingHit( );
	void stopBeingHit( );
};