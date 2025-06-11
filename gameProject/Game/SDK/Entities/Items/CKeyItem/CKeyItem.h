#pragma once
#include "../CBaseItem/CBaseItem.h"

class CKeyItem : public CBaseItem
{

public:
	CKeyItem( CBaseEntityConstructor , ItemType itemType );
	CKeyItem( const CKeyItem & other );

	void applyEffect( ) override;
	std::unique_ptr<CBaseItem> ItemClone( ) const override;
};



