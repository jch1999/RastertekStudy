////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	// m_Model = 0;
	// m_ColorShader = 0;
	// m_TextureShader = 0;
	// m_LightShader = 0;
	// m_Lights = 0;
	m_TextureShader = 0;
	// m_Bitmap = 0;
	m_Sprite = 0;
	m_Timer = 0;
	m_FontShader = 0;
	m_Font = 0;
	// m_TextString1 = 0;
	// m_TextString2 = 0;
	m_Fps = 0;
	m_FpsString = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// char testString1[32], testString2[32];
	char fpsString[32];
	// char textureFileName[128];
	//char modelFileName[128];
	// char bitmapFilename[128];
	// char spriteFilename[128];
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
	
	// 모델 파일 이름을 설정한다.
	// strcpy_s(modelFileName, "data/plane.txt");
	
	// 로드할 텍스처 파일의 이름을 설정한다.
	// strcpy_s(textureFileName, "data/stone01.tga");
	
	// 모델 생성 및 초기화
	/*m_Model = new ModelClass;
	
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
	
	// 라이트 오브젝트 개수 설정
	m_numLights = 4;
	// 라이트 오브젝트 배열 생성 및 초기화
	m_Lights = new LightClass[m_numLights];
	
	// 각 라이트 오브젝트의 색과 위치를 수동으로 설정한다
	m_Lights[0].SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Lights[0].SetPosition(-3.0f, 1.0f, 3.0f);
	
	m_Lights[1].SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Lights[1].SetPosition(3.0f, 1.0f, 3.0f);
	
	m_Lights[2].SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Lights[2].SetPosition(-3.0f, 1.0f, -3.0f);
	
	m_Lights[3].SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Lights[3].SetPosition(3.0f, 1.0f, -3.0f);*/
	
	/*// 텍스처 셰이더 객체를 생성하고 초기화한다.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	
	// sprite info file we will be using를 설정한다.
	strcpy_s(spriteFilename, "data/sprite_data_01.txt");
	
	// sprite 객체를 생성하고 초기화한다.
	m_Sprite  = new SpriteClass;
	result = m_Sprite->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, spriteFilename, 50, 50);
	
	if(!result)
	{
		return false;
	}
	
	// timer 객체를 생성하고 초기화한다.
	m_Timer = new TimerClass;

	result = m_Timer->Initialize();
	if (!result)
	{
		return false;
	}*/
	
	// font shader 객체를 생성하고 초기화한다.
	m_FontShader = new FontShaderClass;

	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}
	
	// font 객체를 생성하고 초기화한다.
	m_Font = new FontClass;

	result = m_Font->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 0);
	if(!result)
	{
		return false;
	}
	
	/*// strings we want to display를 설정한다.
	strcpy_s(testString1, "Hello");
	strcpy_s(testString2, "Goodbye");
	// first text 객체를 생성하고 초기화한다.
	m_TextString1 = new TextClass;

	result = m_TextString1->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, testString1, 10, 10, 0.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}

	// second text 객체를 생성하고 초기화한다.
	m_TextString2 = new TextClass;

	result = m_TextString2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, testString2, 10, 50, 1.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}*/
	
	// fps 객체를 생성하고 초기화한다.
	m_Fps = new FpsClass();

	m_Fps->Initialize();

	// initial fps and fps string를 설정한다.
	m_previousFps = -1;
	strcpy_s(fpsString, "Fps: 0");

	// text object for the fps string를 생성하고 초기화한다.
	m_FpsString  = new TextClass;

	result = m_FpsString ->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, m_Font, fpsString, 10, 10, 0.0f, 1.0f, 0.0f);
	if(!result)
	{
		return false;
	}
	
	return true;
}

