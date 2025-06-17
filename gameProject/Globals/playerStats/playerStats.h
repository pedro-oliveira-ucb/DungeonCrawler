#pragma once
#include <mutex>

// Enum para identificar os stats de forma única para o switch-case
enum PlayerStatIndex {
	MaxHealth ,
	MaxStamina ,
	StaminaRegen ,
	StaminaLoss ,
	MeleeAttackDamageFactor ,
	MeleeAttackSpeedFactor ,
	MeleeAttackRangeFactor ,
	RangeAttackDamageFactor ,
	RangeAttackSpeedFactor ,
	RangeAttackRangeFactor ,
	Defense ,
	MovementSpeed,
	ManaDropProbability ,
	HealthDropProbability,
};

class playerStats
{
	std::mutex playerStatsMutex;

	float healthDropProbability = 20.f;
	float manaDropProbability = 30.f;

	float MaxHealth = 100.f;

	float MaxStamina = 100.f;
	float StaminaLossFactor = 50.0f;
	float StaminaRegenFactor = 30.0f;

	// Novos stats impactantes
	float MaxMana = 50.f;
	float ManaRegenFactor = 0.5f; // Ex: 0.5 mana por segundo
	float MovementSpeed = 100.f; // Ex: unidades por segundo

	float rangeAttackDamageFactor = 1.0f; // Fator de dano para ataques à distância
	float rangeAttackAreaFactor = 1.0f;
	float rangeAttackSpeedFactor = 1.0f; // Fator de velocidade de ataque para ataques à distância
	float rangeAttackRangeFactor = 1.0f;

	float meleeAttackDamageFactor = 1.0f; // Fator de dano para ataques corpo a corpo
	float meleeAttackAreaFactor = 1.0f; // Fator de área de ataque para ataques corpo a corpo
	float meleeAttackSpeedFactor = 1.0f; // Fator de velocidade de ataque para ataques corpo a corpo
	float meleeAttackRangeFactor = 1.0f; // Fator de alcance de ataque para ataques corpo a corpo

	float Defense = 0.f; // Redução de dano flat ou percentual

public:
	playerStats( );
	playerStats( const playerStats & other );
	// Getters
	float getHealthDropProbability();
	float getManaDropProbability();
	float getMaxHealth();
	float getMaxStamina();
	float getStaminaLossFactor();
	float getStaminaRegenFactor();
	float getMaxMana();
	float getManaRegenFactor();
	float getMovementSpeed();
	float getDefense();
	float getRangeAttackDamageFactor( );
	float getRangeAttackSpeedFactor( );
	float getRangeAttackAreaFactor( );
	float getRangeAttackRangeFactor( );
	float getMeleeAttackDamageFactor( );
	float getMeleeAttackSpeedFactor( );
	float getMeleeAttackAreaFactor( );
	float getMeleeAttackRangeFactor( );

	// Setters
	void setHealthDropProbability(float probability);
	void setManaDropProbability(float probability);
	void setMaxHealth(float maxHealth);
	void setMaxStamina(float maxStamina);
	void setStaminaLossFactor(float factor);
	void setStaminaRegenFactor(float factor);
	void setMaxMana(float maxMana);
	void setManaRegenFactor(float factor);
	void setMovementSpeed(float speed);
	void setDefense(float defense);
	void setRangeAttackDamageFactor( float factor );
	void setRangeAttackSpeedFactor( float factor );
	void setRangeAttackAreaFactor( float factor );
	void setRangeAttackRangeFactor( float factor );
	void setMeleeAttackDamageFactor( float factor );
	void setMeleeAttackSpeedFactor( float factor );
	void setMeleeAttackAreaFactor( float factor );
	void setMeleeAttackRangeFactor( float factor );
	
	void updatePlayer( );
};

