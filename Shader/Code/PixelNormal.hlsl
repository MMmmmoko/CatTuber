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


Texture2D mainTexture : register(t0, space2);
SamplerState mainSampler : register(s0, space2);
//Texture2D maskTexture : register(t1, space2);

// Vertex shader output
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};


// Pixel Shader
// normal
float4 PixelNormal(VS_OUT In) : SV_Target{
    float4 texColor = mainTexture.Sample(mainSampler, In.uv);
    texColor.rgb = texColor.rgb * cBuffer[0].multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + cBuffer[0].screenColor.rgb) - (texColor.rgb * cBuffer[0].screenColor.rgb);
    float4 color = texColor * cBuffer[0].baseColor;
    color.xyz *= color.w;
    return color;
}

//float4 PixelNormal(VS_OUT In) : SV_Target{
//    float4 texColor = mainTexture.Sample(mainSampler, In.uv);
//    texColor.rgb = texColor.rgb * multiplyColor.rgb;
//    texColor.rgb = (texColor.rgb + screenColor.rgb) - (texColor.rgb * screenColor.rgb);
//    float4 color = texColor * baseColor;
//    color.xyz *= color.w;
//    return color;
//}
