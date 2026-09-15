#include "GlobalGraphicResourceManager.h"








bool GlobalGraphicResourceManager::SetUp(SDL_GPUDevice* device)
{
	if (pDevice)
	{
		SDL_assert(false && "device already existed.");
		return false;
	}



	releaseDeviceWhenQuit = false;
	pDevice = device;
	if (!pDevice)
	{

		bool b_debug = false;
#ifdef _DEBUG
		b_debug = true;
#endif // _DEBUG


#if defined SDL_PLATFORM_WINDOWS
		pDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, b_debug, "direct3d12");
#elif defined SDL_PLATFORM_MACOS
		pDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, b_debug, "metal");
#elif defined SDL_PLATFORM_LINUX
		pDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, b_debug, "vulkan");
#endif // 
		if(pDevice)
			releaseDeviceWhenQuit = true;
	}

	return pDevice != nullptr;

}

void GlobalGraphicResourceManager::CleanUp()
{
	if (!pDevice)return;




	//着色器
	for (auto& x : shaderPool)
		SDL_ReleaseGPUShader(pDevice,x.second);
	shaderPool.clear();
	bindsMap.clear();

	SDL_ReleaseGPUTexture(pDevice,renderDisabledTexture);
	SDL_ReleaseGPUTexture(pDevice,emptyWindowTexture);



	if (releaseDeviceWhenQuit)
	{
		SDL_DestroyGPUDevice(pDevice);
		releaseDeviceWhenQuit = false;
	}

	//SDL_CreateGPUBuffer
}





SDL_GPUBuffer* GlobalGraphicResourceManager::CreateIndexBuffer(unsigned short* indices, int indicesCount)
{
	{
		SDL_GPUBufferCreateInfo bufferDesc = {};
		bufferDesc.usage = SDL_GPU_BUFFERUSAGE_INDEX;
		bufferDesc.size = sizeof(uint16_t) * indicesCount;

		SDL_GPUBuffer* pbuffer = SDL_CreateGPUBuffer(pDevice, &bufferDesc);
		if (!pbuffer)
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_GPU, "Indexbuffer create failed : %s", SDL_GetError());
		}
		else
		{
			SDL_GPUTransferBufferCreateInfo createInfo = {};
			createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			createInfo.size = sizeof(uint16_t) * indicesCount;

			SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(pDevice, &createInfo);

			// 2. 映射并写入数据
			void* mapped = SDL_MapGPUTransferBuffer(pDevice, transferBuffer, false);
			memcpy(mapped, indices, createInfo.size);
			SDL_UnmapGPUTransferBuffer(pDevice, transferBuffer);


			// 3. 创建命令缓冲区并开始 CopyPass
			SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(pDevice);
			SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


			SDL_GPUTransferBufferLocation bufferLocation = {};
			bufferLocation.transfer_buffer = transferBuffer;
			bufferLocation.offset = 0;

			SDL_GPUBufferRegion bufferRegion = {};
			bufferRegion.buffer = pbuffer;
			bufferRegion.size = sizeof(uint16_t) * indicesCount;


			SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

			// 5. 结束并提交
			SDL_EndGPUCopyPass(copyPass);
			SDL_SubmitGPUCommandBuffer(cmd);
			// 6. 释放 TransferBuffer
			SDL_ReleaseGPUTransferBuffer(pDevice, transferBuffer);
		}

		return pbuffer;
	}
}



