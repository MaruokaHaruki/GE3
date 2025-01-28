#include "Line.hlsli"
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.color = input.color;
    return output;
}

