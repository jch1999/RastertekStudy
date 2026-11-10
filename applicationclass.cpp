////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

#include "lightshaderclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Light = 0;
	m_ShaderManager = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFileName[128], textureFileName1[128], textureFileName2[128], textureFileName3[128];
	bool result;

	// Direct3D 객체 생성 및 초기화
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}
	
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	
	// Camera 객체 생성
	m_Camera = new CameraClass;
	
	// 카메라 초기 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -8.0f);
	m_Camera->Render();
	
	// 도델 파일 이름을 설정한다.
	strcpy_s(modelFileName, "data/sphere.txt");
	
	// file name of the textures를 설정한다.
	strcpy_s(textureFileName1, "data/stone01.tga");
	strcpy_s(textureFileName2, "data/normal01.tga");
	strcpy_s(textureFileName3, "data/spec02.tga");
	
	// 모델 객체를 생성하고 초기화한다.
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFileName, textureFileName1, textureFileName2, textureFileName3);
	if (!result)
	{
		return false;
	}
	
	// 빛 객체를 생성하고 초기화한다.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(2.0f);
	
	// 노멀 맵 셰이더 객체를 생성하고 초기화한다.
	m_ShaderManager = new ShaderManagerClass;

	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}

void ApplicationClass::Shutdown()
{
	// shader manager 객체를 해제한다.
	if(m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}
	
	// light 객체를 해제한다.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	
	// model 객체 해제
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	
	// Camera 객체 해제
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	
	// Direct3D 객체 해제.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame(InputClass* Input)
{
	// float frameTime;
	bool result;//, mouseDown;
	// int mouseX, mouseY;
	
	static float rotation = 360.0f;
	
	// 사용자가 종료를 원해 ESC를 눌렀는지 확인한다.
	if(Input->IsEscapePressed())
	{
		return false;
	}
	
	// 큐브 회전값을 매 프레임 갱신.
	rotation -= 0.0174532925f * 0.25f;
	if(rotation <= 0.0f)
	{
		rotation += 360.0f;
	}
	
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix;
	bool result;
	
	// 씬 시작을 위해 버퍼 비우기
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 카메라 위치에 기반해서 뷰 행렬 생성하기
	m_Camera->Render();
	
	// 카메라와 d3d 객체에서 world, view, projection 행렬 가져오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	// 행렬들을 설정한다.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);
	
	// 텍스처 셰이더를 사용해 모델을 렌더한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(0));
	if(!result)
	{
		return false;
	}
	
	// 행렬들을 설정한다.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(-1.0f, -1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);
	
	// 텍스처 셰이더를 사용해 모델을 렌더한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(0), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}
	
	// 행렬들을 설정한다.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(1.5f, -1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);
	
	// 텍스처 셰이더를 사용해 모델을 렌더한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ShaderManager->RenderNormalMapShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(0), m_Model->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}
	
	// 행렬들을 설정한다.
	rotateMatrix = XMMatrixRotationY(rotation);
	translateMatrix = XMMatrixTranslation(2.5f, 1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);
	
	// 텍스처 셰이더를 사용해 모델을 렌더한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ShaderManager->RenderSpecMapShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(0), m_Model->GetTexture(1), m_Model->GetTexture(2), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
		m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if(!result)
	{
		return false;
	}
	
	// 씬에 랜더된 씬을 투영하기
	m_Direct3D->EndScene();

	return true;
}