#pragma once

#include <mutex>
#include <unordered_map>


#include "../../math/gAngle/GAngle.h"
#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {

    std::unordered_map<CBaseAttackType, std::shared_ptr<CBaseAttack>> attacks; // Lista de ataques do jogador
	std::unordered_map<CBaseAttackType , double> attackUseTime; // Lista de ciclos de animação dos ataques do jogador

    float deltaTime = 0.0f;
    float currentTime = 0.0f;
    float lastWalkingTime = 0.0f;

    bool inAttackLoadingAnimation = false;
    bool alreadyThrowedAttack = false;

    CBaseAttackType currentLoadingAttack;
    int AnimationCycleOnAttackInit = -1;
    int lastTimeAttacking = -1;
    double lastStepSoundTime = -1;

    double AnimationCycleSinceLastStep = 0;

    DIRECTION localDirection;
    bool blockMovement = false;
    bool loopAnimation = true;
    bool updateAnimation = true;
    bool reverseAnimation = false;
    float lookingAngle = 0.0f;
    std::uint32_t newEntityState;
    std::uint32_t previousEntityState;
	CBaseEntityAnimationType previousAnimationType;

	float minimumAttackDistance = 0.0f; // Distância mínima para atacar

private:
    // Funções auxiliares para modularizar updateEntity
    void updateAnimationCycles();
    void updateMovementSounds();
  
    
    std::uint32_t determineEntityState(float lookingAngle, DIRECTION localDirection);
    void handleDeadState( std::uint32_t  & state);
    void handleMovementState( std::uint32_t & state);
    void handleHurtState( std::uint32_t  & state);
    
    bool handleAttackState( std::uint32_t & state );
    
    void updateEntityAnimationAndState();
    void updateAttackCooldowns();

	std::vector< CBaseAttackType > availableAttacksInternal;

public:
    std::mutex localPlayerMutex;

 
    void initializePlayerAttacks( );

	CPlayerEntity( CBaseEntityConstructor builder , std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks );
	CPlayerEntity( const CPlayerEntity & other );

    void UseAttack( CBaseAttackType ); // Usa um ataque específico

    bool isAttacking( );

    void updateEntity( ) override;

    void Respawn(  );

	std::vector< CBaseAttackType> getAvailableAttacks( ); // Retorna os ataques disponíveis do jogador

	void getMinimumDistanceToAttack( float * buffer );
};