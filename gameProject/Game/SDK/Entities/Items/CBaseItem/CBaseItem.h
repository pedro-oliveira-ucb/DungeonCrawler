#pragma once
#include <mutex>

#include "../../CBaseEntity/CBaseEntity.h"

enum ItemType {
	HEALTH_ITEM
};

class CBaseItem : public CBaseEntity
{
	std::mutex baseItemMutex;

	bool active;
	ItemType type;

public:
	CBaseItem( CBaseEntityConstructor, ItemType );
	CBaseItem( const CBaseItem & other );

	virtual void applyEffect( );
	void Active( GVector2D );
	bool isActive( );
	void Deactive( );
	ItemType getItemType( );

	virtual std::unique_ptr<CBaseItem> ItemClone( ) const = 0;
};

