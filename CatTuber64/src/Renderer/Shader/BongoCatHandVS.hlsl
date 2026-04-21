cbuffer CBuffer : register(b0, space1)
{
    float bongoCatVp_X;
    float bongoCatVp_Y;
    float bongoCatVp_W;
    float bongoCatVp_H;
    float _1_canvasW;
    float _1_canvasH;
    float _1_rtW;
    float _1_rtH;
}


struct VS_IN
{
    float2 pos : TEXCOORD0;
    float4 col : TEXCOORD1;
};


struct VSOut
{
    float4 col : TexCoord;
    float4 pos : SV_Position;
};



VSOut BongoCatHandVS(VS_IN In)
{
    VSOut vso;
    //vso.pos = float4(In.pos.x * _1_width * 2.f - 1.f, 1.f - In.pos.y * _1_height * 2.f, 0.0f, 1.0f);
    
    
    float x = In.pos.x * _1_canvasW;
    x = bongoCatVp_X + bongoCatVp_W * x;
    x = 2.f * x * _1_rtW - 1.f;
    
    float y = In.pos.y * _1_canvasH;
    y = bongoCatVp_Y + bongoCatVp_H * y;
    y = 1.f - 2.f * y * _1_rtH;
    
    
    
    vso.pos = float4(x, y, 0.0f, 1.0f);
    
    
    vso.col = In.col;
    return vso;
}