#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
// #include "modelclass.h"
// #include "colorshaderclass.h"
#include "textureshaderclass.h"
// #include "lightshaderclass.h"
// #include "lightclass.h"
#include "bitmapclass.h"

//////////////
// INCLUDES //
//////////////
#include <windows.h>

#include "bitmapclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render(/*float*/);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	// ModelClass* m_Model;
	// ColorShaderClass* m_ColorShader;
	// TextureShaderClass* m_TextureShader;
	// LightShaderClass* m_LightShader;
	// LightClass* m_Lights;
	// int m_numLights;
	BitmapClass* m_Bitmap;
	TextureShaderClass* m_TextureShader;
	
	XMMATRIX scaleMatrix;
	XMMATRIX rotationMatrix;
	XMMATRIX translationMatrix;
};