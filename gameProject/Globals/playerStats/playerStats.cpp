#include "playerStats.h"

// Getters
float playerStats::getHealthDropProbability() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return healthDropProbability;
}

float playerStats::getManaDropProbability() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return manaDropProbability;
}

float playerStats::getMaxHealth() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return MaxHealth;
}

float playerStats::getMaxStamina() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return MaxStamina;
}

float playerStats::getStaminaLossFactor() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return StaminaLossFactor;
}

float playerStats::getStaminaRegenFactor() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return StaminaRegenFactor;
}

float playerStats::getMaxMana() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return MaxMana;
}

float playerStats::getManaRegenFactor() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return ManaRegenFactor;
}

float playerStats::getMovementSpeed() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return MovementSpeed;
}

float playerStats::getAttackDamageFactor() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return AttackDamageFactor;
}

float playerStats::getAttackSpeedFactor() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return AttackSpeedFactor;
}

float playerStats::getDefense() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return Defense;
}

// Setters
void playerStats::setHealthDropProbability(float probability) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    healthDropProbability = probability;
}

void playerStats::setManaDropProbability(float probability) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    manaDropProbability = probability;
}

void playerStats::setMaxHealth(float maxHealth) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    MaxHealth = maxHealth;
}

void playerStats::setMaxStamina(float maxStamina) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    MaxStamina = maxStamina;
}

void playerStats::setStaminaLossFactor(float factor) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    StaminaLossFactor = factor;
}

void playerStats::setStaminaRegenFactor(float factor) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    StaminaRegenFactor = factor;
}

void playerStats::setMaxMana(float maxMana) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    MaxMana = maxMana;
}

void playerStats::setManaRegenFactor(float factor) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    ManaRegenFactor = factor;
}

void playerStats::setMovementSpeed(float speed) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    MovementSpeed = speed;
}

void playerStats::setAttackDamageFactor(float factor) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    AttackDamageFactor = factor;
}

void playerStats::setAttackSpeedFactor(float factor) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    AttackSpeedFactor = factor;
}

void playerStats::setDefense(float defense) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    Defense = defense;
}
