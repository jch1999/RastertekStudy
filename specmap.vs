////////////////////////////////////////////////////////////////////////////////
// Filename: specmap.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SpecMapVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// 올바른 행렬 계산을 위해 위치 벡터를 4성분으로 만든다.
	input.position.w = 1.0f;

	// 월드, 뷰, 투영 행렬에 대해 정점의 위치를 계산한다.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 셰이더를 위해 텍스처 좌표를 저장한다.
	output.tex = input.tex;

	// 법선 벡터를 월드 행렬에 대해서만 계산한 뒤 최종 값을 정규화한다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// 접선(tangent) 벡터를 월드 행렬에 대해서만 계산한 뒤 최종 값을 정규화한다.
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	// 종법선(binormal) 벡터를 월드 행렬에 대해서만 계산한 뒤 최종 값을 정규화한다.
	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);
	
	// 월드 공간에서의 정점 위치를 계산한다.
	worldPosition = mul(input.position, worldMatrix);

	// 카메라 위치와 월드 공간 정점 위치를 이용해 시선 방향을 구한다.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// 시선 방향 벡터를 정규화한다.
	output.viewDirection = normalize(output.viewDirection);

	return output;
}