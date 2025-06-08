#pragma once
#include <cmath>

#define M_PI 3.14159265358979323846f


class GVector2D {
public:
	float x , y;

	GVector2D( ) : x( 0 ) , y( 0 ) { }
	GVector2D( float x , float y ) : x( x ) , y( y ) { }
	GVector2D( int x , int y ) : x( static_cast< float >( x ) ) , y( static_cast< float >( y ) ) { }

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

	bool operator<( const GVector2D & other ) const {
		return ( x < other.x ) || ( x == other.x && y < other.y );
	}

	float distTo( const GVector2D & other ) const {
		float dx = x - other.x;
		float dy = y - other.y;
		return std::sqrt( dx * dx + dy * dy );
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

	static float angleDiff( const GVector2D & a , const GVector2D & b ) {
		float dot = a.x * b.x + a.y * b.y;
		float det = a.x * b.y - a.y * b.x; // Determinant (equivalente ao cross product em 2D)
		return atan2( det , dot ); // Retorna ângulo em radianos, entre -? e ?
	}

	static float angleDiff360( const GVector2D & a , const GVector2D & b ) {
		float angle = atan2( a.x * b.y - a.y * b.x , a.x * b.x + a.y * b.y );
		if ( angle < 0 ) angle += 2 * M_PI;
		return angle; // radianos
	}

	
};
