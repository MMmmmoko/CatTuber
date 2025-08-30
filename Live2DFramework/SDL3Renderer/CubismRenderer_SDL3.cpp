#include "CubismRenderer_SDL3.hpp"


#include "Math/CubismMatrix44.hpp"
#include "Type/csmVector.hpp"
#include "Model/CubismModel.hpp"
#include "CubismShader_SDL3.hpp"






#pragma once



//------------ LIVE2D NAMESPACE ------------
namespace Live2D {namespace Cubism {namespace Framework { namespace Rendering {

/*********************************************************************************************************************
 *                                      CubismRenderer_SDL
 ********************************************************************************************************************/

    glm::mat4x4 ConvertToGLM(CubismMatrix44& mtx)
    {
        glm::mat4x4 glMat;
        glMat[0][0]= mtx.GetArray()[0];
        glMat[0][1]= mtx.GetArray()[1];
        glMat[0][2]= mtx.GetArray()[2];
        glMat[0][3]= mtx.GetArray()[3];

        glMat[1][0]= mtx.GetArray()[4];
        glMat[1][1]= mtx.GetArray()[5];
        glMat[1][2]= mtx.GetArray()[6];
        glMat[1][3]= mtx.GetArray()[7];

        glMat[2][0]= mtx.GetArray()[8];
        glMat[2][1]= mtx.GetArray()[9];
        glMat[2][2]= mtx.GetArray()[10];
        glMat[2][3]= mtx.GetArray()[11];

        glMat[3][0]= mtx.GetArray()[12];
        glMat[3][1]= mtx.GetArray()[13];
        glMat[3][2]= mtx.GetArray()[14];
        glMat[3][3]= mtx.GetArray()[15];
        return glMat;
    }






 //各种静态变量
namespace
{
    //SDL3渲染状态是管线的一部分，而管线不可变
    //可以设置项为顶点缓冲、索引缓冲、常数缓冲、纹理和采样器、渲染目标、viewport
    //还是可以实现状态管理器，用检索的方式创建pipeline
    CubismRenderState_SDL3* s_renderStateManager = NULL;   ///< 渲染状态管理器
    CubismShader_SDL3* s_shaderManagerInstance = NULL;     ///< 着色器管理器

    csmUint32 s_bufferSetNum = 0;           ///<  缓存Set的数量？  创建上下文的数量，必须在模型加载前设置。

    //ID3D11Device* s_device = NULL;          ///< 使用デバイス。モデルロード前に設定されている必要あり。
    //ID3D11DeviceContext* s_context = NULL;  ///< 使用描画コンテキスト

    SDL_GPUDevice* s_device=NULL;
    CubismRenderContext_SDL3* s_context = NULL;

    csmUint32 s_viewportWidth = 0;          ///< 描画ターゲット幅 CubismRenderer_D3D11::startframeで渡される
    csmUint32 s_viewportHeight = 0;         ///< 描画ターゲット高さ CubismRenderer_D3D11::startframeで渡される
}

#pragma region CLIPPING



void CubismClippingManager_SDL3::SetupClippingContext(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext, CubismModel& model, CubismRenderer_SDL3* renderer, csmInt32 offscreenCurrent)
{
    //准备所有clipping 如果设置了相同的clipping就只设置一次
    csmInt32 usingClipCount = 0;
    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        CubismClippingContext_SDL3* cc= _clippingContextListForMask[clipIndex];

        //使用此剪辑计算围绕整个绘图对象组的矩形
        CalcClippedDrawTotalBounds(model, cc);

        if (cc->_isUsing)
        {
            usingClipCount++; //使用计数增加
        }
    }


    if (usingClipCount <= 0)
    {
        return;
    }

    CubismRenderer_SDL3::GetRenderStateManager()->SetViewport(renderContext,
        0,
        0,
        static_cast<float>(_clippingMaskBufferSize.X),
        static_cast<float>(_clippingMaskBufferSize.Y),
        0.0f, 1.0f);

    _currentMaskBuffer = renderer->GetMaskBuffer(offscreenCurrent, 0);


    _currentMaskBuffer->BeginDraw(renderContext);

    //确定各个mask的layout
    SetupLayoutBounds(usingClipCount);

    //如果数组尺寸与渲染纹理的数量不匹配，则重置
   
    //_clearedMaskBufferFlags.Resize(_renderTextureCount, false);
    // 
    //不是TM凭什么上面这句写法和下面不等效啊？？用上面的写法会导致蒙版出问题
    //草Resize不会全清理为false，只会将新增加的项初始化为false
    if (_clearedMaskBufferFlags.GetSize() != _renderTextureCount)
    {
        _clearedMaskBufferFlags.Clear();

        for (csmInt32 i = 0; i < _renderTextureCount; ++i)
        {
            _clearedMaskBufferFlags.PushBack(false);
        }
    }
    else
    {
        for (csmInt32 i = 0; i < _renderTextureCount; ++i)
        {
            _clearedMaskBufferFlags[i] = false;
        }
    }


    //决定如何布局和绘制所有掩码，并存储在ClipContext和ClippedDrawContext中
    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        CubismClippingContext_SDL3* clipContext = _clippingContextListForMask[clipIndex];
        csmRectF* allClippedDrawRect = clipContext->_allClippedDrawRect; //使用此掩码，所有绘图对象逻辑坐标上的方框矩形
        csmRectF* layoutBoundsOnTex01 = clipContext->_layoutBounds; //この中にマスクを収める
        const csmFloat32 MARGIN = 0.05f;
        const csmBool isRightHanded = true;
        //const csmBool isRightHanded = false;

        //取得clipContext设置的渲染纹理
        CubismOffscreenSurface_SDL3* clipContextRenderTexture = renderer->GetMaskBuffer(offscreenCurrent, clipContext->_bufferIndex);

