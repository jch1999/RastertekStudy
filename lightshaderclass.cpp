////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightshaderclass.h"

LightShaderClass::LightShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
	m_samplerState = 0;
    m_lightBuffer = 0;
}

LightShaderClass::LightShaderClass(const LightShaderClass&)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    int error;
    wchar_t vsFileName[128];
    wchar_t psFileName[128];
    
    // 버택스 셰이더의 파일이름 설정
    error = wcscpy_s(vsFileName, 128, L"light.vs");
    if (error != 0)
    {
        return false;
    }
    
    // 픽셀 셰이더의 파일이름 설정
    error = wcscpy_s(psFileName, 128, L"light.ps");
    if (error != 0)
    {
        return false;
    }
    
    // 버택스 & 픽셀 셰이더 초기화
    result = InitializeShader(device, hwnd, vsFileName, psFileName);
    if (!result)
    {
        return false;
    }
    
    return true;
}

void LightShaderClass::Shutdown()
{
    // 버택스 & 픽셀 셰이더들과 관련 오브젝트들을 셧다운합니다.
    ShutdownShader();
    
    return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, 
    int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
    bool result;
    
    // 랜더링에 사용되는 셰이더 파라매터 셋팅
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
    if (!result)
    {
        return false;
    }
    
    // 이제 이 셰이도로 준비딘 버퍼를 렌더
    RenderShader(deviceContext, indexCount);
    
    return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC cameraBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    
    // 이 함수가 사용할 포인터를 null로 초기화
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;
    
    // 버택스 셰이더 코드를 컴파일
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일이 실패했다면 에러 메세지가 존재할 것이다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // 만약 errorMessage에 아무 메세지도 없다면 단순히 스스로 셰이더 파일을 찾지 못한 것일 겁니다.
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }
        
        return false;
    }
    
    // 픽셀 셰이더 코드 컴파일
    result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일이 실패했다면 에러 메세지가 존재할 것이다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // 만약 errorMessage에 아무 메세지도 없다면 단순히 스스로 셰이더 파일을 찾지 못한 것일 겁니다.
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }
        
        return false;
    }
    
    // 버퍼로부터 버택스 셰이더 생성
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
        NULL, &m_vertexShader);
    if (FAILED(result))
    {
        return false;
    }
    
    // 버퍼로부터 픽셀 셰이더 생성
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
        NULL, &m_pixelShader);
    if (FAILED(result))
    {
        return false;
    }
    
    // 버택스 입력 레이아웃 설명 구조체를 생성
    // 이 설정에서 ModelClass와 버택스 셰이더 내의 버택스 타입 구조체를 일치시켜야 합니다.
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
    
    // 레이아웃 내부의 요소들의 개수를 가져오기
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
    
    // 버택스 입력 레이아수 생성
    result = device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
    {
        return false;
    }
    
    // 더 이상 필요하지 않은 버택스 셰이더 버퍼와 픽셀 셰이더 버퍼 해제하기
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;
    
    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;
    
    // 버택스 셰이더 내부의 동적 행렬 상수 백터 설명 구조체를 설정
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    
    // 이 클래스를 통해 버택스 셰이더의 상수 버퍼에 접근할 수 있는 상수 버퍼 포인트 만들기
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    // 텍스처 샘플러 상태 설명 구조체를 생성
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
    
    // 텍스처 샘블러 상태 객체 생성
    result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(result))
    {
        return false;
    }
    
    // 정점 셰이더에 있는 동적 카메라 상수 버퍼의 설명 구조체를 설정한다.
    cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cameraBufferDesc.MiscFlags = 0;
    cameraBufferDesc.StructureByteStride = 0;
    
    // 이 클래스에서 정점 셰이더의 상수 버퍼에 접근할 수 있도록 카메라 상수 버퍼 포인터를 생성한다.
    result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    // 픽셀 셰이더 내부의 동적 조명 상수 백터 설명 구조체를 설정
    // D3D11_BIND_CONSTANT_BUFFER를 사용한 경우 ByteWidth는 항상 16의 배수여야 함을 명심하세요.
    // 아니라면 CreateBuffer는 실패할 것입니다. 
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;
    
    // 이 클래스를 통해 버택스 셰이더의 상수 버퍼에 접근할 수 있는 상수 버퍼 포인트 만들기
    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    return true;
}


