
#include"EmptyWindowSprite.h"
#include"Renderer/GlobalGraphicResourceManager.h"
#include"AppContext.h"


void EmptyWindowSprite::SetUp()
{
	filterIndexbuffer = rendering::IndexBuffer::GetGlobalIndexBuffer(rendering::IndexBuffer::GlobalIndexBuffer_FullScreenFilter);
	filterVertexbuffer = rendering::VertexBuffer::GetGlobalVertexBuffer(rendering::VertexBuffer::GlobalVertexBuffer_FullScreenFilter);


	vs_filterVS = GlobalGraphicResourceManager::GetIns().GetShaderByName("Filter_XY_VS");
	ps_emptyWindowPS = GlobalGraphicResourceManager::GetIns().GetShaderByName("EmptyWindow_PS");

	if (!(filterIndexbuffer && filterVertexbuffer && vs_filterVS && ps_emptyWindowPS))
	{
		filterIndexbuffer.reset();
		filterVertexbuffer.reset();
		vs_filterVS = nullptr;
		ps_emptyWindowPS = nullptr;
	}

}
void EmptyWindowSprite::CleanUp()
{
	filterIndexbuffer.reset();
	filterVertexbuffer.reset();
	vs_filterVS = nullptr;
	ps_emptyWindowPS = nullptr;
}



void EmptyWindowSprite::Draw(int renderW, int renderH)
{
	if (!Avaliable())return;


	auto pContext = AppContext::GetSDL3RenderContext();
	pContext->SetTopology(SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
	pContext->SetFragmentSampler(SDL3Sampler::Sampler_Normal);

	//pContext->SetVertexUniformData(0, &subdata, sizeof(subdata));

	pContext->SetBlendState(SDL3Blend::Blend_Normal);//Normal需要在管线中对源图像进行预乘
	pContext->SetCullMode(SDL3Cull::Cull_Ccw);



	//绑定资源以绘制手的纹理
	pContext->SetVertexShader(vs_filterVS);
	filterVertexbuffer->Bind();
	pContext->SetFragmentShader(ps_emptyWindowPS);
	SDL_GPUTextureSamplerBinding texBinding = {};
	filterIndexbuffer->Bind();









	//渲染参数
	float unitWidth = 60.f;//px
	float cornerRadius = 20.f;//px
	const float minUnitCount = 2.4f;//一条边最少看见2.4个各自
	//准备像素着色器参数
	struct PSUniformData
	{
		float windowW;
		float windowH;
		float offsetX;
		float offsetY;


		float unitWidth;
		float _Density;//密度factor，与坐标相乘得到网格id
		float cornerRadius;//圆角半径 px

		float padding=0.f;


		float color1[4];
		//float color2[4] = { 0.5f * 243.f / 255.f,0.5f * 247.f / 255.f,0.5f * 253.f / 253.f,0.5f };
		float color2[4];
	}psData;
	psData.windowW = static_cast<float>(renderW);
	psData.windowH = static_cast<float>(renderH);



	psData.offsetX=SDL_GetTicks()/100.f;
	psData.offsetY = psData.offsetX;





	float minLenth = static_cast<float>(SDL_min(renderW, renderH));
	float minCount = minLenth / unitWidth;
	if (minCount < minUnitCount)
	{
		unitWidth = minLenth / minUnitCount;
	}

	psData.unitWidth = unitWidth;
	psData._Density = 1 / unitWidth;
	psData.cornerRadius =SDL_min(cornerRadius, minLenth*0.4f);

	//psData.color1 = { 233.f,239.f,253.f,0.f };

	//251, 132, 187;
	auto fillColor = [](float* colorArr,uint8_t r, uint8_t g, uint8_t b, float a)
		{
			colorArr[0] = r / 255.f;
			colorArr[1] = g / 255.f;
			colorArr[2] = b / 255.f;
			colorArr[3] = a;
		};
	fillColor(psData.color1, 251, 132, 186,0.2f);
	fillColor(psData.color2, 0, 0, 0,0);


	//psData.offsetX =-1.f*SDL_GetTicks() / 200.f;
	psData.offsetX =-1.f*SDL_GetTicks() / 200.f;
	psData.offsetY = -psData.offsetX*0.8f;



	SDL_PushGPUFragmentUniformData(pContext->GetCommandBuffer(), 0,&psData,sizeof(psData));

	//重新设置视口
	//外部已经设置好了视口


	//设置混合模式



	pContext->DrawIndexed(filterIndexbuffer->GetCount());


}
