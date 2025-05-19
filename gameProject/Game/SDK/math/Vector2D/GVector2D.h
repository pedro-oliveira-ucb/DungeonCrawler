#pragma once
#include <cmath>

#define M_PI 3.14159265358979323846f


class GVector2D {
public:
    float x , y;

    GVector2D( ) : x( 0 ) , y( 0 ) {}
    GVector2D( float x , float y ) : x( x ) , y( y ) {}
    GVector2D( int x , int y ) : x( static_cast< float >( x ) ) , y( static_cast< float >( y ) ) {}

    // Operadores aritméticos
    GVector2D operator+( const GVector2D & other ) const {
        return { x + other.x, y + other.y };
    }

    GVector2D operator-( const GVector2D & other ) const {
        return { x - other.x, y - other.y };
    }

    GVector2D operator*( const GVector2D & other ) const {
        return { x * other.x, y * other.y };
    }

    GVector2D operator/( const GVector2D & other ) const {
        return { x / other.x, y / other.y };
    }

    // Operadores com escalar
    GVector2D operator+( float scalar ) const {
        return { x + scalar, y + scalar };
    }
    GVector2D operator* ( float scalar ) const {
        return { x * scalar, y * scalar };
    }

    GVector2D operator-( float scalar ) const {
        return { x - scalar, y - scalar };
    }

    // Operadores compostos
    GVector2D & operator+=( const GVector2D & other ) {
        x += other.x;
        y += other.y;
        return *this;
    }

    GVector2D & operator+=( float scalar ) {
        x += scalar;
        y += scalar;
        return *this;
    }

    GVector2D & operator-=( float scalar ) {
        x -= scalar;
        y -= scalar;
        return *this;
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
