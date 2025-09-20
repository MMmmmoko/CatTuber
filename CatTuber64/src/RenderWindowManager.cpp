



#include <SDL3/SDL_log.h>

#include<../../Live2DFramework/SDL3Renderer/CubismRenderer_SDL3.hpp>
#include"Model/Live2DModelBase.h"
#include"AppSettings.h"
#include"AppContext.h"
#include"Util/Util.h"
#include"RenderThread.h"
#include"RenderWindowManager.h"
#include"UserEvent.h"
#include"../Tray.h"
SDL_FColor RenderWindowController::clearColor = { 0.f,0.f, 0.f, 0.f };



RenderWindowController::RenderWindowController(const char* t,int width, int height, int px, int py)
    : title(t), targetX(px), targetY(py), targetW(width), targetH(height), renderW(width), renderH(height){





}

RenderWindowController::~RenderWindowController() {
    Shutdown();
}

bool RenderWindowController::_CreateWindow()
{
    SDL_PropertiesID props = SDL_CreateProperties();
    if (props == 0) {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Unable to create properties: %s", SDL_GetError());
        throw std::runtime_error(SDL_GetError());
    }
    //根据信息创建窗口
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, targetW);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, targetH);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, targetX);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, targetY);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);




    isTransparent = AppSettings::GetIns().GetWindowTransparent();
    SetClearColor(AppSettings::GetIns().GetWindowBackgroundColor());


    //实验透明
    //isTransparent = true;


    if (isTransparent)
    {
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, false);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);
    }

    if (AppSettings::GetIns().GetWindowTop())
    {
        SDL_SetBooleanProperty(props,SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN,true);
    }



    window = SDL_CreateWindowWithProperties(props);
    if (!window)
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Unable to create window: %s", SDL_GetError());
        return false;
    }




    _AfterCreateWindow();
    
    SetAspectSize(targetW, targetH);

    //让整个窗口可拖动 
    SDL_HitTest hittestFunc = [](SDL_Window* window, const SDL_Point* point, void* userData)->SDL_HitTestResult
        {
            if (((RenderWindowController*)userData)->window == window)
                return SDL_HitTestResult::SDL_HITTEST_DRAGGABLE;
            else
                return SDL_HitTestResult::SDL_HITTEST_NORMAL;
        };
    //SDL_SetWindowHitTest(window, hittestFunc,this);


    windowID = SDL_GetWindowID(window);
    return true;
}

void RenderWindowController::_ResetOffscreenTex()
{
//windows中，改变clear颜色或者设置transparency的时候重建纹理//因为dx12提醒不重建的话clear颜色对不上会导致性能下降，也会导致警告刷屏



    if (offscreenTex)
    {
        SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), offscreenTex);
        offscreenTex = nullptr;
    }

    //重新创建为指定目标颜色的纹理
    SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), offscreenTex);
    SDL_GPUTextureCreateInfo textureDesc = {};
    textureDesc.type = SDL_GPU_TEXTURETYPE_2D;
    //textureDesc.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureDesc.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    textureDesc.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    textureDesc.width = renderW;
    textureDesc.height = renderH;
    textureDesc.layer_count_or_depth = 1;
    textureDesc.num_levels = 1;
#ifdef SDL_PLATFORM_WINDOWS

    if (!isTransparent)
    {
        auto props = SDL_CreateProperties();
        SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT, clearColor.r);
        SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT, clearColor.g);
        SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT, clearColor.b);
        SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT, clearColor.a);
        textureDesc.props = props;
    }
#endif

    offscreenTex = SDL_CreateGPUTexture(AppContext::GetGraphicDevice(), &textureDesc);
    if (!offscreenTex)
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Call CreateGPUTexture() failed! %s", SDL_GetError());
        throw(std::runtime_error("Call CreateGPUTexture() failed!"));
    }


}

