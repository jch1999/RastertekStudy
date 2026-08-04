////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
    bool result;
    
    // 버택스 & 인덱스 버퍼 초기화
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void ModelClass::Shutdown()
{
    // 버택스 & 인덱스 버퍼 셧다운
    ShutdownBuffers();
    
    return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // 그리기를 위해 그래픽 파이프 라인에 버택스 & 인덱스 버퍼 넣기
    RenderBuffers(deviceContext);
    
    return;
}

int ModelClass::GetIndexCount()
{
    return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    
    // 버택스 배열의 버택스 수 설정
    m_vertexCount = 3;
    
    // 인덱스 배열의 인덱스 수 설정
    m_indexCount = 3;
    
    // 버택스 배열 생성
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }
    
    // 인덱스 배열 생성
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }
    
    // 버택스 배열에 데이터 로드
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    
    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    
    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    
    // 인덱스 배열에 데이터 로드
    indices[0] = 0;  // Bottom left.
    indices[1] = 1;  // Top middle.
    indices[2] = 2;  // Bottom right.
    
    // 정적 버택스 버퍼의 설명 구조체를 설정한다.
    vertexBufferDesc.Usage= D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    
    // 서브 리소스 구조체에 버택스 데이터의 포인터를 지정한다.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch  = 0;
    
    // 이제 버택스 버퍼를 생성한다
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    // 정적 인덱스 버퍼의 설명 구조체를 설정한다.
    indexBufferDesc.Usage= D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    
    // 서브 리소스 구조체에 인덱스 데이터의 포인터를 지정한다.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch  = 0;
    
    // 이제 인덱스 버퍼를 생성한다.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    // 버택스 & 인덱스 버퍼가 생성 및 로드됬으니 배열들을 해제
    delete[] vertices;
    vertices = 0;
    
    delete[] indices;
    indices = 0;
    
    return true;
}

void ModelClass::ShutdownBuffers()
{
    // 인덱스 버퍼 해제
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }
    
    // 버택스 버퍼 해제
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
    
    return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;
    
    // 버택스 버퍼의 스트라드와 옵셋 설정
    stride = sizeof(VertexType);
    offset = 0;
    
    // 랜더될 수 있게 IA 단계에서 동작하도록 버택스 버퍼 설정
    deviceContext->IASetVertexBuffers(0, 1, & m_vertexBuffer, &stride, &offset);
    
    // 랜더될 수 있게 IA 단계에서 동작하도록 인덱스 버퍼 설정
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    
    // 백터 버퍼로 부터 랜더되야 하는 도형을 설정, 이 경우는 삼각형.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    return;
}
