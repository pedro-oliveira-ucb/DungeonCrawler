#pragma once
#include <string>
#include <raylib/raylib.h>
#include <memory>


class rSprite
{
    std::string modelPath;
    std::unique_ptr<Texture2D> texture;
    void initializeTexture( );

public:
    rSprite( const rSprite & ) = delete;
    rSprite & operator=( const rSprite & ) = delete;

    rSprite( std::string model );
    ~rSprite( );
    Texture2D * getTexture( );
};
