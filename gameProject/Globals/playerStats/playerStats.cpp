#include "playerStats.h"

#include "../../Game/Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Game/Handlers/attackHandler/attackHandler.h"



playerStats::playerStats( ) {

}

playerStats::playerStats( const playerStats & other )
{
    this->healthDropProbability = other.healthDropProbability;
    this->manaDropProbability = other.manaDropProbability;

    this->MaxHealth = other.MaxHealth;

    this->MaxStamina = other.MaxStamina;
    this->StaminaLossFactor = other.StaminaLossFactor;
    this->StaminaRegenFactor = other.StaminaRegenFactor;

	this->Defense = other.Defense; // Redução de dano flat ou percentual
	this->rangeAttackDamageFactor = other.rangeAttackDamageFactor; // Fator de dano do ataque à distância
	this->rangeAttackSpeedFactor = other.rangeAttackSpeedFactor; // Fator de velocidade do ataque à distância
	this->rangeAttackAreaFactor = other.rangeAttackAreaFactor; // Fator de área do ataque à distância
	this->meleeAttackDamageFactor = other.meleeAttackDamageFactor; // Fator de dano do ataque corpo a corpo
	this->meleeAttackSpeedFactor = other.meleeAttackSpeedFactor; // Fator de velocidade do ataque corpo a corpo
	this->meleeAttackAreaFactor = other.meleeAttackAreaFactor; // Fator de área do ataque corpo a corpo

    this->MaxMana = other.MaxMana;
    this->ManaRegenFactor = other.ManaRegenFactor; // Ex: 0.5 mana por segundo
    this->MovementSpeed = other.MovementSpeed; // Ex: unidades por segundo
    this->Defense = other.Defense; // Redução de dano flat ou percentual
}

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



float playerStats::getDefense() {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return Defense;
}

float playerStats::getRangeAttackDamageFactor( )
{
	// This function should return the factor for ranged attack damage.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->rangeAttackDamageFactor; 
}

float playerStats::getRangeAttackSpeedFactor( )
{
    // This function should return the factor for ranged attack speed.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->rangeAttackSpeedFactor;
}

float playerStats::getRangeAttackAreaFactor( )
{
    // This function should return the factor for ranged attack area.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->rangeAttackAreaFactor;
}

float playerStats::getRangeAttackRangeFactor( )
{
    // This function should return the factor for ranged attack range.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->rangeAttackRangeFactor;
}

float playerStats::getMeleeAttackDamageFactor( )
{
    // This function should return the factor for melee attack damage.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->meleeAttackDamageFactor;
}

float playerStats::getMeleeAttackSpeedFactor( )
{
    // This function should return the factor for melee attack speed.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->meleeAttackSpeedFactor;
}

float playerStats::getMeleeAttackAreaFactor( )
{
    // This function should return the factor for melee attack area.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	return this->meleeAttackAreaFactor;
}

float playerStats::getMeleeAttackRangeFactor( )
{
    // This function should return the factor for melee attack range.
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    return this->meleeAttackRangeFactor;
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


void playerStats::setDefense(float defense) {
    std::lock_guard<std::mutex> lock(playerStatsMutex);
    Defense = defense;
}

void playerStats::setRangeAttackDamageFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	rangeAttackDamageFactor = factor;
}

void playerStats::setRangeAttackSpeedFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	rangeAttackSpeedFactor = factor;      
}

void playerStats::setRangeAttackAreaFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	rangeAttackAreaFactor = factor;
}

void playerStats::setRangeAttackRangeFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	rangeAttackRangeFactor = factor; // Assuming range attack area factor is used for range as well
}

void playerStats::setMeleeAttackDamageFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	meleeAttackDamageFactor = factor;
}

void playerStats::setMeleeAttackSpeedFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	meleeAttackSpeedFactor = factor;
}

void playerStats::setMeleeAttackAreaFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	meleeAttackAreaFactor = factor;
}

void playerStats::setMeleeAttackRangeFactor( float factor )
{
    std::lock_guard<std::mutex> lock(playerStatsMutex);
	meleeAttackRangeFactor = factor; // Assuming melee attack area factor is used for range as well
}

void playerStats::updatePlayer( ){
    CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );
    if ( localPlayer == nullptr ) {
        return;
    }

    localPlayer->setEntityMaxStamina( this->MaxStamina );
    localPlayer->setEntityMaxStamina( this->MaxHealth );
    localPlayer->setStaminaLossRate( this->StaminaLossFactor );
    localPlayer->setStaminaRegenRate( this->StaminaRegenFactor );
    localPlayer->setMovementSpeed( this->MovementSpeed );
	localPlayer->setEntityDefense( this->Defense );

    auto it = attackHandler::Get( ).getAvailableLocalPlayerAttack( )->find( CBaseAttackType_Ranged );
    if(it != attackHandler::Get( ).getAvailableLocalPlayerAttack( )->end( )) {
        it->second->setDamage( this->rangeAttackDamageFactor );
        it->second->setSpeed( this->rangeAttackSpeedFactor );
		it->second->setRange( this->rangeAttackRangeFactor );
	}

    it = attackHandler::Get( ).getAvailableLocalPlayerAttack( )->find( CBaseAttackType_Melee );
    if(it != attackHandler::Get( ).getAvailableLocalPlayerAttack( )->end( )) {
        it->second->setDamage( this->meleeAttackDamageFactor );
        it->second->setSpeed( this->meleeAttackSpeedFactor );
		it->second->setRange( this->meleeAttackRangeFactor );
	}
}
