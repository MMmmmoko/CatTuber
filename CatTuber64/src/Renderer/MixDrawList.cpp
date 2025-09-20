
#include <algorithm>

#include "Renderer/MixDrawList.h"




MixDrawList::MixDrawList()
{
	layers.resize(1001);
	_ResetLastData();
}

void MixDrawList::InsertDrawCommand(void* mixRenderData)
{
	//对mixRenderData进行解析
	uint32_t structSize = *(uint32_t*)mixRenderData;
	
	
	
	if (structSize == sizeof(MixRenderData))
	{
		MixRenderData* renderData = (MixRenderData*)mixRenderData;

		//对renderData的一些数据进行检查
		if (renderData->vsBufferNum > 4)
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_RENDER,"Can not support more than 4 vertex buffer in MixDraw Shader.");
			return;
		}
		if (renderData->texNum > 8)
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_RENDER,"Can not support more than 8 texture in MixDraw Shader.");
			return;
		}


		int layerZ = renderData->layerZ;
		layerZ = SDL_clamp(layerZ, 0, 1000);
		if (layers[layerZ].empty())
		{
			activeLayer.push_back(layerZ);
		}


		auto& __renderData = layers[layerZ].emplace_back();

		__renderData.layerZ = renderData->layerZ;
		__renderData.pipeLine = renderData->pipeLine;
		__renderData.vsStartSlot = renderData->vsStartSlot;
		__renderData.vsBufferNum = renderData->vsBufferNum;
		//暂时只支持最大四个顶点缓存吧，应该是够用的
		for(uint32_t i=0;i<__renderData.vsBufferNum;i++)
		{
			__renderData.vsBuffers[i] = renderData->vsBuffers[i];
		}

		__renderData.indexElementSize = renderData->indexElementSize;
		__renderData.indexBuffer = *renderData->indexBuffer;
		__renderData.indexCount = renderData->indexCount;
		__renderData.indexStart = renderData->indexStart;

		__renderData.texStartSlot = renderData->texStartSlot;
		__renderData.texNum = renderData->texNum;
		//暂时只支持最大8个着色器纹理吧，应该是够用的
		for (uint32_t i = 0; i < __renderData.texNum; i++)
		{
			__renderData.textures[i] = renderData->textures[i];
		}

		__renderData.vsStartSlot = renderData->vsStartSlot;
		__renderData.vsUniformData = renderData->vsUniformData;
		__renderData.vsUniformDataLength = renderData->vsUniformDataLength;

		__renderData.psUniformSlot = renderData->psUniformSlot;
		__renderData.psUniformData = renderData->psUniformData;
		__renderData.psUniformDataLength = renderData->psUniformDataLength;

		__renderData.beforeDrawCallback = renderData->beforeDrawCallback;
		__renderData.callbackUserData = renderData->callbackUserData;
		__renderData.callbackUserData2 = renderData->callbackUserData2;
	}



	//layerZ = SDL_clamp(layerZ,0,1000);
	//if (layers[layerZ].empty())
	//{
	//	activeLayer.push_back(layerZ);
	//}
	//layers[layerZ].push_back({ pipeLine,*vertexBuffer,*indexBuffer,*psTextures ,*vsUniformData,*psUniformData,indexCount,indexStart });
}

void MixDrawList::InsertDrawCommandCallback(void* pThis, void* mixRenderData)
{
	MixDrawList* _pthis = (MixDrawList*)pThis;
	_pthis->InsertDrawCommand(mixRenderData);



}


void MixDrawList::DoDraw(SDL_GPUCommandBuffer* mainCmd, SDL_GPURenderPass* mainPass)
{
	//这部分原本计划写入Start函数中以防止未来可能的在不同位置DoDraw的情况//如一份绘制在主窗口，一份绘制在ui预览
	//但为了减少接口复杂度，这里将Start内容整合进DoDraw里，  start内容比较轻量，重复执行也不会有什么性能损失
	//memset(&lastVertexBuffer,0,sizeof(lastVertexBuffer));



	//Viewport再外部设置
	//activeLayer顺序和插入层的先后有关，重新排序
	std::sort(activeLayer.begin(), activeLayer.end());
	/////////

	//SDL_GPUViewport vp = {};
	//vp.w = 400;
	//vp.h = 300;
	//vp.max_depth = 1;
	//vp.min_depth = 0;
	//SDL_SetGPUViewport(mainPass,&vp);

	for (auto layerZ : activeLayer)
	{
		for (auto& curCommand : layers[layerZ])
		{
			//绘制前回调
			if (curCommand.beforeDrawCallback)
			{
				curCommand.beforeDrawCallback(curCommand.callbackUserData,curCommand.callbackUserData2);
			}


			//对资源进行绑定
			_CheckAndBindAndUpdate_PipeLine(curCommand.pipeLine,mainPass);
			_CheckAndBindAndUpdate_VertexBuffer(curCommand.vsStartSlot, curCommand.vsBufferNum, curCommand.vsBuffers,mainPass);
			_CheckAndBindAndUpdate_IndexBuffer(curCommand.indexElementSize,&curCommand.indexBuffer,mainPass);
			_CheckAndBindAndUpdate_Texture(curCommand.texStartSlot,curCommand.texNum,curCommand.textures,mainPass);
			_CheckAndBindAndUpdate_VsUniform(curCommand.vsUniformSlot,curCommand.vsUniformData,curCommand.vsUniformDataLength, mainCmd);
			_CheckAndBindAndUpdate_PsUniform(curCommand.psUniformSlot,curCommand.psUniformData,curCommand.psUniformDataLength, mainCmd);
			
			//绑定完进行绘制
			SDL_DrawGPUIndexedPrimitives(mainPass, curCommand.indexCount, 1, curCommand.indexStart,0,0);
		}
	}



	//TODO移动到更合理的地方，重启用Start函数?

	_ResetLastData();


}


