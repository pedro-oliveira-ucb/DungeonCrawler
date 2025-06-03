#pragma once
#include <string>
#include "../../gameResources/gameResource/rSprites/rSprite/rSprite.h"	

#include <memory>

enum class mapObjectType
{
	mapObjectNone,
	tallWall,
	shortWall,
	lateralTallWall, 
	lateralShortWall,
	ground,
	door,
	mapObjectCount
};

class rMapObject
{
	mapObjectType objectType;
	std::shared_ptr<rSprite> sprite;
public:
	rMapObject(mapObjectType type, std::shared_ptr<rSprite> spritePtr)
		: objectType( type ) , sprite( spritePtr ) {
	}

	mapObjectType getObjectType() const {
		return objectType;
	}

	std::shared_ptr<rSprite> * getSprite( ) {
		return &this->sprite;
	}

	static std::string geMapObjectTypeName( mapObjectType type ) {
		switch ( type ) {
		case mapObjectType::tallWall:
			return "tallWall";
		case mapObjectType::shortWall:
			return "shortWall";
		case mapObjectType::lateralTallWall:
			return "lateralTallWall";
		case mapObjectType::lateralShortWall:
			return "lateralShortWall";
		case mapObjectType::ground:
			return "ground";
		case mapObjectType::door:
			return "door";
		default:
			return "unknown";
		}
	}	
};

