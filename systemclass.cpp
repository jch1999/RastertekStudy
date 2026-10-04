////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Application = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
	
}


SystemClass::~SystemClass()
{
	
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// 변수를 함수에 넘기기 전에 화면의 너비와 높이를 0으로 초기화한다.
	screenWidth = 0;
	screenHeight = 0;

	// 윈도우 API를 초기화한다.
	InitializeWindows(screenWidth, screenHeight);

	// 입력 객체를 생성하고 초기화한다. 이 객체는 사용자의 키보드 입력을 읽어 처리하는 데 사용된다.
	m_Input = new InputClass;

	// m_Input->Initialize();
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if(!result)
	{
		return false;
	}
	
	// 애플리케이션 클래스 객체를 생성하고 초기화한다. 이 객체는 이 애플리케이션의 모든 그래픽 렌더링을 담당한다.
	m_Application = new ApplicationClass;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// 애플리케이션 클래스 객체를 해제한다.
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}

	// 입력 객체를 해제한다.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// 창을 종료한다.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// 메시지 구조체를 초기화한다.
	ZeroMemory(&msg, sizeof(MSG));

	// 창이나 사용자로부터 종료 메시지가 올 때까지 반복한다.
	done = false;
	while (!done)
	{
		// 윈도우 메시지를 처리한다.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우가 애플리케이션 종료를 알리면 빠져나간다.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 그렇지 않으면 프레임 처리를 한다.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}


bool SystemClass::Frame()
{
	bool result;


	/*// 사용자가 ESC를 눌러 애플리케이션을 종료하려는지 확인한다.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}*/
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// 애플리케이션 클래스 객체의 프레임 처리를 수행한다.
	result = m_Application->Frame(m_Input);
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*switch (umsg)
	{
		// 키보드에서 키가 눌렸는지 확인한다.
		case WM_KEYDOWN:
		{
			// 키가 눌리면 입력 객체로 보내 그 상태를 기록하게 한다.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// 키보드에서 키가 떼어졌는지 확인한다.
		case WM_KEYUP:
		{
			// 키가 떼어지면 입력 객체로 보내 그 키의 상태를 해제하게 한다.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// 그 외 메시지는 애플리케이션에서 쓰지 않으므로 기본 메시지 핸들러로 보낸다.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}*/
	
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 창이 파괴되는지 확인한다.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 창이 닫히는지 확인한다.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// 그 외 모든 메시지는 시스템 클래스의 메시지 핸들러로 넘긴다.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// 이 객체에 대한 외부 포인터를 얻는다.
	ApplicationHandle = this;

	// 이 애플리케이션의 인스턴스를 얻는다.
	m_hinstance = GetModuleHandle(NULL);

	// 애플리케이션에 이름을 부여한다.
	m_applicationName = L"Engine";

	// 윈도우 클래스를 기본 설정으로 구성한다.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스를 등록한다.
	RegisterClassEx(&wc);

	// 클라이언트 데스크톱 화면의 해상도를 알아낸다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 전체화면인지 창 모드인지에 따라 화면 설정을 구성한다.
	if (FULL_SCREEN)
	{
		// 전체화면이면 화면을 사용자 데스크톱의 최대 크기와 32비트로 설정한다.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 디스플레이 설정을 전체화면으로 변경한다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 창 위치를 좌측 상단 모서리로 설정한다.
		posX = posY = 0;
	}
	else
	{
		// 창 모드면 해상도를 800x600으로 설정한다.
		screenWidth = 800;
		screenHeight = 600;

		// 창을 화면 중앙에 배치한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 화면 설정으로 창을 생성하고 그 핸들을 얻는다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 창을 화면에 띄우고 메인 포커스로 설정한다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서를 숨긴다.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// 마우스 커서를 표시한다.
	ShowCursor(true);

	// 전체화면 모드를 벗어날 때 디스플레이 설정을 복구한다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거한다.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 애플리케이션 인스턴스를 제거한다.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 이 클래스에 대한 포인터를 해제한다.
	ApplicationHandle = NULL;

	return;
}