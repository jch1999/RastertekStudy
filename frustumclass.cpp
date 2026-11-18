////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

FrustumClass::FrustumClass()
{
}

FrustumClass::FrustumClass(const FrustumClass& other)
{
}

FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float screenDepth)
{
    XMMATRIX finalMatrix;
    XMFLOAT4X4 projMatrix, matrix;
    float zMinimum, r, t;
    
    // 투영 행렬을 XMFLOAT4X4 구조체에 적재한다.
    XMStoreFloat4x4(&projMatrix, projectionMatrix);
    
    // 프러스텀에서 최소 z 거리를 계산한다.
    zMinimum = -projMatrix._43 / projMatrix._33;
    r = screenDepth / (screenDepth - zMinimum);
    projMatrix._33 = r;
    projMatrix._43 = -r * zMinimum;
    
    // 갱신된 XMFLOAT4X4를 원래 투영 행렬에 다시 적재한다.
    projectionMatrix = XMLoadFloat4x4(&projMatrix);
    
    // 뷰 행렬과 갱신된 투영 행렬로부터 프러스텀 행렬을 생성한다.
    finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
    
    // 최종 행렬을 XMFLOAT4X4 구조체에 적재한다.
    XMStoreFloat4x4(&matrix, finalMatrix);
    
    // 프러스텀의 근평면을 구한다.
    m_planes[0].x = matrix._13;
    m_planes[0].y = matrix._23;
    m_planes[0].z = matrix._33;
    m_planes[0].w = matrix._43;
    
    // 정규화한다.
    t = (float)sqrt((m_planes[0].x * m_planes[0].x) + (m_planes[0].y * m_planes[0].y) + (m_planes[0].z * m_planes[0].z));
    m_planes[0].x /= t;
    m_planes[0].y /= t;
    m_planes[0].z /= t;
    m_planes[0].w /= t;
    
    // 프러스텀의 원평면을 계산한다.
    m_planes[1].x = matrix._14 - matrix._13;
    m_planes[1].y = matrix._24 - matrix._23;
    m_planes[1].z = matrix._34 - matrix._33;
    m_planes[1].w = matrix._44 - matrix._43;
    
    // 정규화한다.
    t = (float)sqrt((m_planes[1].x * m_planes[1].x) + (m_planes[1].y * m_planes[1].y) + (m_planes[1].z * m_planes[1].z));
    m_planes[1].x /= t;
    m_planes[1].y /= t;
    m_planes[1].z /= t;
    m_planes[1].w /= t;
 
    // 프러스텀의 좌평면을 계산한다.
    m_planes[2].x = matrix._14 + matrix._11;
    m_planes[2].y = matrix._24 + matrix._21;
    m_planes[2].z = matrix._34 + matrix._31;
    m_planes[2].w = matrix._44 + matrix._41;
    
    // 정규화한다.
    t = (float)sqrt((m_planes[2].x * m_planes[2].x) + (m_planes[2].y * m_planes[2].y) + (m_planes[2].z * m_planes[2].z));
    m_planes[2].x /= t;
    m_planes[2].y /= t;
    m_planes[2].z /= t;
    m_planes[2].w /= t;
    
    // 프러스텀의 우평면을 계산한다.
    m_planes[3].x = matrix._14 - matrix._11;
    m_planes[3].y = matrix._24 - matrix._21;
    m_planes[3].z = matrix._34 - matrix._31;
    m_planes[3].w = matrix._44 - matrix._41;
    
    // 정규화한다.
    t = (float)sqrt((m_planes[3].x * m_planes[3].x) + (m_planes[3].y * m_planes[3].y) + (m_planes[3].z * m_planes[3].z));
    m_planes[3].x /= t;
    m_planes[3].y /= t;
    m_planes[3].z /= t;
    m_planes[3].w /= t;

    // 프러스텀의 상평면을 계산한다.
    m_planes[4].x = matrix._14 - matrix._12;
    m_planes[4].y = matrix._24 - matrix._22;
    m_planes[4].z = matrix._34 - matrix._32;
    m_planes[4].w = matrix._44 - matrix._42;

    // 정규화한다.
    t = (float)sqrt((m_planes[4].x * m_planes[4].x) + (m_planes[4].y * m_planes[4].y) + (m_planes[4].z * m_planes[4].z));
    m_planes[4].x /= t;
    m_planes[4].y /= t;
    m_planes[4].z /= t;
    m_planes[4].w /= t;

    // 프러스텀의 하평면을 계산한다.
    m_planes[5].x = matrix._14 + matrix._12;
    m_planes[5].y = matrix._24 + matrix._22;
    m_planes[5].z = matrix._34 + matrix._32;
    m_planes[5].w = matrix._44 + matrix._42;

    // 정규화한다.
    t = (float)sqrt((m_planes[5].x * m_planes[5].x) + (m_planes[5].y * m_planes[5].y) + (m_planes[5].z * m_planes[5].z));
    m_planes[5].x /= t;
    m_planes[5].y /= t;
    m_planes[5].z /= t;
    m_planes[5].w /= t;

    return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
    int i;

    // 점이 뷰 프러스텀의 여섯 평면 모두의 안쪽에 있는지 검사한다.
    for(i=0; i<6; i++)
    {
        if(((m_planes[i].x * x) + (m_planes[i].y * y) + (m_planes[i].z * z) + m_planes[i].w) < 0.0f)
        {
            return false;
        }
    }

    return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
    int i;
    
    // 정육면체의 한 점이라도 뷰 프러스텀 안에 있는지 검사한다.
    for(i=0; i<6; i++)
    {
        if(m_planes[i].x * (xCenter - radius) +
           m_planes[i].y * (yCenter - radius) +
           m_planes[i].z * (zCenter - radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + radius) +
           m_planes[i].y * (yCenter - radius) +
           m_planes[i].z * (zCenter - radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - radius) +
           m_planes[i].y * (yCenter + radius) +
           m_planes[i].z * (zCenter - radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + radius) +
           m_planes[i].y * (yCenter + radius) +
           m_planes[i].z * (zCenter - radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - radius) +
           m_planes[i].y * (yCenter - radius) +
           m_planes[i].z * (zCenter + radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + radius) +
           m_planes[i].y * (yCenter - radius) +
           m_planes[i].z * (zCenter + radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - radius) +
           m_planes[i].y * (yCenter + radius) +
           m_planes[i].z * (zCenter + radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + radius) +
           m_planes[i].y * (yCenter + radius) +
           m_planes[i].z * (zCenter + radius) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
    int i;
    
    // 구의 반지름이 뷰 프러스텀 안에 있는지 검사한다.
    for(i=0; i<6; i++)
    {
        if(((m_planes[i].x * xCenter) + (m_planes[i].y * yCenter) + (m_planes[i].z * zCenter) + m_planes[i].w) < -radius)
        {
            return false;
        }
    }

    return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
    int i;
    
    // 직육면체의 여섯 면 중 하나라도 뷰 프러스텀 안에 있는지 검사한다.
    for(i=0; i<6; i++)
    {
        if(m_planes[i].x * (xCenter - xSize) +
           m_planes[i].y * (yCenter - ySize) +
           m_planes[i].z * (zCenter - zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + xSize) +
           m_planes[i].y * (yCenter - ySize) +
           m_planes[i].z * (zCenter - zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - xSize) +
           m_planes[i].y * (yCenter + ySize) +
           m_planes[i].z * (zCenter - zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - xSize) +
           m_planes[i].y * (yCenter - ySize) +
           m_planes[i].z * (zCenter + zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + xSize) +
           m_planes[i].y * (yCenter + ySize) +
           m_planes[i].z * (zCenter - zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + xSize) +
           m_planes[i].y * (yCenter - ySize) +
           m_planes[i].z * (zCenter + zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter - xSize) +
           m_planes[i].y * (yCenter + ySize) +
           m_planes[i].z * (zCenter + zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        if(m_planes[i].x * (xCenter + xSize) +
           m_planes[i].y * (yCenter + ySize) +
           m_planes[i].z * (zCenter + zSize) + m_planes[i].w >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}
