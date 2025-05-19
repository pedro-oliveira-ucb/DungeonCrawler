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

void CBaseEntity::addMoveRequest( CBaseEntityMovementDirection movement ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	switch ( movement ) {
	case CBaseEntityMovementDirection::MOVEMENT_FORWARD:
		this->movementsRequest.y -= movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_BACKWARD:
		this->movementsRequest.y += movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_LEFT:
		this->movementsRequest.x -= movementSpeed;
		break;
	case CBaseEntityMovementDirection::MOVEMENT_RIGHT:
		this->movementsRequest.x += movementSpeed;
		break;
	}

}

float CBaseEntity::getMovementAngle( ) {
	return this->movementAngle;
}

void CBaseEntity::clearMovementRequest( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	this->movementsRequest = GVector2D( 0 , 0 );
}

void CBaseEntity::move( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	if ( this->movementsRequest.x != 0 || this->movementsRequest.y != 0 ) {
		// Calcula o ângulo em radianos

		float angleRadians = std::atan2( this->movementsRequest.y , this->movementsRequest.x );

		// Converte para graus, se quiser
		float angleDegrees = angleRadians * ( 180.0f / static_cast< float >( M_PI ) );

		// Salva o ângulo se você tiver uma variável pra isso
		this->movementAngle = angleDegrees; // Exemplo: float lastMovementAngle;

		// Atualiza a posição
		this->entityPosition.x += this->movementsRequest.x;
		this->entityPosition.y += this->movementsRequest.y;

		// Zera o movimento pendente
		this->movementsRequest = GVector2D( 0 , 0 );
	}
}

void CBaseEntity::setLookingAngle( float degress )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex );

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