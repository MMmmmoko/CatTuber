cbuffer CBuffer : register(b0, space3)
{
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 multiplyColor;
    float4 screenColor;
    float4 channelFlag;
}


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
// normal premult alpha
float4 PixelNormalPremult(VS_OUT In) : SV_Target{
    float4 texColor = mainTexture.Sample(mainSampler, In.uv);
    texColor.rgb = texColor.rgb * multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + screenColor.rgb * texColor.a) - (texColor.rgb * screenColor.rgb);
    float4 color = texColor * baseColor;
    return color;
}
