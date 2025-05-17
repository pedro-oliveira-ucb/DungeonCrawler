#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"

enum CBaseEntityType {
	ITEM,
	PLAYER,
	MOB,
	ATTACK
};

enum CBaseEntityMovementDirection {
	MOVEMENT_FORWARD,
	MOVEMENT_BACKWARD,
	MOVEMENT_LEFT,
	MOVEMENT_RIGHT
};

class CBaseEntity
{
	std::string Name;

	GVector2D entityPosition;
	int health;

	CBaseEntityType entityType;
	CBaseEntityMovementDirection entityMovementDirection = MOVEMENT_FORWARD;

public:

	CBaseEntity( std::string name, GVector2D initialPos, int initialHealth, CBaseEntityType ent_type )
		: Name( name ), entityPosition( initialPos ), health( initialHealth), entityType( ent_type) {};

	std::string GetEntityName( );
	GVector2D getEntityPosition( );
	CBaseEntityType getEntityType( );
	CBaseEntityMovementDirection getEntityMovementDirection();
	int getHealth( );

	void setEntityMovementDirection( CBaseEntityMovementDirection move );
	void setHealth( int health );
	void setEntityPosition( GVector2D pos );
};