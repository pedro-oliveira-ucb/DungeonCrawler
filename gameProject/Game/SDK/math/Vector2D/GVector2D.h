#pragma once
#include <cmath>

#define M_PI 3.14159265358979323846f


class GVector2D {
public:
    float x , y;

    GVector2D( ) : x( 0 ) , y( 0 ) {}
    GVector2D( float x , float y ) : x( x ) , y( y ) {}
    GVector2D( int x , int y ) : x( static_cast< float >( x ) ) , y( static_cast< float >( y ) ) {}

    // Operador de subtração
    GVector2D operator-( const GVector2D & other ) const {
        return GVector2D( x - other.x , y - other.y );
    }

    // Calcula o comprimento (magnitude) do vetor
    float length( ) const {
        return std::sqrt( x * x + y * y );
    }

    // Retorna o vetor normalizado (direção)
    GVector2D normalized( ) const {
        float len = length( );
        if ( len == 0 ) return GVector2D( 0 , 0 );
        return GVector2D( x / len , y / len );
    }
};
