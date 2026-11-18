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
	
	m_Font = 0;
	m_RenderCountString = 0;
	m_ModelList = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Frustum = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFileName[128], textureFileName1[128], textureFileName2[128], textureFileName3[128], renderString[32];
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	
	// 2D 렌더링에 사용할 고정 뷰 행렬을 저장한다.
	m_Camera->GetViewMatrix(m_baseViewMatrix);
	
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
	
	// 폰트 객체를 생성하고 초기화한다.
	m_Font = new FontClass;

	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if(!result)
	{
		return false;
	}
	
	// initial render count string를 설정한다.
	strcpy_s(renderString, "Render Count: 0");

	// text object for the render count string를 생성하고 초기화한다.
	m_RenderCountString = new TextClass;
	
	result = m_RenderCountString->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, renderString, 10, 10, 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}
	
	// model list 객체를 생성하고 초기화한다.
	m_ModelList = new ModelListClass;
	m_ModelList->Initialize(25);
	
	// timer 객체를 생성하고 초기화한다.
	m_Timer = new TimerClass;

	result = m_Timer->Initialize();
	if(!result)
	{
		return false;
	}
	
	// position class 객체를 생성한다.
	m_Position = new PositionClass;
	
	// frustum class 객체를 생성한다.
	m_Frustum = new FrustumClass;
	
	return true;
}

void ApplicationClass::Shutdown()
{
	// frustum class 객체를 해제한다.
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}
	
	// position 객체를 해제한다.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}
	
	// timer 객체를 해제한다.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}
	
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
	
	// model list 객체를 해제한다.
	if(m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// text objects for the render count string를 해제한다.
	if(m_RenderCountString)
	{
		m_RenderCountString->Shutdown();
		delete m_RenderCountString;
		m_RenderCountString = 0;
	}
	
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
	float rotationY;
	bool result, keyDown;
	
	m_Timer->Frame();
	
	// 사용자가 종료를 원해 ESC를 눌렀는지 확인한다.
	if(Input->IsEscapePressed())
	{
		return false;
	}
	
	// frame time for calculating the updated position를 설정한다.
	m_Position->SetFrameTime(m_Timer->GetTime());
	
	// 좌우 방향키가 눌렸는지 확인하고, 눌렸으면 카메라를 그에 맞게 회전한다.
	keyDown = Input->IsLeftArrowPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = Input->IsRightArrowPressed();
	m_Position->TurnRight(keyDown);
	
	// current view point rotation를 얻는다.
	m_Position->GetRotation(rotationY);

	// rotation of the camera를 설정한다.
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);
	m_Camera->Render();
	
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(/*float rotation*/)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;//, rotateMatrix, translateMatrix;
	float positionX, positionY, positionZ, radius;
	int modelCount, renderCount, i;
	bool renderModel, result;
	
	// 씬 시작을 위해 버퍼 비우기
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 카메라 위치에 기반해서 뷰 행렬 생성하기
	m_Camera->Render();
	
	// 카메라와 d3d 객체에서 world, view, projection, ortho 행렬 가져오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	
	// 뷰 프러스텀을 구성한다.
	m_Frustum->ConstructFrustum(viewMatrix, projectionMatrix, SCREEN_DEPTH);
	
	// 렌더될 모델의 개수를 얻는다
	modelCount = m_ModelList->GetModelCount();
	
	// 렌더된 모델의 개수를 초기화한다.
	renderCount = 0;
	
	// 모든 모델을 순회하며 카메라 사이에 보이는 것만 렌더링한다.
	for (i = 0; i < modelCount; i++)
	{
		// 이 인덱스에서 구체 모델의 위치와 색상을 얻는다.
		m_ModelList->GetData(i, positionX, positionY, positionZ);
		
		// 이미 알려진 값이므로 구의 반지름을 1.0으로 설정합니다.
		radius = 1.0f;
		
		// 구 모델이 뷰 프러스텀 안에 있는지 검사한다.
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);
		
		// 보이면 렌더링하고, 아니면 이 모델을 건너뛰고 다음 구를 검사한다.
		if (renderModel)
		{
			// 모델응 렌더링할 위치로 이동한다.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			
			// 라이스 셰이더로 모델을 렌더링한다.
			m_Model->Render(m_Direct3D->GetDeviceContext());
			
			result = m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(0), m_Light->GetDirection(), m_Light->GetDiffuseColor());
			if(!result)
			{
				return false;
			}
			
			// 이 모델을 렌더링했으므로 이번 프레임의 카운트를 증가시킨다.
			renderCount++;
		}
	}
	
	// render count text를 갱신한다.
	result = UpdateRenderCountString(renderCount);
	if(!result)
	{
		return false;
	}
	
	// 2D 렌더링을 위해 Z 버퍼를 끄고 알파 블렌딩을 켠다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();
	
	// 월드 행렬을 리셋한다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// render count text string using the font shader를 렌더링한다.
	m_RenderCountString->Render(m_Direct3D->GetDeviceContext());
	
	result = m_ShaderManager->RenderFontShader(m_Direct3D->GetDeviceContext(), m_RenderCountString->GetIndexCount(), worldMatrix, m_baseViewMatrix, orthoMatrix,
								  m_Font->GetTexture(), m_RenderCountString->GetPixelColor());
	if(!result)
	{
		return false;
	}
	
	// 2D 렌더링이 완료되었으니 Z 버퍼를 키고 알파 블렌딩을 끈다.
	m_Direct3D->TurnZBufferOn();
	m_Direct3D->DisableAlphaBlending();
	
	// 씬에 랜더된 씬을 투영하기
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::UpdateRenderCountString(int renderCount)
{
	char tempString[16], finalString[32];
	bool result;

	// 렌더 카운트 정수를 문자열로 변환한다.
	sprintf_s(tempString, "%d", renderCount);

	// render count string를 설정한다.
	strcpy_s(finalString, "Render Count: ");
	strcat_s(finalString, tempString);

	// 새 문자열 정보로 문장 정점 버퍼를 갱신한다.
	result = m_RenderCountString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}

	return true;
}
