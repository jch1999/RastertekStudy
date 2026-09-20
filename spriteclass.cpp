#include "spriteclass.h"

SpriteClass::SpriteClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_textures = 0;
}

SpriteClass::SpriteClass(const SpriteClass&)
{
}

SpriteClass::~SpriteClass()
{
}

bool SpriteClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* spriteFilename, int renderX, int renderY)
{
    bool result;
    
    // 화면 크기를 저장한다.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    
    // 스프라이트를 렌더링할 위치를 저장한다.
    m_renderX = renderX;
    m_renderY = renderY;

    // 이 스프라이트 객체의 프레임 시간을 초기화한다.
    m_frameTime = 0;
    
    // 스프라이트 비트맵의 지오메트리를 담을 정점·인덱스 버퍼를 초기화한다.
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
    
    // 이 스프라이트에 쓸 텍스처들을 불러온다.
    result = LoadTextures(device, deviceContext, spriteFilename);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void SpriteClass::Shutdown()
{
    // 이 스프라이트에 쓴 텍스처들을 해제한다.
    ReleaseTextures();

    // 정점·인덱스 버퍼를 해제한다.
    ShutdownBuffers();

    return;
}

bool SpriteClass::Render(ID3D11DeviceContext* deviceContext)
{
    bool result;
    
    // 스프라이트 위치가 이전 위치에서 바뀌었으면 버퍼를 갱신한다.
    result = UpdateBuffers(deviceContext);
    if (!result)
    {
        return false;
    }
    
    // 정점·인덱스 버퍼를 그래픽 파이프라인에 올려 그릴 준비를 한다.
    RenderBuffers(deviceContext);
    
    return true;
}

void SpriteClass::Update(float franeTime)
{
    // 매 프레임마다 프레임 시간을 누적한다.
    m_frameTime += franeTime;
    
    // 프레임 시간이 순환 주기에 도달했는지 확인한다.
    if (m_frameTime >= m_cycleTime)
    {
        // 도달했다면 프레임 시간을 초기화하고 텍스처 배열의 다음 스프라이트로 넘어간다.
        m_frameTime -= m_cycleTime;
        m_currentTexture++;
        
        // 마지막 스프라이트 텍스처라면 다시 텍스처 배열의 처음(첫 텍스처)으로 돌아간다.
        if(m_currentTexture == m_textureCount)
        {
            m_currentTexture = 0;
        }
    }
    
    return;
}

int SpriteClass::GetIndexCount()
{
    return m_indexCount;
}

ID3D11ShaderResourceView* SpriteClass::GetTexture()
{
    return m_textures[m_currentTexture].GetTexture();
}

void SpriteClass::SetRenderLocation(int x, int y)
{
    m_renderX = x;
    m_renderY = y;
    return;
}

bool SpriteClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;
    
    // 이전 렌더링 위치를 -1로 초기화한다.
    m_prevPosX = -1;
    m_prevPosY = -1;

    // 정점 배열의 정점 개수를 설정한다.
    m_vertexCount = 6;

    // 인덱스 배열의 인덱스 개수를 설정한다.
    m_indexCount = m_vertexCount;

    // 정점 배열을 생성한다.
    vertices = new VertexType[m_vertexCount];

    // 인덱스 배열을 생성한다.
    indices = new unsigned long[m_indexCount];

    // 먼저 정점 배열을 0으로 초기화한다.
    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    // 인덱스 배열에 데이터를 채운다.
    for(i=0; i<m_indexCount; i++)
    {
        indices[i] = i;
    }
    
    // 동적 정점 버퍼의 description을 설정한다.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 서브리소스 구조체에 정점 데이터의 포인터를 지정한다.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 이제 마지막으로 정점 버퍼를 생성한다.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // 인덱스 버퍼의 설명 구조체를 설정한다.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 서브리소스 구조체에 인덱스 데이터의 포인터를 지정한다.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼를 생성한다.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // 정점·인덱스 버퍼를 생성하고 채웠으니 배열을 해제한다.
    delete [] vertices;
    vertices = 0;

    delete [] indices;
    indices = 0;

    return true;
}

