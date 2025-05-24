#include "CPlayerEntity.h"

#include <mutex>

#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
	: CBaseEntity( builder )
{
	this->attacks = attackHandler::Get( ).getAvailableLocalPlayerAttack( );
	if ( this->attacks.empty( ) )
		return;

	for ( auto it = this->attacks.begin( ); it != attacks.end( ); ++it ) {
		this->attackUseTime.emplace( it->first , 0 );
	}
}

void CPlayerEntity::UseAttack( CBaseAttackType attack ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	if ( this->inAttackLoadingAnimation )
		return;

	if ( this->hasEntityState( CBaseEntityState::ATTACKING ) )
		return;

	if ( this->attackUseTime.at( attack ) < this->attacks.at( attack )->getCooldown( ) ) {
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

void CPlayerEntity::updateAnimationCycles( std::uint32_t previousEntityState ) {
	// Detecta o início de uma animação de ataque
	if ( this->inAttackLoadingAnimation && !( previousEntityState & CBaseEntityState::ATTACKING ) ) {
		this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	// Detecta quando o jogador começa a receber dano
	if ( this->isBeingHit( ) && !( previousEntityState & CBaseEntityState::HURT ) ) {
		this->AnimationCycleOnHurtInit = this->getEntityAnimations( )->getAnimationCycle( );
	}
}

void CPlayerEntity::updateMovementSounds( std::uint32_t previousEntityState ) {
	// Emite eventos de som de passos quando o jogador está em movimento
	if ( ( previousEntityState & CBaseEntityState::MOVING ) || ( previousEntityState & CBaseEntityState::RUNNING ) ) {
		bool running = ( previousEntityState & CBaseEntityState::RUNNING );
		int stepTrigger = running ? 1 : 2;

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

void CPlayerEntity::updateCurrentAttackState( ) {
	// Verifica e atualiza o estado de ataque atual
	if ( this->currentAttack != nullptr ) {
		this->attacking = true;
		if ( this->currentAttack->getEntityAnimations( )->getAnimationCycle( ) ) {
			this->currentAttack = nullptr;
			this->attacking = false;
		}
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
		reverseAnimation = ( fabsf( delta ) > 90.0f );
	}
	else {
		state |= CBaseEntityState::STOPPED;
	}
}

void CPlayerEntity::handleHurtState( std::uint32_t & state ) {
	bool hurtAnimationEnded = this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnHurtInit;

	if ( !hurtAnimationEnded ) {
		state |= CBaseEntityState::HURT;
	}
	else {
		this->stopBeingHit( );
	}

	reverseAnimation = false;
}

std::shared_ptr<CBaseAttack> CPlayerEntity::handleAttackState( std::uint32_t & state ) {
	state |= CBaseEntityState::ATTACKING;
	bool attackAnimationEnded = this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnAttackInit;
	std::shared_ptr<CBaseAttack> newAttack = nullptr;

	switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
	case CBaseAttackType_Melee:
		// Para ataques corpo-a-corpo, lança o ataque imediatamente
		if ( !this->alreadyThrowedAttack ) {
			newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
		}

		// Finaliza o estado de ataque quando a animação termina
		if ( this->alreadyThrowedAttack && attackAnimationEnded ) {
			this->inAttackLoadingAnimation = false;
			this->alreadyThrowedAttack = false;
			state &= ~CBaseEntityState::ATTACKING;
		}
		break;

	case CBaseAttackType_Ranged:
		// Para ataques à distância, lança o ataque quando a animação termina
		if ( attackAnimationEnded ) {
			newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
			this->inAttackLoadingAnimation = false;
			state &= ~CBaseEntityState::ATTACKING;
		}
		break;
	}

	// Configura parâmetros de animação baseados no estado de ataque
	if ( state & CBaseEntityState::ATTACKING ) {
		//loopAnimation = false;
		reverseAnimation = false;
	}
	else {
		this->attackUseTime.at( this->currentLoadingAttack ) = 0;
	}

	return newAttack;
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

		if ( this->inAttackLoadingAnimation ) {
			std::shared_ptr<CBaseAttack> newAttack = handleAttackState( newEntityState );

			if ( newAttack.get( ) != nullptr ) {
				this->currentAttack = newAttack.get( );
				this->alreadyThrowedAttack = true;
				this->attacking = true;
			}
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

	Log::Print( "EntityState: %s" , CBaseEntityAnimation::getAnimationTypeName( updatedAnimationType ).c_str( ) );

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
		this->attackUseTime.at( it->first )++;
	}
}

void CPlayerEntity::updateEntity( ) {
	std::lock_guard<std::mutex> lock( localPlayerMutex );
	this->lookingAngle = this->getLookingAngle( ).getDegrees( );
	this->localDirection = this->getEntityLookingDirection( );
	std::uint32_t previousEntityState = this->getEntityStates( );

	// Atualiza os ciclos de animação e estados iniciais
	updateAnimationCycles( previousEntityState );

	// Gerencia eventos de som de movimento
	updateMovementSounds( previousEntityState );

	// Atualiza o estado do ataque atual
	updateCurrentAttackState( );

	// Variáveis para controle do comportamento da entidade
	this->blockMovement = false;
	this->loopAnimation = true;
	this->updateAnimation = true;
	this->reverseAnimation = false;

	// Determina o novo estado da entidade
	this->newEntityState = determineEntityState( lookingAngle , localDirection );

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