        //如果当前渲染纹理与clipContext不同
        if (_currentMaskBuffer != clipContextRenderTexture)
        {
            _currentMaskBuffer->EndDraw(renderContext);
            _currentMaskBuffer = clipContextRenderTexture;
            _currentMaskBuffer->BeginDraw(renderContext);
        }

        //使用模型坐标上的矩形进行适当的裕度调整
        _tmpBoundsOnModel.SetRect(allClippedDrawRect);
        _tmpBoundsOnModel.Expand(allClippedDrawRect->Width * MARGIN, allClippedDrawRect->Height * MARGIN);

        //求原本不使用全部分配区域的必要最低限度尺寸的着色器用计算式。如果不考虑旋转，则如下
         // movePeriod' = movePeriod * scaleX + offX [[ movePeriod' = (movePeriod - tmpBoundsOnModel.movePeriod)*scale + layoutBoundsOnTex01.movePeriod ]]

        csmFloat32 scaleX = layoutBoundsOnTex01->Width / _tmpBoundsOnModel.Width;
        csmFloat32 scaleY = layoutBoundsOnTex01->Height / _tmpBoundsOnModel.Height;
        //求生成mask时使用的矩阵
        createMatrixForMask(isRightHanded, layoutBoundsOnTex01, scaleX, scaleY);
        clipContext->_matrixForMask.SetMatrix(_tmpMatrixForMask.GetArray());
        clipContext->_matrixForDraw.SetMatrix(_tmpMatrixForDraw.GetArray());

        const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
        for (csmInt32 i = 0; i < clipDrawCount; i++)
        {
            const csmInt32 clipDrawIndex = clipContext->_clippingIdList[i];

            //顶点无变化就跳过绘制
            if (!model.GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
            {
                continue;
            }

            renderer->IsCulling(model.GetDrawableCulling(clipDrawIndex) != 0);

            //如果不clear mask
            if (!_clearedMaskBufferFlags[clipContext->_bufferIndex])
            {
                // マスクをクリアする
                //（暂定规格）1为无效（未绘制）区域，0为有效（绘制）区域。(用着色器Cd*Cs乘以接近0的值来制作掩码。乘1，什么都不会发生。
                renderer->GetMaskBuffer(offscreenCurrent, clipContext->_bufferIndex)->Clear(renderContext, 1.0f, 1.0f, 1.0f, 1.0f);
                _clearedMaskBufferFlags[clipContext->_bufferIndex] = true;
            }


            renderer->SetClippingContextBufferForMask(clipContext);
            renderer->DrawMeshSDL3(model, clipDrawIndex);
        }

    }

