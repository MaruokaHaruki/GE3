#include "Particle.hlsli"

//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

//ConstantBufferを使って色を指定する
struct Material
{
    float4 color : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
};

//平行光源
struct DirectionalLight
{
    float4 color; //!<ライトの色
    float3 direction; //!<ライトの向き
    float intensity; //!<輝度
};

//TransformationMatrixを拡張する
struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    //float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
   // output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
    //output.color = gMaterial.color;
   // output.color = gMaterial.color * textureColor;
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color * textureColor * input.color;
    ////textureのα値が0.5以下のときにPixelを棄却
    //if (textureColor.a <= 0.5)
    //{
    //    discard;
    //}
    ////textureのα値が0のときにPixelを棄却
    //if (textureColor.a == 0.0)
    //{
    //    discard;
    //}
    
    
    
    //Textureのα値が0のときにPixelを棄却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    //////=========Lightingの計算を行う=========////

  //  if (gMaterial.enableLighting != 0)
  //  {
		////Lightする場合
  //      //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
  //      float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
  //      float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
  //     // output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
  //      output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
  //      output.color.a = gMaterial.color.a * textureColor.a;
        
  //  }
  //  else
  //  {
		////Lightしない場合
  //      output.color = gMaterial.color * textureColor;
  //  }
    
    
    return output;
}

//float4 main() : SV_TARGET
//{
//    return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

//ConstantBufferを使って色を指定する
//struct Material
//{
//    float4 color;
//};

//ConstantBuffer<Material> gMaterial : register(b0);
//struct PixelShaderOutput
//{
//    float4 color : SV_TARGET0;
//};

//PixelShaderOutput main()
//{
//    PixelShaderOutput output;
//   // output.color = float32_t4(1.0, 1.0, 1.0, 1.0);
//    output.color = gMaterial.color;
//    return output;
//}