////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"
#include <fstream>

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_texture = 0;
    m_model = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFileName, char* modelFileName)
{
    bool result;
    
    // 모델 데이터를 읽어 들인다.
    result = LoadModel(modelFileName);
    if (!result)
    {
        return false;
    }
    
    // 버택스 & 인덱스 버퍼 초기화
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
    
    // 이 모델을 위해 텍스처 로드
    result = LoadTexture(device, deviceContext, textureFileName);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void ModelClass::Shutdown()
{
    // 모델 텍스처 해제하기
    ReleaseTexture();
    
    // 버택스 & 인덱스 버퍼 셧다운
    ShutdownBuffers();
    
    // 모델 데이터 해제
    ReleaseModel();
    
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

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    return m_texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;
    
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
    
    // 정점 배열과 인덱스 배열을 데이터로 채운다.
    for (i = 0; i < m_vertexCount; i++)
    {
        vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        
        indices[i] = i;
    }
    
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

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName)
{
    bool result;
    
    // 택스처 객체 생성 및 초기화
    m_texture = new TextureClass;
    
    result = m_texture->Initialize(device, deviceContext, fileName);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void ModelClass::ReleaseTexture()
{
    // 텍스처 객체 해제
    if (m_texture)
    {
        m_texture->Shutdown();
        delete m_texture;
        m_texture = 0;
    }
}

bool ModelClass::LoadModel(char* filename)
{
    ifstream fin;
    char input;
    int i;
    
    // 모델 파일을 연다.
    fin.open(filename);
    
    // 파일을 열 수 없으면 종료한다.
    if (fin.fail())
    {
        return false;
    }
    
    // 정점 수 값이 나올 때까지 읽는다.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    
    // 정점 수를 읽어 들인다.
    fin >> m_vertexCount;
    
    // 인덱스 수를 정점 수와 같게 설정한다.
    m_indexCount = m_vertexCount;
    
    // 읽어 들인 정점 수만큼 모델 배열을 생성한.
    m_model = new ModelType[m_vertexCount];
    
    // 데이터 시작 지점까지 읽는다.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);
    
    // 정점 데이터를 읽어 들인다.
    for (i = 0; i< m_vertexCount; i++)
    {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }
    
    // 모델 파일을 닫는다.
    fin.close();
    
    return true;
}

void ModelClass::ReleaseModel()
{
    if (m_model)
    {
        delete[] m_model;
        m_model = 0;
    }
    return;
}
