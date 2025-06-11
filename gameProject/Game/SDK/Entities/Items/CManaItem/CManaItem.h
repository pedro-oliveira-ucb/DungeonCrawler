#pragma once
#include "../CBaseItem/CBaseItem.h"

class CManaItem : public CBaseItem
{
	int healAmount;
public:
	CManaItem( CBaseEntityConstructor , ItemType itemType );
	CManaItem( const CManaItem & other );

	void applyEffect( ) override;
	std::unique_ptr<CBaseItem> ItemClone( ) const override;
};



