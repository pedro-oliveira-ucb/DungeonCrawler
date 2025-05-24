#pragma once

#include <mutex>

#include <unordered_set>

#include "../../../math/gAngle/GAngle.h"
#include "../../CBaseEntity/CBaseEntity.h"





enum CBaseAttackType {
	CBaseAttackType_None,
	CBaseAttackType_Melee ,
	CBaseAttackType_Ranged ,
	CBaseAttackType_Spell
};

struct CBaseAttackConstructor {
	std::string Name;
	float damage;
	float range;
	float cooldown;
	float delay;
	float speed;
	GVector2D area;
	CBaseAttackType type;
};


class CBaseAttack : public CBaseEntity {

	std::mutex cBaseAttackMutex;

	std::string Name;
	float damage;
	float range;
	float cooldown; // Tempo de recarga
	float delay;    // Tempo antes de causar dano
	bool active;  // Indica se o ataque está ativo
	CBaseAttackType type; // Tipo de ataque
	GVector2D initialPos;
	float speed; // Velocidade do ataque (para ataques a distância)
	GVector2D area;

	virtual void otherActiveLogic( CBaseEntity * );
	virtual void otherDeactiveLogic( );

	std::unordered_set<CBaseEntity *> hitEntities;

public:
	CBaseAttack( CBaseEntityConstructor builder , CBaseAttackConstructor attackBuilder );
	CBaseAttack( const CBaseAttack & other ); // Construtor de cópia

	bool IsActive( ) const;
	float getDamage( ) const;
	float getRange( ) const;
	float getDelay( ) const;
	float getCooldown( ) const;
	float getSpeed( ) const;
	GVector2D getArea() const;
	GVector2D getInitialPosition( ) const;
	std::string getName( ) const;

	CBaseAttackType getAttackType( );

	bool hasAlreadyHit( CBaseEntity * entity ) {
		std::lock_guard<std::mutex>( this->cBaseAttackMutex );
		return hitEntities.find( entity ) != hitEntities.end( );
	}

	void registerHit( CBaseEntity * entity ) {
		std::lock_guard<std::mutex>( this->cBaseAttackMutex );
		hitEntities.insert( entity );
	}

	void resetHits( ) {
		std::lock_guard<std::mutex>( this->cBaseAttackMutex );
		hitEntities.clear( );
	}

	void Deactive( );
	virtual void updateAttackPosition( ); // Atualiza o estado do ataque
	virtual std::shared_ptr<CBaseAttack> Clone( );
	void Active( CBaseEntity * sender );

	void setInitialPosition( GVector2D position );
};

