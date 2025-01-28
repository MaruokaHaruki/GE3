#include "Line.hlsli"
#include "Line.hlsli"

struct VertexShaderInput
{
    float3 position : POSITION0;
    float4 color : COLOR0;
};

cbuffer TransformationMatrix : register(b0)
{
    float4x4 WVP;
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(float4(input.position, 1.0f), WVP);
    output.color = input.color;
    return output;
}
