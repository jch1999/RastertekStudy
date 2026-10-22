////////////////////////////////////////////////////////////////////////////////
// Filename: alphamap.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

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
    float2 tex  : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType AlphaMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 올바른 행렬 계산을 위해 위치 백터를 4성분으로 만든다,
    input.position.w = 1.0f;
    
    // 월드,뷰. 투영 행렬에 대해 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이덜르 위해 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
    return output;
}