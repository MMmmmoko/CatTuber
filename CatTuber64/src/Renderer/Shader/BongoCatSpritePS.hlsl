



Texture2D mainTexture : register(t0, space2);
SamplerState mainSampler : register(s0, space2);


struct VSOut
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

float4 BongoCatSpritePS(VSOut In) : SV_TARGET
{
    float4 psout = mainTexture.Sample(mainSampler, In.tex);
    psout.xyz *= psout.w; //预乘
	
    //return float4(0.5f,0.5f,0.5f,1.f);
    
    return psout;
}