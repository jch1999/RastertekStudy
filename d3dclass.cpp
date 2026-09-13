#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

// 시작 — 지역 변수 선언 포함
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;


	// vsync 설정을 저장한다.
	m_vsync_enabled = vsync;

//팩토리 → 어댑터 → 출력 → 모드 목록
	
	// DirectX 그래픽 인터페이스 팩토리를 생성한다.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	
	if (FAILED(result))
	{
		return false;
	}

	// 팩토리로 기본 그래픽 인터페이스(비디오 카드)의 어댑터를 생성한다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 기본 어댑터 출력(모니터)을 열거한다.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 어댑터 출력(모니터)에서 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 포맷에 맞는 모드 개수를 얻는다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 이 모니터/비디오 카드 조합에서 가능한 모든 디스플레이 모드를 담을 목록을 생성한다.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 이제 디스플레이 모드 목록 구조체를 채운다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

// 해상도 일치 모드에서 리프레시율 추출
	// 이제 모든 디스플레이 모드를 훑어 화면 너비·높이와 일치하는 것을 찾는다.
	// 일치하는 것을 찾으면 그 모니터 리프레시율의 분자와 분모를 저장한다.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

// 카드 이름 / VRAM
	// 어댑터(비디오 카드)의 설명을 얻는다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 전용 비디오 카드 메모리를 메가바이트 단위로 저장한다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오 카드 이름을 문자 배열로 변환해 저장한다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}
	
// 임시 DXGI 객체 정리(역순)
	// 디스플레이 모드 목록을 해제한다.
	delete[] displayModeList;
	displayModeList = 0;

	// 어댑터 출력을 해제한다.
	adapterOutput->Release();
	adapterOutput = 0;

	// 어댑터를 해제한다.
	adapter->Release();
	adapter = 0;

	// 팩토리를 해제한다.
	factory->Release();
	factory = 0;

// 스왑체인 설정 + 디바이스 생성
	// 스왑체인 설명 구조체를 초기화한다.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백버퍼를 하나로 설정한다.
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 너비와 높이를 설정한다.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백버퍼를 일반 32비트 서피스로 설정한다.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 리프레시율을 설정한다.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 용도를 설정한다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링할 창의 핸들을 설정한다.
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링을 끈다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 전체화면 또는 창 모드로 설정한다.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔라인 순서와 스케일링을 미지정으로 설정한다.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 표시(present) 후 백버퍼 내용을 폐기한다.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 고급 플래그는 설정하지 않는다.
	swapChainDesc.Flags = 0;

// 디바이스 + 스왑체인 + 컨텍스트 생성
	swapChainDesc.Flags = 0;
	// 기능 수준을 DirectX 11로 설정한다.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트를 생성한다.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}
	
// 렌더 타겟 뷰
	// 백버퍼의 포인터를 얻는다.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 포인터로 렌더 타겟 뷰를 생성한다.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 필요 없으므로 백버퍼 포인터를 해제한다.
	backBufferPtr->Release();
	backBufferPtr = 0;

// 깊이 / 스텐실 텍스처 — 2D 텍스처로 생성
	// 깊이 버퍼의 설명 구조체를 초기화한다.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 설명 구조체를 설정한다.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 채워진 설명 구조체로 깊이 버퍼용 텍스처를 생성한다.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

// 깊이 / 스텐실 상태
	// 스텐실 상태의 설명 구조체를 초기화한다.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태의 설명 구조체를 설정한다.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀이 앞면일 때의 스텐실 연산.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀이 뒷면일 때의 스텐실 연산.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 생성한다.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}
	
	// 깊이 스텐실 상태를 설정한다.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

// 깊이/스텐실 뷰 + 출력 병합 단계 바인딩
	// 깊이 스텐실 뷰를 초기화한다.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 설명 구조체를 설정한다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 생성한다.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩한다.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

// 래스터라이저 · 뷰포트 · 행렬
// 래스터라이저 상태
	// 어떤 폴리곤을 어떻게 그릴지 결정하는 래스터 설명 구조체를 설정한다.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 방금 채운 설명 구조체로 래스터라이저 상태를 생성한다.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 래스터라이저 상태를 설정한다.
	m_deviceContext->RSSetState(m_rasterState);

// 뷰포트 설정
	// 렌더링용 뷰포트를 설정한다.
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// 뷰포트를 생성한다.
	m_deviceContext->RSSetViewports(1, &m_viewport);

// 행렬 (투영/월드/직교)
	// 투영 행렬을 설정한다.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링용 투영 행렬을 생성한다.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 월드 행렬을 단위 행렬로 초기화한다.
	m_worldMatrix = XMMatrixIdentity();

	// 2D 렌더링용 직교 투영 행렬을 생성한다.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	
	// 파라미터를 설정하기 전에 두 번째 depth stencil 상태를 초기화한다.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	
	// 이제 2D 렌더링을 위해 Z 버퍼를 끄는 두 번째 depth stencil 상태를 생성한다. 유일한 차이는
	// DepthEnable이 false로 설정된다는 점이며, 나머지 파라미터는 기존 depth stencil 상태와 동일하다.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// 디바이스를 사용해 상태를 생성한다.
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if(FAILED(result))
	{
		return false;
	}
	
	return true;
}

void D3DClass::Shutdown()
{
	// 종료 전에 창 모드로 설정한다. 그러지 않으면 스왑체인 해제 시 예외가 발생한다.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// 버퍼를 지울 색을 설정한다.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백버퍼를 지운다.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼를 지운다.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// 렌더링이 끝났으므로 백버퍼를 화면에 표시한다.
	if (m_vsync_enabled)
	{
		// 화면 리프레시율에 맞춰 동기화한다.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 가능한 한 빠르게 표시한다.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::SetBackBufferRenderTarget()
{
	// 출력 렌더 파이프라인에 랜더 타겟 뷰와 뎁스 스텐실 버퍼를 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}

void D3DClass::ResetViewport()
{
	// 뷰포트 설정
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}
