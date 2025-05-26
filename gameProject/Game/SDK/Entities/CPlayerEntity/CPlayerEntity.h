#pragma once

#include <mutex>

#include "../../math/gAngle/GAngle.h"

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {

    std::unordered_map<CBaseAttackType, std::shared_ptr<CBaseAttack>> attacks; // Lista de ataques do jogador
	std::unordered_map<CBaseAttackType , std::chrono::steady_clock::time_point> attackUseTime; // Lista de ciclos de animação dos ataques do jogador

    bool attacking = false;
    bool inAttackLoadingAnimation = false;
    bool alreadyThrowedAttack = false;

    CBaseAttackType currentLoadingAttack;
    int AnimationCycleOnAttackInit = -1;
    int lastTimeAttacking = -1;

    int AnimationCycleOnHurtInit = -1;
    int AnimationCycleOnStep = -1;

    int AnimationCycleSinceLastStep = 0;

	CBaseAttack * currentAttack = nullptr; // Ataque atual do jogador

    DIRECTION localDirection;
    bool blockMovement = false;
    bool loopAnimation = true;
    bool updateAnimation = true;
    bool reverseAnimation = false;
    float lookingAngle = 0.0f;
    std::uint32_t newEntityState;
    std::uint32_t previousEntityState;
	CBaseEntityAnimationType previousAnimationType;



private:
    // Funções auxiliares para modularizar updateEntity
    void updateAnimationCycles();
    void updateMovementSounds();
    void updateCurrentAttackState();
    
    std::uint32_t determineEntityState(float lookingAngle, DIRECTION localDirection);
    void handleDeadState( std::uint32_t  & state);
    void handleMovementState( std::uint32_t & state);
    void handleHurtState( std::uint32_t  & state);
    
    std::shared_ptr<CBaseAttack> handleAttackState( std::uint32_t & state );
    
    void updateEntityAnimationAndState();
    void updateAttackCooldowns();

public:
    std::mutex localPlayerMutex;

    CPlayerEntity( CBaseEntityConstructor builder );

    void UseAttack( CBaseAttackType ); // Usa um ataque específico

    bool isAttacking( );

    void updateEntity( ) override;

    void Respawn(  );
};