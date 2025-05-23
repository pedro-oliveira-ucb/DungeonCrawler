#pragma once

#include <mutex>

#include "../../math/gAngle/GAngle.h"

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {

    std::unordered_map<CBaseAttackType, std::shared_ptr<CBaseAttack>> attacks; // Lista de ataques do jogador
    bool attacking = false;
    bool inAttackLoadingAnimation = false;
    bool alreadyThrowedAttack = false;
    CBaseAttackType currentLoadingAttack;
    int AnimationCycleOnAttackInit = -1;
    int lastTimeAttacking = -1;

    int AnimationCycleOnHurtInit = -1;
    int AnimationCycleOnStep = -1;

	CBaseAttack * currentAttack = nullptr; // Ataque atual do jogador

public:
    std::mutex localPlayerMutex;

    CPlayerEntity( CBaseEntityConstructor builder );

    void UseAttack( CBaseAttackType ); // Usa um ataque específico

    bool isAttacking( );

    void updateEntity( ) override;

    void Respawn(  );
};