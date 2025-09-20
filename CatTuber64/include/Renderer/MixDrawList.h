#ifndef _MixDrawList_h
#define _MixDrawList_h


//混合绘制列表，为Live2d模型以及其他模型提供混合Z轴一起绘制的功能

#include"CatRendererContext.h"


struct MixRenderData
{
	uint32_t sizeOfThisStruct; int layerZ; SDL_GPUGraphicsPipeline* pipeLine;
	uint32_t vsStartSlot; uint32_t vsBufferNum; SDL_GPUBufferBinding* vsBuffers;
	SDL_GPUIndexElementSize indexElementSize; SDL_GPUBufferBinding* indexBuffer; uint32_t indexCount; uint32_t indexStart;
	uint32_t texStartSlot; uint32_t texNum; SDL_GPUTextureSamplerBinding* textures;
	uint32_t vsUniformSlot; const void* vsUniformData; uint32_t vsUniformDataLength;
	uint32_t psUniformSlot; const void* psUniformData; uint32_t psUniformDataLength;
	SDL_GPUViewport viewPort;
	//由于一些绘制需要额外绘制蒙版，这里也提供函数让一些动作在绘制前执行
	void(*beforeDrawCallback)(void* userData, uint64_t userData2); void* callbackUserData; uint64_t callbackUserData2;
};

//MixRenderData里一些指针的数据需要进行存储(转为数组)
struct MixRenderData_Internal
{
	int layerZ; SDL_GPUGraphicsPipeline* pipeLine;
	uint32_t vsStartSlot; uint32_t vsBufferNum; SDL_GPUBufferBinding vsBuffers[4];
	SDL_GPUIndexElementSize indexElementSize; SDL_GPUBufferBinding indexBuffer; uint32_t indexCount; uint32_t indexStart;
	uint32_t texStartSlot; uint32_t texNum; SDL_GPUTextureSamplerBinding textures[8];
	uint32_t vsUniformSlot; const void* vsUniformData; uint32_t vsUniformDataLength;//vsUniformData 由其他地方保持
	uint32_t psUniformSlot; const void* psUniformData; uint32_t psUniformDataLength;
	SDL_GPUViewport viewPort;

	//由于一些绘制需要额外绘制蒙版，这里也提供函数让一些动作在绘制前执行
	void(*beforeDrawCallback)(void* userData, uint64_t userData2); void* callbackUserData; uint64_t callbackUserData2;
};


class MixDrawList
{
public:
	MixDrawList();


	//RenderPass是渲染目标的信息，不由模型提供 pineline后面参数是需要绑定的
	void InsertDrawCommand(void* mixRenderData);


	static void InsertDrawCommandCallback(void* pThis, void* mixRenderData);

	
	void DoDraw(SDL_GPUCommandBuffer* mainCmd, SDL_GPURenderPass* mainPass);

	//void Start();//每帧开始的时候调用
	//void End();




private:
	struct RenderData
	{
		SDL_GPUGraphicsPipeline* pipeLine;
		CatVertexBufferBinding vertexBuffer;
		CatIndexBufferBinding indexBuffer;
		CatTextureBinding psTextures;

		CatUniformDataBinding vsUniformData;
		CatUniformDataBinding psUniformData;

		uint32_t indexCount;
		uint32_t indexStart;
	};
	//渲染顺序为0~1000?
	std::vector<int> activeLayer;
	std::vector<std::vector<MixRenderData_Internal>> layers;
	
	
	//减少重复绑定做一些必要的检测//Live2d每个图层有一次drawcall但实际上很多资源并没有切换

	void _ResetLastData();
	void _CheckAndBindAndUpdate_PipeLine(SDL_GPUGraphicsPipeline* pipeLine,SDL_GPURenderPass* mainPass);
	void _CheckAndBindAndUpdate_VertexBuffer(uint32_t vsStartSlot, uint32_t vsBufferNum, SDL_GPUBufferBinding* vsBuffers, SDL_GPURenderPass* mainPass);
	void _CheckAndBindAndUpdate_IndexBuffer(SDL_GPUIndexElementSize indexElementSize, SDL_GPUBufferBinding* indexBuffer, SDL_GPURenderPass* mainPass);
	void _CheckAndBindAndUpdate_Texture(uint32_t texStartSlot, uint32_t texNum, SDL_GPUTextureSamplerBinding* textures, SDL_GPURenderPass* mainPass);
	void _CheckAndBindAndUpdate_VsUniform(uint32_t vsUniformSlot, const void* vsUniformData, uint32_t vsUniformDataLength,SDL_GPUCommandBuffer* mainCmd);
	void _CheckAndBindAndUpdate_PsUniform(uint32_t psUniformSlot, const void* psUniformData, uint32_t psUniformDataLength, SDL_GPUCommandBuffer* mainCmd);
	void _CheckAndBindAndUpdate_ViewPort(SDL_GPUViewport* viewPort, SDL_GPURenderPass* mainPass);

	SDL_GPUGraphicsPipeline* lastPipeLine = NULL;

	uint32_t lastVsStartSlot; 
	uint32_t lastVsBufferNum; 
	SDL_GPUBufferBinding lastVsBuffers[4];

	//SDL_GPUIndexElementSize lastIndexElementSize; 
	SDL_GPUBufferBinding lastIndexBuffer;

	uint32_t lastTexStartSlot; 
	uint32_t lastTexNum;
	SDL_GPUTextureSamplerBinding lastTextures[8];

	uint32_t lastVsUniformSlot; 
	const void* lastVsUniformData; 
	uint32_t lastVsUniformDataLength;

	uint32_t lastPsUniformSlot; 
	const void* lastPsUniformData; 
	uint32_t lastPsUniformDataLength;

	SDL_GPUViewport lastGpuViewport;



	//CatVertexBufferBinding lastVertexBuffer;
	//CatIndexBufferBinding lastIndexBuffer;
	//CatTextureBinding lastPsTextures;
	//CatUniformDataBinding lastVsUniformData;
	//CatUniformDataBinding lastPsUniformData;
};






















#endif