void MixDrawList::_ResetLastData()
{
	for(auto index: activeLayer)
	{
		layers[index].clear();
	}
	activeLayer.clear();


	lastPipeLine = NULL;

	lastVsStartSlot = 0;
	lastVsBufferNum = 0;
	SDL_memset(lastVsBuffers,0,sizeof(lastVsBuffers));

	//lastIndexElementSize = 0;
	lastIndexBuffer = {};

	lastTexStartSlot = 0;
	lastTexNum = 0;
	SDL_memset(lastTextures, 0, sizeof(lastTextures));

	lastVsUniformSlot = 0;
	lastVsUniformData = NULL;
	lastVsUniformDataLength = 0;

	lastPsUniformSlot = 0;
	lastPsUniformData = NULL;
	lastPsUniformDataLength = 0;

	lastGpuViewport = {};
}

void MixDrawList::_CheckAndBindAndUpdate_PipeLine(SDL_GPUGraphicsPipeline* pipeLine, SDL_GPURenderPass* mainPass)
{
	if (lastPipeLine != pipeLine)
	{
		SDL_BindGPUGraphicsPipeline(mainPass, pipeLine);
		lastPipeLine = pipeLine;
	}
}

void MixDrawList::_CheckAndBindAndUpdate_VertexBuffer(uint32_t vsStartSlot, uint32_t vsBufferNum, SDL_GPUBufferBinding* vsBuffers, SDL_GPURenderPass* mainPass)
{
	if (lastVsStartSlot != vsStartSlot || lastVsBufferNum != vsBufferNum
		||0!=SDL_memcmp(vsBuffers, lastVsBuffers, vsBufferNum*sizeof(SDL_GPUBufferBinding)))
	{
		SDL_BindGPUVertexBuffers(mainPass, vsStartSlot, vsBuffers, vsBufferNum);
		lastVsStartSlot = vsStartSlot;
		lastVsBufferNum = vsBufferNum;
		SDL_memcpy(lastVsBuffers, vsBuffers, vsBufferNum*sizeof(SDL_GPUBufferBinding));
	}
}

void MixDrawList::_CheckAndBindAndUpdate_IndexBuffer(SDL_GPUIndexElementSize indexElementSize, SDL_GPUBufferBinding* indexBuffer, SDL_GPURenderPass* mainPass)
{
	if (0 != SDL_memcmp(&lastIndexBuffer, indexBuffer, sizeof(indexBuffer)))
	{
		SDL_BindGPUIndexBuffer(mainPass, indexBuffer, indexElementSize);
		lastIndexBuffer = *indexBuffer;
	}
}

void MixDrawList::_CheckAndBindAndUpdate_Texture(uint32_t texStartSlot, uint32_t texNum, SDL_GPUTextureSamplerBinding* textures, SDL_GPURenderPass* mainPass)
{
	if (lastTexStartSlot != texStartSlot || lastTexNum != texNum
		|| 0 != SDL_memcmp(textures, lastTextures, texNum * sizeof(SDL_GPUTextureSamplerBinding)))
	{
		SDL_BindGPUFragmentSamplers(mainPass, texStartSlot, textures, texNum);
		lastTexStartSlot = texStartSlot;
		lastTexNum = texNum;
		SDL_memcpy(lastVsBuffers, textures, texNum * sizeof(SDL_GPUTextureSamplerBinding));
	}
}

void MixDrawList::_CheckAndBindAndUpdate_VsUniform(uint32_t vsUniformSlot, const void* vsUniformData, uint32_t vsUniformDataLength, SDL_GPUCommandBuffer* mainCmd)
{


	if (lastVsUniformSlot != vsUniformSlot || lastVsUniformData != vsUniformData || lastVsUniformDataLength != vsUniformDataLength)
	{
		if(vsUniformData)
			SDL_PushGPUVertexUniformData(mainCmd, vsUniformSlot, vsUniformData, vsUniformDataLength);
		lastVsUniformSlot = vsUniformSlot;
		lastVsUniformData = vsUniformData;
		lastVsUniformDataLength = vsUniformDataLength;
	}
}

void MixDrawList::_CheckAndBindAndUpdate_PsUniform(uint32_t psUniformSlot, const void* psUniformData, uint32_t psUniformDataLength, SDL_GPUCommandBuffer* mainCmd)
{
	if (lastPsUniformSlot != psUniformSlot || lastPsUniformData != psUniformData || lastPsUniformDataLength != psUniformDataLength)
	{
		if (psUniformData)
			SDL_PushGPUFragmentUniformData(mainCmd, psUniformSlot, psUniformData, psUniformDataLength);
		lastPsUniformSlot = psUniformSlot;
		lastPsUniformData = psUniformData;
		lastPsUniformDataLength = psUniformDataLength;
	}
}

void MixDrawList::_CheckAndBindAndUpdate_ViewPort(SDL_GPUViewport* viewPort, SDL_GPURenderPass* mainPass)
{
	if (0!=SDL_memcmp(viewPort,&lastGpuViewport,sizeof(lastGpuViewport)))
	{
		SDL_SetGPUViewport(mainPass, viewPort);
		lastGpuViewport = *viewPort;
	}

}




