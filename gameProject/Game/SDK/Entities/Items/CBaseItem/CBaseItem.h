#pragma once
#include <mutex>

#include "../../CBaseEntity/CBaseEntity.h"

enum ItemType {
	HEALTH_ITEM,
	KEY_ITEM,
	MANA_ITEM
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
	void Active( );
	bool isActive( );
	void Deactive( );
	ItemType getItemType( );

	virtual std::unique_ptr<CBaseItem> ItemClone( ) const = 0;
};

