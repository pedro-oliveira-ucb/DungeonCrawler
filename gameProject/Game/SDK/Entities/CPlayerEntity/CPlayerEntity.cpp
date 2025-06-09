#include "CPlayerEntity.h"

#include <mutex>

#include "../../../../Globals/Globals.h"

#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../../Utils/Log/Log.h"

#include "../../math/math.h"

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
	double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );

	for ( auto it = this->attacks.begin( ); it != this->attacks.end( ); ++it ) {
		if ( it->second->getRange( ) < minAttackDis ) {
			minAttackDis = it->second->getRange( );
		}
		this->availableAttacksInternal.emplace_back( it->second->getAttackType( ) );

		this->attackUseTime.emplace( it->first , currentGameTime );
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

	double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );

	for ( auto it = this->attacks.begin( ); it != this->attacks.end( ); ++it ) {
		if ( it->second->getRange( ) < minAttackDis ) {
			minAttackDis = it->second->getRange( );
		}
		this->availableAttacksInternal.emplace_back( it->second->getAttackType( ) );

		this->attackUseTime.emplace( it->first , currentGameTime );
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

	double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );
	double lastUse = this->attackUseTime.at( attack );
	// Calcula delta time em segundos
	double delta = currentGameTime - lastUse;

	if ( delta < this->attacks.at( attack )->getCooldown( ) ) {
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

void CPlayerEntity::updateAnimationCycles( ) {
	
}

void CPlayerEntity::updateMovementSounds( ) {
	// Emite eventos de som de passos quando o jogador est� em movimento
	if ( ( previousEntityState & CBaseEntityState::MOVING ) || ( previousEntityState & CBaseEntityState::RUNNING ) ) {
		bool running = ( previousEntityState & CBaseEntityState::RUNNING );
		double stepTrigger = running ? 0.5 : 0.8;

		double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );
		double deltaTime = currentGameTime - this->lastStepSoundTime;

		if ( deltaTime > stepTrigger ) {
			this->lastStepSoundTime = currentGameTime;
			EventManager::Get( ).Trigger( this->GetEntityName( ) + "_walking" );
		}
	}
}


void CPlayerEntity::handleDeadState( std::uint32_t & state ) {
	state = CBaseEntityState::DEAD;
	loopAnimation = false;
}

void CPlayerEntity::handleMovementState( std::uint32_t & state ) {
	float delta = math::AngleDiff( lookingAngle , this->getMovementAngle( ) );

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
		this->inAttackLoadingAnimation = false;
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
	state |= CBaseEntityState::ATTACKING;
	loopAnimation = false; // Anima��o de ataque nunca deve ser em loop
	reverseAnimation = false;

	// Se a anima��o de ataque acabou de ser selecionada, reseta ela
	if ( CBaseEntityAnimation::isDifferentAnimationType( previousAnimationType , CBaseEntityAnimationType::ATTACKING_FORWARD ) ) {
		this->getEntityAnimations( )->resetAnimation( );
		EventManager::Get( ).Trigger( this->attacks.at( this->currentLoadingAttack )->GetEntityName( ) + "_attackLoad" );
		return false;
	}

	bool animationJustFinished = this->getEntityAnimations( )->isAnimationFinished( );

	// Lan�a o ataque em um ponto espec�fico da anima��o (ex: meio do caminho)
	// Para simplificar aqui, vamos usar a l�gica original, mas aprimorada.
	// Para ataques Ranged, esperamos o fim. Para Melee, pode ser antes.
	bool shouldThrowAttack = false;
	switch ( this->attacks.at( this->currentLoadingAttack )->getAttackType( ) ) {
	case CBaseAttackType_Melee:
		// Dispara o ataque melee na primeira vez que entramos aqui, para dar a sensa��o de imediatismo
		if ( !this->alreadyThrowedAttack ) {
			shouldThrowAttack = true;
		}
		break;
	case CBaseAttackType_Ranged:
		// Dispara o ataque ranged quando a anima��o termina
		if ( animationJustFinished && !this->alreadyThrowedAttack ) {
			shouldThrowAttack = true;
		}
		break;
	}

	if ( shouldThrowAttack ) {
		attackHandler::Get( ).throwNewAttack( this , this->attacks.at( this->currentLoadingAttack ).get( ) );
		this->alreadyThrowedAttack = true;
	}

	// Se a anima��o terminou, limpa o estado de ataque
	if ( animationJustFinished ) {
		this->inAttackLoadingAnimation = false;
		this->alreadyThrowedAttack = false;
		this->attackUseTime.at( this->currentLoadingAttack ) = Globals::Get( ).getGame( )->getCurrentGameTime( );
		// O estado de ATTACKING ser� removido no pr�ximo ciclo de updateEntity, pois inAttackLoadingAnimation � false
	}

	return shouldThrowAttack;
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

	if ( this->isAlive( ) ) {

		this->setDeathAnimationFinished( false );

		// Atualiza os ciclos de anima��o e estados iniciais
		updateAnimationCycles( );

		// Gerencia eventos de som de movimento
		updateMovementSounds( );

		// Vari�veis para controle do comportamento da entidade
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