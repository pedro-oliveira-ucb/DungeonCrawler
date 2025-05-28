#pragma once
#include "../CBaseItem/CBaseItem.h"

class CHealthItem : public CBaseItem
{
	int healAmount;
public:
	CHealthItem( CBaseEntityConstructor, ItemType itemType , int healAmount );
	CHealthItem( const CHealthItem & other );

	void applyEffect( ) override;
	std::unique_ptr<CBaseItem> ItemClone( ) const override;
};