    _currentMaskBuffer->EndDraw(renderContext);
    renderer->SetClippingContextBufferForMask(NULL);




}








CubismClippingContext_SDL3::CubismClippingContext_SDL3(CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>* manager, CubismModel& model, const csmInt32* clippingDrawableIndices, csmInt32 clipCount)
    : CubismClippingContext(clippingDrawableIndices, clipCount)
{
    _isUsing = false;
    _owner = manager;
}
CubismClippingContext_SDL3::~CubismClippingContext_SDL3()
{
}
CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>* CubismClippingContext_SDL3::GetClippingManager()
{
    return _owner;
}
#pragma endregion




//通过这两个方法设置渲染后端
CubismRenderer* CubismRenderer::Create()
{
    return CSM_NEW CubismRenderer_SDL3();
}

void CubismRenderer::StaticRelease()
{
    CubismRenderer_SDL3::DoStaticRelease();
}

CubismRenderState_SDL3* CubismRenderer_SDL3::GetRenderStateManager()
{
    if (s_renderStateManager == NULL)
    {
        s_renderStateManager = CSM_NEW CubismRenderState_SDL3();
    }
    return s_renderStateManager;
}

void CubismRenderer_SDL3::DeleteRenderStateManager()
{
    if (s_renderStateManager)
    {
        CSM_DELETE_SELF(CubismRenderState_SDL3, s_renderStateManager);
        s_renderStateManager = NULL;
    }
}

CubismShader_SDL3* CubismRenderer_SDL3::GetShaderManager()
{
    if (s_shaderManagerInstance == NULL)
    {
        s_shaderManagerInstance = CSM_NEW CubismShader_SDL3();
    }
    return s_shaderManagerInstance;
}

void CubismRenderer_SDL3::DeleteShaderManager()
{
    if (s_shaderManagerInstance)
    {
        CSM_DELETE_SELF(CubismShader_SDL3, s_shaderManagerInstance);
        s_shaderManagerInstance = NULL;
    }
}

void CubismRenderer_SDL3::GenerateShader(SDL_GPUDevice* device)
{
    CubismShader_SDL3* shaderManager = GetShaderManager();
    if (shaderManager)
    {
        shaderManager->GenerateShaders(device);
    }
}

SDL_GPUDevice* CubismRenderer_SDL3::GetCurrentDevice()
{
    return s_device;
}

void CubismRenderer_SDL3::OnDeviceLost()
{
    ReleaseShader();
}
void CubismRenderer_SDL3::ReleaseShader()
{
    CubismShader_SDL3* shaderManager = GetShaderManager();
    if (shaderManager)
    {
        shaderManager->ReleaseShaderProgram();
    }
}


CubismRenderer_SDL3::CubismRenderer_SDL3()
    : _vertexBuffers(NULL)
    , _indexBuffers(NULL)
    , _constantBuffers(NULL)
    , _drawableNum(0)
    , _clippingManager(NULL)
    , _clippingContextBufferForMask(NULL)
    , _clippingContextBufferForDraw(NULL)
{
    _commandBufferNum = 0;
    _commandBufferCurrent = 0;

    _textures.PrepareCapacity(32, true);
}

CubismRenderer_SDL3::~CubismRenderer_SDL3()
{
    {
        // オフスクリーンを作成していたのなら開放
        for (csmUint32 i = 0; i < _offscreenSurfaces.GetSize(); i++)
        {
            for (csmUint32 j = 0; j < _offscreenSurfaces[i].GetSize(); j++)
            {
                _offscreenSurfaces[i][j].DestroyOffscreenSurface();
            }
            _offscreenSurfaces[i].Clear();
        }
        _offscreenSurfaces.Clear();
    }

    const csmInt32 drawableCount = _drawableNum; //GetModel()->GetDrawableCount();

    for (csmUint32 buffer = 0; buffer < _commandBufferNum; buffer++)
    {
        for (csmUint32 drawAssign = 0; drawAssign < drawableCount; drawAssign++)
        {
            if (_constantBuffers[buffer][drawAssign])
            {
                //_constantBuffers[buffer][drawAssign]->Release();
                _constantBuffers[buffer][drawAssign] = NULL;
            }
            if (_indexBuffers[buffer][drawAssign])
            {
                SDL_ReleaseGPUBuffer(s_device,_indexBuffers[buffer][drawAssign]);
                _indexBuffers[buffer][drawAssign] = NULL;
            }
            if (_vertexBuffers[buffer][drawAssign])
            {
                SDL_ReleaseGPUBuffer(s_device, _vertexBuffers[buffer][drawAssign]);
                _vertexBuffers[buffer][drawAssign] = NULL;
            }
        }

        CSM_FREE(_constantBuffers[buffer]);
        CSM_FREE(_indexBuffers[buffer]);
        CSM_FREE(_vertexBuffers[buffer]);
    }

    CSM_FREE(_constantBuffers);
    CSM_FREE(_indexBuffers);
    CSM_FREE(_vertexBuffers);

    CSM_DELETE_SELF(CubismClippingManager_SDL3, _clippingManager);
}

void CubismRenderer_SDL3::DoStaticRelease()
{
    DeleteRenderStateManager();
    DeleteShaderManager();
}




void CubismRenderer_SDL3::Initialize(Framework::CubismModel* model)
{
	Initialize(model,1); 
}

void Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::Initialize(Framework::CubismModel* model, csmInt32 maskBufferCount)
{

    if (s_device == 0)
    {
        CubismLogError("Device has not been set.");
        CSM_ASSERT(0);
        return;
    }


    //至少设置1个maskBuffer
    if (maskBufferCount < 1)
    {
        maskBufferCount = 1;
        CubismLogWarning("The number of render textures must be an integer greater than or equal to 1. Set the number of render textures to 1.");
    }

    if (model->IsUsingMasking())
    {
        _clippingManager = CSM_NEW CubismClippingManager_SDL3();
        _clippingManager->Initialize(*model,maskBufferCount);


        const csmInt32 bufferWidth = _clippingManager->GetClippingMaskBufferSize().X;
        const csmInt32 bufferHeight = _clippingManager->GetClippingMaskBufferSize().Y;

        _offscreenSurfaces.Clear();
        
        
        //确保backbuffer
        for (csmUint32 i = 0; i < s_bufferSetNum; i++)
        {
            csmVector<CubismOffscreenSurface_SDL3> vector;
            _offscreenSurfaces.PushBack(vector);
            for (csmUint32 j = 0; j < maskBufferCount; j++)
            {
                CubismOffscreenSurface_SDL3 offscreenSurface;
                offscreenSurface.CreateOffscreenSurface(s_device, bufferWidth, bufferHeight);
                _offscreenSurfaces[i].PushBack(offscreenSurface);
            }
        }


    }


    _sortedDrawableIndexList.Resize(model->GetDrawableCount(),0);

    //调用父类初始化
    CubismRenderer::Initialize(model, maskBufferCount);


    
    _vertexBuffers = static_cast<SDL_GPUBuffer***>(CSM_MALLOC(sizeof(SDL_GPUBuffer**) * s_bufferSetNum));
    _vertexBuffers_tb = static_cast<SDL_GPUTransferBuffer***>(CSM_MALLOC(sizeof(SDL_GPUTransferBuffer**) * s_bufferSetNum));
    _indexBuffers = static_cast<SDL_GPUBuffer***>(CSM_MALLOC(sizeof(SDL_GPUBuffer**) * s_bufferSetNum));
    _constantBuffers = static_cast<CubismConstantBufferSDL3***>(CSM_MALLOC(sizeof(CubismConstantBufferSDL3**) * s_bufferSetNum));

    
    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    _drawableNum = drawableCount;

    for (csmUint32 buffer = 0; buffer < s_bufferSetNum; buffer++)
    {
        _vertexBuffers[buffer] = static_cast<SDL_GPUBuffer**>(CSM_MALLOC(sizeof(SDL_GPUBuffer*) * drawableCount));
        _vertexBuffers_tb[buffer] = static_cast<SDL_GPUTransferBuffer**>(CSM_MALLOC(sizeof(SDL_GPUTransferBuffer*) * drawableCount));
        _indexBuffers[buffer] = static_cast<SDL_GPUBuffer**>(CSM_MALLOC(sizeof(SDL_GPUBuffer*) * drawableCount));
        _constantBuffers[buffer] = static_cast<CubismConstantBufferSDL3**>(CSM_MALLOC(sizeof(CubismConstantBufferSDL3*) * drawableCount));



        for (csmUint32 drawAssign = 0; drawAssign < drawableCount; drawAssign++)
        {
            
            const  csmInt32 vcount = GetModel()->GetDrawableVertexCount(drawAssign);

            if (vcount != 0)
            {
                //如果绘制对象有顶点，为其创建顶点缓存

                SDL_GPUBufferCreateInfo bufferDesc = {};
                bufferDesc.usage= SDL_GPU_BUFFERUSAGE_VERTEX;
                bufferDesc.size = sizeof(CubismVertexSDL3)*vcount;

                SDL_GPUBuffer* pbuffer = SDL_CreateGPUBuffer(s_device,&bufferDesc);
                if (!pbuffer)
                {
                    CubismLogError("Vertexbuffer create failed : %d", vcount);

                    _vertexBuffers_tb[buffer][drawAssign] = NULL;
                }
                else
                {
                    //缓存有效，创建transferbuffer
                    SDL_GPUTransferBufferCreateInfo createInfo = {};
                    createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
                    createInfo.size = sizeof(CubismVertexSDL3) * vcount;
                    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(s_device, &createInfo);
                    _vertexBuffers_tb[buffer][drawAssign] = transferBuffer;
                }

                _vertexBuffers[buffer][drawAssign] = pbuffer;
            }
            else
            {
                _vertexBuffers[buffer][drawAssign] = NULL;
                _vertexBuffers_tb[buffer][drawAssign] = NULL;
            }







            //创建对象的顶点缓存
            _indexBuffers[buffer][drawAssign] = NULL;
            const csmInt32 icount = GetModel()->GetDrawableVertexIndexCount(drawAssign);
            if (icount != 0)
            {
                SDL_GPUBufferCreateInfo bufferDesc = {};
                bufferDesc.usage = SDL_GPU_BUFFERUSAGE_INDEX;
                bufferDesc.size = sizeof(uint16_t) * icount;
                
                SDL_GPUBuffer* pbuffer = SDL_CreateGPUBuffer(s_device, &bufferDesc);
                if (!pbuffer)
                {
                    CubismLogError("Vertexbuffer create failed : %d", vcount);
                }
                else
                {
                    SDL_GPUTransferBufferCreateInfo createInfo = {};
                    createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
                    createInfo.size = sizeof(uint16_t) * icount;

                    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(s_device, &createInfo);

                    // 2. 映射并写入数据
                    void* mapped = SDL_MapGPUTransferBuffer(s_device, transferBuffer, false);
                    memcpy(mapped, GetModel()->GetDrawableVertexIndices(drawAssign), createInfo.size);
                    SDL_UnmapGPUTransferBuffer(s_device, transferBuffer);


                    // 3. 创建命令缓冲区并开始 CopyPass
                    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_device);
                    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


                    SDL_GPUTransferBufferLocation bufferLocation = {};
                    bufferLocation.transfer_buffer = transferBuffer;
                    bufferLocation.offset = 0;

                    SDL_GPUBufferRegion bufferRegion = {};
                    bufferRegion.buffer = pbuffer;
                    bufferRegion.size= sizeof(uint16_t) * icount;


                    SDL_UploadToGPUBuffer(copyPass,&bufferLocation,&bufferRegion,false);

                    // 5. 结束并提交
                    SDL_EndGPUCopyPass(copyPass);
                    SDL_SubmitGPUCommandBuffer(cmd);
                    // 6. 释放 TransferBuffer
                    SDL_ReleaseGPUTransferBuffer(s_device, transferBuffer);

                    _indexBuffers[buffer][drawAssign] = pbuffer;
                }
            }

            

            _constantBuffers[buffer][drawAssign] = NULL;
            {
                _constantBuffers[buffer][drawAssign] =(CubismConstantBufferSDL3*)SDL_malloc(sizeof(CubismConstantBufferSDL3));
            }
            

            //CONSTANT BUFFER(uniform data)直接用push的形式传输？？
            // SDL_PushGPUFragmentUniformData
            // 
            // 
            //_constantBuffers[buffer][drawAssign] = NULL;
            //{
            //    SDL_GPUBufferCreateInfo createInfo = {};
            //    createInfo.size = sizeof(CubismConstantBufferSDL3); // 你的 uniform 数据结构大小
            //    createInfo.usage = sdl_gpu_uniform;

            //    sdl_gpu_


            //    D3D11_BUFFER_DESC bufferDesc;
            //    memset(&bufferDesc, 0, sizeof(bufferDesc));
            //    bufferDesc.ByteWidth = sizeof(CubismConstantBufferD3D11);    // 総長 構造体サイズ*個数
            //    bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 定数バッファに関しては「Map用にDynamic」にしなくともよい
            //    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            //    bufferDesc.CPUAccessFlags = 0;
            //    bufferDesc.MiscFlags = 0;
            //    bufferDesc.StructureByteStride = 0;

            //    if (FAILED(s_device->CreateBuffer(&bufferDesc, NULL, &_constantBuffers[buffer][drawAssign])))
            //    {
            //        CubismLogError("ConstantBuffers create failed");
            //    }
            //}







        }
    }



