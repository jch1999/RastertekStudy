////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	// m_ColorShader = 0;
	// m_TextureShader = 0;
	m_LightShader = 0;
	m_Light = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFileName[128];
	char modelFileName[128];
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
	m_Camera->SetPostion(0.0f, 0.0f,-10.0f);
	
	// 모델 파일 이름을 설정한다.
	strcpy_s(modelFileName, "data/cube.txt");
	
	// 로드할 텍스처 파일의 이름을 설정한다.
	strcpy_s(textureFileName, "data/stone01.tga");
	
	// 모델 생성 및 초기화
	m_Model = new ModelClass;
	
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFileName, modelFileName);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize this model object.", L"Error", MB_OK);
		return false;
	}
	
	// 라이트 셰이더 객체 생성 및 초기화
	m_LightShader   = new LightShaderClass;
	
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize this light shader object.", L"Error", MB_OK);
		return false;
	}
	
	// 라이트 오브젝트 생성 및 초기화
	m_Light = new LightClass;
	
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	
	return true;
}

void ApplicationClass::Shutdown()
{
	// 라이트 오브젝트 해제
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	
	// Light Shader 객체 해제
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
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


bool ApplicationClass::Frame()
{
	static float rotation = 0.0f;
	bool result;

	// 각 프레임별 rotation 변수 갱신
	rotation -= 0.0174532925f * 0.1f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}
	
	// 그래픽 씬 랜더
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, srMatrix;
	bool result;
	
	// 씬 시작을 위해 버퍼 비우기
	m_Direct3D->BeginScene(0.6f, 0.8f, 0.5f, 1.0f);
	
	// 카메라 위치에 기반해서 뷰 행렬 생성하기
	m_Camera->Render();
	
	// 카메라와 d3d 객체에서 world, view, projection 행렬 가져오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	rotationMatrix = XMMatrixRotationY(rotation); // 회전 행렬을 만든다.
	translationMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f); // 이동 행렬을 만든다.
	
	// 이들을 곱해 최종 월드 변환 행렬을 만든다.
	worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

	// 모델의 버택스 & 인데스 버퍼를 파이프라인에 넣어 그리기 준비하기
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 셰이더를 사용해서 모델 렌덜하기
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
								   m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}
	
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	rotationMatrix = XMMatrixRotationY(rotation); // 회전 행렬을 만든다.
	translationMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f); // 이동 행렬을 만든다.
	
	// 이들을 곱해 최종 월드 변환 행렬을 만든다.
	// worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);
	srMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translationMatrix);
	
	// 모델의 버택스 & 인데스 버퍼를 파이프라인에 넣어 그리기 준비하기
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	// 텍스처 셰이더를 사용해서 모델 렌덜하기
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}
	
	// 씬에 랜더된 씬을 투영하기
	m_Direct3D->EndScene();

	return true;
}
