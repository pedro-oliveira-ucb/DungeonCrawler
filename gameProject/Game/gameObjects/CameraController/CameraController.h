#pragma once // Evita múltiplas inclusões do header

#include "../../../Utils/singleton.h"

#include <mutex>

#include <raylib/raylib.h>

class CameraController : public CSingleton<CameraController>
{
private:
    mutable std::mutex m_mutex;
    Camera2D m_camera;
	Vector2 m_old_target_position;

    float lastUpdateTime = 0.0f;
    float m_shake_timer;
    float m_shake_intensity;
    float m_smooth_speed;

    float zoomLevel = 0.0f;
public:
    void initialize(  Vector2 startPosition , float smooth_speed );
    void Update(float deltaTime);
    void TriggerShake( float intensity , float duration );
    Camera2D * GetCamera( );
	Vector2 GetCameraPosition( ) const;
};