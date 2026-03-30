#include"Renderer/GlobalGraphicResourceManager.h"
#include"AppContext.h"
#include "IndexBuffer.h"



rendering::IndexBuffer::~IndexBuffer()
{
    SDL_ReleaseGPUBuffer(AppContext::GetGraphicDevice(), pIndexBuffer);
    pIndexBuffer = nullptr;
}

void rendering::IndexBuffer::Bind()
{
	SDL3Context* pContext = AppContext::GetSDL3RenderContext();

    pContext->SetIndexBuffer(pIndexBuffer,elementSize);


}





void rendering::IndexBuffer::UpdateIndices(const void* data, SDL_GPUIndexElementSize index_element_size,uint32_t indicesCount)
{
    const uint32_t unitSize[] = {2,4};
    uint32_t dataSize = unitSize[index_element_size] * indicesCount;

	if (!pIndexBuffer)
	{
		SDL_GPUBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
		bufferCreateInfo.size = dataSize;
		pIndexBuffer=SDL_CreateGPUBuffer(AppContext::GetGraphicDevice(),&bufferCreateInfo);
		SDLGPUCHECK(pIndexBuffer);
	}
	//创建传输缓冲区

    SDL_GPUTransferBuffer* pTransferBuffer;
    {
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = dataSize;
        pTransferBuffer = SDL_CreateGPUTransferBuffer(AppContext::GetGraphicDevice(), &transferBufferCreateInfo);
        SDLGPUCHECK(pTransferBuffer);
    }




    SDL3Context* pContext = AppContext::GetSDL3RenderContext();
    // 映射并复制数据
    void* mapped = SDL_MapGPUTransferBuffer(pContext->GetDevice(), pTransferBuffer, false);

    if (mapped)
    {
        memcpy(mapped, data, dataSize);
        SDL_UnmapGPUTransferBuffer(pContext->GetDevice(), pTransferBuffer);
       
        // 创建命令缓冲区并开始复制过程
        SDL_GPUCommandBuffer* cmd = pContext->GetCopyCommandBuffer();
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTransferBufferLocation bufferLocation = { pTransferBuffer,0 };
        SDL_GPUBufferRegion bufferRegion;
        bufferRegion.buffer = pIndexBuffer;
        bufferRegion.offset = 0;
        bufferRegion.size = dataSize;

        SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

        SDL_EndGPUCopyPass(copyPass);

        //不要单独开启CommandBufer并Submit，非常耗时
        //这里修改后一下子帧率从40上升到400了...不过离Live2D的D3D11还有差距
        //SDL_SubmitGPUCommandBuffer(cmd);
    }

    SDL_ReleaseGPUTransferBuffer(pContext->GetDevice(), pTransferBuffer);

    elementSize = index_element_size;
    indexCount = indicesCount;

}


