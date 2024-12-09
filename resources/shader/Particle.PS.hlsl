#include "Particle.hlsli"

struct Material{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float4> gTexture : register(t0); //SRVのRegister

SamplerState gSampler : register(s0); //SamplerのRegister

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //TextureのSampling
    float4 transformedUV = mul(float4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    //Textureの色を取得
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    //Textureのα値が0のときにPixelを棄却
    output.color = gMaterial.color * textureColor;
    //Alphaが0の場合は描画しない
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}