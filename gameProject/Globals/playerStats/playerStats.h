#pragma once
#include <mutex>

class playerStats
{
	std::mutex playerStatsMutex;

	float healthDropProbability = 20.f;
	float manaDropProbability = 30.f;

	float MaxHealth = 100.f;

	float MaxStamina = 100.f;
	float StaminaLossFactor = 1.0f;
	float StaminaRegenFactor = 1.0f;

	// Novos stats impactantes
	float MaxMana = 50.f;
	float ManaRegenFactor = 0.5f; // Ex: 0.5 mana por segundo
	float MovementSpeed = 100.f; // Ex: unidades por segundo
	float AttackDamageFactor = 1.0f; // Multiplicador de dano
	float AttackSpeedFactor = 1.0f; // Multiplicador de velocidade de ataque (valores menores -> mais rápido)
	float Defense = 0.f; // Redução de dano flat ou percentual

public:
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
	float getAttackDamageFactor();
	float getAttackSpeedFactor();
	float getDefense();

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
	void setAttackDamageFactor(float factor);
	void setAttackSpeedFactor(float factor);
	void setDefense(float defense);
};

