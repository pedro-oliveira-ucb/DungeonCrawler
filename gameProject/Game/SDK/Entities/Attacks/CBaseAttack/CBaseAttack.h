#pragma once

#include "../../../math/gAngle/GAngle.h"
#include "../../CBaseEntity/CBaseEntity.h"

class CBaseAttack : public CBaseEntity {
    float damage;
    float range;
    float duration; // Dura��o do ataque
    float cooldown; // Tempo de recarga
    float delay;    // Tempo antes de causar dano
    bool isActive;  // Indica se o ataque est� ativo
    float timer;    // Controla a dura��o do ataque

public:
    CBaseAttack( CBaseEntityConstructor builder , float damage , float range , float duration , float cooldown , float delay );

    void Activate( ); // Ativa o ataque
    void Update( float deltaTime ); // Atualiza o estado do ataque
    bool IsActive( ) const; // Verifica se o ataque est� ativo
    float GetDamage( ) const;
    float GetRange( ) const;
};
