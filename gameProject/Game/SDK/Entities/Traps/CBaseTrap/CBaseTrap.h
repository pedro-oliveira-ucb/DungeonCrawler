#pragma once
#include <mutex>

#include "../../CBaseEntity/CBaseEntity.h"

enum TrapType {
	SPIKES,
	EXPLOSIVE
};

class CBaseTrap : public CBaseEntity
{
	mutable std::mutex baseTrapMutex;

	bool initialized;
	bool activated;
	TrapType type;
	

public:
	CBaseTrap( CBaseEntityConstructor , TrapType );
	CBaseTrap( const CBaseTrap & other );

	virtual void applyEffect( );
	void initialize( GVector2D position );
	bool isInitialized( ) const; 
	void unitialize( );
	void activeTrap( );
	virtual void updateActivatedTrap( ) { }
	void deactivateTrap( );
	bool isActive( ) const;

	TrapType getTrapType( ) const;

	virtual std::unique_ptr<CBaseTrap> TrapClone( ) const = 0;
};