    _commandBufferNum = s_bufferSetNum;
    _commandBufferCurrent = 0;
}



void CubismRenderer_SDL3::PreDraw()
{
    SetDefaultRenderState();
}

void CubismRenderer_SDL3::PostDraw()
{
    // ダブル・トリプルバッファを回す
    _commandBufferCurrent++;
    if (_commandBufferNum <= _commandBufferCurrent)
    {
        _commandBufferCurrent = 0;
    }
}


void CubismRenderer_SDL3::DoDrawModel()
{

    CSM_ASSERT(s_device != NULL);


    PreDraw();//SetDefaultRenderstates

    //在使用裁剪蒙版·缓冲区预处理方式的情况下
    if (_clippingManager != NULL)
    {
        //如果大小不同，在此处重新创建
        for (csmInt32 i = 0; i < _clippingManager->GetRenderTextureCount(); ++i)
        {
            if (_offscreenSurfaces[_commandBufferCurrent][i].GetBufferWidth() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X) ||
                _offscreenSurfaces[_commandBufferCurrent][i].GetBufferHeight() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y))
            {
                _offscreenSurfaces[_commandBufferCurrent][i].CreateOffscreenSurface(s_device,
                    static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X), static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y));
            }
        }
        //是否使用高精度蒙版
        if (IsUsingHighPrecisionMask())
        {
            _clippingManager->SetupMatrixForHighPrecision(*GetModel(), true);

          
        }
        else
        {
            //??为什么使用高精蒙版就不需要设置裁剪上下文？
            _clippingManager->SetupClippingContext(s_device, s_context, *GetModel(), this, _commandBufferCurrent);
        }

        if (!IsUsingHighPrecisionMask())
        {
            //恢复viewport
            GetRenderStateManager()->SetViewport(s_context,
                0.0f,
                0.0f,
                static_cast<float>(s_viewportWidth),
                static_cast<float>(s_viewportHeight),
                0.0f, 1.0f);
        }
    }


    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    const csmInt32* renderOrder = GetModel()->GetDrawableRenderOrders();

    //将索引按照各自的描画顺序进行排序
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 order = renderOrder[i];
        _sortedDrawableIndexList[order] = i;
    }



    //绘制
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 drawableIndex = _sortedDrawableIndexList[i];

        //如果Drawable不可见，则跳过绘制
        if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
        {
            continue;
        }


        //设置剪贴蒙版
        CubismClippingContext_SDL3* clipContext = (_clippingManager != NULL)
            ? (*_clippingManager->GetClippingContextListForDraw())[drawableIndex]
            : NULL;

        if (clipContext != NULL && IsUsingHighPrecisionMask()) //绘制蒙版
        {
            if (clipContext->_isUsing) //
            {
                CubismRenderer_SDL3::GetRenderStateManager()->SetViewport(s_context,
                    0,
                    0,
                    static_cast<float>(_clippingManager->GetClippingMaskBufferSize().X),
                    static_cast<float>(_clippingManager->GetClippingMaskBufferSize().Y),
                    0.0f, 1.0f);

                //调用具有正确渲染目标的离屏表面缓冲区
                CubismOffscreenSurface_SDL3* currentHighPrecisionMaskColorBuffer = &_offscreenSurfaces[_commandBufferCurrent][clipContext->_bufferIndex];

                currentHighPrecisionMaskColorBuffer->BeginDraw(s_context);
                currentHighPrecisionMaskColorBuffer->Clear(s_context, 1.0f, 1.0f, 1.0f, 1.0f);

                const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                for (csmInt32 ctx = 0; ctx < clipDrawCount; ctx++)
                {
                    const csmInt32 clipDrawIndex = clipContext->_clippingIdList[ctx];

                    // 如果顶点信息未更新，则跳过绘制
                    if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
                    {
                        continue;
                    }

                    //是否裁剪背面（单面绘制）
                    IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0);

                    // 今回専用の変換を適用して描く
                    // 频道也需要切换(A,R,G,B)
                    SetClippingContextBufferForMask(clipContext);
                    DrawMeshSDL3(*GetModel(), clipDrawIndex);
                }

                {
                    //后处理
                    currentHighPrecisionMaskColorBuffer->EndDraw(s_context);
                    SetClippingContextBufferForMask(NULL);

                    // 恢复viewport
                    GetRenderStateManager()->SetViewport(s_context,
                        0.0f,
                        0.0f,
                        static_cast<float>(s_viewportWidth),
                        static_cast<float>(s_viewportHeight),
                        0.0f, 1.0f);

                    PreDraw(); // バッファをクリアする
                }
            }
        }

        // クリッピングマスクをセットする
        SetClippingContextBufferForDraw(clipContext);

        IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0);

        DrawMeshSDL3(*GetModel(), drawableIndex);
    }

    PostDraw();
}