bool RenderWindowController::ResetGraphic(int W, int H) {
    

    if (offscreenTex_2D)
    {
        SDL_DestroyTexture(offscreenTex_2D);
        offscreenTex_2D = nullptr;
    }
    //if (renderer)
    //{
    //    SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), offscreenTex);
    //    offscreenTex = nullptr;
    //}
    if (depthStencil)
    {
        SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), depthStencil);
        depthStencil = nullptr;
    }
    if (offscreenTex)
    {
        SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), offscreenTex);
        offscreenTex = nullptr;
    }
    if (offscreenTexTb)
    {
        SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), offscreenTexTb);
        offscreenTexTb = nullptr;
    }




    do
    {
        //if (!SDL_CreateWindowAndRenderer(title, targetX, targetY,/* SDL_WINDOW_HIDDEN |*/ SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT,
        //    &window, &renderer
        //))
        //{
        //    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Call CreateWindowAndRenderer() failed! %s",SDL_GetError());
        //    break;
        //}
        //2D renderer




        //offscreenTex
        //_ResetOffscreenTex();
        needResetOffscreenTex = true;

        //SDL_GPUTextureCreateInfo textureDesc = {};
        //textureDesc.type = SDL_GPU_TEXTURETYPE_2D;
        ////textureDesc.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        //textureDesc.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
        //textureDesc.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        //textureDesc.width = W;
        //textureDesc.height = H;
        //textureDesc.layer_count_or_depth = 1;
        //textureDesc.num_levels = 1;
        //offscreenTex = SDL_CreateGPUTexture(AppContext::GetGraphicDevice(), &textureDesc);
        //if (!offscreenTex)
        //{
        //    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Call CreateGPUTexture() failed! %s", SDL_GetError());
        //    break;
        //}


        //offscreenTexTb 在需要的时候才开始创建
        /*
        SDL_GPUTransferBufferCreateInfo tbinfo = {};
        tbinfo.size = 1920 * 1080 * 4;
        tbinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
        offscreenTexTb = SDL_CreateGPUTransferBuffer(AppContext::GetGraphicDevice(), &tbinfo);
        if (!offscreenTexTb)
        {
            SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Call CreateGPUTransferBuffer() failed! %s", SDL_GetError());
            break;
        }*/



        if (isTransparent)
        {
            //renderer = SDL_CreateRenderer(window, NULL);
#ifdef SDL_PLATFORM_WINDOWS

#ifdef _DEBUG
            if (!renderer)
            {
                renderer = SDL_CreateRenderer(window, "direct3d12");
                if (renderer)
                {
                    SDL_assert(false && "Direct3D12 is support in sdl 2d transparent renderer!");
                }
            }
#endif // //检测透明窗口对SDL12的支持.  目前SDL的透明窗口只支持direct3d11
            //renderer不进行多次创建
            if (!renderer)
            {
                renderer = SDL_CreateRenderer(window, "direct3d11");
                if (!renderer)
                {
                    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create D3d11 renderer in transparent window failed! %s", SDL_GetError());
                    throw(std::runtime_error("Create D3d11 renderer in transparent window failed!"));
                }


            }
            //2D的API可以随时改变窗口大小
            //int w, h;
            //SDL_GetCurrentRenderOutputSize(renderer, &w, &h);
            //SDL_Log("Renderer: %d,%d", w, h);
#endif


            offscreenTex_2D = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, W, H);
            if (!offscreenTex_2D)
            {
                SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Call SDL_CreateTexture() failed! %s", SDL_GetError());
                break;
            }

            //从渲染器中获取D3D1纹理











        }else
        {
            if (!deviceClaimed)
            {
                if (!SDL_ClaimWindowForGPUDevice(AppContext::GetGraphicDevice(), window))
                {
                    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Call ClaimWindowForGPUDevice() failed! %s", SDL_GetError());
                    break;
                }
                deviceClaimed = true;
            }
        }


        scene.SetCanvasSize(renderW, renderH);
        return true;
    } while (false);
    Shutdown();
    return false;
    /*
#if defined SDL_PLATFORM_WINDOWS
    window = SDL_CreateWindow(title, x, y, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_TRANSPARENT);
#elif defined SDL_PLATFORM_MACOS
    window = SDL_CreateWindow(title, x, y, SDL_WINDOW_METAL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
#elif defined SDL_PLATFORM_LINUX
    window = SDL_CreateWindow(title, x, y, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);

#endif // 0

    //window = SDL_CreateWindow(title, x, y,  SDL_WINDOW_RESIZABLE);



    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL3 Create Window Failed: %s", SDL_GetError());
        return false;
    }
    windowID = SDL_GetWindowID(window);



   
    if (!SDL_ClaimWindowForGPUDevice(AppContext::GetGraphicDevice(), window))
    {
        SDL_LogError(SDL_LOG_CATEGORY_GPU, "SDL3 ClaimWindowForGPUDevice Failed: %s", SDL_GetError());
        Shutdown();
        return false;
    }

    SDL_ShowWindow(window);
    return true;
    */
}

void RenderWindowController::HandleEvent(const SDL_Event& event) {
    // 仅处理属于本窗口的事件
    if (event.window.windowID != windowID)
        return;

    if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
        int pxW, pxH;
        if(SDL_GetWindowSizeInPixels(window, &pxW, &pxH))
            _OnResize(pxW, pxH);
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            //右键按下
        }
    }




    // 可继续扩展其他事件（键盘、鼠标等）
}


void RenderWindowController::SetTitle(const char* title)
{
    if (window)
        SDL_SetWindowTitle(window, title);
}

void RenderWindowController::SetTransparent(bool t)
{
    //TODO/FIXME
    //assert(false);



    //如果是windows，则重置一下纹理
#ifdef SDL_PLATFORM_WINDOWS
    needResetOffscreenTex=true;
#endif
}

void RenderWindowController::SetTop(bool b)
{
    if (window)
        SDL_SetWindowAlwaysOnTop(window,b);
}


void RenderWindowController::SetClearColor(SDL_Color color)
{
    clearColor.r = color.r / 255.f;
    clearColor.g = color.g / 255.f;
    clearColor.b = color.b / 255.f;
    clearColor.a = 0.f;

    //如果是windows，则重置一下纹理
#ifdef SDL_PLATFORM_WINDOWS
    //_ResetOffscreenTex();
    needResetOffscreenTex = true;
#endif

    
}

void RenderWindowController::SetWindowSize(int W, int H)
{
    SDL_SetWindowSize(window,W,H);
    aspectRatioW = W;
    aspectRatioH = H;
}

void RenderWindowController::GetRenderSize(int* pw, int* ph)
{
    if (pw)*pw = renderW;
    if (ph)*ph = renderH;
}

void RenderWindowController::GetAspectSize(int* aw, int* ah)
{
    if (aw)*aw = aspectRatioW;
    if (ah)*ah = aspectRatioH;
}