void ApplicationClass::Shutdown()
{
	
	// text object for the fps string를 해제한다.
	if(m_FpsString)
	{
		m_FpsString->Shutdown();
		delete m_FpsString;
		m_FpsString = 0;
	}
	
	// fps 객체를 해제한다.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}
	
	/*// text string 객체를 해제한다.
	if(m_TextString2)
	{
		m_TextString2->Shutdown();
		delete m_TextString2;
		m_TextString2 = 0;
	}
		
	if(m_TextString1)
	{
		m_TextString1->Shutdown();
		delete m_TextString1;
		m_TextString1 = 0;
	}*/
	
	// font 객체를 해제한다.
	if(m_Font)
	{
			m_Font->Shutdown();
			delete m_Font;
			m_Font = 0;
	}

	// font shader 객체를 해제한다.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}
	
	/*// timer 객체를 해제한다.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}
	
	// sprite 객체를 해제한다.
	if(m_Sprite)
	{
		m_Sprite->Shutdown();
		delete m_Sprite;
		m_Sprite = 0;
	}*/
	
	// bitmap 객체를 해제한다.
	/*if(m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}*/
	
	// 텍스처 셰이더 객체를 해제한다.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	
	/*// 라이트 오브젝트 해제
	if (m_Lights)
	{
		delete[] m_Lights;
		m_Lights = 0;
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


bool ApplicationClass::Frame()
{
	float frameTime;
	bool result;
	
	// frames per second each frame를 갱신한다.
	result = UpdateFps();
	if(!result)
	{
		return false;
	}
	
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
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(/*float rotation*/)
{
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix, srMatrix;
	// XMFLOAT4 diffuseColor[4], lightPosition[4];
	// int i;
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;
	
	// 씬 시작을 위해 버퍼 비우기
	// m_Direct3D->BeginScene(0.6f, 0.8f, 0.5f, 1.0f);
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 카메라 위치에 기반해서 뷰 행렬 생성하기
	m_Camera->Render();
	
	// 카메라와 d3d 객체에서 world, view, projection 행렬 가져오기
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	// m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	
	/*// 라이트 속성들을 가져온다
	for(i = 0; i < m_numLights; i++)
	{
		// 네 라이트 색으로 디퓨즈 색 배열을 만든다.
		diffuseColor[i] = m_Lights[i].GetDiffuseColor();
		
		// 네 라이트 위치로 라이트 위치 배열을 만든다.
		lightPosition[i] = m_Lights[i].GetPosition();
	}*/
	
	/*rotationMatrix = XMMatrixRotationY(rotation); // 회전 행렬을 만든다.
	translationMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f); // 이동 행렬을 만든다.
	
	// 이들을 곱해 최종 월드 변환 행렬을 만든다.
	worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);
	*/

	// 모델의 버택스 & 인데스 버퍼를 파이프라인에 넣어 그리기 준비하기
	/*m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 셰이더를 사용해서 모델 렌덜하기
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
								   diffuseColor, lightPosition);
	if(!result)
	{
		return false;
	}*/
	
	/*scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	rotationMatrix = XMMatrixRotationY(rotation); // 회전 행렬을 만든다.
	translationMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f); // 이동 행렬을 만든다.
	
	// 이들을 곱해 최종 월드 변환 행렬을 만든다.
	// worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);
	srMatrix = XMMatrixMultiply(scaleMatrix, rotationMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translationMatrix);
	
	// 모델의 버택스 & 인데스 버퍼를 파이프라인에 넣어 그리기 준비하기
	m_Model->Render(m_Direct3D->GetDeviceContext());
	
	// 텍스처 셰이더를 사용해서 모델 렌덜하기
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}*/
	
	// 2D 렌더링을 위해 Z 버퍼를 끄고 알파 블렌딩을 켠다.
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->EnableAlphaBlending();

	/*// 스프라이트의 정점·인덱스 버퍼를 그래픽 파이프라인에 올려 그리기를 준비한다.
	result = m_Sprite->Render(m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// 텍스처 셰이더로 스프라이트를 렌더링한다.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Sprite->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Sprite->GetTexture());
	if(!result)
	{
		return false;
	}*/

	/*// first text string using the font shader를 렌더링한다.
	m_TextString1->Render(m_Direct3D->GetDeviceContext());

	result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TextString1->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
								  m_Font->GetTexture(), m_TextString1->GetPixelColor());
	if(!result)
	{
		return false;
	}
	
	// second text string using the font shader를 렌더링한다.
	m_TextString2->Render(m_Direct3D->GetDeviceContext());

	result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_TextString2->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
								  m_Font->GetTexture(), m_TextString2->GetPixelColor());
	if(!result)
	{
		return false;
	}*/
	
	// fps text string using the font shader를 렌더링한다.
	m_FpsString->Render(m_Direct3D->GetDeviceContext());

	result = m_FontShader->Render(m_Direct3D->GetDeviceContext(), m_FpsString->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
								  m_Font->GetTexture(), m_FpsString->GetPixelColor());
	
	if(!result)
	{
		return false;
	}
	
	// 2D 렌더링이 끝났으니 Z 버퍼를 켜고 알파 블렌딩을 끈다.
	m_Direct3D->TurnZBufferOn();
	m_Direct3D->DisableAlphaBlending();
	
	// 씬에 랜더된 씬을 투영하기
	m_Direct3D->EndScene();

	return true;
}

bool ApplicationClass::UpdateFps()
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
}
