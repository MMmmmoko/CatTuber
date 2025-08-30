cbuffer CBuffer : register(b0, space1)
{
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 multiplyColor;
    float4 screenColor;
    float4 channelFlag;
}

// Vertex shader input
struct VS_IN
{
    float2 pos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

// Vertex shader output
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};


// Vertex shader
// masked
VS_OUT VertMasked(VS_IN In) {
    VS_OUT Out = (VS_OUT)0;
    Out.Position = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);
    Out.clipPosition = mul(float4(In.pos, 0.0f, 1.0f), clipMatrix);
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0f - In.uv.y;
    return Out;
}