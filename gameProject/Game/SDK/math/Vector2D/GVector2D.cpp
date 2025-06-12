#include "GVector2D.h"
#include <cmath>
#include <ostream> // Include completo para a implementação do operador <<

// --- Construtores ---
GVector2D::GVector2D( ) : x( 0.0f ) , y( 0.0f ) { }

GVector2D::GVector2D( float x , float y ) : x( x ) , y( y ) { }

GVector2D::GVector2D( int ix , int iy ) : x( static_cast< float >( ix ) ) , y( static_cast< float >( iy ) ) { }


// --- Operadores de Atribuição Composta (Vetor) ---
GVector2D & GVector2D::operator+=( const GVector2D & other ) {
    x += other.x;
    y += other.y;
    return *this;
}

GVector2D & GVector2D::operator-=( const GVector2D & other ) {
    x -= other.x;
    y -= other.y;
    return *this;
}

GVector2D & GVector2D::operator*=( const GVector2D & other ) {
    x *= other.x;
    y *= other.y;
    return *this;
}

GVector2D & GVector2D::operator/=( const GVector2D & other ) {
    if ( other.x != 0.0f ) x /= other.x;
    if ( other.y != 0.0f ) y /= other.y;
    return *this;
}

// --- Operadores de Atribuição Composta (Escalar) ---
GVector2D & GVector2D::operator*=( float scalar ) {
    x *= scalar;
    y *= scalar;
    return *this;
}

GVector2D & GVector2D::operator/=( float scalar ) {
    if ( scalar != 0.0f ) {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

// --- Operador Unário ---
GVector2D GVector2D::operator-( ) const {
    return { -x, -y };
}

// --- Operadores de Comparação ---
bool GVector2D::operator==( const GVector2D & other ) const {
    return x == other.x && y == other.y;
}

bool GVector2D::operator!=( const GVector2D & other ) const {
    return !( *this == other );
}

bool GVector2D::operator<( const GVector2D & other ) const {
    return ( x < other.x ) || ( x == other.x && y < other.y );
}

bool GVector2D::operator>( const GVector2D & other ) const {
    return other < *this;
}

bool GVector2D::operator<=( const GVector2D & other ) const {
    return !( other < *this );
}

bool GVector2D::operator>=( const GVector2D & other ) const {
    return !( *this < other );
}

// --- Métodos de Vetor ---
float GVector2D::length( ) const {
    return std::sqrt( x * x + y * y );
}

float GVector2D::lengthSq( ) const {
    return x * x + y * y;
}

void GVector2D::normalize( ) {
    float len = length( );
    if ( len > 0.0f ) {
        *this /= len;
    }
}

GVector2D GVector2D::normalized( ) const {
    GVector2D result = *this;
    result.normalize( );
    return result;
}

float GVector2D::distTo( const GVector2D & other ) const {
    return ( *this - other ).length( );
}

float GVector2D::dot( const GVector2D & other ) const {
    return x * other.x + y * other.y;
}

float GVector2D::cross( const GVector2D & other ) const {
    return x * other.y - y * other.x;
}

// --- Métodos Estáticos ---
float GVector2D::angleBetween( const GVector2D & a , const GVector2D & b ) {
    return std::atan2( a.cross( b ) , a.dot( b ) );
}

// --- Implementação dos Operadores Binários Livres ---

GVector2D operator+( const GVector2D & lhs , const GVector2D & rhs ) {
    return GVector2D( lhs ) += rhs;
}

GVector2D operator-( const GVector2D & lhs , const GVector2D & rhs ) {
    return GVector2D( lhs ) -= rhs;
}

GVector2D operator*( const GVector2D & lhs , const GVector2D & rhs ) {
    return GVector2D( lhs ) *= rhs;
}

GVector2D operator/( const GVector2D & lhs , const GVector2D & rhs ) {
    return GVector2D( lhs ) /= rhs;
}

GVector2D operator*( const GVector2D & vec , float scalar ) {
    return GVector2D( vec ) *= scalar;
}

GVector2D operator*( float scalar , const GVector2D & vec ) {
    return GVector2D( vec ) *= scalar;
}

GVector2D operator/( const GVector2D & vec , float scalar ) {
    return GVector2D( vec ) /= scalar;
}

// --- Implementação do Operador de Stream ---
std::ostream & operator<<( std::ostream & os , const GVector2D & v ) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}