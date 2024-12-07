#include "Particle.hlsli"

struct Material{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight{
    float4 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0); //SRVのRegister
SamplerState gSampler : register(s0); //SamplerのRegister

struct PixelShaderOutput{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    //TextureのSampling
    float4 transformedUV = mul(float4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
 
    ////ランバート反射モデルの計算
    //if (gMaterial.enableLighting != 0){//Lightngを使用する場合
    //    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
    //    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    //    output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    //}
    //else{//Lightngを使用しない場合
    //    output.color = gMaterial.color * textureColor;
    //}
    
    //Textureのα値が0のときにPixelを棄却
    output.color = gMaterial.color * textureColor;
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}