#include "CPlayerEntity.h"

#include <mutex>

std::mutex localPlayerMutex;

CPlayerEntity::CPlayerEntity( CBaseEntityConstructor builder )
    : CBaseEntity( builder ) {}

void CPlayerEntity::AddAttack( CBaseAttack * attack ) {
    attacks.push_back( attack );
}

void CPlayerEntity::UseAttack( int index ) {
    if ( index >= 0 && index < attacks.size( ) ) {
        attacks[ index ]->Activate( );
    }
}

void CPlayerEntity::UpdateAttacks( float deltaTime ) {
    for ( auto & attack : attacks ) {
        attack->Update( deltaTime );
    }
}