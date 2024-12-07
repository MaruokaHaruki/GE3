#include "Particle.hlsli"

//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

TransformationMatrix gTransformationMatrices[10];


//StructuredBuffer<TransformationMatrix> gTransformationMatrix : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    //float4 color : COLOR0;
};

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

//struct VertexShaderInput
//{
//    float4 position : SV_POSITION;
//    float2 texcoord : TEXCOORD0;
//    float3 normal : NORMAL0;
//};


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    //output.position = input.position;
    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    
    //////=========法線の座標系を変換してPixelShaderに送る=========////

    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceId].World));
    
    output.color = gParticle[instanceId].color;

    return output;
}