void RenderWindowController::SetAspectSize(int aw, int ah)
{
    aspectRatioW = aw;
    aspectRatioH = ah;
}


//void RenderWindowController::_OnResizing(int newW, int newH)
//{
//
//    //SDL_SendWindowEvent(window,SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED, newW, newH);
//    
//    
//    //提取并处理task事件
//    SDL_Event event;
//    if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_USER + UserEvent::TASK, SDL_EVENT_USER + UserEvent::TASK) > 0)
//    {
//        UserEvent::HandleUserEvent(&event.user);
//    }
//}



void RenderWindowController::Update(uint64_t deltaTicksNS) {
    scene.Update(deltaTicksNS);
}


void RenderWindowController::Render() {



    if (needResetOffscreenTex)
    {
        _ResetOffscreenTex();
        needResetOffscreenTex = false;
    }





    do
    {



        Csm::Rendering::CubismRenderContext_SDL3* pContext = AppContext::GetLive2DRenderContext();

        //窗口的渲染

        //好像这个cmd还是放外面好点
        //创建当前帧的命令缓存
        cmdCurframe = SDL_AcquireGPUCommandBuffer(AppContext::GetGraphicDevice());
        cmdCurframeCopy = SDL_AcquireGPUCommandBuffer(AppContext::GetGraphicDevice());
        if (!cmdCurframe || !cmdCurframeCopy)
        {
            break;
        }
        auto cmd = cmdCurframe;



        unsigned int curRenderSizeW, curRenderSzieH;
        SDL_GPUTexture* curTargetTex = NULL;
        //TODO 后续增加新功能时（如后处理功能）将isTransparent改为isNeedOffscreen()
        if (isTransparent)
        {
            curTargetTex = offscreenTex;
            curRenderSizeW = renderW;
            curRenderSzieH = renderH;
        }
        else
        {
            curTargetTex = offscreenTex;
            curRenderSizeW = renderW;
            curRenderSzieH = renderH;

        }




        //创建ClearRenderPass清理
        SDL_GPURenderPass* _clearPass;
        {
            SDL_GPUColorTargetInfo colorTargetInfo = {};
            colorTargetInfo.texture = curTargetTex;
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            if (isTransparent)
                colorTargetInfo.clear_color = { 0.f,0.f,0.f ,0.F };
            else
                colorTargetInfo.clear_color = clearColor;//没有透明的时候设置特定背景色
            //colorTargetInfo.clear_color = { clearValue,clearValue,clearValue ,0.F };//没有透明的时候设置特定背景色
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;



            //帧开始时的清理
            //SDL_GPURenderPass* _clearPass;
            if (!depthStencil)
            {

                //pContext->StartFrame(cmd, & colorTargetInfo,NULL);
                _clearPass = SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, NULL);
            }
            else
            {
                SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {};
                depthStencilTargetInfo.texture = depthStencil;
                depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
                depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
                depthStencilTargetInfo.clear_depth = 1.f;
                depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
                depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;
                depthStencilTargetInfo.clear_stencil = 0;


                //pContext->StartFrame(cmd, &colorTargetInfo, &depthStencilTargetInfo);
                _clearPass = SDL_BeginGPURenderPass(cmd, &colorTargetInfo, 1, &depthStencilTargetInfo);
            }


            pContext->StartFrame(cmdCurframe,_clearPass,cmdCurframeCopy);
            //SDL_EndGPURenderPass(_clearPass);
        }




        //int renderW, renderH;
        //GetRenderSize(&renderW,&renderH);

        Csm::Rendering::CubismRenderer_SDL3::StartFrame(
            AppContext::GetGraphicDevice(),
            pContext, renderW, renderH
        );


		//Rendering
		//Rendering
		//Rendering
		//Rendering
		//Rendering
		//Rendering
		//Rendering
		//Rendering
        //scene.Draw(curTargetTex, depthStencil, renderW, renderH, cmd, cmdCurframeCopy);
        scene.Draw(_clearPass, renderW, renderH, cmd, cmdCurframeCopy);
        //Rendering
        //Rendering
        //Rendering
        //Rendering
        //Rendering




        Csm::Rendering::CubismRenderer_SDL3::EndFrame(AppContext::GetGraphicDevice());
        //SDL_EndGPURenderPass(_clearPass);FIXME目前在下面的函数中调用，这应该是不合理的，应该修改
        pContext->EndFrame();

        //SDL_EndGPURenderPass(_clearPass);


        //SDL_SubmitGPUCommandBuffer

        return;


    }while (false);

    //中间出了问题，不进行渲染，进行可能的资源清理
    if (cmdCurframe)
    {
        SDL_CancelGPUCommandBuffer(cmdCurframe);
        cmdCurframe = NULL;
    }
    if (cmdCurframeCopy)
    {
        SDL_CancelGPUCommandBuffer(cmdCurframeCopy);
        cmdCurframe = NULL;
    }

    return;
}



