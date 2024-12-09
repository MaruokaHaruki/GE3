#include "Particle.hlsli"

struct TransformationMatrix{
    float4x4 WVP;
    float4x4 World;
};

StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);

struct VertexShaderInput{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID){
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrices[instanceID].WVP);
    output.texcoord = input.texcoord;
    //NOTE:法線の変換には拡縮回転情報のみが必要なため取り出す処理を行っている
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrices[instanceID].World));
    
    return output;
}