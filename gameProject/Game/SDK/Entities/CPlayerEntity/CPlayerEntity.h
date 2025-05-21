#pragma once

#include <mutex>

#include "../../math/gAngle/GAngle.h"

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {

    std::vector<std::shared_ptr<CBaseAttack>> attacks; // Lista de ataques do jogador
    bool attacking = false;
    bool inAttackLoadingAnimation = false;
    bool alreadyThrowedAttack = false;
    int currentLoadingAttack = -1;
    int AnimationCycleOnAttackInit = -1;

	CBaseAttack * currentAttack = nullptr; // Ataque atual do jogador

public:
    std::mutex localPlayerMutex;

    CPlayerEntity( CBaseEntityConstructor builder );

    void UseAttack( int index ); // Usa um ataque específico

    bool isAttacking( );

    void updateEntity( ) override;

};