void RenderWindowController::Present()
{
    if (!cmdCurframe)return;
    auto cmd = cmdCurframe;

    if (isTransparent)
    {



        //windows中sdl renderder使用direct3D11 ，GPU API使用direct3D12
        //虽然SDL本身不提供这个两个图形API的互通，但它们实际上是能互通而不需要通过内存互通
        //所以为了性能考虑，这里需要特殊处理
        
        //初始化资源
        {
            HANDLE shareHandle = 0;
            if (!d3d12ShareTex)
            {
                //创建共享纹理
                SDL_GPUTextureCreateInfo textureDesc = {};
                textureDesc.type = SDL_GPU_TEXTURETYPE_2D;
                //textureDesc.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
                textureDesc.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;//应该与原离屏纹理相同
                textureDesc.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
                textureDesc.width = renderW;
                textureDesc.height = renderH;
                textureDesc.layer_count_or_depth = 1;
                textureDesc.num_levels = 1;

                auto props = SDL_CreateProperties();
                SDL_SetBooleanProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_BOOL, true);
                SDL_SetPointerProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_HANDLE_POINTER, &shareHandle);
                textureDesc.props = props;

                d3d12ShareTex = SDL_CreateGPUTexture(AppContext::GetGraphicDevice(), &textureDesc);
                //在对SDL代码修改后，设置上述Property时，创建共享纹理成功后会自动设置handle值
                SDL_DestroyProperties(props);
                if (!d3d12ShareTex)
                {
                    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Create Share D3D12 Texture failed! %s", SDL_GetError());
                    throw(std::runtime_error("Create Share D3D12 Texture failed!"));
                }

                if (!shareHandle)
                {
                    throw(std::runtime_error("Create D3D12 ShareTexture handle failed!"));
                }
            }



            if (!rendererD3d11Device)
            {
                //从renderer中获取device指针
                rendererD3d11Device = static_cast<ID3D11Device1*>(
                    SDL_GetPointerProperty(
                        SDL_GetRendererProperties(renderer),
                        SDL_PROP_RENDERER_D3D11_DEVICE_POINTER,
                        nullptr
                    ));
            }

            if (!texToD3D12Copy)
            {
                //从共享句柄创建纹理
                ID3D11Texture2D* ptexture;
                if (S_OK != rendererD3d11Device->OpenSharedResource1(
                    shareHandle,
                    __uuidof(ID3D11Texture2D),
                    (void**) &ptexture
                ))
                {
                    throw(std::runtime_error("D3D11 OpenSharedResource Failed."));
                };

                //将D3D11纹理提供给SDL进行封装
                auto props = SDL_CreateProperties();
                SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, renderW);
                SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, renderH);
                SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_ARGB8888);
                SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_D3D11_TEXTURE_POINTER, ptexture);
                texToD3D12Copy=SDL_CreateTextureWithProperties(renderer, props);
                SDL_DestroyProperties(props);
                if (!texToD3D12Copy)
                {
                    SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "D3D11 CreateTextureWithProperties Failed. %s", SDL_GetError());
                    throw(std::runtime_error("D3D11 CreateTextureWithProperties Failed."));
                }
            }
            if (shareHandle)
            {
                CloseHandle(shareHandle);
            }
        }




        //资源拷贝
        {
            //主离屏到d3d12 copyTex的拷贝
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
            SDL_GPUTextureRegion texRegion = {};
            texRegion.texture = offscreenTex;
            texRegion.w = renderW;
            texRegion.h = renderH;
            texRegion.d = 1;
            SDL_GPUTextureLocation source = {};
            source.texture = offscreenTex;
            SDL_GPUTextureLocation target = {};
            target.texture = d3d12ShareTex;

            SDL_CopyGPUTextureToTexture(copyPass,&source,&target,renderW,renderH,1,false);
            SDL_EndGPUCopyPass(copyPass);
        }







        SDL_SubmitGPUCommandBuffer(cmdCurframeCopy);
        //SDL_SubmitGPUCommandBuffer(cmd);
        //确保下方的SDL_RenderPresent已经执行完？
        SDL_GPUFence* fence= SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
        if (!fence)
        {
            SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "SubmitGPUCommandBufferAndAcquireFence failed! %s", SDL_GetError());
            return;
        }

        if (!SDL_WaitForGPUFences(AppContext::GetGraphicDevice(), 0, &fence, 1))
        {
            SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "WaitForGPUFences failed! %s", SDL_GetError());
            SDL_ReleaseGPUFence(AppContext::GetGraphicDevice() ,fence);
            return;
        }
        
        //渲染完成将D3D11纹理copy到交换链上
        SDL_RenderTexture(renderer, texToD3D12Copy, NULL, NULL);
        SDL_RenderPresent(renderer);
        //按理来说应该需要个手段防止下一帧渲染到共享纹理的时候共享纹理没有使用完成。（D3D11\D3D12并行）
        //但这里因为太麻烦而不进行任何作为吗？哈基妮，你这家伙

        //optimize
        //PRESENT里等待的情况比较多,且每个窗口相对较为独立
        //有必要给每个窗口单独开个渲染线程吗？



    }
    else
    {
        uint32_t swapchainTextureWidth, swapchainTextureHeight;
        SDL_GPUTexture* swapchainTexture;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, &swapchainTextureWidth, &swapchainTextureHeight);


        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTextureLocation source = {};
        source.texture = offscreenTex;



		//SDL_AcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, &swapchainTextureWidth, &swapchainTextureHeight);
		if (!swapchainTexture)
		{
			//SDL_LogError(SDL_LOG_CATEGORY_GPU, "SDL3 AcquireGPUSwapchainTexture Failed");
            SDL_EndGPUCopyPass(copyPass);
			return;
		}

        SDL_GPUTextureLocation destination = {};
        destination.texture = swapchainTexture;
        SDL_CopyGPUTextureToTexture(copyPass,&source,&destination,
            SDL_min(static_cast<Uint32>(renderW), swapchainTextureWidth), SDL_min(static_cast<Uint32>(renderH), swapchainTextureHeight),1,false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmdCurframeCopy);
        SDL_SubmitGPUCommandBuffer(cmd);
    }






    //在一帧结束的时候进行纹理的或窗口尺寸变化的处理
    //
    // 
    // 
    // 
    // 
    // 
    //


    if (renderW != targetW || renderH != targetH)
    {
        
        if (isTransparent)
        {
            //2DAPI不不需要修改交换链相关的代码(D3D11真神吧)
            if (!ResetGraphic(targetW, targetH))
            {
                throw(std::runtime_error("Can not Reset Window Graphic!"));
            }

            renderW = targetW;
            renderH = targetH;
            scene.SetCanvasSize(renderW, renderH);
        }
        else
        {
            if (_ResizeSwapchain(cmd, window))
            {
                //仅在交换链重建成功时设置数据
                if (!ResetGraphic(targetW, targetH))
                {
                    throw(std::runtime_error("Can not Reset Window Graphic!"));
                }

                renderW = targetW;
                renderH = targetH;



                scene.SetCanvasSize(renderW, renderH);
            }
        }


        //不管怎么样都给SDL发送重绘消息
        
#ifdef SDL_PLATFORM_WINDOWS
        SDL_PropertiesID props = SDL_GetWindowProperties(window);
        HWND hwnd = (HWND)SDL_GetPointerProperty(
            props,
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            nullptr       // 可选：传入 SDL_bool* 获取是否查询成功
        );
        InvalidateRect(hwnd, NULL, TRUE);
#endif // DEBUG

    }


    //无论如何两个cmd指针都不可用了，设置为空
    cmdCurframe = NULL;
    cmdCurframeCopy = NULL;
}

