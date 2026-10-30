////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

#include "lightshaderclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	// m_Model = 0;
	// m_ColorShader = 0;
	// m_TextureShader = 0;
	// m_LightShader = 0;
	m_Light = 0;
	// m_TextureShader = 0;
	// m_MultiTextureShader = 0;
	// m_LightMapShader = 0;
	// m_AlphaMapShader = 0;
	m_NormalMapShader = 0;
	// m_Bitmap = 0;
	m_Sprite = 0;
	m_Timer = 0;
	// m_FontShader = 0;
	// m_Font = 0;
	// m_TextString1 = 0;
	// m_TextString2 = 0;
	// m_Fps = 0;
	// m_FpsString = 0;
	// m_MouseStrings = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFileName[128], textureFileName1[128], textureFileName2[128];
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
	/*m_Camera->SetPostion(0.0f, 5.0f,-12.0f);
	m_Camera->SetRotation(22.0f, 0.0f, 0.0f);*/
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	
	/*// alpha map shader 객체를 생성하고 초기화한다.
	m_AlphaMapShader = new AlphaMapShaderClass;
	
	result = m_AlphaMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the alpha map shdaer object.", L"Error", MB_OK);
		return false;
	}*/
	
	// 노멀 맵 셰이더 객체를 생성하고 초기화한다.
	m_NormalMapShader = new NormalMapShaderClass;

	result = m_NormalMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the normal map shader object.", L"Error", MB_OK);
		return false;
	}
	
	// 도델 파일 이름을 설정한다.
	strcpy_s(modelFileName, "data/cube.txt");
	
	// file name of the textures를 설정한다.
	strcpy_s(textureFileName1, "data/stone01.tga");
	strcpy_s(textureFileName2, "data/normal01.tga");
	
	// 모델 객체를 생성하고 초기화한다.
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFileName, textureFileName1, textureFileName2);
	if (!result)
	{
		return false;
	}
	
	// 빛 객체를 생성하고 초기화한다.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void ApplicationClass::Shutdown()
{
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
	
	// 노멀 맵 셰이더 객체를 해제한다.
	if(m_NormalMapShader)
	{
		m_NormalMapShader->Shutdown();
		delete m_NormalMapShader;
		m_NormalMapShader = 0;
	}
	/*// alpha map shader 객체를 해제한다.
	if (m_AlphaMapShader)
	{
		m_AlphaMapShader->Shutdown();
		delete m_AlphaMapShader;
		m_AlphaMapShader = 0;
	}*/
	
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
	bool result, mouseDown;
	// int mouseX, mouseY;
	
	static float rotation = 360.0f;
	
	// 사용자가 종료를 원해 ESC를 눌렀는지 확인한다.
	if(Input->IsEscapePressed())
	{
		return false;
	}
	
	/*// 입력 객체에서 마우스 위치를 얻는다,
	Input->GetMouseLocation(mouseX, mouseY);

	// 마우스 버튼이 눌렸는지 확인한다.
	mouseDown = Input->IsMousePressed();

	// mouse strings each frame를 갱신한다.
	result = UpdateMouseStrings(mouseX, mouseY, mouseDown);
	if(!result)
	{
		return false;
	}*/
	
	/*// frames per second each frame를 갱신한다.
	result = UpdateFps();
	if(!result)
	{
		return false;
	}*/
	
	/*static float rotation = 0.0f;

	// 각 프레임별 rotation 변수 갱신
	rotation -= 0.0174532925f * 0.1f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}*/
	
	/*// 시스템 통계를 갱신한다.
	m_Timer->Frame();
	
	// 횬재 프레임 시간을 가져온다.
	frameTime = m_Timer->GetTime();
	
	// 프레임 시간을 사용해 스프라이트 객체를 갱신한다.
	m_Sprite->Update(frameTime);*/
	
	// 그래픽 씬 랜더
	// result = Render(rotation);
	
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
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix, srMatrix;
	// XMFLOAT4 diffuseColor[4], lightPosition[4];
	int i;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;//orthoMatrix;
	bool result;
	
	// 씬 시작을 위해 버퍼 비우기
	// m_Direct3D->BeginScene(0.6f, 0.8f, 0.5f, 1.0f);
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 카메라 위치에 기반해서 뷰 행렬 생성하기
	m_Camera->Render();
	
	// 카메라와 d3d 객체에서 world, view, projection 행렬 가져오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	// m_Direct3D->GetOrthoMatrix(orthoMatrix);
	// 모델이 회전하도록 월드 행렬을 회전 값으로 회전시킨다.
	worldMatrix = XMMatrixRotationY(rotation);
	
	// model using the multitexture shader를 렌더링한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	result = m_NormalMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
									  m_Model->GetTexture(0), m_Model->GetTexture(1), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}
	
	// 2D 렌더링을 위해 Z 버퍼를 끄고 알파 블렌딩을 켠다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();
	
	// 2D 렌더링이 끝났으니 Z 버퍼를 켜고 알파 블렌딩을 끈다.
	m_Direct3D->TurnZBufferOn();
	m_Direct3D->DisableAlphaBlending();
	
	// 씬에 랜더된 씬을 투영하기
	m_Direct3D->EndScene();

	return true;
}

