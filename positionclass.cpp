#include "positionclass.h"

#include <ctime>

PositionClass::PositionClass()
{
    m_frameTime = 0.0f;
    m_rotationY = 0.0f;
    m_leftTurnSpeed  = 0.0f;
    m_rightTurnSpeed = 0.0f;
}

PositionClass::PositionClass(const PositionClass& other)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float time)
{
    m_frameTime = time;
    return;
}

void PositionClass::GetRotation(float& y)
{
    y = m_rotationY;
    return;
}

void PositionClass::TurnLeft(bool keydown)
{
	// 키가 눌려 있으면 카메라가 왼쪽으로 도는 속도를 높이고, 아니면 회전 속도를 늦춘다.
    if (keydown)
    {
        m_leftTurnSpeed += m_frameTime * 1.5f;
        
        if (m_leftTurnSpeed > (m_frameTime * 200.0f))
        {
            m_leftTurnSpeed = m_frameTime * 200.0f;
        }
    }
    else
    {
        m_leftTurnSpeed -= m_frameTime * 1.0f;
        
        if (m_leftTurnSpeed < 0.0f)
        {
            m_leftTurnSpeed = 0.0f;
        }
    }
    
    // 회전 속도로 회전 값을 갱신한다.
    m_rotationY -= m_leftTurnSpeed;
    if (m_rotationY < 0.0f)
    {
        m_rotationY = 0.0f;
    }
    
    return;
}

void PositionClass::TurnRight(bool keydown)
{
	// 키가 눌려 있으면 카메라가 오른쪽으로 도는 속도를 높이고, 아니면 회전 속도를 늦춘다.
    if (keydown)
    {
        m_rightTurnSpeed += m_frameTime * 1.5f;
        
        if (m_rightTurnSpeed > (m_frameTime * 200.0f))
        {
            m_rightTurnSpeed = m_frameTime * 200.0f;
        }
    }
    else
    {
        m_rightTurnSpeed -= m_frameTime * 1.0f;
        
        if (m_rightTurnSpeed < 0.0f)
        {
            m_rightTurnSpeed = 0.0f;
        }
    }
    
    // 회전 속도로 회전 값을 갱신한다.
    m_rotationY += m_rightTurnSpeed;
    if (m_rotationY > 360.0f)
    {
        m_rotationY -= 360.0f;
    }
    
    return;
}
