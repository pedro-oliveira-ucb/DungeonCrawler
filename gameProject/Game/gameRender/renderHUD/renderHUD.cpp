#include "renderHUD.h"

#include <raylib/raylib.h>
#include <string> // Para usar std::to_string

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Globals/Globals.h"

// Função auxiliar para desenhar texto com uma borda preta para melhor legibilidade
void DrawTextWithOutline( const char * text , int posX , int posY , int fontSize , Color textColor , Color outlineColor = BLACK ) {
    DrawText( text , posX - 2 , posY - 2 , fontSize , outlineColor );
    DrawText( text , posX + 2 , posY - 2 , fontSize , outlineColor );
    DrawText( text , posX + 2 , posY + 2 , fontSize , outlineColor );
    DrawText( text , posX - 2 , posY + 2 , fontSize , outlineColor );
    DrawText( text , posX , posY , fontSize , textColor );
}

void renderHUD::render( ) {
    CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );

    // Se o jogador não existir, não renderize o HUD
    if ( !player ) {
        return;
    }

    float screenWidth = Globals::Get( ).screenWidth;
    float screenHeight = Globals::Get( ).screenHeight;

    // --- Obtenção das Estatísticas do Jogador ---
    float healthPercentage = Globals::Get( ).getGame( )->getLocalPlayerHealthPercentage( ); // Valor de 0.0f a 1.0f
    int numMana = Globals::Get( ).getGame( )->getNumMana( );
    bool inCorridor = Globals::Get( ).getGame( )->isInCorridor( );
    float simpleAttackCooldown = Globals::Get( ).getGame( )->getAttackTypeCooldown( CBaseAttackType_Melee );
    float magicAttackCooldown = Globals::Get( ).getGame( )->getAttackTypeCooldown( CBaseAttackType_Ranged );

    // --- Configurações do HUD ---
    int barWidth = 250;
    int barHeight = 25;
    int padding = 20;
    int fontSize = 20;

    // --- Barra de Vida (Canto Inferior Esquerdo) ---
    Vector2 healthBarPosition = { ( float ) padding, screenHeight - barHeight - padding };

    Color healthColor = GREEN;
    if ( healthPercentage < 0.5f ) healthColor = YELLOW;
    if ( healthPercentage < 0.25f ) healthColor = RED;

    DrawRectangle( healthBarPosition.x , healthBarPosition.y , barWidth , barHeight , Fade( BLACK , 0.6f ) );
    DrawRectangle( healthBarPosition.x , healthBarPosition.y , ( int ) ( barWidth * healthPercentage ) , barHeight , healthColor );
    DrawRectangleLinesEx( { healthBarPosition.x, healthBarPosition.y, ( float ) barWidth, ( float ) barHeight } , 2 , DARKGRAY );

    std::string healthText = "HP: " + std::to_string( ( int ) ( healthPercentage * 100 ) ) + "%";
    DrawTextWithOutline( healthText.c_str( ) , healthBarPosition.x + 10 , healthBarPosition.y + 4 , fontSize , WHITE );


    // --- Barra de Mana (Acima da Barra de Vida) ---
    Vector2 manaPosition = { ( float ) padding, healthBarPosition.y - 35 };
    DrawRectangle( manaPosition.x , manaPosition.y , 150 , 25 , Fade( BLACK , 0.6f ) );
    DrawRectangle( manaPosition.x , manaPosition.y , 150 , 25 , BLUE );
    DrawRectangleLinesEx( { manaPosition.x, manaPosition.y, 150, 25 } , 2 , DARKGRAY );

    std::string manaText = "Mana: " + std::to_string( numMana );
    DrawTextWithOutline( manaText.c_str( ) , manaPosition.x + 10 , manaPosition.y + 4 , fontSize , WHITE );


    // --- Cooldowns de Habilidades (Canto Inferior Direito) ---
    int abilitySize = 60;
    Vector2 magicAttackPosition = { screenWidth - abilitySize - padding, screenHeight - abilitySize - padding };
    Vector2 simpleAttackPosition = { magicAttackPosition.x - abilitySize - padding, magicAttackPosition.y };

    DrawRectangle( simpleAttackPosition.x , simpleAttackPosition.y , abilitySize , abilitySize , Fade( DARKGRAY , 0.8f ) );
    DrawText( "S" , simpleAttackPosition.x + 22 , simpleAttackPosition.y + 15 , 30 , WHITE );
    DrawRectangleLines( simpleAttackPosition.x , simpleAttackPosition.y , abilitySize , abilitySize , BLACK );

    if ( simpleAttackCooldown > 0 ) {
        DrawRectangle( simpleAttackPosition.x , simpleAttackPosition.y , abilitySize , abilitySize , Fade( BLACK , 0.7f ) );
        std::string cooldownText = TextFormat( "%.1f" , simpleAttackCooldown );
        DrawText( cooldownText.c_str( ) , simpleAttackPosition.x + 15 , simpleAttackPosition.y + 20 , 25 , YELLOW );
    }

    DrawRectangle( magicAttackPosition.x , magicAttackPosition.y , abilitySize , abilitySize , Fade( PURPLE , 0.8f ) );
    DrawText( "M" , magicAttackPosition.x + 20 , magicAttackPosition.y + 15 , 30 , WHITE );
    DrawRectangleLines( magicAttackPosition.x , magicAttackPosition.y , abilitySize , abilitySize , BLACK );

    if ( magicAttackCooldown > 0 ) {
        DrawRectangle( magicAttackPosition.x , magicAttackPosition.y , abilitySize , abilitySize , Fade( BLACK , 0.7f ) );
        std::string cooldownText = TextFormat( "%.1f" , magicAttackCooldown );
        DrawText( cooldownText.c_str( ) , magicAttackPosition.x + 15 , magicAttackPosition.y + 20 , 25 , YELLOW );
    }


    // --- Indicador de "InCorridor" (Topo Central) ---
    if ( inCorridor ) {
        const char * corridorText = "Você está em um corredor";
        const char * shopText = "Pressione U para abrir a loja";
        int lineSpacing = 10; // Aumentado o espaço para clareza

        // Medir o texto mais longo para centralizar a caixa corretamente
        int corridorTextWidth = MeasureText( corridorText , fontSize );
        int shopTextWidth = MeasureText( shopText , fontSize );
        int maxWidth = ( corridorTextWidth > shopTextWidth ) ? corridorTextWidth : shopTextWidth;

        // Definir a posição e o tamanho da caixa de fundo para acomodar duas linhas
        int boxWidth = maxWidth + 40; // Mais padding lateral
        int boxHeight = ( fontSize * 2 ) + ( lineSpacing * 2 );
        Vector2 boxPosition = { screenWidth / 2 - boxWidth / 2, ( float ) padding };

        // Desenhar a caixa de fundo semitransparente
        DrawRectangleRec( { boxPosition.x, boxPosition.y, ( float ) boxWidth, ( float ) boxHeight } , Fade( BLACK , 0.7f ) );

        // Posições para cada linha de texto, centralizadas individualmente
        Vector2 corridorTextPosition = { screenWidth / 2 - corridorTextWidth / 2, boxPosition.y + lineSpacing / 2 };
        Vector2 shopTextPosition = { screenWidth / 2 - shopTextWidth / 2, corridorTextPosition.y + fontSize + lineSpacing };

        // Desenhar os textos com contorno para melhor legibilidade
        DrawTextWithOutline( corridorText , corridorTextPosition.x , corridorTextPosition.y , fontSize , SKYBLUE );
        DrawTextWithOutline( shopText , shopTextPosition.x , shopTextPosition.y , fontSize , YELLOW );
    }
}