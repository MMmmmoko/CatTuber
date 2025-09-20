cbuffer CBuffer : register(b0, space3)
{
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 multiplyColor;
    float4 screenColor;
    float4 channelFlag;
}


// Vertex shader output
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};

Texture2D mainTexture : register(t0, space2);
SamplerState mainSampler : register(s0, space2);
Texture2D maskTexture : register(t1, space2);

//需要声明采样器，虽然不用
SamplerState maskSampler : register(s1, space2);


// Pixel Shader
// masked inverted premult alpha
float4 PixelMaskedInvertedPremult(VS_OUT In) : SV_Target{
    float4 texColor = mainTexture.Sample(mainSampler, In.uv);
    texColor.rgb = texColor.rgb * multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + screenColor.rgb * texColor.a) - (texColor.rgb * screenColor.rgb);
    float4 color = texColor * baseColor;
    float4 clipMask = (1.0f - maskTexture.Sample(mainSampler, In.clipPosition.xy / In.clipPosition.w)) * channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    color = color * (1.0f - maskVal);
    return color;
}