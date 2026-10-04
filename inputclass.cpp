////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	/*int i;

	// 모든 키를 눌리지 않은(떼어진) 상태로 초기화한다.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}*/
	
	HRESULT result;

	// 마우스 커서 위치 지정에 사용할 화면 크기를 저장한다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 화면상의 마우스 위치를 초기화한다.
	m_mouseX = 0;
	m_mouseY = 0;

	// 메인 다이렉트 인풋 인터페이스를 초기화한다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// 키보드용 다이렉트 인풋 인터페이스를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// 데이터 포맷을 설정한다. 키보드이므로 미리 정의된 데이터 포맷을 사용할 수 있다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// 키보드의 협력 수준을 다른 프로그램과 공유하지 않도록 설정한다.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// 이제 키보드를 획득(Acquire)한다.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// 마우스용 다이렉트 인풋 인터페이스를 초기화한다.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// 미리 정의된 마우스 데이터 포맷으로 마우스의 데이터 포맷을 설정한다.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// 마우스의 협력 수준을 다른 프로그램과 공유하도록 설정한다.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// 마우스를 획득(Acquire)한다.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}
	
	return true;
}

void InputClass::Shutdown()
{
	// 마우스 해제
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// 키보드 해제
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Direct 입력의 주 인터페이스 해제
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	// 현재 키보드 상태를 읽는다.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// 현재 마우스 상태를 읽는다.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// 마우스와 키보드의 변화를 처리한다.
	ProcessInput();

	return true;
}

bool InputClass::IsEscapePressed()
{
	// 키보드 상태에 비트 AND 연산을 하여 현재 ESC 키가 눌려 있는지 확인한다.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

bool InputClass::IsMousePressed()
{
	// 왼쪽 마우스 버튼 상태를 확인한다.
	if(m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// 키보드 장치를 읽는다.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// 키보드가 포커스를 잃었거나 획득되지 않았다면 제어권을 되찾으려 시도한다.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	// 마우스 장치를 읽는다.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
	
	if(FAILED(result))
	{
		// 마우스가 포커스를 잃었거나 획득되지 않았다면 제어권을 되찾으려 시도한다.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// 이번 프레임 동안의 마우스 위치 변화에 따라 마우스 커서 위치를 갱신한다.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 마우스 위치가 화면 너비나 높이를 넘지 않도록 한다.
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }

	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}


/*void InputClass::KeyDown(unsigned int input)
{
	// 키가 눌리면 그 상태를 키 배열에 저장한다.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// 키가 떼어지면 키 배열에서 그 상태를 지운다.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// 해당 키가 어떤 상태인지(눌림/안 눌림) 반환한다.
	return m_keys[key];
}*/
