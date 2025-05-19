#pragma once

#include "../../math/gAngle/GAngle.h"

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"


class CPlayerEntity : public CBaseEntity {
    int dashPower;
    std::vector<CBaseAttack *> attacks; // Lista de ataques do jogador

public:
    CPlayerEntity( CBaseEntityConstructor builder );

    void AddAttack( CBaseAttack * attack ); // Adiciona um ataque ao jogador
    void UseAttack( int index ); // Usa um ataque específico
    void UpdateAttacks( float deltaTime ); // Atualiza todos os ataques
};