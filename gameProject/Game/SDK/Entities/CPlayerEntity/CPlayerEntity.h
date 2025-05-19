#pragma once

#include <mutex>

#include "../../math/gAngle/GAngle.h"

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {

    std::vector<CBaseAttack *> attacks; // Lista de ataques do jogador
    bool attacking = false;

	CBaseAttack * currentAttack = nullptr; // Ataque atual do jogador

public:
    std::mutex localPlayerMutex;

    CPlayerEntity( CBaseEntityConstructor builder );

    void AddAttack( CBaseAttack * attack ); // Adiciona um ataque ao jogador
    void UseAttack( int index ); // Usa um ataque específico

    bool isAttacking( );

    void updateEntity( ) override;

};