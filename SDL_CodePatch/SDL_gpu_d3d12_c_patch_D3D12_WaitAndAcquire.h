//由于一些原因，CatTuber必须改动SDL的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。

//修改方式：原代码中用#ifndef #else注释源代码


static bool D3D12_INTERNAL_AcquireSwapchainTexture(
    bool block,
    SDL_GPUCommandBuffer* commandBuffer,
    SDL_Window* window,
    SDL_GPUTexture** swapchainTexture,
    Uint32* swapchainTextureWidth,
    Uint32* swapchainTextureHeight)
{
    D3D12CommandBuffer* d3d12CommandBuffer = (D3D12CommandBuffer*)commandBuffer;
    D3D12Renderer* renderer = d3d12CommandBuffer->renderer;
    D3D12WindowData* windowData;
    Uint32 swapchainIndex;
    HRESULT res;

    *swapchainTexture = NULL;
    if (swapchainTextureWidth) {
        *swapchainTextureWidth = 0;
    }
    if (swapchainTextureHeight) {
        *swapchainTextureHeight = 0;
    }

    windowData = D3D12_INTERNAL_FetchWindowData(window);
    if (windowData == NULL) {
        SET_STRING_ERROR_AND_RETURN("Cannot acquire swapchain texture from an unclaimed window!", false);
    }

    //这里是CatTuber修改部分！
    //这里是CatTuber修改部分！
    //这里是CatTuber修改部分！
    //这里是CatTuber修改部分！
    //CatTuber不希望由SDL来控制交换链Resize，所以将下面代码注释掉了，因为CatTuber不在主线程进行渲染
    //if (windowData->needsSwapchainRecreate) {
    //    if (!D3D12_INTERNAL_ResizeSwapchain(renderer, windowData)) {
    //        return false;
    //    }
    //}
    //上面是CatTuber修改部分！
    //上面是CatTuber修改部分！
    //上面是CatTuber修改部分！
    //上面是CatTuber修改部分！
    if (swapchainTextureWidth) {
        *swapchainTextureWidth = windowData->width;
    }
    if (swapchainTextureHeight) {
        *swapchainTextureHeight = windowData->height;
    }

    if (windowData->inFlightFences[windowData->frameCounter] != NULL) {
        if (block) {
            // In VSYNC mode, block until the least recent presented frame is done
            if (!D3D12_WaitForFences(
                (SDL_GPURenderer*)renderer,
                true,
                &windowData->inFlightFences[windowData->frameCounter],
                1)) {
                return false;
            }
        }
        else {
            // If we are not blocking and the least recent fence is not signaled,
            // return true to indicate that there is no error but rendering should be skipped.
            if (!D3D12_QueryFence(
                (SDL_GPURenderer*)renderer,
                windowData->inFlightFences[windowData->frameCounter])) {
                return true;
            }
        }

        D3D12_ReleaseFence(
            (SDL_GPURenderer*)renderer,
            windowData->inFlightFences[windowData->frameCounter]);

        windowData->inFlightFences[windowData->frameCounter] = NULL;
    }

#if (defined(SDL_PLATFORM_XBOXONE) || defined(SDL_PLATFORM_XBOXSERIES))
    // FIXME: Should this happen before the inFlightFences stuff above?
    windowData->frameToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
    renderer->device->WaitFrameEventX(D3D12XBOX_FRAME_EVENT_ORIGIN, INFINITE, NULL, D3D12XBOX_WAIT_FRAME_EVENT_FLAG_NONE, &windowData->frameToken);
    swapchainIndex = windowData->frameCounter;
#else
    swapchainIndex = IDXGISwapChain3_GetCurrentBackBufferIndex(windowData->swapchain);

    // Set the handle on the windowData texture data.
    res = IDXGISwapChain_GetBuffer(
        windowData->swapchain,
        swapchainIndex,
        D3D_GUID(D3D_IID_ID3D12Resource),
        (void**)&windowData->textureContainers[swapchainIndex].activeTexture->resource);
    CHECK_D3D12_ERROR_AND_RETURN("Could not acquire swapchain!", false);
#endif

    // Set up presentation
    if (d3d12CommandBuffer->presentDataCount == d3d12CommandBuffer->presentDataCapacity) {
        d3d12CommandBuffer->presentDataCapacity += 1;
        d3d12CommandBuffer->presentDatas = (D3D12PresentData*)SDL_realloc(
            d3d12CommandBuffer->presentDatas,
            d3d12CommandBuffer->presentDataCapacity * sizeof(D3D12PresentData));
    }
    d3d12CommandBuffer->presentDatas[d3d12CommandBuffer->presentDataCount].windowData = windowData;
    d3d12CommandBuffer->presentDatas[d3d12CommandBuffer->presentDataCount].swapchainImageIndex = swapchainIndex;
    d3d12CommandBuffer->presentDataCount += 1;

    // Set up resource barrier
    D3D12_RESOURCE_BARRIER barrierDesc;
    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrierDesc.Transition.pResource = windowData->textureContainers[swapchainIndex].activeTexture->resource;
    barrierDesc.Transition.Subresource = 0;

    ID3D12GraphicsCommandList_ResourceBarrier(
        d3d12CommandBuffer->graphicsCommandList,
        1,
        &barrierDesc);

    *swapchainTexture = (SDL_GPUTexture*)&windowData->textureContainers[swapchainIndex];
    return true;
}



//添加一个用于CatTuber中自主resize Swapchain的接口
//外部用extern bool SDL_D3D12_ResizeSwapchain(SDL_GPUCommandBuffer* commandBuffer, SDL_Window* window)进行引用

/* Some compilers use a special export keyword */
#define SDL_PLATFORM_WINDOWS
# if defined(SDL_PLATFORM_WINDOWS)
#  ifdef DLL_EXPORT
#   define SDL_DECLSPEC __declspec(dllexport)
#  else
#   define SDL_DECLSPEC
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define SDL_DECLSPEC __attribute__ ((visibility("default")))
#  else
#   define SDL_DECLSPEC
#  endif
# endif

/* By default SDL uses the C calling convention */
#ifndef SDLCALL
#if defined(SDL_PLATFORM_WINDOWS) && !defined(__GNUC__)
#define SDLCALL __cdecl
#else
#define SDLCALL
#endif
#endif /* SDLCALL */
SDL_DECLSPEC bool SDLCALL SDL_D3D12_ResizeSwapchain(SDL_GPUCommandBuffer* commandBuffer, SDL_Window* window)
{
    D3D12CommandBuffer* d3d12CommandBuffer = (D3D12CommandBuffer*)commandBuffer;
    D3D12Renderer* renderer = d3d12CommandBuffer->renderer;
    D3D12WindowData* windowData = D3D12_INTERNAL_FetchWindowData(window);

    return D3D12_INTERNAL_ResizeSwapchain(renderer, windowData);
}