/*bool ApplicationClass::UpdateFps()
{
	int fps;
	char tempString[16], finalString[16];
	float red, green, blue;
	bool result;


	// 매 프레임마다 fps를 갱신한다.
	m_Fps->Frame();

	// 현재 fps를 가져온다.
	fps = m_Fps->GetFps();

	// 직전 프레임의 fps와 같은지 확인한다. 같으면 텍스트 문자열을 갱신할 필요가 없다.
	if(m_previousFps == fps)
	{
		return true;
	}

	// 다음 프레임에서 비교하기 위해 fps를 저장한다.
	m_previousFps = fps;

	// fps를 100,000 미만으로 잘라낸다.
	if(fps > 99999)
	{
		fps = 99999;
	}

	// fps 정수를 문자열 형식으로 변환한다.
	sprintf_s(tempString, "%d", fps);

	// fps 문자열을 구성한다.
	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	// fps가 60 이상이면 fps 색을 녹색으로 설정한다.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 60 미만이면 fps 색을 노란색으로 설정한다.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 30 미만이면 fps 색을 빨간색으로 설정한다.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// 새 문자열 정보로 문장 정점 버퍼를 갱신한다.
	result = m_FpsString->UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, red, green, blue);
	if(!result)
	{
		return false;
	}

	return true;
}*/

/*
bool ApplicationClass::UpdateMouseStrings(int mouseX, int mouseY, bool mouseDown)
{
	char tempString[16], finalString[32];
	bool result;
	
	// 마우스 X 정수를 문자열로 변환한다.
	sprintf_s(tempString, "%d", mouseX);

	// mouse X string를 설정한다.
	strcpy_s(finalString, "Mouse X: ");
	strcat_s(finalString, tempString);

	// 새 문자열 정보로 문장 정점 버퍼를 갱신한다.
	result = m_MouseStrings[0].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 10, 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}

	// 마우스 Y 정수를 문자열로 변환한다.
	sprintf_s(tempString, "%d", mouseY);

	// mouse Y string를 설정한다.
	strcpy_s(finalString, "Mouse Y: ");
	strcat_s(finalString, tempString);

	// 새 문자열 정보로 문장 정점 버퍼를 갱신한다.
	result = m_MouseStrings[1].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 35, 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}

	// mouse button string를 설정한다.
	if(mouseDown)
	{
		strcpy_s(finalString, "Mouse Button: Yes");
	}
	else
	{
		strcpy_s(finalString, "Mouse Button: No");
	}

	// 새 문자열 정보로 문장 정점 버퍼를 갱신한다.
	result = m_MouseStrings[2].UpdateText(m_Direct3D->GetDeviceContext(), m_Font, finalString, 10, 60, 1.0f, 1.0f, 1.0f);
	if(!result)
	{
		return false;
	}
	
	return true;
}
*/
