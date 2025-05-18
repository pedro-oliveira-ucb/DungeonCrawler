#pragma once
#include <cmath>


#define M_PI 3.14159265358979323846f


class GAngle
{
private:
    float angleInDegrees; // Armazena o ângulo em graus

    // Método interno para normalizar um valor em graus para [-180, 180]
    static float normalizeDegrees( float degrees ) {
        while ( degrees > 180.0f ) degrees -= 360.0f;
        while ( degrees < -180.0f ) degrees += 360.0f;
        return degrees;
    }

public:
    // Construtores
    GAngle( ) : angleInDegrees( 0.0f ) {}
    GAngle( float degrees ) : angleInDegrees( normalizeDegrees( degrees ) ) {}

    // Métodos de acesso
    float getDegrees( ) const { return angleInDegrees; }
    void setDegrees( float degrees ) { angleInDegrees = normalizeDegrees( degrees ); }

    float getRadians( ) const { return toRadians( angleInDegrees ); }
    void setRadians( float radians ) { angleInDegrees = normalizeDegrees( toDegrees( radians ) ); }

    // Métodos estáticos de conversão
    static float toRadians( float degrees ) { return degrees * ( M_PI / 180.0f ); }
    static float toDegrees( float radians ) { return radians * ( 180.0f / M_PI ); }

    // Seno e cosseno
    float sin( ) const { return std::sin( getRadians( ) ); }
    float cos( ) const { return std::cos( getRadians( ) ); }

    // Normaliza este ângulo (caso alterado manualmente fora do setter)
    void normalize( ) { angleInDegrees = normalizeDegrees( angleInDegrees ); }

    // Operadores aritméticos
    GAngle operator+( const GAngle & other ) const {
        return GAngle( angleInDegrees + other.angleInDegrees );
    }

    GAngle operator-( const GAngle & other ) const {
        return GAngle( angleInDegrees - other.angleInDegrees );
    }

    GAngle & operator+=( const GAngle & other ) {
        angleInDegrees = normalizeDegrees( angleInDegrees + other.angleInDegrees );
        return *this;
    }

    GAngle & operator-=( const GAngle & other ) {
        angleInDegrees = normalizeDegrees( angleInDegrees - other.angleInDegrees );
        return *this;
    }
};
