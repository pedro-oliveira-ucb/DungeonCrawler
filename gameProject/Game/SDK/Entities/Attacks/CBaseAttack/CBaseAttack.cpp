#include "CBaseAttack.h"

CBaseAttack::CBaseAttack( CBaseEntityConstructor builder , float damage , float range , float duration , float cooldown , float delay )
    : CBaseEntity( builder ) , damage( damage ) , range( range ) , duration( duration ) , cooldown( cooldown ) , delay( delay ) , isActive( false ) , timer( 0.0f ) {}

void CBaseAttack::Activate( ) {
    if ( isActive || timer > 0.0f ) return; // N�o ativa se j� estiver ativo ou em cooldown
    isActive = true;
    timer = duration;
}

void CBaseAttack::Update( float deltaTime ) {
    if ( isActive ) {
        timer -= deltaTime;
        if ( timer <= 0.0f ) {
            isActive = false; // Desativa o ataque ap�s a dura��o
            timer = cooldown; // Inicia o cooldown
        }
    }
    else if ( timer > 0.0f ) {
        timer -= deltaTime; // Atualiza o cooldown
    }
}

bool CBaseAttack::IsActive( ) const {
    return isActive;
}

float CBaseAttack::GetDamage( ) const {
    return damage;
}

float CBaseAttack::GetRange( ) const {
    return range;
}