void LightShaderClass::ShutdownShader()
{
    // 라이트 상수 버퍼 해제
    if (m_lightBuffer)
    {
        m_lightBuffer->Release();
        m_lightBuffer = 0;
    }
    
    // 카메라 상수 버퍼를 해제한다.
    if (m_cameraBuffer)
    {
        m_cameraBuffer->Release();
        m_cameraBuffer = 0;
    }
    
    // 샘플러 상태 객체 해제.
    if (m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = 0;
    }
    
    // 행렬 상수 버퍼 해제.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }
    
    // 레이아웃 해제.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }
    
    // 픽셀 셰이더 해제
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }
    
    // 버텍스 셰이더 해제
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
    
    return;
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
	ofstream fout;
    
    // 에러 메시지 텍스트 버퍼의 포인터 가져오기
    compileErrors = (char*)(errorMessage->GetBufferPointer());
    
    // 메시지 길이 가져오기
    bufferSize = errorMessage->GetBufferSize();
   
    // 에러 메시지를 적기 위해 파일 열기
    fout.open("shader-error.txt");
    
    // 에러 메시지 쓰기
    for (i = 0; i< bufferSize; i++)
    {
        fout << compileErrors[i];
    }
    
    // 파일 닫기
    fout.close();
    
    // error message 해제
    errorMessage->Release();
    errorMessage = 0;
    
    // 유저가 컴파일 에러 확인을 위해 텍스트 파일을 확인할 수 있도록 화면에 메시지를 띄워 알리기
    MessageBox(hwnd, L"Error Compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
    
    return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    LightBufferType* dataPtr2;
    CameraBufferType* dataPtr3;
    unsigned int bufferNumber;
    
    // 셰이더에 사용하기 위해 행렬들을 전치시킨다
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);
    
    // 쓸 수 있게 상수 버퍼를 잠그기 - GPU에서 사용 불가, CPU에서 사용 가능
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }
    
    // 상수 버퍼 안의 데이터 포인터를 가져오기
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    
    // 상수 버퍼에 행렬 복사하기
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    
    // 상수 버퍼 잠금해제
    deviceContext->Unmap(m_matrixBuffer, 0);
    
    // 버택스 셰이더안의 상수 버퍼의 위치 설정
    bufferNumber = 0;
    
    // 마침내 버택스 셰이더에 업그래이드된 값의 상수 버퍼를 설정한다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    
    // 카메라 상수 버퍼에 쓸 수 있도록 잠근다.
    result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }
    
    // 상수 버퍼 내부 데이터의 포인터를 얻는다.
    dataPtr3 = (CameraBufferType*)mappedResource.pData;
    dataPtr3->cameraPosition = cameraPosition;
    dataPtr3->paddding = 0.0f;
    
    // 카메라 상수 버퍼의 잠금을 해제한다.
    deviceContext->Unmap(m_cameraBuffer, 0);
    
    // 정점 셰이더에서 카메라 상수 버퍼의 위치를 설정한다.
    bufferNumber = 1;
    
    // 이제 갱신된 값으로 정점 셰이더의 카메라 상수 버퍼를 설정한다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
    
    // 픽셀 셰이더에 셰이더 텍스처 리소스를 설정한다.
    deviceContext->PSSetShaderResources(0, 1, &texture);
    
    // 라이트 상수 버퍼가 쓰기가 가능한 상태인지 확인하기
    result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }
    
    // 상수 버퍼안의 데이터 포인터 가져오기
    dataPtr2 = (LightBufferType*)mappedResource.pData;
    
    // 상수 버퍼에 라이트 변수들을 복사하기
    dataPtr2->ambientColor = ambientColor;
    dataPtr2->diffuseColor = diffuseColor;
    dataPtr2->lightDirection = lightDirection;
    dataPtr2->specularColor = specularColor;
    dataPtr2->specularPower = specularPower;
    
    // 상수 버퍼 잠금 해제
    deviceContext->Unmap(m_lightBuffer, 0);
    
    // 픽셀 셰이더 안에서의 라이트 상수 버퍼의 위치 설정
    bufferNumber = 0;
    
    // 마지막으로 업데이트된 값으로 픽셀 셰이더내의 라이트 상수 버퍼 설정하기
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
    
    return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // 버택스 입력 레이아웃 설정
    deviceContext->IASetInputLayout(m_layout);
    
    // 삼각형 그리기에 버택스 & 픽셀 셰이더를 사용하기 위해 설정
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);
    
    // 픽셀 셰이더에 샘플러 상태 설정
    deviceContext->PSSetSamplers(0, 1, &m_samplerState);
    
    // 삼각형 랜더
    deviceContext->DrawIndexed(indexCount, 0, 0);
    
    return;
}
