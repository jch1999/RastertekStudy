#include "normalmapshaderclass.h"

NormalMapShaderClass::NormalMapShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
    m_samplerState = 0;
    m_lightBuffer = 0;
}

NormalMapShaderClass::NormalMapShaderClass(const NormalMapShaderClass&)
{
}

NormalMapShaderClass::~NormalMapShaderClass()
{
}

bool NormalMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    int error;
    
    // 정점 셰이더 파일의 이름을 설정한다.
    error = wcscpy_s(vsFileName, 128, L"normalmap.vs");
    if (error != 0)
    {
        return false;
    }
    
    // 픽셀 셰이더 파일의 이름을 설정한다.
    error = wcscpy_s(psFileName, 128, L"normalmap.ps");
    if (error != 0)
    {
        return false;
    }
    
    // 정점/픽셀 셰이더를 초기화한다.
    result = InitializeShader(device, hwnd, vsFileName, psFileName);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void NormalMapShaderClass::Shutdown()
{
    // 정점/픽셀 셰이더와 관련 객체들을 해제한다.
    ShutdownShader();
    
    return;
}

bool NormalMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
    bool result;
    
    // 렌더링에 사용할 셰이더 매개변수를 설정한다.
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture1, texture2, lightDirection, diffuseColor);
    if (!result)
    {
        return false;
    }
    
    // 준비된 버퍼를 셰이더를 렌더링한다.
    RenderShader(deviceContext, indexCount);
    
    return true;
}

bool NormalMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    
    // 이 함수에서 사용할 포인터들을 null로 초기화한다.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;
    
    // 정점 셰이더 코드를 컴파일한다.
    result = D3DCompileFromFile(vsFileName, NULL, NULL, "NormalMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
        0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일에 실패하면 오류 메시지에 무언가 기록되어 있을 것이다.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
        }
        // 오류 메시지가 비어 있으면 셰이더 파일 자체를 찾지 못한 것이다.
        else
        {
            MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    
    // 픽셀 셰이더 코드를 컴파일한다.
    result = D3DCompileFromFile(psFileName, NULL, NULL, "NormalMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
        0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일에 실패하면 오류 메시지에 무언가 기록되어 있을 것이다.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
        }
        // 오류 메시지가 비어 있으면 셰이더 파일 자체를 찾지 못한 것이다.
        else
        {
            MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    
    // 버퍼로부터 정점 셰이더를 생성한다.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result))
    {
        return false;
    }
    
    // 버퍼로부터 픽셀 셰이더를 생성한다.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
    {
        return false;
    }

	// 정점 입력 레이아웃 설명 구조체를 생성한다.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;
    
    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;
    
    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;
    
    polygonLayout[3].SemanticName = "TANGENT";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;
    
    polygonLayout[4].SemanticName = "BINORMAL";
    polygonLayout[4].SemanticIndex = 0;
    polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[4].InputSlot = 0;
    polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[4].InstanceDataStepRate = 0;
    
    // 레이아웃의 요소 개수를 구한다.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // 정점 입력 레이아웃을 생성한다.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
    {
        return false;
    }
    
    // 더 이상 필요 없는 정점 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제한다.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;
    
    // 정점 셰이더에 있는 동적 행렬 상수 버퍼의 설명 구조체를 설정한다.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    
    // 이 클래스에서 정점 셰이더의 상수 버퍼에 접근할 수 있도록 상수 버퍼 포인터를 생성한다.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);;
    if (FAILED(result))
    {
        return false;
    }
    
    // 텍스처 샘플러 상태 설명 구조체를 생성한다.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // 텍스처 샘플러 상태를 생성한다.
    result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if(FAILED(result))
    {
        return false;
    }
    
    // 픽셀 셰이더에 있는 동적 조명 상수 버퍼의 설명 구조체를 설정한다.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // 이 클래스에서 정점 셰이더의 상수 버퍼에 접근할 수 있도록 상수 버퍼 포인터를 생성한다.
    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
    if(FAILED(result))
    {
        return false;
    }
    
    return true;
}

void NormalMapShaderClass::ShutdownShader()
{
    // 조명 상수 버퍼를 해제한다.
    if(m_lightBuffer)
    {
        m_lightBuffer->Release();
        m_lightBuffer = 0;
    }

    // 샘플러 상태를 해제한다.
    if(m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = 0;
    }

    // 행렬 상수 버퍼를 해제한다.
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // 레이아웃을 해제한다.
    if(m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // 픽셀 셰이더를 해제한다.
    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // 정점 셰이더를 해제한다.
    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    return;
}

void NormalMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;

    // 오류 메시지 텍스트 버퍼의 포인터를 얻는다.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // 메시지의 길이를 얻는다.
    bufferSize = errorMessage->GetBufferSize();

    // 오류 메시지를 기록할 파일을 연다.
    fout.open("shader-error.txt");

    // 오류 메시지를 기록한다.
    for(i=0; i<bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // 파일을 닫는다.
    fout.close();

    // 오류 메시지를 해제한다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 오류가 텍스트 파일에 있으니 확인하라고 사용자에게 알리는 메시지 박스를 띄운다.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFileName, MB_OK);

    return;
}

bool NormalMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;
    LightBufferType* dataPtr2;
    
    // 셰이더에 넘기기 위해 행렬을 전치한다.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);
    
    // 상수 버퍼에 쓸 수 있도록 잠근다.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }
    
    // 상수 버퍼 내부 데이터와 포인터를 얻는다.
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    
    // 행렬들을 상수 버퍼에 복사한다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    
    // 상수 버퍼의 잠금을 해제한다.
    deviceContext->Unmap(m_matrixBuffer, 0);
    
    // 정점 셰이더에서 상수 버퍼의 위치를 설정한다.
    bufferNumber = 0;

    // 마지막으로 갱신된 값으로 정점 셰이더의 상수 버퍼를 설정한다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);;
    
    // 픽셀 셰이더에 셰이더 텍스처 자원을 설정한다.
    deviceContext->PSSetShaderResources(0, 1, &texture1);
    deviceContext->PSSetShaderResources(1, 1, &texture2);
    
    // 조명 상수 버퍼에 쓸 수 있도록 잠근다
    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);;
    if (FAILED(result))
    {
        return false;
    }
    
    // 상수 버퍼 내부 데이터의 포인터를 얻는다.
    dataPtr2 = (LightBufferType*)mappedResource.pData;

    // 조명 변수들을 상수 버퍼에 복사한다.
    dataPtr2->diffuseColor = diffuseColor;
    dataPtr2->lightDirection = lightDirection;
    dataPtr2->padding = 0.0f;

    // 상수 버퍼의 잠금을 해제한다.
    deviceContext->Unmap(m_lightBuffer, 0);

    // 픽셀 셰이더에서 조명 상수 버퍼의 위치를 설정한다.
    bufferNumber = 0;

    // 마지막으로 갱신된 값으로 픽셀 셰이더의 조명 상수 버퍼를 설정한다.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

    return true;
}

void NormalMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // 정점 입력 레이아웃을 설정한다.
    deviceContext->IASetInputLayout(m_layout);

    // 이 삼각형을 렌더링하는 데 사용할 정점·픽셀 셰이더를 설정한다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 픽셀 셰이더에 샘플러 상태를 설정한다.
    deviceContext->PSSetSamplers(0, 1, &m_samplerState);

    // 삼각형을 렌더링한다.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}
