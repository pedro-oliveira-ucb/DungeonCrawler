#pragma once

#include "../CBaseEntity/CBaseEntity.h"
#include "../Attacks/CBaseAttack/CBaseAttack.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class CEnemyEntity : public CBaseEntity {
private:
    std::unordered_map<CBaseAttackType, std::shared_ptr<CBaseAttack>> attacks;
    bool attacking = false;
    bool inAttackLoadingAnimation = false;
    bool alreadyThrowedAttack = false;
    bool beingHit = false;

    CBaseAttackType currentLoadingAttack = CBaseAttackType::CBaseAttackType_None;
    CBaseAttack* currentAttack = nullptr;

    int AnimationCycleOnAttackInit = -1;
    int AnimationCycleOnHurtInit   = -1;

    float attackCooldown = 1.5f;  // Exemplo de cooldown
    float attackDelay    = 0.5f;  // Exemplo de delay
    float attackTimer    = 0.0f;  // Timer controlado no update

public:
    std::mutex enemyMutex;

    CEnemyEntity(CBaseEntityConstructor builder);

    void updateEntity() override;

    bool isAttacking();

    void Respawn( );

    void UseAttack(CBaseAttackType type);
    void Hit(int damage);
};