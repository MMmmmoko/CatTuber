//cbuffer CBuffer : register(b0, space3)
//{
//    float4x4 projectMatrix;
//    float4x4 clipMatrix;
//    float4 baseColor;
//    float4 multiplyColor;
//    float4 screenColor;
//    float4 channelFlag; 
//}

//这里的绘制参数是每个图层持有的对象参数，不是全局参数，再加上SDL对Uniform的容量限制（超过32768字节后会重置缓存导致数据访问出问题）
//因此需要换一个缓存形式

struct _ConstantBuffer
{
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 multiplyColor;
    float4 screenColor;
    float4 channelFlag;
};
StructuredBuffer<_ConstantBuffer> cBuffer : register(t1, space2);


Texture2D mainTexture : register(t0,space2);
SamplerState mainSampler : register(s0, space2);
//Texture2D maskTexture : register(t1, space2);


// Vertex shader output
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};



float4 PixelSetupMask(VS_OUT In) : SV_Target{
    float isInside =
    step(cBuffer[0].baseColor.x, In.clipPosition.x / In.clipPosition.w)
    * step(cBuffer[0].baseColor.y, In.clipPosition.y / In.clipPosition.w)
    * step(In.clipPosition.x / In.clipPosition.w, cBuffer[0].baseColor.z)
    * step(In.clipPosition.y / In.clipPosition.w, cBuffer[0].baseColor.w);
    return cBuffer[0].channelFlag * mainTexture.Sample(mainSampler, In.uv).a * isInside;
}


//float4 PixelSetupMask(VS_OUT In) : SV_Target{
//    float isInside =
//    step(baseColor.x, In.clipPosition.x / In.clipPosition.w)
//    * step(baseColor.y, In.clipPosition.y / In.clipPosition.w)
//    * step(In.clipPosition.x / In.clipPosition.w, baseColor.z)
//    * step(In.clipPosition.y / In.clipPosition.w, baseColor.w);
//    return channelFlag * mainTexture.Sample(mainSampler, In.uv).a * isInside;
//}