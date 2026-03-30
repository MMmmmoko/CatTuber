#include"Renderer/GlobalGraphicResourceManager.h"
#include"AppContext.h"
#include "VertexBuffer.h"



rendering::VertexBuffer::~VertexBuffer()
{
    SDL_ReleaseGPUBuffer(AppContext::GetGraphicDevice(), pVertexBuffer);
    SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), pTransferBuffer);
    pVertexBuffer = nullptr;
    pTransferBuffer = nullptr;
}

void rendering::VertexBuffer::Bind()
{
	SDL3Context* pContext = AppContext::GetSDL3RenderContext();
	SDL_GPUBufferBinding bufferBinding = {};
	bufferBinding.buffer = pVertexBuffer;
	pContext->SetVertexBuffers(0, 1, &bufferBinding);



    pContext->SetInputLayout(&inputlayout);

}





void rendering::VertexBuffer::UpdateVertex(void* data, size_t dataSize)
{
	if (!pVertexBuffer)
	{
		SDL_GPUBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
		bufferCreateInfo.size = static_cast<uint32_t>(dataSize);
		pVertexBuffer=SDL_CreateGPUBuffer(AppContext::GetGraphicDevice(),&bufferCreateInfo);
		SDLGPUCHECK(pVertexBuffer);




	}

    if (!pTransferBuffer)
    {
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferCreateInfo.size = static_cast<uint32_t>(dataSize);
        pTransferBuffer = SDL_CreateGPUTransferBuffer(AppContext::GetGraphicDevice(), &transferBufferCreateInfo);
        SDLGPUCHECK(pTransferBuffer);
    }




    SDL3Context* pContext = AppContext::GetSDL3RenderContext();
    // 映射并复制数据
    void* mapped = SDL_MapGPUTransferBuffer(pContext->GetDevice(), pTransferBuffer, false);

    if (mapped)
    {

        SDL_UnmapGPUTransferBuffer(pContext->GetDevice(), pTransferBuffer);
       
        // 创建命令缓冲区并开始复制过程
        SDL_GPUCommandBuffer* cmd = pContext->GetCopyCommandBuffer();
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTransferBufferLocation bufferLocation = { pTransferBuffer,0 };
        SDL_GPUBufferRegion bufferRegion;
        bufferRegion.buffer = pVertexBuffer;
        bufferRegion.offset = 0;
        bufferRegion.size = static_cast<uint32_t>(dataSize);

        SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

        SDL_EndGPUCopyPass(copyPass);

        //不要单独开启CommandBufer并Submit，非常耗时
        //这里修改后一下子帧率从40上升到400了...不过离Live2D的D3D11还有差距
        //SDL_SubmitGPUCommandBuffer(cmd);



    }
}



static const uint8_t strideTable[] =
{
    0,//SDL_GPU_VERTEXELEMENTFORMAT_INVALID
    4,8,12,16,//SDL_GPU_VERTEXELEMENTFORMAT_INT
    4,8,12,16,//SDL_GPU_VERTEXELEMENTFORMAT_UINT
    4,8,12,16,//SDL_GPU_VERTEXELEMENTFORMAT_FLOAT
    2,4,//SDL_GPU_VERTEXELEMENTFORMAT_BYTE2
    2,4,//SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2
    2,4,//SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM
    2,4,//SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM
    4,8,//SDL_GPU_VERTEXELEMENTFORMAT_SHORT2
    4,8,//SDL_GPU_VERTEXELEMENTFORMAT_USHORT2
    4,8,//SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM
    4,8,//SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM
    4,8//SDL_GPU_VERTEXELEMENTFORMAT_HALF2
};

void rendering::VertexBuffer::PushLayout(SDL_GPUVertexElementFormat format)
{
    _vertexBufferDescription.pitch += strideTable[format];
    
    uint32_t curAttrIndex = inputlayout.num_vertex_attributes;
    inputlayout.num_vertex_attributes++;
    _vertexAttr[curAttrIndex].location = curAttrIndex;
    _vertexAttr[curAttrIndex].format = format;
    
    for (uint32_t i = 0; i < curAttrIndex; i++)
    {
        _vertexAttr[curAttrIndex].offset += strideTable[_vertexAttr[i].format];
    }


}

void rendering::VertexBuffer::ReleaseTransferBuffer()
{
    //对于不需要改变的顶点Buffer,手动释放
    if (pTransferBuffer)
    {
        SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), pTransferBuffer);
        pTransferBuffer = NULL;
	}


}
