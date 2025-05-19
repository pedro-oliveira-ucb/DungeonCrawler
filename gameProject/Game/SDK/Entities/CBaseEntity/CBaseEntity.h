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
	STOPPED,
	MOVING,
	ATTACKING
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
	CBaseEntityState entityState;
	std::unique_ptr<CBaseEntityAnimation> entityAnimations;
	GVector2D movementsRequest;
	GAngle lookingAngle;
	float movementAngle;
	CBaseEntityHitbox entityHitbox;

public:

	CBaseEntity( CBaseEntityConstructor builder ) {
		this->Name = builder.Name;
		this->entityPosition = builder.entityPosition;
		this->health = builder.health;
		this->movementSpeed = builder.movementSpeed;
		this->entityType = builder.entityType;
		this->entityMovementDirection = builder.entityMovementDirection;
		this->entityAnimations = std::make_unique<CBaseEntityAnimation>(builder.entityAnimations);
		this->entityHitbox = CBaseEntityHitbox(entityAnimations->getSpriteSize( ));
		this->entityState = CBaseEntityState::STOPPED;
		this->movementsRequest = GVector2D( 0 , 0 );
		this->lookingAngle = GAngle( 0 );
		this->movementAngle = 0;
		this->entityLookingDirection = DIRECTION::FORWARD;
	}

	void addMoveRequest( CBaseEntityMovementDirection movement );
	void clearMovementRequest( );
	void move( );

	float getMovementAngle( );

	std::string GetEntityName( );
	GVector2D getEntityPosition( );
	CBaseEntityType getEntityType( );
	CBaseEntityMovementDirection getEntityMovementDirection();
	CBaseEntityAnimation * getEntityAnimations( );
	CBaseEntityState getEntityState( );
	DIRECTION getEntityLookingDirection( );
	GAngle getLookingAngle( );
	int getHealth( );
	CBaseEntityHitbox getHitbox( );

	void setEntityLookingDirection( DIRECTION direction );
	void setLookingAngle( float degrees );
	void setEntityState( CBaseEntityState state );
	void setEntityMovementDirection( CBaseEntityMovementDirection move );
	void setHealth( int health );
	void setEntityPosition( GVector2D pos );
};