#include "CameraController.h"

#include "../../../Globals/Globals.h"

#include <raylib/raymath.h>

void CameraController::initialize( Vector2 startPosition , float smooth_speed )
{
	m_camera = { 0 };
	m_camera.offset = { ( float ) GetScreenWidth( ) / 2.0f, ( float ) GetScreenHeight( ) / 2.0f };
	m_camera.target = startPosition; // Inicia a câmera na posição inicial
	m_camera.rotation = 0.0f;
	m_camera.zoom = 1.0f;

	m_shake_timer = 0.0f;
	m_shake_intensity = 0.0f;
}

void CameraController::Update( float deltaTime )
{
	GVector2D playerPosition = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );
	Vector2 targetPosition = { playerPosition.x, playerPosition.y };

	// 1. Suavização (Lerp) em direção ao alvo passado como argumento
	m_camera.target = Vector2Lerp( targetPosition , m_old_target_position , deltaTime * m_smooth_speed );

	if ( m_shake_timer > 0 )
	{
		m_shake_timer -= deltaTime;
		float offsetX = ( float ) GetRandomValue( -1 , 1 ) * m_shake_intensity;
		float offsetY = ( float ) GetRandomValue( -1 , 1 ) * m_shake_intensity;

		m_camera.offset.x = ( GetScreenWidth( ) / 2.0f ) + offsetX;
		m_camera.offset.y = ( GetScreenHeight( ) / 2.0f ) + offsetY;

		if ( m_shake_timer <= 0 )
		{
			m_camera.offset = { ( float ) ( GetScreenWidth( ) ) / 2.0f, ( float ) ( GetScreenHeight( ) ) / 2.0f };
		}

		m_shake_timer = Clamp( m_shake_timer , 0.0f , 10.0f );
	}

	float healthPercentage = Globals::Get( ).getGame( )->getLocalPlayerHealthPercentage( );
	float maxZoom = 2.0f;
	float minZoom = maxZoom - healthPercentage;

	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , minZoom , maxZoom );
	m_camera.zoom = zoomLevel;

	Vector2 mouseWorld = GetScreenToWorld2D( GetMousePosition( ) , m_camera );
	Globals::Get( ).mousePosWorldX = mouseWorld.x;
	Globals::Get( ).mousePosWorldY = mouseWorld.y;


	m_old_target_position = targetPosition;
}

void CameraController::TriggerShake( float intensity , float duration )
{
	m_shake_intensity = intensity;
	m_shake_timer = duration;
}

Camera2D * CameraController::GetCamera( )
{
	return &m_camera;
}
