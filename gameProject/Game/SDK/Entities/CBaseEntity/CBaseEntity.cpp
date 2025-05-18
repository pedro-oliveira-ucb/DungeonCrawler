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

CBaseEntityState CBaseEntity::getEntityState( ) {
	return this->entityState;
}

int CBaseEntity::getHealth( ) {
	return this->health;
}

CBaseEntityAnimation * CBaseEntity::getEntityAnimations( ) {
	return this->entityAnimations.get();
}

void CBaseEntity::setHealth( int health ) {
	this->health = health;
}

void CBaseEntity::setEntityPosition( GVector2D pos ) {
	this->entityPosition = pos;
}

CBaseEntityMovementDirection CBaseEntity::getEntityMovementDirection( ) {
	return this->entityMovementDirection;
}

void CBaseEntity::setEntityMovementDirection( CBaseEntityMovementDirection move ) {
	this->entityMovementDirection = move;
}

void CBaseEntity::setEntityState( CBaseEntityState state ) {
	this->entityState = state;
}

void CBaseEntity::MoveEntity( CBaseEntityMovementDirection movement ) {
	switch ( movement ) {
	case CBaseEntityMovementDirection::MOVEMENT_FORWARD:
		this->entityPosition.y -= movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_BACKWARD:
		this->entityPosition.y += movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_LEFT:
		this->entityPosition.x -= movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_RIGHT:
		this->entityPosition.x += movementSpeed;
		break;
	}
}