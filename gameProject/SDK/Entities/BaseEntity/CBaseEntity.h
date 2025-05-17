#pragma once
#include <string>
#include "../../math/Vector2D/GVector2D.h"

enum CBaseEntityType {
	ITEM,
	PLAYER,
	MOB
};

class CBaseEntity
{
	std::string Name;

	GVector2D entityPosition;
	int health;

	CBaseEntityType entityType;

public:

	CBaseEntity( std::string name, GVector2D initialPos, int initialHealth, CBaseEntityType ent_type )
		: Name( name ), entityPosition( initialPos ), health( initialHealth), entityType( ent_type) {};

	std::string GetEntityName( );
	GVector2D getEntityPosition( );
	CBaseEntityType getEntityType( );
	int getHealth( );

	void setHealth( int health );
	void setEntityPosition( GVector2D pos );
};