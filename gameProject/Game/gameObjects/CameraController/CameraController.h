#pragma once // Evita m�ltiplas inclus�es do header

#include "../../../Utils/singleton.h"

#include <raylib/raylib.h>

class CameraController : public CSingleton<CameraController>
{
private:
    Camera2D m_camera;
	Vector2 m_old_target_position;

    float m_shake_timer;
    float m_shake_intensity;
    float m_smooth_speed;

    float zoomLevel = 0.0f;
public:
    void initialize(  Vector2 startPosition , float smooth_speed = 5.0f );
    void Update(float deltaTime);
    void TriggerShake( float intensity , float duration );
    Camera2D * GetCamera( );
};