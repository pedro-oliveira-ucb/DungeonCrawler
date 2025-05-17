#include "CBaseEntity.h"

std::string CBaseEntity::GetEntityName( ) {
	return this->Name;
}

GVector2D CBaseEntity::getEntityPosition( ) {
	return this->entityPosition;
}

CBaseEntityType CBaseEntity::getEntityType( ) {
	return this->entityType;
}

int CBaseEntity::getHealth( ) {
	return this->health;
}

void CBaseEntity::setHealth( int health ) {
	this->health = health;
}

void CBaseEntity::setEntityPosition( GVector2D pos ) {
	this->entityPosition = pos;
}