void RenderWindowController::_OnResize(int newW, int newH)
{
    //窗口已经发生Resize的时候调用这个函数
    //会在主线程调用，所以用post的方法？、

    SDL_Log("Window Resize %d,%d", newW, newH);


    uint64_t taskParam;
    UTIL_SETLOW32VALUE(taskParam, newW);
    UTIL_SETHIGH32VALUE(taskParam, newH);
    RenderThread::GetIns().PostTask([](void* userdata,uint64_t taskParam) {

        RenderWindowController* pThis = (RenderWindowController*)userdata;
        pThis->targetW = UTIL_GETLOW32VALUE(taskParam);
        pThis->targetH = UTIL_GETHIGH32VALUE(taskParam);

        },this, taskParam);
    //发送窗口重绘的消息
    //SDL_Event event = {};
    //event.type = SDL_EVENT_WINDOW_EXPOSED;
    //event.window.windowID = windowID;
    //SDL_PushEvent(&event);
}

void RenderWindowController::Shutdown() {


    if (window) {
        AppSettings::GetIns().GetWindowTransparent();
        SDL_ReleaseWindowFromGPUDevice(AppContext::GetGraphicDevice(),window);
    }

    //清理scene
    scene.Reset();

    if (offscreenTex_2D)
    {
        SDL_DestroyTexture(offscreenTex_2D);
        offscreenTex_2D = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (depthStencil)
    {
        SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), depthStencil);
        depthStencil = nullptr;
    }
    if (offscreenTex)
    {
        SDL_ReleaseGPUTexture(AppContext::GetGraphicDevice(), offscreenTex);
        offscreenTex = nullptr;
    }
    if (offscreenTexTb)
    {
        SDL_ReleaseGPUTransferBuffer(AppContext::GetGraphicDevice(), offscreenTexTb);
        offscreenTexTb = nullptr;
    }




    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

Json::Value RenderWindowController::Save()
{
    //保存与软件设置无关的项目，比如clearColor和isTransparent就不需要保存
    
    Json::Value windowJson;
    if (!window)
        return windowJson;



    //获取像素单位的窗口大小
    int _w, _h;
    if (!SDL_GetWindowSizeInPixels(window, &_w, &_h))
    {
        _w = targetW;
        _h = targetH;
    }


    bool bSaveAsAbsoluteCoordinate = false;
    SDL_DisplayID display=SDL_GetDisplayForWindow(window);
    if (0 == display)
    {
        //获取屏幕失败的话，用绝对坐标存储
        bSaveAsAbsoluteCoordinate = true;
    }
    else
    {
        //寻找同名屏幕,如果有同名屏幕就用绝对坐标存储
        int displaysCount;
        SDL_DisplayID* displays= SDL_GetDisplays(&displaysCount);
        const char* curDispalyName = SDL_GetDisplayName(display);

        for (int index = 0; index < displaysCount; index++)
        {
            if (displays[index] == display)
            {
                continue;
            }
            if (SDL_strcmp(SDL_GetDisplayName(displays[index]), curDispalyName) == 0)
            {
                SDL_LogInfo(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,
                    "Other Same Name Display Finded , Window Pos Will Save As Absolute Coordinate: %s", curDispalyName);
                bSaveAsAbsoluteCoordinate = true;
                break;
            }
        }

        SDL_free(displays);
        windowJson["Display"] = SDL_GetDisplayName(display);
    }

    //获取窗口的坐标
    int posX, posY;
    if (SDL_GetWindowPosition(window, &posX, &posY))
    {
        if (bSaveAsAbsoluteCoordinate)
        {
            windowJson["AbsolutePosition"][0] = posX;
            windowJson["AbsolutePosition"][1] = posY;
        }
        else
        {
            SDL_Rect displayRect;
            if (SDL_GetDisplayBounds(display, &displayRect))
            {
                //获取到了显示器范围，存储相对显示器的坐标
                windowJson["PositionInDisplay"][0] = posX-displayRect.x;
                windowJson["PositionInDisplay"][1] = posY- displayRect.y;
            }
            else
            {
                //未获取显示器范围，存储绝对坐标
                windowJson["AbsolutePosition"][0] = posX;
                windowJson["AbsolutePosition"][1] = posY;
            }

        }
    }
    else
    {
        //无法获取窗口坐标时不进行位置保存
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,
            "Can not get window position: %s", SDL_GetWindowTitle(window));

        //无法获取窗口坐标的话，Load时默认将窗口放在显示器中心
        // 无法获取显示器的话，Load时不指定显示器
    }

    //TODO窗口尺寸考虑采用渲染尺寸而不是直接获取窗口尺寸？
    windowJson["Size"][0]= _w;
    windowJson["Size"][1]= _h;
    




    //保存窗口的场景
    windowJson["Scene"] = scene.GenerateAttributes();

    return windowJson;
}

