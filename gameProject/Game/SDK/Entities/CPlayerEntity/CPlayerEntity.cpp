#include "CPlayerEntity.h"

#include <mutex>

#include "../../../gameObjects/attackHandler/attackHandler.h"

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder ) {}

void CPlayerEntity::AddAttack( CBaseAttack * attack ) {
	attacks.push_back( attack );
}

void CPlayerEntity::UseAttack( int index ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	this->attacking = true;

	if ( index >= 0 && index < attacks.size( ) ) {
		std::shared_ptr<CBaseAttack> newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( index ) );
		if ( newAttack.get( ) != nullptr ) {
			this->currentAttack = newAttack.get( );
		}
	}
}

float AngleDiff( float a , float b ) {
	float diff = fmodf( a - b + 180.0f , 360.0f );
	if ( diff < 0 ) diff += 360.0f;
	return diff - 180.0f;
}

void CPlayerEntity::updateEntity( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	float lookingAngle = this->getLookingAngle( ).getDegrees( );
	DIRECTION localDirection = this->getEntityLookingDirection( );
	CBaseEntityState EntityState = this->getEntityState( );


	//float delta = AngleDiff( lookingAngle , this->getMovementAngle( ) );
	//bool reverse = ( fabsf( delta ) > 90.0f );

	if ( this->attacking ) {
		EntityState = CBaseEntityState::ATTACKING;
		if ( this->currentAttack == nullptr || this->currentAttack->getEntityAnimations()->getAnimationCycle() ) {
			this->currentAttack = nullptr;
			this->attacking = false;
		}
	}
	else {
		if ( lookingAngle > -45 && lookingAngle <= 45 )
			localDirection = ( DIRECTION::RIGHT );
		else if ( lookingAngle > 45 && lookingAngle <= 135 )
			localDirection = ( DIRECTION::BACKWARD );
		else if ( lookingAngle > 135 || lookingAngle <= -135 )
			localDirection = ( DIRECTION::LEFT );
		else
			localDirection = ( DIRECTION::FORWARD );

		if ( this->hasMovementRequest( ) ) {
			EntityState = CBaseEntityState::MOVING;
		}
		else
			EntityState = CBaseEntityState::STOPPED;
	}

	switch ( EntityState ) {
	case  CBaseEntityState::ATTACKING:
		this->clearMovementRequest( );
		break;
	case CBaseEntityState::MOVING:
		this->move( );
		break;
	case CBaseEntityState::STOPPED:

		break;
	}

	CBaseEntityAnimationType updateAnimation = CBaseEntity::getAnimationTypeBasedOnStateAndDirection( EntityState , localDirection );

	this->setEntityState( EntityState );
	this->setEntityLookingDirection( localDirection );
	this->getEntityAnimations( )->setCurrentAnimationType( updateAnimation , false );
	this->getEntityAnimations( )->updateAnimation( true, false );
}

bool CPlayerEntity::isAttacking( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	return this->attacking;
}
