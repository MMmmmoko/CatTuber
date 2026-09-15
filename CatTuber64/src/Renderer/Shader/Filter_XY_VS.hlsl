
struct VS_IN
{
    float2 pos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};


struct VSOut
{
    float2 uv : TexCoord;
    float4 pos : SV_Position;
};



VSOut Filter_XY_VS(VS_IN In)
{
    VSOut vso;
    
    vso.pos = float4(In.pos, 0.0f, 1.0f);
    vso.uv = In.uv;
    return vso;
}