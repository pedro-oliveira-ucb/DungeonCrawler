#pragma once
#include <string>
#include "../../../../SDK/math/Vector2D/GVector2D.h"

class rSprite
{
    std::string modelPath;
    void * texture; // tipo opaco
    GVector2D spriteSize;

    void initializeTexture( );
public:
    rSprite( const rSprite & ) = delete;
    rSprite & operator=( const rSprite & ) = delete;

    rSprite( std::string model );
    ~rSprite( );

	GVector2D getSpriteSize( ) { return spriteSize; }
    void * getTexture( ); // retorna tipo opaco
};
