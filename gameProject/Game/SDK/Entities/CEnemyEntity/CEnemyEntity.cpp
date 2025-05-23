#include "CEnemyEntity.h"
#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../Events/EventManager.h"
#include "../../../Utils/Log/Log.h"
#include "../CPlayerEntity/CPlayerEntity.h"
#include "../../../gameObjects/entitiesHandler/entitiesHandler.h"



CEnemyEntity::CEnemyEntity(CBaseEntityConstructor builder)
    : CBaseEntity(builder)
{
    // Inicialize os ataques do inimigo (pode ser customizado)
    this->attacks = attackHandler::Get().getAvailableLocalPlayerAttack(); // ou getAvailableEnemyAttack()
}

void CEnemyEntity::UseAttack(CBaseAttackType attack) {
    if (this->inAttackLoadingAnimation)
        return;
    if (this->attacks.find(attack) != this->attacks.end()) {
        this->inAttackLoadingAnimation = true;
        this->currentLoadingAttack = attack;
    }
}

bool CEnemyEntity::isAttacking() {
    return this->inAttackLoadingAnimation;
}

void CEnemyEntity::Hit(int dmg) {
    this->beingHit = true;
    this->setHealth(this->getHealth() - dmg);
    // Aqui pode disparar eventos de morte/dano
}

void CEnemyEntity::updateEntity( ) {
    std::lock_guard<std::mutex> lock( enemyMutex );

    // Lógica original
    CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
    if ( !player || !player->isAlive( ) || !this->isAlive( ) )
        return;

    GVector2D myPos = this->getEntityPosition( );
    GVector2D playerPos = player->getEntityPosition( );
    GVector2D toPlayer = playerPos - myPos;
    float distance = toPlayer.length( );

    float angle = atan2( toPlayer.y , toPlayer.x ) * 180.0f / 3.14159265f;
    this->setLookingAngle( angle );

    if ( distance > 40.0f ) {
        if ( fabs( toPlayer.x ) > fabs( toPlayer.y ) )
            this->addMoveRequest( toPlayer.x > 0 ? MOVEMENT_RIGHT : MOVEMENT_LEFT );
        else
            this->addMoveRequest( toPlayer.y > 0 ? MOVEMENT_BACKWARD : MOVEMENT_FORWARD );

        this->move( );
    }
    else {
        attackTimer -= 0.1f;
        if ( attackTimer <= 0.0f && !this->isAttacking( ) ) {
            this->UseAttack( CBaseAttackType_Melee );
            attackTimer = attackCooldown;
        }
    }

    // Simula delay de ataque
    static float delayTimer = 0.0f;
    if ( this->inAttackLoadingAnimation ) {
        delayTimer += 0.1f;
        if ( delayTimer >= attackDelay ) {
            if ( distance <= 40.0f ) {
                player->Hit( 10 );
            }
            this->inAttackLoadingAnimation = false;
            delayTimer = 0.0f;
        }
    }

    // ---------------------------------------------------
    // Lógica de animação e estado, inspirada no CPlayerEntity
    CBaseEntityState newState = CBaseEntityState::STOPPED;

    // Verifica se está sendo atingido
    if ( this->beingHit ) {
        bool hurtAnimationEnded =
            this->getEntityAnimations( )->getAnimationCycle( ) > this->AnimationCycleOnHurtInit;
        if ( !hurtAnimationEnded ) {
            newState = CBaseEntityState::HURT;
        }
        else {
            this->beingHit = false;
        }
    }
    // Verifica se está em animação de ataque
    else if ( this->inAttackLoadingAnimation ) {
        newState = CBaseEntityState::ATTACKING;
        // Armazena frame inicial de ataque se necessário
        if ( this->AnimationCycleOnAttackInit < 0 ) {
            this->AnimationCycleOnAttackInit =
                this->getEntityAnimations( )->getAnimationCycle( );
        }
    }
    // Verifica se está se movendo
    else if ( this->hasMovementRequest( ) ) {
        newState = CBaseEntityState::MOVING;
    }

    // Atualiza estado e animação
    this->setEntityState( newState );
    CBaseEntityAnimationType updatedAnimationType =
        CBaseEntity::getAnimationTypeBasedOnStateAndDirection(
            newState , this->getEntityLookingDirection( )
        );
    this->getEntityAnimations( )->setCurrentAnimationType( updatedAnimationType , false );
    this->getEntityAnimations( )->updateAnimation( true , false );

    // ---------------------------------------------------
}

void CEnemyEntity::Respawn() {
	this->setHealth( this->getMaxHealth( ) );
}