////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

TextureClass::TextureClass()
{
    m_targaData = 0;
    m_texture = 0;
    m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName)
{
    bool result;
    int height, width;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT hResult;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    
    // 메모리에 targa 이미지 데이터 로드하기
    result = LoadTarga32Bit(fileName);
    if (!result)
    {
        return false;
    }
    
    // 텍스처의 설명 구조체를 셋업
    textureDesc.Height = m_height;
    textureDesc.Width = m_width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    
    // 빈 텍스처 생성하기
    hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
    if (FAILED(hResult))
    {
        return false;
    }
    
    // targa 이미지 데이터의 행 pitch 설정
    rowPitch = (m_width * 4) * sizeof(unsigned char);
    
    // 테스처에 targa 이미지 데이터 복사하기
    deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
    
    // 셰이더 리소스 뷰 설명 구조체 셋업
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;
    
    // 텍스처로부터 셰이더 리소스 뷰 생성하기
    hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    if (FAILED(hResult))
    {
        return false;
    }
    
    // 이 텍스처의 밉맵들을 생성하기
    deviceContext->GenerateMips(m_textureView);
    
    // 이미지 데이터가 텍스처에 로드되었으므로 targa 이미지 데이터 해제하기
    delete[] m_targaData;
    m_targaData = 0;
    
    return true;
}

void TextureClass::Shutdown()
{
    // 텍스처 뷰 리소스 해제
    if (m_textureView)
    {
        m_textureView->Release();
        m_textureView = 0;
    }
    
    // 텍스처 해제
    if (m_texture)
    {
        m_texture->Release();
        m_texture = 0;
    }
    
    // targa 데이터 해제
    if (m_targaData)
    {
        delete[] m_targaData;
        m_targaData = 0;
    }
    
    return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_textureView;
}

int TextureClass::GetWidth()
{
    return m_width;
}

int TextureClass::GetHeight()
{
    return m_height;
}

bool TextureClass::LoadTarga32Bit(char* fileName)
{
    int error, bpp, imageSize, index, i, j, k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;
    
    // 바이너리로 읽기로 targa 파일 열기
    error = fopen_s(&filePtr, fileName, "rb");
    if (error != 0)
    {
        return false;
    }
    
    // 파일 헤더 읽기
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    if (count != 1)
    {
        return false;
    }
    
    // 헤더에서 중요한 정보 가져오기
    m_height = (int)targaFileHeader.height;
    m_width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;
    
    // 32비트이고 24비트가 아닌 지 체크하기 ?
    if (bpp != 32)
    {
        return false;
    }
    
    // 32 비트 이미지 데이터의 크기 계산하기
    imageSize = m_width * m_height * 4;
    
    // targe 이미지 데이터를 위한 메모리 할당
    targaImage = new unsigned char[imageSize];
    
    // targa 이미지 데이터 읽기
    count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    if (count != imageSize)
    {
        return false;
    }
    
    // 파일 닫기
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }
    
    // targa destination 데이터를 위한 메모리 할당
    m_targaData = new unsigned char[imageSize];
    
    // targa destination 데이터 배열을 위한 인덱스 초기화
    index = 0;
    
    // targa 이미지 데이터에 인덱스 초기화
    k = (m_width * m_height * 4) - (m_width * 4);
    
    // targa 포맷은 상하가 뒤집혀 저장되고 RGBA 순서도 아니므로, targa 이미지 데이터를 올바른 순서로 대상 배열에 복사한다.
    for (j = 0; j < m_height; j++)
    {
        for (i = 0; i < m_width; i++)
        {
            m_targaData[index + 0] = targaImage[k + 2]; // Red
            m_targaData[index + 1] = targaImage[k + 1]; // Green
            m_targaData[index + 2] = targaImage[k + 0]; // Blue
            m_targaData[index + 3] = targaImage[k + 3]; // Alpha
            
            // targa 데이터에 인덱스를 증가시키기
            k += 4;
            index += 4;
        }
        
        // 상하가 뒤집혀 읽히므로 targa 이미지 데이터 인덱스를 이전 행의 열 시작 위치로 되돌린다.
        k -= (m_width * 8);
    }
    
    // targa 이미지 데이터가 destination 배열에 복사되었으므로 해제한다.
    delete[] targaImage;
    targaImage = 0;
    
    return true;
}
