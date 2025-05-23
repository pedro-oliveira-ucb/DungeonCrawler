#include "CEnemyEntity.h"
#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"
#include "../CPlayerEntity/CPlayerEntity.h"
#include "../../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../../Managers/collisionManager/collisionManager.h"
#include "../../Events/EventManager.h"



CEnemyEntity::CEnemyEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder )
{
	// Inicialize os ataques do inimigo (pode ser customizado)
	this->attacks = attackHandler::Get( ).getAvailableLocalPlayerAttack( ); // ou getAvailableEnemyAttack()
}

CEnemyEntity::CEnemyEntity( const CEnemyEntity & other ) :
	CBaseEntity( other )
{
	this->attacks = other.attacks;
	this->attackCooldown = other.attackCooldown;
	this->attackDelay = other.attackDelay;
	this->attackTimer = other.attackTimer;
}

void CEnemyEntity::UseAttack( CBaseAttackType attack ) {
	if ( this->inAttackLoadingAnimation )
		return;
	if ( this->attacks.find( attack ) != this->attacks.end( ) ) {
		this->inAttackLoadingAnimation = true;
		this->currentLoadingAttack = attack;
	}
}

bool CEnemyEntity::isAttacking( ) {
	return this->inAttackLoadingAnimation;
}

std::shared_ptr<CEnemyEntity> CEnemyEntity::Clone( ) {
	std::shared_ptr<CEnemyEntity> clone = std::make_shared<CEnemyEntity>( *this ); // Usa c�pia
	return clone;
}

int sign( float x ) {
	return ( x > 0 ) - ( x < 0 );
}

GVector2D CEnemyEntity::findBestDirectionToPlayer( GVector2D & toPlayer ) {
	GVector2D dirs[ ] = {
		{ sign( toPlayer.x ), sign( toPlayer.y ) },                // Diagonal principal
		{ sign( toPlayer.x ), 0 }, { 0, sign( toPlayer.y ) },      // Eixos principais
		{ sign( toPlayer.x ), -sign( toPlayer.y ) },               // Diagonal invertida
		{ -sign( toPlayer.x ), sign( toPlayer.y ) },               // Outra diagonal invertida
		{ -sign( toPlayer.x ), 0 }, { 0, -sign( toPlayer.y ) }     // Dire��es opostas
	};

	for ( const auto & dir : dirs ) {
		GVector2D newPos = this->getEntityPosition( ) + dir * this->getMovementSpeed( );
		if ( CollisionManager::Get( ).CanMoveTo( this , newPos ) ) {
			return dir;
		}
	}

	return { 0, 0 }; // Sem dire��o v�lida
}


void CEnemyEntity::updateEntity( ) {
	std::lock_guard<std::mutex> lock( enemyMutex );

	// L�gica original
	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	if ( !player || !player->isAlive( ) )
		return;

	GVector2D myPos = this->getEntityPosition( );
	GVector2D playerPos = player->getEntityPosition( );
	GVector2D toPlayer = playerPos - myPos;
	float distance = toPlayer.length( );
	CBaseEntityState newState = this->getEntityState( );

	bool loopAnimation = true;

	if ( newState != CBaseEntityState::DEAD && !this->isAlive( ) ) {
		this->AnimationCycleOnDeathInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	if ( this->isAlive( ) ) {
		static bool lastMoveFailed = false;
		static GVector2D lastDirection = GVector2D( 0 , 0 );

		float angle = atan2( toPlayer.y , toPlayer.x ) * 180.0f / 3.14159265f;
		this->setLookingAngle( angle );

		// ---------------------------------------------------
		// L�gica de anima��o e estado, inspirada no CPlayerEntity

		if ( newState != CBaseEntityState::ATTACKING && this->inAttackLoadingAnimation ) {
			this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
		}

		if ( newState != CBaseEntityState::HURT && this->isBeingHit( ) ) {
			this->AnimationCycleOnHurtInit = this->getEntityAnimations( )->getAnimationCycle( );
		}


		if ( distance > 40.0f ) {
			GVector2D toPlayer = playerPos - myPos;
			GVector2D bestDir = findBestDirectionToPlayer( toPlayer );

			if ( bestDir.x != 0 ) {
				this->addMoveRequest( bestDir.x > 0 ? MOVEMENT_RIGHT : MOVEMENT_LEFT );
			}

			if ( bestDir.y != 0 ) {
				this->addMoveRequest( bestDir.y > 0 ? MOVEMENT_BACKWARD : MOVEMENT_FORWARD );
			}

			newState = CBaseEntityState::MOVING;
			this->move( );
		}
		else {
			attackTimer -= 0.1f;
			if ( attackTimer <= 0.0f && !this->isAttacking( ) ) {
				this->UseAttack( CBaseAttackType_Melee );
				attackTimer = attackCooldown;
			}
		}

		// Verifica se est� sendo atingido
		if ( this->isBeingHit( ) ) {
			bool hurtAnimationEnded =
				this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnHurtInit;
			if ( !hurtAnimationEnded ) {
				newState = CBaseEntityState::HURT;
			}
			else {
				this->stopBeingHit( );
			}
			loopAnimation = false;
		}
		// Verifica se est� em anima��o de ataque
		else if ( this->inAttackLoadingAnimation ) {
			newState = CBaseEntityState::ATTACKING;
			// Armazena frame inicial de ataque se necess�rio

			if ( this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnAttackInit ) {
				if ( distance <= 40.0f ) {
					player->Hit( 10 );
				}
				this->inAttackLoadingAnimation = false;
			}
			loopAnimation = false;
		}
	}
	else {
		loopAnimation = false;
		newState = CBaseEntityState::DEAD;
		if ( this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnDeathInit ) {
			this->setDeathAnimationFinished( true );
		}
	}
	// Atualiza estado e anima��o
	this->setEntityState( newState );
	CBaseEntityAnimationType updatedAnimationType =
		CBaseEntity::getAnimationTypeBasedOnStateAndDirection(
			newState , this->getEntityLookingDirection( )
		);
	this->getEntityAnimations( )->setCurrentAnimationType( updatedAnimationType , false );
	this->getEntityAnimations( )->updateAnimation( loopAnimation , false );

	// ---------------------------------------------------
}

void CEnemyEntity::Respawn( ) {
	this->setHealth( this->getMaxHealth( ) );
	this->setDeathAnimationFinished( false );
	this->setEntityState( CBaseEntityState::STOPPED );

	CBaseEntityAnimationType updatedAnimationType =
	CBaseEntity::getAnimationTypeBasedOnStateAndDirection(
		this->getEntityState() , this->getEntityLookingDirection( )
	);

	this->getEntityAnimations( )->setCurrentAnimationType( updatedAnimationType );
}