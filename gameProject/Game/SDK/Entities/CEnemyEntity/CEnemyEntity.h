#pragma once

#include <unordered_map>

#include "../CPlayerEntity/CPlayerEntity.h"
#include <memory>

enum CEnemyType {
    MELEE_ENEMY ,
    RANGED_ENEMY ,
    MAGIC_ENEMY ,
    BOSS_ENEMY ,            // Inimigo com muita vida e ataques variados
    SUICIDE_ENEMY ,         // Corre até o jogador e explode
    STEALTH_ENEMY ,         // Fica invisível ou se esconde até atacar
    TANK_ENEMY ,            // Muito resistente, mas lento
    SUMMONER_ENEMY ,        // Invoca outros inimigos
    FLYING_ENEMY ,          // Pode voar, ignorando obstáculos do mapa
    SNIPER_ENEMY ,          // Ataca de muito longe com precisão
    HEALER_ENEMY ,          // Cura aliados próximos
    POISON_ENEMY ,          // Aplica dano ao longo do tempo
    SHIELDED_ENEMY ,        // Tem escudo que precisa ser quebrado antes de causar dano
    TELEPORTER_ENEMY ,      // Pode se mover rapidamente entre pontos
    TRICKSTER_ENEMY        // Usa ilusões, clones ou habilidades de confusão
};

class CEnemyEntity : public CPlayerEntity {
private:

    CEnemyType enemyType;

public:

    CEnemyType getEnemyType( );

    static GVector2D findBestDirectionToPlayer( CBaseEntity * entity, GVector2D & toPlayer );

    CEnemyEntity( CBaseEntityConstructor builder , std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> attacks, CEnemyType type);
	CEnemyEntity( const CEnemyEntity & other );

    std::shared_ptr<CEnemyEntity> Clone( );
};