#include "CPlayerEntity.h"

#include <mutex>

#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder )
{
	this->attacks = attackHandler::Get( ).getAvailableLocalPlayerAttack( );
}


void CPlayerEntity::UseAttack( CBaseAttackType attack ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	if ( this->inAttackLoadingAnimation )
		return;

	if ( this->getEntityState( ) == CBaseEntityState::ATTACKING )
		return;

	if ( !CBaseEntityAnimation::isDifferentAnimationType( this->getEntityAnimations( )->getCurrentAnimationType( ) ,
		CBaseEntityAnimationType::ATTACKING_BACKWARD ) )
		return;

	if ( this->beingHit ) {
		return;
	}

	if ( this->attacks.find( attack ) != this->attacks.end( ) ) {
		this->inAttackLoadingAnimation = true;
		this->currentLoadingAttack = attack;
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

	if ( this->inAttackLoadingAnimation && EntityState != CBaseEntityState::ATTACKING )
	{
		//Started attacking rn
		this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	if ( this->beingHit && EntityState != CBaseEntityState::HURT )
	{
		//Started attacking rn
		this->AnimationCycleOnHurtInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	bool blockMovement = false;
	bool loopAnimation = true;
	bool updateAnimation = true;


	if ( !this->isAlive( ) ) {
		EntityState = CBaseEntityState::DEAD;
		loopAnimation = false;
	}
	else {
		//float delta = AngleDiff( lookingAngle , this->getMovementAngle( ) );
		//bool reverse = ( fabsf( delta ) > 90.0f );

		if ( this->currentAttack != nullptr ) {
			this->attacking = true;
			if ( this->currentAttack->getEntityAnimations( )->getAnimationCycle( ) ) {
				this->currentAttack = nullptr;
				this->attacking = false;
			}
		}

		

		if ( this->hasMovementRequest( ) ) {
			EntityState = CBaseEntityState::MOVING;
		}
		else
			EntityState = CBaseEntityState::STOPPED;

		if ( this->beingHit ) {
			bool hurtAnimationEnded = this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnHurtInit;

			if ( !hurtAnimationEnded )
				EntityState = CBaseEntityState::HURT;
			else
				this->beingHit = false;
		}
		else if ( this->inAttackLoadingAnimation ) {
			EntityState = CBaseEntityState::ATTACKING;
			bool attackAnimationEnded = this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnAttackInit;

			//Log::Print( "Current animationCyle: %d, AttackInit: %d, ended: %d" , this->getEntityAnimations( )->getAnimationCycle( ) , this->AnimationCycleOnAttackInit, attackAnimationEnded );

			std::shared_ptr<CBaseAttack> newAttack = nullptr;

			switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
			case CBaseAttackType_Melee:
				//insta throw on melee
				if ( !this->alreadyThrowedAttack ) {
					newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
					blockMovement = true;
				}

				//end attacking state when the animation & attack animation has ended
				if ( this->alreadyThrowedAttack && !this->attacking ) {
					this->inAttackLoadingAnimation = false;
					this->alreadyThrowedAttack = false;
					return;
				}

				break;
			case CBaseAttackType_Ranged:
				// throw attack when the attack animation ended
				if ( attackAnimationEnded ) {
					// if we dont throw it yet
					newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
					this->inAttackLoadingAnimation = false;
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

		if ( !blockMovement )
			this->move( );
		else
			this->clearMovementRequest( );

	}


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

void CPlayerEntity::Hit( int dmg ) {
	this->beingHit = true;
	this->setHealth( this->getHealth( ) - dmg );

	if ( this->isAlive( ) )
		EventManager::Get( ).Trigger( "localPlayer_hurt" );
	else
		EventManager::Get( ).Trigger( "localPlayer_dead" );
}

void CPlayerEntity::Respawn( ) {
	this->setHealth( this->getMaxHealth( ) );
	this->setEntityState( CBaseEntityState::STOPPED );
	this->setEntityLookingDirection( DIRECTION::FORWARD );
	this->getEntityAnimations( )->setCurrentAnimationType( CBaseEntityAnimationType::IDLE_FORWARD , false );
	this->getEntityAnimations( )->updateAnimation( true , false );
	this->beingHit = false;
	this->inAttackLoadingAnimation = false;
	this->alreadyThrowedAttack = false;
}