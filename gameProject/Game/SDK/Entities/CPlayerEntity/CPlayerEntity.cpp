#include "CPlayerEntity.h"

#include <mutex>

#include "../../../../Globals/Globals.h"

#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"


CPlayerEntity::CPlayerEntity( const CPlayerEntity & other )
	:CBaseEntity( other )
{
	this->attacks = other.attacks;
	this->minimumAttackDistance = other.minimumAttackDistance;
	this->availableAttacksInternal = other.availableAttacksInternal;
}

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder , std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> _attacks )
	: CBaseEntity( builder ) // Chama explicitamente o construtor de CBaseEntity
{
	this->attacks = _attacks;
	if ( this->attacks.empty( ) )
		return;

	float minAttackDis = 999999;

	for ( auto it = this->attacks.begin( ); it != this->attacks.end( ); ++it ) {
		if ( it->second->getRange( ) < minAttackDis ) {
			minAttackDis = it->second->getRange( );
		}
		this->availableAttacksInternal.emplace_back( it->second->getAttackType( ) );

		this->attackUseTime.emplace( it->first , now );
	}

	this->minimumAttackDistance = minAttackDis;
}

void CPlayerEntity::getMinimumDistanceToAttack( float * buffer ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	*buffer = minimumAttackDistance;
}

std::vector<CBaseAttackType> CPlayerEntity::getAvailableAttacks( ) {
	return this->availableAttacksInternal;
}

void CPlayerEntity::initializePlayerAttacks( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	float minAttackDis = 999999;

	for ( auto it = this->attacks.begin( ); it != this->attacks.end( ); ++it ) {
		if ( it->second->getRange( ) < minAttackDis ) {
			minAttackDis = it->second->getRange( );
		}
		this->availableAttacksInternal.emplace_back( it->second->getAttackType( ) );

		this->attackUseTime.emplace( it->first , now );
	}

	this->minimumAttackDistance = minAttackDis;
}