void SpriteClass::ShutdownBuffers()
{
    // 인덱스 버퍼를 해제한다.
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // 정점 버퍼를 해제한다.
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

bool SpriteClass::UpdateBuffers(ID3D11DeviceContext* deviceContent)
{
    float left, right, top, bottom;
    VertexType* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* dataPtr;
    HRESULT result;

    // 이 비트맵을 렌더링할 위치가 바뀌지 않았다면 정점 버퍼를 갱신하지 않는다.
    if((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY))
    {
        return true;
    }

    // 렌더링 위치가 바뀌었다면 새 위치를 저장하고 정점 버퍼를 갱신한다.
    m_prevPosX = m_renderX;
    m_prevPosY = m_renderY;

    // 정점 배열을 생성한다.
    vertices = new VertexType[m_vertexCount];

    // 비트맵 왼쪽 변의 화면 좌표를 계산한다.
    left = (float)((m_screenWidth / 2) * -1) + (float)m_renderX;

    // 비트맵 오른쪽 변의 화면 좌표를 계산한다.
    right = left + (float)m_bitmapWidth;

    // 비트맵 위쪽 변의 화면 좌표를 계산한다.
    top = (float)(m_screenHeight / 2) - (float)m_renderY;

    // 비트맵 아래쪽 변의 화면 좌표를 계산한다.
    bottom = top - (float)m_bitmapHeight;
    
    // 정점 배열에 데이터를 채운다.
    // 첫 번째 삼각형.
    vertices[0].position = XMFLOAT3(left, top, 0.0f);  // 좌상단.
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단.
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // 좌하단.
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // 두 번째 삼각형.
    vertices[3].position = XMFLOAT3(left, top, 0.0f);  // 좌상단.
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);  // 우상단.
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단.
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    // 정점 버퍼를 잠근다.
    result = deviceContent->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }

    // 상수 버퍼 내부 데이터의 포인터를 얻는다.
    dataPtr = (VertexType*)mappedResource.pData;

    // 데이터를 정점 버퍼에 복사한다.
    memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    // 정점 버퍼의 잠금을 해제한다.
    deviceContent->Unmap(m_vertexBuffer, 0);

    // 포인터 참조를 해제한다.
    dataPtr = 0;

    // 더 이상 필요 없으므로 정점 배열을 해제한다.
    delete [] vertices;
    vertices = 0;

    return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;


    // 정점 버퍼의 스트라이드와 오프셋을 설정한다.
    stride = sizeof(VertexType);
    offset = 0;

    // 렌더링할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성화한다.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 렌더링할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성화한다.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 이 정점 버퍼로 그릴 프리미티브 종류를 설정한다(여기서는 삼각형).
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

bool SpriteClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName)
{
    char textureFilename[128];
    ifstream fin;
    int i, j;
    char input;
    bool result;


    // 스프라이트 정보 데이터 파일을 연다.
    fin.open(fileName);
    if(fin.fail())
    {
        return false;
    }

    // 텍스처 개수를 읽어 들인다.
    fin >> m_textureCount;

    // 파일에서 읽은 텍스처 개수로 텍스처 배열을 생성하고 초기화한다.
    m_textures = new TextureClass[m_textureCount];

    // 다음 줄 시작까지 읽는다.
    fin.get(input);

    // 각 텍스처 파일 이름을 읽어 들인다.
    for(i=0; i<m_textureCount; i++)
    {
        j=0;
        fin.get(input);
        while(input != '\n')
        {
            textureFilename[j] = input;
            j++;
            fin.get(input);
        }
        textureFilename[j] = '\0';

        // 파일 이름을 얻었으면 그 텍스처를 텍스처 배열에 불러온다.
        result = m_textures[i].Initialize(device, deviceContext, textureFilename);
        if(!result)
        {
            return false;
        }
    }

    // 사이클 시간을 읽어 들인다.
    fin >> m_cycleTime;

    // 정수 밀리초를 실수 표현으로 변환한다.
    m_cycleTime = m_cycleTime * 0.001f;

    // 파일을 닫는다.
    fin.close();

    // 첫 번째 텍스처의 크기를 얻어 2D 스프라이트 이미지의 크기로 사용한다.
    m_bitmapWidth = m_textures[0].GetWidth();
    m_bitmapHeight = m_textures[0].GetHeight();

    // 사이클의 시작 텍스처를 목록의 첫 번째로 설정한다.
    m_currentTexture = 0;

    return true;
}

void SpriteClass::ReleaseTextures()
{
    int i;
    
    //  텍스처 객체들을 해제한다.
    if (m_textures)
    {
        for (i = 0; i < m_textureCount; i++)
        {
            m_textures[i].Shutdown();
        }
        
        delete[] m_textures;
        m_textures = 0;
    }
    
    return;
}
