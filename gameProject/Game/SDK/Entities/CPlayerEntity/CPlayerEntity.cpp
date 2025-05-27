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

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder , std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks )
	: CBaseEntity( builder ) // Chama explicitamente o construtor de CBaseEntity
{
	this->attacks = attacks;
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

float CPlayerEntity::getMinimumDistanceToAttack( ) {
	return this->minimumAttackDistance;
}

std::vector<CBaseAttackType> CPlayerEntity::getAvailableAttacks( ) {
	return this->availableAttacksInternal;
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
	// Detecta o in�cio de uma anima��o de ataque
	if ( this->inAttackLoadingAnimation && !( previousEntityState & CBaseEntityState::ATTACKING ) ) {
		this->AnimationCycleOnAttackInit = this->getEntityAnimations( )->getAnimationCycle( );
	}

	// Detecta quando o jogador come�a a receber dano
	if ( this->isBeingHit( ) && !( previousEntityState & CBaseEntityState::HURT ) ) {
		this->AnimationCycleOnHurtInit = this->getEntityAnimations( )->getAnimationCycle( );
	}
}

void CPlayerEntity::updateMovementSounds( ) {
	// Emite eventos de som de passos quando o jogador est� em movimento
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

void CPlayerEntity::updateCurrentAttackState( ) {
	// Verifica e atualiza o estado de ataque atual
	if ( this->currentAttack != nullptr ) {
		this->attacking = true;
		if ( this->currentAttack->getEntityAnimations( )->isAnimationFinished( ) ) {
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
		// reverseAnimation = ( fabsf( delta ) > 90.0f );
	}
	else {
		state |= CBaseEntityState::STOPPED;
	}
}

void CPlayerEntity::handleHurtState( std::uint32_t & state ) {
	//Aguarda at� a animacao ser de hurt
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

std::shared_ptr<CBaseAttack> CPlayerEntity::handleAttackState( std::uint32_t & state ) {

	reverseAnimation = false;

	if ( CBaseEntityAnimation::isDifferentAnimationType( previousAnimationType , CBaseEntityAnimationType::ATTACKING_FORWARD ) ) {
		state |= CBaseEntityState::ATTACKING;
		this->loopAnimation = false;
		EventManager::Get( ).Trigger( this->attacks.at( this->currentLoadingAttack )->GetEntityName( ) + "_attackLoad" );
		this->getEntityAnimations( )->resetAnimation( );
		return nullptr;
	}

	bool attackAnimationEnded = this->getEntityAnimations( )->isAnimationFinished( );

	std::shared_ptr<CBaseAttack> newAttack = nullptr;

	switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
	case CBaseAttackType_Melee:
		// Para ataques corpo-a-corpo, lan�a o ataque imediatamente
		if ( !this->alreadyThrowedAttack ) {
			newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
			this->alreadyThrowedAttack = true;
		}
		break;

	case CBaseAttackType_Ranged:
		// Para ataques � dist�ncia, lan�a o ataque quando a anima��o termina
		if ( attackAnimationEnded ) {
			newAttack = attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
		}
		break;
	}

	// Finaliza o estado de ataque quando a anima��o termina
	if ( !attackAnimationEnded ) {
		state |= CBaseEntityState::ATTACKING;
		loopAnimation = false;
	}
	else {
		this->inAttackLoadingAnimation = false;
		this->alreadyThrowedAttack = false;
		this->attackUseTime.at( this->currentLoadingAttack ) = now;
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
		else if ( this->inAttackLoadingAnimation ) {
			std::shared_ptr<CBaseAttack> newAttack = handleAttackState( newEntityState );

			if ( newAttack.get( ) != nullptr ) {
				this->currentAttack = newAttack.get( );
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
	// Determina o tipo de anima��o baseado no novo estado e dire��o
	CBaseEntityAnimationType updatedAnimationType = CBaseEntity::getAnimationTypeBasedOnStateAndDirection( newEntityState , localDirection );


	// Atualiza os estados e anima��es da entidade
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

	// Atualiza os ciclos de anima��o e estados iniciais
	updateAnimationCycles( );

	// Gerencia eventos de som de movimento
	updateMovementSounds( );

	// Atualiza o estado do ataque atual
	updateCurrentAttackState( );

	// Vari�veis para controle do comportamento da entidade
	this->blockMovement = false;
	this->loopAnimation = true;
	this->updateAnimation = true;
	this->reverseAnimation = false;

	// Determina o novo estado da entidade
	this->newEntityState = determineEntityState( lookingAngle , localDirection );

	// Atualiza a anima��o e o estado da entidade
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