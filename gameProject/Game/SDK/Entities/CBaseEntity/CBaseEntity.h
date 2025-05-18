#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"
#include "../../Animation/CBaseEntityAnimation/CBaseEntityAnimation.h"

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

class CBaseEntity
{
	std::string Name;

	GVector2D entityPosition;
	int health;
	int movementSpeed;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;
	CBaseEntityState entityState;
	std::unique_ptr<CBaseEntityAnimation> entityAnimations;

public:

	CBaseEntity( CBaseEntityConstructor builder ) {
		this->Name = builder.Name;
		this->entityPosition = builder.entityPosition;
		this->health = builder.health;
		this->movementSpeed = builder.movementSpeed;
		this->entityType = builder.entityType;
		this->entityMovementDirection = builder.entityMovementDirection;
		this->entityAnimations = std::make_unique<CBaseEntityAnimation>(builder.entityAnimations);
	}

	void MoveEntity( CBaseEntityMovementDirection movement );

	std::string GetEntityName( );
	GVector2D getEntityPosition( );
	CBaseEntityType getEntityType( );
	CBaseEntityMovementDirection getEntityMovementDirection();
	CBaseEntityAnimation * getEntityAnimations( );
	CBaseEntityState getEntityState( );
	int getHealth( );

	void setEntityState( CBaseEntityState state );
	void setEntityMovementDirection( CBaseEntityMovementDirection move );
	void setHealth( int health );
	void setEntityPosition( GVector2D pos );
};