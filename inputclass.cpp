////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	int i;


	// 모든 키를 눌리지 않은(떼어진) 상태로 초기화한다.
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
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
}