#include "CBaseEntity.h"

#include "../../../Utils/Log/Log.h"

CBaseEntity::CBaseEntity( const CBaseEntity & other ) {
	this->Name = other.Name;
	this->entityPosition = other.entityPosition;
	this->health = other.health;
	this->entityType = other.entityType;
	this->entityMovementDirection = other.entityMovementDirection;
	this->entityState = other.entityState;
	this->entityAnimations = other.entityAnimations;
	this->movementsRequest = other.movementsRequest;
	this->lookingAngle = other.lookingAngle;
	this->movementAngle = other.movementAngle;
	this->movementSpeed = other.movementSpeed;
	this->entityHitbox = other.entityHitbox;

	Log::Print( "[%s] Copy constructor called" , this->GetEntityName( ).c_str() );
}

CBaseEntity::CBaseEntity( CBaseEntityConstructor builder ) {
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
	GVector2D finalMovement = GVector2D(0.f, 0.f);

	for ( auto move : this->movementsRequest ) {
		switch ( move ) {
		case CBaseEntityMovementDirection::MOVEMENT_FORWARD:
			finalMovement.y -= movementSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_BACKWARD:
			finalMovement.y += movementSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_LEFT:
			finalMovement.x -= movementSpeed;
			break;
		case CBaseEntityMovementDirection::MOVEMENT_RIGHT:
			finalMovement.x += movementSpeed;
			break;
		}
	}

	this->movementsRequest.clear( );

	if ( finalMovement.x != 0 || finalMovement.y != 0 ) {
		// Calcula o ângulo em radianos

		float angleRadians = std::atan2(finalMovement.y , finalMovement.x );

		// Converte para graus, se quiser
		float angleDegrees = angleRadians * ( 180.0f / static_cast< float >( M_PI ) );

		// Salva o ângulo se você tiver uma variável pra isso
		this->movementAngle = angleDegrees; // Exemplo: float lastMovementAngle;

		// Atualiza a posição
		this->entityPosition.x += finalMovement.x;
		this->entityPosition.y += finalMovement.y;
	}
}

bool CBaseEntity::hasMovementRequest( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex );
	return !this->movementsRequest.empty( );
}

void CBaseEntity::updateEntity( ) {



}

CBaseEntityAnimationType animationTable[ 5 ][ 4 ] = {
	// STOPPED
	{ IDLE_LEFT, IDLE_RIGHT, IDLE_FORWARD, IDLE_BACKWARD },
	// MOVING
	{ WALKING_LEFT, WALKING_RIGHT, WALKING_FORWARD, WALKING_BACKWARD },
	// ATTACKING
	{ ATTACKING_LEFT, ATTACKING_RIGHT, ATTACKING_FORWARD, ATTACKING_BACKWARD },
	// HURT
	{ HURT_LEFT, HURT_RIGHT, HURT_FORWARD, HURT_BACKWARD },
	//DEAD
	{ DEAD_LEFT, DEAD_RIGHT, DEAD_FORWARD, DEAD_BACKWARD }
};

CBaseEntityAnimationType CBaseEntity::getAnimationTypeBasedOnStateAndDirection( CBaseEntityState state , DIRECTION direction ) {
	if ( state < CBaseEntityState::STOPPED || state > CBaseEntityState::DEAD ||
		direction < DIRECTION::LEFT || direction > DIRECTION::BACKWARD ) {
		return IDLE_FORWARD; // fallback
	}

	return animationTable[ state ][ direction ];
}

bool CBaseEntity::isAlive( ) {
	return this->health > 0;
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