void CubismRenderer_SDL3::ExecuteDrawForMask(const CubismModel& model, const csmInt32 index)
{
    CubismShader_SDL3* shaderManager = Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::GetShaderManager();
    if (!shaderManager)
    {
        return;
    }

    SDL_GPUShader* vs = shaderManager->GetVertexShader(ShaderNames_SetupMask);
    if (vs == NULL)
    {
        return;
    }
    SDL_GPUShader* ps = shaderManager->GetFragmentShader(ShaderNames_SetupMask);
    if (ps == NULL)
    {
        return;
    }

    //*****************
//*****************
//这里逻辑和D3d11的example不一致，添加了SDL3 RenderContext的StartRender和EndRender
    s_context->StartRender();
    //*****************
    //*****************

    //纹理和采样器
    SetSamplerAccordingToAnisotropy();//根据各向异性数值设置采样器
    SetTextureView(model, index);

    //着色器
    s_context->SetVertexShader(vs);
    s_context->SetFragmentShader(ps);


    //mask用的RenderState
    GetRenderStateManager()->SetBlend(s_context,
        CubismRenderState_SDL3::Blend_Mask,
        glm::vec4(0, 0, 0, 0),
        0xffffffff);


    //常数缓存（uniform data）
    {
        CubismConstantBufferSDL3 cb;
        memset(&cb, 0, sizeof(cb));

        //设置要使用的颜色通道
        CubismClippingContext_SDL3* contextBuffer = GetClippingContextBufferForMask();
        SetColorChannel(cb, contextBuffer);

        //颜色
        csmRectF* rect = GetClippingContextBufferForMask()->_layoutBounds;
        CubismTextureColor baseColor = { rect->X * 2.0f - 1.0f, rect->Y * 2.0f - 1.0f, rect->GetRight() * 2.0f - 1.0f, rect->GetBottom() * 2.0f - 1.0f };
        CubismTextureColor multiplyColor = model.GetMultiplyColor(index);
        CubismTextureColor screenColor = model.GetScreenColor(index);
        SetColorConstantBuffer(cb, model, index, baseColor, multiplyColor, screenColor);

        // proj矩阵
        SetProjectionMatrix(cb, GetClippingContextBufferForMask()->_matrixForMask);

        // Update
        UpdateConstantBuffer(cb, index);
    }

    //设置顶点几何布局为TRIANGLELIST
    s_context->SetTopology(SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
    DrawDrawableIndexed(model, index);

    s_context->EndRender();
}

void CubismRenderer_SDL3::ExecuteDrawForDraw(const CubismModel& model, const csmInt32 index)
{
    CubismShader_SDL3* shaderManager = Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::GetShaderManager();
    if (!shaderManager)
    {
        return;
    }
    //*****************
//*****************
//这里逻辑和D3d11的example不一致，添加了SDL3 RenderContext的StartRender和EndRender
    s_context->StartRender();
    //*****************
    //*****************


    //纹理和采样器
    SetSamplerAccordingToAnisotropy();//根据各向异性数值设置采样器
    SetTextureView(model, index);

    //SDL3不能像D3d11那样灵活设置着色器，而应该根据着色器组合创建多个渲染管线，然后按需绑定资源到不同管线
    if (!SetShader(model, index))
    {
        return;
    }



    //BlendMode
    {
        CubismBlendMode colorBlendMode = model.GetDrawableBlendMode(index);
        SetBlendState(colorBlendMode);
    }

    //常数缓存（uniform data）
    {
        CubismConstantBufferSDL3 cb;
        memset(&cb, 0, sizeof(cb));


        const csmBool masked = GetClippingContextBufferForDraw() != NULL;
        if (masked)
        {
            //设置矩阵以将View坐标转换为ClippingContext坐标
            cb.clipMatrix = glm::transpose( ConvertToGLM(GetClippingContextBufferForDraw()->_matrixForDraw));

            //设置要使用的颜色通道
            CubismClippingContext_SDL3* contextBuffer = GetClippingContextBufferForDraw();
            SetColorChannel(cb, contextBuffer);
        }

        // 色
        CubismTextureColor baseColor = GetModelColorWithOpacity(model.GetDrawableOpacity(index));
        CubismTextureColor multiplyColor = model.GetMultiplyColor(index);
        CubismTextureColor screenColor = model.GetScreenColor(index);
        SetColorConstantBuffer(cb, model, index, baseColor, multiplyColor, screenColor);


        SetProjectionMatrix(cb, GetMvpMatrix());
        UpdateConstantBuffer(cb, index);

    }


    //设置顶点几何布局为TRIANGLELIST
    s_context->SetTopology(SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
    DrawDrawableIndexed(model, index);



    s_context->EndRender();

}


void CubismRenderer_SDL3::DrawDrawableIndexed(const CubismModel& model, const csmInt32 index)
{
    uint32_t offsets = 0;
    SDL_GPUBuffer* vertexBuffer = _vertexBuffers[_commandBufferCurrent][index];
    SDL_GPUBuffer* indexBuffer = _indexBuffers[_commandBufferCurrent][index];
    const csmInt32 indexCount = model.GetDrawableVertexIndexCount(index);

    SDL_GPUBufferBinding vbufferbinding;
    vbufferbinding.buffer = vertexBuffer;
    vbufferbinding.offset = 0;
    s_context->SetVertexBuffers(0, 1, &vbufferbinding);


    s_context->SetIndexBuffer(indexBuffer, SDL_GPUIndexElementSize::SDL_GPU_INDEXELEMENTSIZE_16BIT,0);


    s_context->DrawIndexed(indexCount, 0);
    

}





void CubismRenderer_SDL3::DrawMeshSDL3(const CubismModel& model, const csmInt32 index)
{
    if (s_device == NULL) 
        return;

    //空层
    if (model.GetDrawableVertexIndexCount(index) == 0)
        return;

    //不需要绘制的情况
    if (model.GetDrawableOpacity(index) <= 0.0f && !IsGeneratingMask())
        return;

    //如果模型引用的纹理未绑定，则跳过绘制
    if (GetTextureViewWithIndex(model, index) == NULL)
        return;

    //设置是否裁剪背面
    GetRenderStateManager()->SetCullMode(s_context, (IsCulling() ? CubismRenderState_SDL3::Cull_Ccw : CubismRenderState_SDL3::Cull_None));

     //更新顶点数据
    {
        const csmInt32 drawableIndex = index;
        const csmInt32 vertexCount = model.GetDrawableVertexCount(index);
        const csmFloat32* vertexArray = model.GetDrawableVertices(index);
        const csmFloat32* uvArray = reinterpret_cast<const csmFloat32*>(model.GetDrawableVertexUvs(index));
        CopyToBuffer(s_context, drawableIndex, vertexCount, vertexArray, uvArray);
    }

    //设置着色器并绘制
    if (IsGeneratingMask())
    {
        ExecuteDrawForMask(model, index);
    }
    else
    {
        ExecuteDrawForDraw(model, index);
    }


    SetClippingContextBufferForDraw(NULL);
    SetClippingContextBufferForMask(NULL);
}


void CubismRenderer_SDL3::SaveProfile()
{
    // NULLは許されず
    CSM_ASSERT(s_device != NULL);
    CSM_ASSERT(s_context != NULL);

    // 現在のレンダリングステートをPush
    //GetRenderStateManager()->SaveCurrentNativeState(s_device, s_context);
}
void CubismRenderer_SDL3::RestoreProfile()
{
    // NULLは許されず
    CSM_ASSERT(s_device != NULL);
    CSM_ASSERT(s_context != NULL);

    // SaveCurrentNativeStateと対
    //GetRenderStateManager()->RestoreNativeState(s_device, s_context);
}

void CubismRenderer_SDL3::BindTexture(csmUint32 modelTextureAssign, SDL_GPUTexture* textureView)
{
    _textures[modelTextureAssign] = textureView;
}
const csmMap<csmInt32, SDL_GPUTexture*>& CubismRenderer_SDL3::GetBindedTextures() const
{
    return _textures;
}


void CubismRenderer_SDL3::SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height)
{
    if (_clippingManager == NULL)
    {
        return;
    }
    //在实例销毁前保存渲染纹理的数量
    const csmInt32 renderTextureCount = _clippingManager->GetRenderTextureCount();

    //为了更改离屏表面（OffscreenSurface）的尺寸，需要销毁并重新创建实例。
    CSM_DELETE_SELF(CubismClippingManager_SDL3, _clippingManager);
    _clippingManager = CSM_NEW CubismClippingManager_SDL3();
    _clippingManager->SetClippingMaskBufferSize(width,height);
    _clippingManager->Initialize(*GetModel(),renderTextureCount);
}
csmInt32 CubismRenderer_SDL3::GetRenderTextureCount() const
{
    return _clippingManager->GetRenderTextureCount();
}

CubismVector2 CubismRenderer_SDL3::GetClippingMaskBufferSize() const
{
    return _clippingManager->GetClippingMaskBufferSize();
}

CubismOffscreenSurface_SDL3* CubismRenderer_SDL3::GetMaskBuffer(csmUint32 backbufferNum, csmInt32 offscreenIndex)
{
    return &_offscreenSurfaces[backbufferNum][offscreenIndex];
}

void CubismRenderer_SDL3::InitializeConstantSettings(csmUint32 bufferSetNum, SDL_GPUDevice* device)
{
    s_bufferSetNum = bufferSetNum;
    s_device = device;
    //Get会创建实例，预先创建
    CubismRenderer_SDL3::GetRenderStateManager();
}

void CubismRenderer_SDL3::SetDefaultRenderState()
{
    //不使用深度，由绘图缓存控制
    GetRenderStateManager()->SetZEnable(s_context,
        CubismRenderState_SDL3::Depth_Disable,
        0);

    GetRenderStateManager()->SetViewport(s_context,
        0.0f,
        0.0f,
        static_cast<float>(s_viewportWidth),
        static_cast<float>(s_viewportHeight),
        0.0f, 1.0f);
}



void CubismRenderer_SDL3::StartFrame(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext, csmUint32 viewportWidth, csmUint32 viewportHeight)
{
    //这tama是开始一帧还是开始这个框架啊
    //当前帧中使用的设备
    s_device = device;
    s_context = renderContext;
    s_viewportWidth = viewportWidth;
    s_viewportHeight = viewportHeight;

    GetRenderStateManager()->StartFrame();
    GetRenderStateManager()->SetViewport(renderContext,0,0, viewportWidth,viewportHeight,0.f,1.f);
    GetShaderManager()->SetupShader(s_device, s_context);
}
void CubismRenderer_SDL3::EndFrame(SDL_GPUDevice* device)
{
}
void CubismRenderer_SDL3::SetClippingContextBufferForDraw(CubismClippingContext_SDL3* clip)
{
    _clippingContextBufferForDraw = clip;
}
CubismClippingContext_SDL3* CubismRenderer_SDL3::GetClippingContextBufferForDraw() const
{
    return _clippingContextBufferForDraw;
}

void CubismRenderer_SDL3::SetClippingContextBufferForMask(CubismClippingContext_SDL3* clip)
{
    _clippingContextBufferForMask = clip;
}
CubismClippingContext_SDL3* CubismRenderer_SDL3::GetClippingContextBufferForMask() const
{
    return _clippingContextBufferForMask;
}

void CubismRenderer_SDL3::CopyToBuffer(CubismRenderContext_SDL3* renderContext, csmInt32 drawAssign, const csmInt32 vcount, const csmFloat32* varray, const csmFloat32* uvarray)
{
    //将指定绘制对象的顶点更新进顶点缓存中

    if (_vertexBuffers[_commandBufferCurrent][drawAssign])
    {
        SDL_GPUTransferBuffer* transferBuffer = _vertexBuffers_tb[_commandBufferCurrent][drawAssign];


        // 映射并复制数据
        void* mapped = SDL_MapGPUTransferBuffer(renderContext->GetDevice(), transferBuffer, false);

        if (mapped)
        {
            CubismVertexSDL3* lockPointer = reinterpret_cast<CubismVertexSDL3*>(mapped);
            for (csmInt32 ct = 0; ct < vcount * 2; ct += 2)
            {// モデルデータからのコピー
                lockPointer[ct / 2].x = varray[ct + 0];
                lockPointer[ct / 2].y = varray[ct + 1];
                lockPointer[ct / 2].u = uvarray[ct + 0];
                lockPointer[ct / 2].v = uvarray[ct + 1];
            }

            SDL_UnmapGPUTransferBuffer(renderContext->GetDevice(), transferBuffer);

            // 创建命令缓冲区并开始复制过程
            SDL_GPUCommandBuffer* cmd = renderContext->GetCommandBuffer();
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
            
            SDL_GPUTransferBufferLocation bufferLocation = { transferBuffer,0};
            SDL_GPUBufferRegion bufferRegion;
            bufferRegion.buffer = _vertexBuffers[_commandBufferCurrent][drawAssign];
            bufferRegion.offset = 0;
            bufferRegion.size = sizeof(CubismVertexSDL3) * vcount;

            SDL_UploadToGPUBuffer(copyPass,&bufferLocation,&bufferRegion,false);

            SDL_EndGPUCopyPass(copyPass);

            //不要单独开启CommandBufer并Submit，非常耗时
            //这里修改后一下子帧率从40上升到400了...不过离Live2D的D3D11还有差距
            //SDL_SubmitGPUCommandBuffer(cmd);



        }



    }
}

SDL_GPUTexture* CubismRenderer_SDL3::GetTextureViewWithIndex(const CubismModel& model, const csmInt32 index)
{
    SDL_GPUTexture* result = NULL;
    const csmInt32 textureIndex = model.GetDrawableTextureIndex(index);
    if (textureIndex >= 0)
    {
        result = _textures[textureIndex];
    }
    



    return result;
}

void CubismRenderer_SDL3::SetBlendState(const CubismBlendMode blendMode)
{
    switch (blendMode)
    {
    case CubismRenderer::CubismBlendMode::CubismBlendMode_Normal:
    default:
        GetRenderStateManager()->SetBlend(s_context,
            CubismRenderState_SDL3::Blend_Normal,
            glm::vec4(0, 0, 0, 0),
            0xffffffff);
        break;

    case CubismRenderer::CubismBlendMode::CubismBlendMode_Additive:
        GetRenderStateManager()->SetBlend(s_context,
            CubismRenderState_SDL3::Blend_Add,
            glm::vec4(0, 0, 0, 0),
            0xffffffff);
        break;

    case CubismRenderer::CubismBlendMode::CubismBlendMode_Multiplicative:
        GetRenderStateManager()->SetBlend(s_context,
            CubismRenderState_SDL3::Blend_Mult,
            glm::vec4(0, 0, 0, 0),
            0xffffffff);
        break;
    }
}

Csm::csmBool CubismRenderer_SDL3::SetShader(const CubismModel& model, const csmInt32 index)
{
    //根据状态设置着色器
    const csmBool masked = GetClippingContextBufferForDraw() != NULL;
    const csmBool premult = IsPremultipliedAlpha();
    const csmBool invertedMask = model.GetDrawableInvertedMask(index);

    const ShaderNames vertexShaderNames = (masked ?
        ShaderNames_NormalMasked :
        ShaderNames_Normal);
    ShaderNames pixelShaderNames;
    if (masked)
    {
        if (premult)
        {
            if (invertedMask)
            {
                pixelShaderNames = ShaderNames_NormalMaskedInvertedPremultipliedAlpha;
            }
            else
            {
                pixelShaderNames = ShaderNames_NormalMaskedPremultipliedAlpha;
            }
        }
        else
        {
            if (invertedMask)
            {
                pixelShaderNames = ShaderNames_NormalMaskedInverted;
            }
            else
            {
                pixelShaderNames = ShaderNames_NormalMasked;
            }
        }
    }
    else
    {
        if (premult)
        {
            pixelShaderNames = ShaderNames_NormalPremultipliedAlpha;
        }
        else
        {
            pixelShaderNames = ShaderNames_Normal;
        }
    }

    CubismShader_SDL3* shaderManager = Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::GetShaderManager();
    SDL_GPUShader* vs = shaderManager->GetVertexShader(vertexShaderNames);
    if (vs == NULL)
    {
        return false;
    }
    SDL_GPUShader* ps = shaderManager->GetFragmentShader(pixelShaderNames);
    if (ps == NULL)
    {
        return false;
    }

    s_context->SetVertexShader(vs);
    s_context->SetFragmentShader(ps);
    return true;
}

void CubismRenderer_SDL3::SetTextureView(const CubismModel& model, const csmInt32 index)
{
    //根据状态设置着色器纹理资源
    const csmBool masked = GetClippingContextBufferForDraw() != NULL;
    const csmBool drawing = !IsGeneratingMask();

    SDL_GPUTexture* textureView = GetTextureViewWithIndex(model, index);
    SDL_GPUTexture* maskView = (masked && drawing ? _offscreenSurfaces[_commandBufferCurrent][GetClippingContextBufferForDraw()->_bufferIndex].GetTextureView() : NULL);
    SDL_GPUTextureSamplerBinding viewArray[2] = { 
        {textureView,textureView ? s_context->GetFragmentSampler():NULL },
        maskView ,maskView ? s_context->GetFragmentSampler() : NULL
    };
    uint32_t numViews = 0;
    if (textureView)
    {
        if (maskView)
        {
            numViews = 2;
        }
        else
        {
            numViews = 1;
        }
    }
    //s_context->SetFragmentTextureResources(0,2, viewArray);
    s_context->SetFragmentTextureResources(0, numViews, viewArray);
    
}

void CubismRenderer_SDL3::SetColorConstantBuffer(CubismConstantBufferSDL3& cb, const CubismModel& model, const csmInt32 index,
    CubismTextureColor& baseColor, CubismTextureColor& multiplyColor, CubismTextureColor& screenColor)
{
    cb.baseColor = glm::vec4(baseColor.R, baseColor.G, baseColor.B, baseColor.A);
    cb.multiplyColor = glm::vec4(multiplyColor.R, multiplyColor.G, multiplyColor.B, multiplyColor.A);
    cb.screenColor = glm::vec4(screenColor.R, screenColor.G, screenColor.B, screenColor.A);
}
void CubismRenderer_SDL3::SetColorChannel(CubismConstantBufferSDL3& cb, CubismClippingContext_SDL3* contextBuffer)
{
    const csmInt32 channelIndex = contextBuffer->_layoutChannelIndex;
    CubismRenderer::CubismTextureColor* colorChannel = contextBuffer->GetClippingManager()->GetChannelFlagAsColor(channelIndex);
    cb.channelFlag= glm::vec4(colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A);
}
void CubismRenderer_SDL3::SetProjectionMatrix(CubismConstantBufferSDL3& cb, CubismMatrix44 matrix)
{
   cb.projectMatrix = glm::transpose(ConvertToGLM(matrix));
}

void CubismRenderer_SDL3::UpdateConstantBuffer(CubismConstantBufferSDL3& cb, csmInt32 index)
{
    //_constantBuffers[_commandBufferCurrent][index];
    //传入参数那我直接上传给SDL就得了
    s_context->SetVertexUniformData(0, &cb, sizeof(cb));
    s_context->SetFragmentUniformData(0,&cb,sizeof(cb));
}

void CubismRenderer_SDL3::SetSamplerAccordingToAnisotropy()
{
    if (GetAnisotropy() >= 1.0f)
    {
        GetRenderStateManager()->SetSampler(s_context, CubismRenderState_SDL3::Sampler_Anisotropy, GetAnisotropy(), s_device);
    }
    else
    {
        GetRenderStateManager()->SetSampler(s_context, CubismRenderState_SDL3::Sampler_Normal);
    }
}

const csmBool inline CubismRenderer_SDL3::IsGeneratingMask() const
{
    return (GetClippingContextBufferForMask() != NULL);
}




}
}
}
}

//------------ LIVE2D NAMESPACE ------------