void RenderWindowController::Load(const Json::Value& json)
{
    Shutdown();
    //从json创建窗口
    //先计算窗口位置
    bool hasWindowPos = false;
    SDL_DisplayID displayID = 0;
    int windowPosX, windowPosY;

    //int windowSizeX = 400;
    //int windowSizeY = 300;

    int windowSizeX = 400;
    int windowSizeY = 300;
    if (json.isMember("Size") && json["Size"].isArray()
        && json["Size"][0].isInt() && json["Size"][1].isInt() )
    {
        windowSizeX = json["Size"][0].asInt();
        windowSizeY = json["Size"][1].asInt();
    }
    else
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,
            "Can not read window size, CatTuber will create window in default (400x300).");
    }


    //如果直接存储了绝对坐标
    if (json.isMember("AbsolutePosition") && json["AbsolutePosition"].isArray()
        && json["AbsolutePosition"][0].isInt() && json["AbsolutePosition"][1].isInt()
        )
    {
        hasWindowPos = true;
        windowPosX = json["AbsolutePosition"][0].asInt();
        windowPosY = json["AbsolutePosition"][1].asInt();
    }
    //如果存储了显示器



    if (json.isMember("Display") && json["Display"].isString())
    {
        std::string displayName = json["Display"].asString();
        //先根据显示器名寻找对应的display
        int displayCount = 0;
        SDL_DisplayID* displayArray= SDL_GetDisplays(&displayCount);
       
        for (int i = 0; i < displayCount; i++)
        {
            if (SDL_strcmp(SDL_GetDisplayName(displayArray[displayCount]), displayName.c_str()) == 0)
            {
                displayID = displayArray[displayCount];
                break;
            }
        }
        SDL_free(displayArray);

        //如果存有坐标则用相对坐标计算，如果没存坐标则创建于中心
        if (json.isMember("PositionInDisplay") && json["PositionInDisplay"].isArray()
            && json["PositionInDisplay"][0].isInt() && json["PositionInDisplay"][1].isInt())
        {
            int positionInDisplayX = json["PositionInDisplay"][0].asInt();
            int positionInDisplayY = json["PositionInDisplay"][1].asInt();

            if (displayID)
            {
                //获取显示器坐标
                SDL_Rect displayRect;
                if (SDL_GetDisplayBounds(displayID, &displayRect))
                {
                    hasWindowPos = true;
                    windowPosX = displayRect.x + positionInDisplayX;
                    windowPosY = displayRect.y + positionInDisplayY;
                }
            }
            if (!hasWindowPos)
            {
                hasWindowPos = true;
                windowPosX = positionInDisplayX;
                positionInDisplayY = positionInDisplayY;
            }
        }
    }


    

    if (displayID && !hasWindowPos)
    {
        //有显示器但无坐标的情况，在显示器中心创建窗口
        SDL_Rect displayRect;
        if (SDL_GetDisplayBounds(displayID, &displayRect))
        {
            hasWindowPos = true;
            int centerPointX = displayRect.x + displayRect.w / 2;
            int centerPointY = displayRect.y + displayRect.h / 2;
            
            windowPosX = centerPointX- windowSizeX/2;
            windowPosY = centerPointY - windowSizeY/2;
        }
    }




    targetW = windowSizeX;
    targetH = windowSizeY;
    targetX = hasWindowPos ? windowPosX : SDL_WINDOWPOS_UNDEFINED;
    targetY = hasWindowPos ? windowPosY : SDL_WINDOWPOS_UNDEFINED;



    



   if (!_CreateWindow())
   {
       SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Unable to create window: %s", SDL_GetError());
       throw std::runtime_error(SDL_GetError());
   }

   //创建创建后创建其余各种资源
   if (!SDL_GetWindowSizeInPixels(window,&renderW,&renderH))
   {
       SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Unable to get window px size: %s", SDL_GetError());
       throw std::runtime_error(SDL_GetError());
   }
   if (!ResetGraphic(renderW, renderH))
   {
       SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Unable to init window graphic!");
       throw std::runtime_error("Unable to init window graphic!");
   }



   SetTransparent(isTransparent);
   SetLock(AppSettings::GetIns().GetWindowLock());
   SetTop(AppSettings::GetIns().GetWindowTop());

   //创建Scene
   //QUESTION:涉及到模型文件加载，需要移动到其他线程防止ui卡顿吗？
   if (json.isMember("Scene"))
   {
       scene.ApplyAttributes(json["Scene"]);
   }

}






























