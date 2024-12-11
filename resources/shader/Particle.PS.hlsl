#include "Particle.hlsli"
//ConstantBufferを使って色を指定する
struct Material
{
    float4 color : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
};

//TransformationMatrixを拡張する
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color * textureColor * input.color;
    //Textureのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    } 
    return output;
}


