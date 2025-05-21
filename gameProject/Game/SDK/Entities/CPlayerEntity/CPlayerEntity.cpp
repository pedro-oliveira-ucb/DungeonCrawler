#include "CPlayerEntity.h"

#include <mutex>

#include "../../../gameObjects/attackHandler/attackHandler.h"

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder ) 
{
	this->attacks = attackHandler::Get( ).getAvailableLocalPlayerAttack( );
}


void CPlayerEntity::UseAttack( int index ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	if ( this->inAttackLoadingAnimation )
		return;

	if ( index >= 0 && index < attacks.size( ) ) {
		this->inAttackLoadingAnimation = true;
		this->currentLoadingAttack = index;
		this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
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

	bool loopAnimation = true;
	bool updateAnimation = true;

	//float delta = AngleDiff( lookingAngle , this->getMovementAngle( ) );
	//bool reverse = ( fabsf( delta ) > 90.0f );

	if ( this->currentAttack != nullptr ) {
		this->attacking = true;
		if ( this->currentAttack->getEntityAnimations( )->getAnimationCycle( ) ) {
			this->currentAttack = nullptr;
			this->attacking = false;
		}
	}

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

	if ( this->inAttackLoadingAnimation ) {
		EntityState = CBaseEntityState::ATTACKING;
		bool attackAnimationEnded = this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnAttackInit;


		std::shared_ptr<CBaseAttack> newAttack = nullptr;

		switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
		case CBaseAttackType_Melee:
			//insta throw on melee
			if ( this->currentAttack == nullptr && !this->alreadyThrowedAttack) {
				newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get());
			}
				
			//end attacking state when the animation & attack animation has ended
			if ( this->inAttackLoadingAnimation && this->alreadyThrowedAttack && !this->attacking ) {
				this->inAttackLoadingAnimation = false;
				this->alreadyThrowedAttack = false;
			}

			break;
		case CBaseAttackType_Ranged:
			// throw attack when the attack animation ended
			if ( attackAnimationEnded && !this->alreadyThrowedAttack ) {
				newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get() );
			}

			//end attacking state when the animation has ended
			if ( attackAnimationEnded ) {
				this->inAttackLoadingAnimation = false;
				this->alreadyThrowedAttack = false;
			}

			break;
		}

		if ( newAttack.get( ) != nullptr ) {
			this->currentAttack = newAttack.get( );
			this->alreadyThrowedAttack = true;
		}

		loopAnimation = false;
	}


	switch ( EntityState ) {
	case  CBaseEntityState::ATTACKING:

		break;
	case CBaseEntityState::MOVING:

		break;
	case CBaseEntityState::STOPPED:

		break;
	}

	this->move( );

	CBaseEntityAnimationType updatedAnimationType = CBaseEntity::getAnimationTypeBasedOnStateAndDirection( EntityState , localDirection );

	this->setEntityState( EntityState );
	this->setEntityLookingDirection( localDirection );
	this->getEntityAnimations( )->setCurrentAnimationType( updatedAnimationType , false );
	if ( updateAnimation )
		this->getEntityAnimations( )->updateAnimation( loopAnimation , false );
}

bool CPlayerEntity::isAttacking( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	return this->inAttackLoadingAnimation;
}