//RenderWindowManager
//RenderWindowManager
//RenderWindowManager
//RenderWindowManager
//RenderWindowManager
//RenderWindowManager
//RenderWindowManager
//RenderWindowManager



RenderWindowManager::RenderWindowManager() = default;

RenderWindowManager::~RenderWindowManager() {
    ShutdownAll();
}

bool RenderWindowManager::CreateRenderWindow(const char* title,
    int w, int h, int x, int y)
{
    auto wc = std::make_unique<RenderWindowController>(title, w, h, x, y);

    
    if (!wc->_CreateWindow())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "WindowController Create Failed!");
        return false;
    }

    if (!SDL_GetWindowSizeInPixels(wc->window, &wc->renderW, &wc->renderH))
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Unable to get window px size: %s", SDL_GetError());
        return false;
    }

    if (!wc->ResetGraphic(wc->renderW, wc->renderH)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "WindowController Init Failed");
        return false;
    }
    controllers.emplace_back(std::move(wc));
    return true;
}

void RenderWindowManager::HandleEvent(const SDL_Event& event) {
    //SDL_Event event;
        // 全局退出处理
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        //监控窗口关闭事件
        else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            auto targetWindowID=event.window.windowID;

            auto it = std::find_if(controllers.begin(), controllers.end(),
                [targetWindowID](std::unique_ptr<RenderWindowController>& target) {return target->_GetWindowID() == targetWindowID; });
            
            if (it != controllers.end())
            {
                //// 先销毁渲染器再销毁窗口
                //if ((*it)->renderer) SDL_DestroyRenderer(it->renderer);
                //if ((*it)->window)   SDL_DestroyWindow(it->window);

                //erase会触发析构，所以不使用上方清理代码
                controllers.erase(it);
            }

        }




        // 分发到每个 WindowController
        for (auto& wc : controllers) {
            wc->HandleEvent(event);
        }
}

void RenderWindowManager::RenderAll() {


    //auto cmd=SDL_AcquireGPUCommandBuffer(AppContext::GetGraphicDevice());

    for (auto& wc : controllers) {
        wc->Render(/*cmd*/);
    }

    //SDL_SubmitGPUCommandBuffer(cmd);
}

void RenderWindowManager::PresentAll() {
    //主线程同步

    for (auto& wc : controllers) {
        wc->Present();
    }
    return;
    canStartFrame = false;
    void(*task)(void*) = [](void* data) {
        auto& rm= *((RenderWindowManager*)data);

        for (auto& wc : rm.controllers) {
            wc->Present();
        }
        //告知渲染线程Present完毕 canStartFrame=TRUE
        RenderThread::GetIns().PostTask([](void* userData, uint64_t userData2) {
            auto& rm = *((RenderWindowManager*)userData);
            rm.canStartFrame = true;
            }, &rm);
        };

    while (!UserEvent::PushEvent(UserEvent::TASK, task, this)) { SDL_DelayNS(1); };

}

void RenderWindowManager::UpdateAll(uint64_t deltaTicksNS)
{
    for (auto& wc : controllers) {
        wc->Update(deltaTicksNS);
    }
}

void RenderWindowManager::ShutdownAll() {
    for (auto& wc : controllers) {
        wc->Shutdown();
    }
    controllers.clear();
}

void RenderWindowManager::SetWindowTop(bool b)
{
    for (auto& wc : controllers)
    {
        //懒得转发了
        SDL_SetWindowAlwaysOnTop(wc->window,b);
    }
}

void RenderWindowManager::SetWindowTransparent(bool b)
{
    for (auto& wc : controllers)
    {
        wc->SetTransparent(b);
    }
}

void RenderWindowManager::SetWindowLock(bool b)
{
    for (auto& wc : controllers)
    {
        wc->SetLock(b);
    }
}

//SDL好像不支持窗口透明点击，放在平台相关文件夹中实现
//void RenderWindowManager::SetWindowLock(bool b)
//{
//    for (auto& wc : controllers)
//    {
//        //
//        //sdl_setwino
//        //wc->SetTransparent(b);
//    }
//}

void RenderWindowManager::SetWindowVisible(bool b)
{
    for (auto& wc : controllers)
    {
        if (b)
        {
            SDL_ShowWindow(wc->window);
        }
        else
        {
            SDL_HideWindow(wc->window);
        }
    }
}

void RenderWindowManager::SetWindowBackgroundColor(SDL_Color backgroundColor)
{
    //TODO:锁定渲染线程或者直接发送到渲染线程:

    for (auto& wc : controllers)
    {
        wc->SetClearColor(backgroundColor);
    }



    //_clearColor.r = backgroundColor.r / 255.f;
    //_clearColor.g = backgroundColor.g / 255.f;
    //_clearColor.b = backgroundColor.b / 255.f;
    //_clearColor.a = 0.f;
}

void RenderWindowManager::SetFrameLimit(int frameLimit)
{
    if (frameLimit <=0)
        frameLimit = 999;
    _frameLimit = frameLimit;
}

