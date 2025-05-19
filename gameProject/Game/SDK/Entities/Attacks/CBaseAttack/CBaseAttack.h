#pragma once

#include "../../../math/gAngle/GAngle.h"
#include "../../CBaseEntity/CBaseEntity.h"

enum CBaseAttackType {
	CBaseAttackType_Melee ,
	CBaseAttackType_Ranged ,
	CBaseAttackType_Spell
};

struct CBaseAttackConstructor {
    float damage;
	float range;
    float duration;
    float cooldown;
    float delay;
	float speed;

	CBaseAttackType type;
};

class CBaseAttack : public CBaseEntity {
    float damage;
    float range;
    float duration; // Duração do ataque
    float cooldown; // Tempo de recarga
    float delay;    // Tempo antes de causar dano
    bool isActive;  // Indica se o ataque está ativo
	CBaseAttackType type; // Tipo de ataque
	GVector2D gVector2D; // Posição do ataque
    GVector2D initialPos;
    float speed; // Velocidade do ataque (para ataques a distância)
   
public:
    CBaseAttack( CBaseEntityConstructor builder , CBaseAttackConstructor attackBuilder);
    CBaseAttack( const CBaseAttack & other ); // Construtor de cópia

    void updateAttackPosition(  ); // Atualiza o estado do ataque
    bool IsActive( ) const; // Verifica se o ataque está ativo
    float GetDamage( ) const;
    float GetRange( ) const;

    std::shared_ptr<CBaseAttack> Clone( ) {
        std::shared_ptr<CBaseAttack> clone = std::make_shared<CBaseAttack>( *this ); // Usa cópia
        return clone;
    }

    void Active(CBaseEntity * sender );
};
