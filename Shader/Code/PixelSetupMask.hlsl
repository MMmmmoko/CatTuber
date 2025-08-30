cbuffer CBuffer : register(b0, space3)
{
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 multiplyColor;
    float4 screenColor;
    float4 channelFlag;
}


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
    step(baseColor.x, In.clipPosition.x / In.clipPosition.w)
    * step(baseColor.y, In.clipPosition.y / In.clipPosition.w)
    * step(In.clipPosition.x / In.clipPosition.w, baseColor.z)
    * step(In.clipPosition.y / In.clipPosition.w, baseColor.w);
    return channelFlag * mainTexture.Sample(mainSampler, In.uv).a * isInside;
}