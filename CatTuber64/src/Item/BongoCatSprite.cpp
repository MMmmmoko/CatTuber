#include<SDL3/SDL.h>
#include"AppContext.h"
#include"BongoCatSprite.h"

BongoCatSprite::BongoCatSprite()
{


    vertexbuffer.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2);
    vertexbuffer.PushLayout(SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2);

    //设置顶点和bindable
    {


        //dvbuf.EmplaceBack(DirectX::XMFLOAT2(-1, 1), DirectX::XMFLOAT2(0, 0));
        //dvbuf.EmplaceBack(DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT2(1, 0));
        //dvbuf.EmplaceBack(DirectX::XMFLOAT2(-1, -1), DirectX::XMFLOAT2(0, 1));
        //dvbuf.EmplaceBack(DirectX::XMFLOAT2(1, -1), DirectX::XMFLOAT2(1, 1));
        //按顺时针布点




        indexbuffer= GlobalGraphicResourceManager::GetIns().GetGlobalBindable<rendering::IndexBuffer>("$bcm_sprite.index");
        if (!indexbuffer)
        {
            const unsigned short indices[] =
            {
                0,1,2,3,
            };
            indexbuffer = std::make_shared<rendering::IndexBuffer>();
			indexbuffer->UpdateIndices(indices, SDL_GPUIndexElementSize::SDL_GPU_INDEXELEMENTSIZE_16BIT, 4);
            GlobalGraphicResourceManager::GetIns().StoreBindable("$bcm_sprite.index", indexbuffer);
        }




        //pTopology = (Topology::Resolve(c2d, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP));

    }







    //CatColor opacity = { 0.f,0.f,0.f,1.f };
    //ps_cbuf_opacity = std::make_shared<c2d::bind::PixelConstantBuffer<CatColor>>(c2d, opacity);
    //AddBind(ps_cbuf_opacity);



    //auto tcb = std::make_shared<c2d::bind::TransformCbuf>(c2d);
    //tcb->InitializeParentReference(*this);

    //AddBind(tcb);


    vs_sprite = GlobalGraphicResourceManager::GetIns().GetShaderByName("SpriteVS");
	ps_sprite = GlobalGraphicResourceManager::GetIns().GetShaderByName("SpritePS");



    //有疑问的代码
    //AddBind(std::make_shared<c2d::bind::Viewport>(c2d));
}

BongoCatSprite::~BongoCatSprite()
{
	//unique
	if (pTexture)SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), pTexture);
	pTexture = nullptr;

}



void BongoCatSprite::SetUp(SDL_GPUTexture* texture, int w, int h)
{
	if (pTexture)SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), pTexture);
	pTexture = texture;

	spriteRect.w = static_cast<float>(w);
	spriteRect.h = static_cast<float>(h);
}


void BongoCatSprite::SetPosition(float x, float y)
{
	spriteRect.x = x;
	spriteRect.y = y;
}


void BongoCatSprite::Draw()
{
	if (!pTexture)return;

	if (needUpdateVertex)
	{
		//不太对，应该有个地方直接进行宏观设置而不是给每个BongoCatSprite都设置一遍


		//根据内容设置顶点
		struct SpriteVertex
		{
			float x;
			float y;
			float u;
			float v;
		};
		//转换坐标 Y向上，中心为0，0
		float left = spriteRect.x-0.5f * canvasW;
		float top = 0.5f * canvasH - spriteRect.y;
		float right = left + spriteRect.w;
		float bottom = top - spriteRect.h;

		left /= (0.5f * canvasW);
		right /= (0.5f * canvasW);
		top /= (0.5f * canvasH);
		bottom /= (0.5f * canvasH);


		SpriteVertex vertices[] =
		{
			{left,top,0.f,0.f},//左上
			{right,top,1.f,0.f},
			{left,bottom,0.f,1.f},
			{right,bottom,1.f,1.f},
		};
		vertexbuffer.UpdateVertex(vertices,sizeof(vertices));

		needUpdateVertex = false;
	}


	auto pContext = AppContext::GetSDL3RenderContext();
	pContext->SetTopology(SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP);
	pContext->SetFragmentSampler(SDL3Sampler::Sampler_Normal);

	//pContext->SetVertexUniformData(0, &subdata, sizeof(subdata));

	pContext->SetBlendState(SDL3Blend::Blend_Normal);
	pContext->SetCullMode(SDL3Cull::Cull_Ccw);



	//绑定资源以绘制手的纹理
	pContext->SetVertexShader(vs_sprite);
	vertexbuffer.Bind();
	pContext->SetFragmentShader(ps_sprite);
	SDL_GPUTextureSamplerBinding texBinding = {};
	texBinding.sampler = pContext->GetFragmentSampler();
	texBinding.texture = pTexture;
	pContext->SetFragmentTextureResources(0, 1, &texBinding);
	indexbuffer->Bind();


	//重新设置视口



	pContext->DrawIndexed(indexbuffer->GetCount());



}
