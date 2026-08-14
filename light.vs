////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
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

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 올바른 행렬 계산을 위해 위치 백터의 w를 1로 만든다.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 투영 행렬에 대해 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더에 넘길 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
    // 법선 백터를 월드 행렬에 대해서만 계산한다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    
    // 범선 백터를 정규화한다.
    output.normal = normalize(output.normal);
    
    return output;
}