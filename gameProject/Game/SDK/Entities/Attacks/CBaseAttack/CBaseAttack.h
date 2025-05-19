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
    float duration; // Dura��o do ataque
    float cooldown; // Tempo de recarga
    float delay;    // Tempo antes de causar dano
    bool isActive;  // Indica se o ataque est� ativo
	CBaseAttackType type; // Tipo de ataque
	GVector2D gVector2D; // Posi��o do ataque
    GVector2D initialPos;
    float speed; // Velocidade do ataque (para ataques a dist�ncia)
   
public:
    CBaseAttack( CBaseEntityConstructor builder , CBaseAttackConstructor attackBuilder);
    CBaseAttack( const CBaseAttack & other ); // Construtor de c�pia

    void updateAttackPosition(  ); // Atualiza o estado do ataque
    bool IsActive( ) const; // Verifica se o ataque est� ativo
    float GetDamage( ) const;
    float GetRange( ) const;

    std::shared_ptr<CBaseAttack> Clone( ) {
        std::shared_ptr<CBaseAttack> clone = std::make_shared<CBaseAttack>( *this ); // Usa c�pia
        return clone;
    }

    void Active(CBaseEntity * sender );
};
