#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{
    m_TextureShader = 0;
    m_LightShader = 0;
    m_NormalMapShader = 0;
    m_SpecMapShader = 0;
}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass&)
{
}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    
    // 텍스처 셰이더 객체를 생성하고 초기화한다.
    m_TextureShader = new TextureShaderClass;

    result = m_TextureShader->Initialize(device, hwnd);
    if(!result)
    {
        return false;
    }
    
    // 라이트 셰이더 객체를 생성하고 초기화한다.
    m_LightShader = new LightShaderClass;

    result = m_LightShader->Initialize(device, hwnd);
    if(!result)
    {
        return false;
    }
    
    // 노멀 맵 셰이더 객체를 생성하고 초기화한다.
    m_NormalMapShader = new NormalMapShaderClass;

    result = m_NormalMapShader->Initialize(device, hwnd);
    if(!result)
    {
        return false;
    }
    
    // 스페큘러 맵 셰이더 객체를 생성하고 초기화한다.
    m_SpecMapShader = new SpecMapShaderClass;

    result = m_SpecMapShader->Initialize(device, hwnd);
    if(!result)
    {
        return false;
    }

    return true;
}

void ShaderManagerClass::Shutdown()
{
    // 스페큘러 맵 셰이더 객체를 해제한다.
    if(m_SpecMapShader)
    {
        m_SpecMapShader->Shutdown();
        delete m_SpecMapShader;
        m_SpecMapShader = 0;
    }
    
    // 노멀 맵 셰이더 객체를 해제한다.
    if(m_NormalMapShader)
    {
        m_NormalMapShader->Shutdown();
        delete m_NormalMapShader;
        m_NormalMapShader = 0;
    }

    // 라이트 셰이더 객체를 해제한다.
    if(m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // 텍스처 셰이더 객체를 해제한다.
    if(m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    return;
}

bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture)
{
    bool result;
    
    result = m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
    if(!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
    bool result;

    result = m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
    if(!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderNormalMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
    bool result;
    
    result = m_NormalMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuseColor);
    if(!result)
    {
        return false;
    }

    return true;
}

bool ShaderManagerClass::RenderSpecMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* specularTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor,
    XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
    bool result;
    
    result = m_SpecMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, specularTexture,
        lightDirection, diffuseColor,cameraPosition, specularColor, specularPower);
    if(!result)
    {
        return false;
    }

    return true;
}