bool RenderWindowManager::SaveScene(const char* sceneName, bool isQuitSave)
{
    if (!sceneName || SDL_strcmp(sceneName, "") == 0)
    {
        //todo 多语言支持
        sceneName = "Unnamed Scene";
    }


    //先构建文件名
    std::string savePath= AppContext::GetPrefPath();
    savePath = savePath + "Scenes";
    //如果不存在，则创建
    //这个函数会在文件夹已经存在的时候也返回true
    if (!SDL_CreateDirectory(savePath.c_str()))
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Can not Create Dir: %s, %s", savePath.c_str(),SDL_GetError());
        return false;
    }

    //构建文件名以及json内容需要用到的毫秒时间戳
    //当前时间戳字符  //SavedScene_20250721114514666
    SDL_Time time;
    if (!SDL_GetCurrentTime(&time))
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not Get Current Time: %s",  SDL_GetError());
        return false;
    }
    SDL_DateTime date;
    if (!SDL_TimeToDateTime(time, &date, true))
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not Transfer DateTime: %s", SDL_GetError());
        return false;
    }
    //2025+07+21+   11+45+14+ 666 + '\0'
    char dateStr[4 + 2 + 2 + 2 + 2 + 2 + 3 + 1] = {0};
    SDL_snprintf(dateStr,sizeof(dateStr),"%04d%02d%02d%02d%02d%02d%03d",
        date.year,date.month,date.day,date.hour,date.minute,date.second,date.nanosecond/1000000
        );
    
    std::string fileName;
    if (isQuitSave)
        fileName = CATTUBER_SCENE_LAST_SAVED_FILE_NAME;
    else
    {
        fileName = std::string("SavedScene_")+ dateStr;
    }
    

    savePath = savePath + "/" + fileName;

    //构建json
    Json::Value saveJson;
    {
        saveJson["SceneName"] = sceneName;
        saveJson["SavingTime"] = dateStr;
        for (int i = 0; i < controllers.size(); i++)
        {
            saveJson["Windows"][i] = controllers[i]->Save();
        }
    }


    //保存文件
    if (!util::SaveJsonToFile(saveJson, savePath.c_str()))
    {
        SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Save File Failed: %s", savePath.c_str());
        return false;
    }
    return true;
}

bool RenderWindowManager::LoadScene(const char* sceneName, bool isQuitSave)
{
    //从文件载入场景
    if (isQuitSave)
    {
        std::string settingsFilePath = AppContext::GetPrefPath();
        settingsFilePath = settingsFilePath + "Scenes/"+ CATTUBER_SCENE_LAST_SAVED_FILE_NAME;
        Json::Value json=util::BuildJsonFromFile(settingsFilePath.c_str());
        _BuildFromJson(json);
        return true;
    }

    //从手动保存的场景进行载入

    Json::Value sceneJson;
    struct _TemStruct
    {
        Json::Value* pjson;
        const char* sceneName;
    }dataStruct = {&sceneJson ,sceneName };
    
    SDL_EnumerateDirectoryCallback fileCallBack = [](void* userdata, const char* dirname, const char* fname) {

        //只读取后缀名为.scene的文件
        if(!util::IsStringEndsWith(fname,".scene"))
            return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
        


        std::string pathstr = dirname;
        pathstr +=fname;
        Json::Value json = util::BuildJsonFromFile(pathstr.c_str());
        
        //从json中读取必要信息
        _TemStruct* pdata = (_TemStruct*)userdata;
        if (!(json.isMember("SceneName") && json["SceneName"].isString()&& json["SceneName"].asString()== pdata->sceneName))
        {
            //如果不存在SceneName项
            return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
        }

        *(pdata->pjson) = json;
        return SDL_EnumerationResult::SDL_ENUM_SUCCESS;
        };



    //通过文件遍历到目标json
    std::string settingsFilePath = AppContext::GetPrefPath();
    settingsFilePath = settingsFilePath + "Scenes";
    SDL_EnumerateDirectory(settingsFilePath.c_str(), fileCallBack, &dataStruct);

    return  _BuildFromJson(sceneJson);
}

bool RenderWindowManager::_BuildFromJson(const Json::Value& json)
{
    //如果有保存窗口
    if (json.isMember("Windows") && json["Windows"].isArray()&& json["Windows"].size()>0)
    {
        for (auto i = 0u; i < json["Windows"].size(); i++)
        {
            //根据json["Windows"][i]创建窗口
            //不抛出异常？
            auto& window = controllers.emplace_back(std::unique_ptr<RenderWindowController>(new RenderWindowController));
            window->Load(json["Windows"][i]);
        }
    }
    else
    {
        //以默认资源创建1个窗口 
        

        auto& window=controllers.emplace_back(std::unique_ptr<RenderWindowController>(new RenderWindowController));
        
        //默认的json
        Json::Value defaultWindowJson;
        defaultWindowJson["Size"][0] = 400;
        defaultWindowJson["Size"][1] = 300;

        //默认构造一个CatTuber经典场景
        auto& item0 = defaultWindowJson["Scene"]["Items"][0];
        item0["Type"] = "ClassicItem";
        //item0["Detail"]["Table"]; //TableObject::CreateFromAttributes
        item0["Detail"]["Table"]["PackPath"] = "[AppBasePath]/Resources/Table/28kGameKeyboard";
        //TODO/FIXME
        //item0["Detail"]["Character"];
        //item0["Detail"]["HandheldItem"];
        window->Load(defaultWindowJson);
    }


    return false;
}

