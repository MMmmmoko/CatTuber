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
StructuredBuffer<_ConstantBuffer> cBuffer : register(t0, space0);


// Vertex shader input
struct VS_IN {
    float2 pos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

// Vertex shader output
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};


// Mask shader
VS_OUT VertSetupMask(VS_IN In) {
    
    
    VS_OUT Out = (VS_OUT)0;
    Out.Position = mul(float4(In.pos, 0.0f, 1.0f), cBuffer[0].projectMatrix);
    Out.clipPosition = mul(float4(In.pos, 0.0f, 1.0f), cBuffer[0].projectMatrix);
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0f - +In.uv.y;
    return Out;
}
//VS_OUT VertSetupMask(VS_IN In) {
//    VS_OUT Out = (VS_OUT)0;
//    Out.Position = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);
//    Out.clipPosition = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);
//    Out.uv.x = In.uv.x;
//    Out.uv.y = 1.0f - +In.uv.y;
//    return Out;
//}