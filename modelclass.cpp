////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"
#include <fstream>

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_textures = 0;
    m_model = 0;
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFileName, char* textureFileName1, char* textureFileName2)
{
    bool result;
    
    // 모델 데이터를 읽어 들인다.
    result = LoadModel(modelFileName);
    if (!result)
    {
        return false;
    }
    
    // 모델의 접선·종법선 벡터를 계산한다.
    CalculateModelVectors();
    
    // 버택스 & 인덱스 버퍼 초기화
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
    
    // 이 모델을 위해 텍스처 로드
    result = LoadTextures(device, deviceContext, textureFileName1, textureFileName2);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void ModelClass::Shutdown()
{
    // 모델 텍스처 해제하기
    ReleaseTextures();
    
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

ID3D11ShaderResourceView* ModelClass::GetTexture(int index)
{
    return m_textures[index].GetTexture();
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
        vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);
        
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

bool ModelClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName1, char* fileName2)
{
    bool result;
    
    // 택스처 객체 생성 및 초기화
    m_textures = new TextureClass[3];
    
    result = m_textures[0].Initialize(device, deviceContext, fileName1);
    if (!result)
    {
        return false;
    }
    
    result = m_textures[1].Initialize(device, deviceContext, fileName2);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void ModelClass::ReleaseTextures()
{
    // 텍스처 객체 해제
    if (m_textures)
    {
        m_textures[0].Shutdown();
        m_textures[1].Shutdown();
        m_textures[2].Shutdown();
        
        delete[] m_textures;
        m_textures = 0;
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

void ModelClass::CalculateModelVectors()
{
    int faceCount, i, index;
    TempVertexType vertex1, vertex2, vertex3;
    VectorType tangent, binormal;
    
    // 모델의 면 개수를 계산한다.
    faceCount = m_vertexCount / 3;
    
    // index to the model data를 초기화한다.
    index = 0;
    
    // 모든 면을 순회하며 접선/종법선 백터를 계산한다.
    for (i = 0; i< faceCount; i++)
    {
        // three vertices for this face form the model를 얻는다.
        vertex1.x = m_model[index].x;
        vertex1.y = m_model[index].y;
        vertex1.z = m_model[index].z;
        vertex1.tu = m_model[index].tu;
        vertex1.tv = m_model[index].tv;
        index++;
        
        vertex2.x = m_model[index].x;
        vertex2.y = m_model[index].y;
        vertex2.z = m_model[index].z;
        vertex2.tu = m_model[index].tu;
        vertex2.tv = m_model[index].tv;
        index++;

        vertex3.x = m_model[index].x;
        vertex3.y = m_model[index].y;
        vertex3.z = m_model[index].z;
        vertex3.tu = m_model[index].tu;
        vertex3.tv = m_model[index].tv;
        index++;
        
        // 그 면의 접선/총법선을 계산한다.
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);
        
        // tangent and binormal for this face back in the model structure를 저장한다.
        m_model[index-1].tx = tangent.x;
        m_model[index-1].ty = tangent.y;
        m_model[index-1].tz = tangent.z;
        m_model[index-1].bx = binormal.x;
        m_model[index-1].by = binormal.y;
        m_model[index-1].bz = binormal.z;

        m_model[index-2].tx = tangent.x;
        m_model[index-2].ty = tangent.y;
        m_model[index-2].tz = tangent.z;
        m_model[index-2].bx = binormal.x;
        m_model[index-2].by = binormal.y;
        m_model[index-2].bz = binormal.z;

        m_model[index-3].tx = tangent.x;
        m_model[index-3].ty = tangent.y;
        m_model[index-3].tz = tangent.z;
        m_model[index-3].bx = binormal.x;
        m_model[index-3].by = binormal.y;
        m_model[index-3].bz = binormal.z;
    }
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];
    float den;
    float length;
    
    // 이 면을 이루는 두 벡터를 계산한다.
    // vertex1 -> vertex2
    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    // vertex1 -> vertex3
    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;
    
    // tu·tv 텍스처 공간 벡터를 계산한다.
    tuVector[0] = vertex2.tu - vertex1.tu;
    tvVector[0] = vertex2.tv - vertex1.tv;

    tuVector[1] = vertex3.tu - vertex1.tu;
    tvVector[1] = vertex3.tv - vertex1.tv;
    
    // 탄젠트/종법선 식의 분모를 계산한다.
    den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // 외적을 계산하고 계수를 곱해 탄젠트와 종법선을 구한다.
    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // 이 법선의 길이를 계산한다.
    length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    // 법선을 정규화한 뒤 저장한다
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    // 이 법선의 길이를 계산한다.
    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    // 법선을 정규화한 뒤 저장한다
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;

    return;
}
