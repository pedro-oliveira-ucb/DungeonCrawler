#pragma once

#include <iosfwd> // Usar forward declaration de ostream para reduzir dependências de include

// --- Declaração da Classe ---
class GVector2D {
public:
    float x , y;

    // --- Construtores ---
    GVector2D( );
    GVector2D( float x , float y );
    GVector2D( int x , int y );

    // --- Operadores de Atribuição Composta (Vetor e Escalar) ---
    GVector2D & operator+=( const GVector2D & other );
    GVector2D & operator-=( const GVector2D & other );
    GVector2D & operator*=( const GVector2D & other ); // Multiplicação elemento a elemento
    GVector2D & operator/=( const GVector2D & other ); // Divisão elemento a elemento

    GVector2D & operator*=( float scalar );
    GVector2D & operator/=( float scalar );

    // --- Operador Unário ---
    GVector2D operator-( ) const;

    // --- Operadores de Comparação ---
    bool operator==( const GVector2D & other ) const;
    bool operator!=( const GVector2D & other ) const;
    bool operator<( const GVector2D & other ) const; // Para ordenação
    bool operator>( const GVector2D & other ) const;
    bool operator<=( const GVector2D & other ) const;
    bool operator>=( const GVector2D & other ) const;

    // --- Métodos de Vetor ---
    float length( ) const;
    float lengthSq( ) const;
    void normalize( );
    GVector2D normalized( ) const;
    float distTo( const GVector2D & other ) const;
    float dot( const GVector2D & other ) const;
    float cross( const GVector2D & other ) const;

    // --- Métodos Estáticos ---
    static float angleBetween( const GVector2D & a , const GVector2D & b );
};


// Operações Vetor-Vetor
GVector2D operator+( const GVector2D & lhs , const GVector2D & rhs );
GVector2D operator-( const GVector2D & lhs , const GVector2D & rhs );
GVector2D operator*( const GVector2D & lhs , const GVector2D & rhs );
GVector2D operator/( const GVector2D & lhs , const GVector2D & rhs );

// Operações Vetor-Escalar
GVector2D operator*( const GVector2D & vec , float scalar );
GVector2D operator*( float scalar , const GVector2D & vec );
GVector2D operator/( const GVector2D & vec , float scalar );

// --- Operador de Stream ---
std::ostream & operator<<( std::ostream & os , const GVector2D & v );