void CPlayerEntity::UseAttack( CBaseAttackType attack ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	if ( this->inAttackLoadingAnimation )
		return;

	if ( this->hasEntityState( CBaseEntityState::ATTACKING ) )
		return;

	if ( this->attackUseTime.find( attack ) == this->attackUseTime.end( ) )
		return;

	auto lastUse = this->attackUseTime.at( attack );
	// Calcula delta time em segundos
	std::chrono::duration<float> delta = now - lastUse;

	if ( delta.count( ) < this->attacks.at( attack )->getCooldown( ) ) {
		return;
	}

	if ( !CBaseEntityAnimation::isDifferentAnimationType( this->getEntityAnimations( )->getCurrentAnimationType( ) ,
		CBaseEntityAnimationType::ATTACKING_BACKWARD ) )
		return;

	if ( this->isBeingHit( ) ) {
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

void CPlayerEntity::updateAnimationCycles( ) {
	// Detecta o início de uma animação de ataque
	if ( this->inAttackLoadingAnimation && !( previousEntityState & CBaseEntityState::ATTACKING ) ) {
		this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	// Detecta quando o jogador começa a receber dano
	if ( this->isBeingHit( ) && !( previousEntityState & CBaseEntityState::HURT ) ) {
		this->AnimationCycleOnHurtInit = this->getEntityAnimations( )->getAnimationCycle( );
	}
}

void CPlayerEntity::updateMovementSounds( ) {
	// Emite eventos de som de passos quando o jogador está em movimento
	if ( ( previousEntityState & CBaseEntityState::MOVING ) || ( previousEntityState & CBaseEntityState::RUNNING ) ) {
		bool running = ( previousEntityState & CBaseEntityState::RUNNING );
		int stepTrigger = running ? 15 : 16;

		if ( this->AnimationCycleOnStep != this->getEntityAnimations( )->getAnimationCycle( ) ) {
			this->AnimationCycleOnStep = this->getEntityAnimations( )->getAnimationCycle( );
		}
		else {
			if ( this->AnimationCycleSinceLastStep <= stepTrigger ) {
				this->AnimationCycleSinceLastStep++;
			}
			else {
				EventManager::Get( ).Trigger( this->GetEntityName( ) + "_walking" );
				this->AnimationCycleSinceLastStep = 0;
			}
		}
	}
	else {
		this->AnimationCycleSinceLastStep = 0;
	}
}


void CPlayerEntity::handleDeadState( std::uint32_t & state ) {
	state = CBaseEntityState::DEAD;
	loopAnimation = false;
}

void CPlayerEntity::handleMovementState( std::uint32_t & state ) {
	float delta = AngleDiff( lookingAngle , this->getMovementAngle( ) );

	if ( this->hasMovementRequest( ) ) {
		state |= this->isSprinting( ) ? CBaseEntityState::RUNNING : CBaseEntityState::MOVING;
		// reverseAnimation = ( fabsf( delta ) > 90.0f );
	}
	else {
		state |= CBaseEntityState::STOPPED;
	}
}

void CPlayerEntity::handleHurtState( std::uint32_t & state ) {
	//Aguarda até a animacao ser de hurt
	if ( CBaseEntityAnimation::isDifferentAnimationType( previousAnimationType , CBaseEntityAnimationType::HURT_BACKWARD ) ) {
		state |= CBaseEntityState::HURT;
		this->loopAnimation = false;
		this->getEntityAnimations( )->resetAnimation( );
		return;
	}

	bool hurtAnimationEnded = this->getEntityAnimations( )->isAnimationFinished( );

	if ( !hurtAnimationEnded ) {
		state |= CBaseEntityState::HURT;
	}
	else {
		this->stopBeingHit( );
	}

	reverseAnimation = false;
	this->loopAnimation = false;
}

bool CPlayerEntity::handleAttackState( std::uint32_t & state ) {

	bool sentAttack = false;

	reverseAnimation = false;

	if ( CBaseEntityAnimation::isDifferentAnimationType( previousAnimationType , CBaseEntityAnimationType::ATTACKING_FORWARD ) ) {
		state |= CBaseEntityState::ATTACKING;
		this->loopAnimation = false;
		EventManager::Get( ).Trigger( this->attacks.at( this->currentLoadingAttack )->GetEntityName( ) + "_attackLoad" );
		this->getEntityAnimations( )->resetAnimation( );
		return sentAttack;
	}

	bool attackAnimationEnded = this->getEntityAnimations( )->isAnimationFinished( );


	switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
	case CBaseAttackType_Melee:
		// Para ataques corpo-a-corpo, lança o ataque imediatamente
		if ( !this->alreadyThrowedAttack ) {
			attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
			sentAttack = true;
			this->alreadyThrowedAttack = true;
		}
		break;

	case CBaseAttackType_Ranged:
		// Para ataques à distância, lança o ataque quando a animação termina
		if ( attackAnimationEnded && !this->alreadyThrowedAttack ) {
			attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
			sentAttack = true;
			this->alreadyThrowedAttack = true;
		}
		break;
	}

	// Finaliza o estado de ataque quando a animação termina
	if ( !attackAnimationEnded ) {
		state |= CBaseEntityState::ATTACKING;
		loopAnimation = false;
	}
	else {
		this->inAttackLoadingAnimation = false;
		this->alreadyThrowedAttack = false;
		this->attackUseTime.at( this->currentLoadingAttack ) = now;
	}


	return sentAttack;
}

std::uint32_t CPlayerEntity::determineEntityState( float lookingAngle , DIRECTION localDirection ) {
	std::uint32_t newEntityState = 0;

	if ( !this->isAlive( ) ) {
		handleDeadState( newEntityState );
	}
	else {
		handleMovementState( newEntityState );

		if ( this->isBeingHit( ) ) {
			handleHurtState( newEntityState );

		}
		else if ( this->inAttackLoadingAnimation ) {
			handleAttackState( newEntityState );
		}

		// Gerencia o movimento do jogador
		if ( !blockMovement ) {
			this->move( );
		}
		else {
			this->clearMovementRequest( );
		}
	}

	return newEntityState;
}

void CPlayerEntity::updateEntityAnimationAndState( ) {
	// Determina o tipo de animação baseado no novo estado e direção
	CBaseEntityAnimationType updatedAnimationType = CBaseEntity::getAnimationTypeBasedOnStateAndDirection( newEntityState , localDirection );


	// Atualiza os estados e animações da entidade
	this->setEntityStates( newEntityState );
	this->setEntityLookingDirection( localDirection );
	this->getEntityAnimations( )->setCurrentAnimationType( updatedAnimationType , false );

	if ( updateAnimation ) {
		this->getEntityAnimations( )->updateAnimation( loopAnimation , reverseAnimation );
	}
}

void CPlayerEntity::updateAttackCooldowns( ) {
	// Incrementa os contadores de cooldown para todos os ataques
	for ( auto it = this->attacks.begin( ); it != attacks.end( ); ++it ) {
		// this->attackUseTime.at( it->first )++;
	}
}

void CPlayerEntity::updateEntity( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	this->lookingAngle = this->getLookingAngle( ).getDegrees( );
	this->localDirection = this->getEntityLookingDirection( );
	previousEntityState = this->getEntityStates( );
	previousAnimationType = this->getEntityAnimations( )->getCurrentAnimationType( );

	if ( this->isAlive( ) ) {

		// Atualiza os ciclos de animação e estados iniciais
		updateAnimationCycles( );

		// Gerencia eventos de som de movimento
		updateMovementSounds( );

		// Variáveis para controle do comportamento da entidade
		this->blockMovement = false;
		this->loopAnimation = true;
		this->updateAnimation = true;
		this->reverseAnimation = false;

		// Determina o novo estado da entidade
		this->newEntityState = determineEntityState( lookingAngle , localDirection );
	}
	else {
		if ( CBaseEntityAnimation::isDifferentAnimationType( previousAnimationType , CBaseEntityAnimationType::DEAD_BACKWARD ) ) {
			this->newEntityState = CBaseEntityState::DEAD;
			this->loopAnimation = false;
			this->getEntityAnimations( )->resetAnimation( );
		}
		else {

			this->setDeathAnimationFinished( this->getEntityAnimations( )->isAnimationFinished( ) );
			this->loopAnimation = false;
			this->newEntityState = CBaseEntityState::DEAD;
		}
	}

	// Atualiza a animação e o estado da entidade
	updateEntityAnimationAndState( );

	// Atualiza os cooldowns dos ataques
	updateAttackCooldowns( );
}

bool CPlayerEntity::isAttacking( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	return this->inAttackLoadingAnimation;
}

void CPlayerEntity::Respawn( ) {
	this->setHealth( this->getMaxHealth( ) );

	this->clearEntityStates( );
	this->addEntityState( CBaseEntityState::STOPPED );

	CBaseEntityAnimationType updatedAnimationType =
		CBaseEntity::getAnimationTypeBasedOnStateAndDirection(
			this->getEntityStates( ) , this->getEntityLookingDirection( )
		);

	this->setEntityLookingDirection( DIRECTION::FORWARD );
	this->getEntityAnimations( )->setCurrentAnimationType( CBaseEntityAnimationType::IDLE_FORWARD , false );
	this->getEntityAnimations( )->updateAnimation( true , false );
	this->inAttackLoadingAnimation = false;
	this->alreadyThrowedAttack = false;
}