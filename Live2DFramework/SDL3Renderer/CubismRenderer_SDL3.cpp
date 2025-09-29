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

    CubismMatrix44 ConvertToCsmMat(glm::mat4x4& glMat)
    {
        CubismMatrix44 mtx;
		mtx.GetArray()[0] = glMat[0][0];
		mtx.GetArray()[1] = glMat[0][1];
		mtx.GetArray()[2] = glMat[0][2];
		mtx.GetArray()[3] = glMat[0][3];

		mtx.GetArray()[4] = glMat[1][0];
		mtx.GetArray()[5] = glMat[1][1];
		mtx.GetArray()[6] = glMat[1][2];
		mtx.GetArray()[7] = glMat[1][3];

		mtx.GetArray()[8] = glMat[2][0];
		mtx.GetArray()[9] = glMat[2][1];
		mtx.GetArray()[10] = glMat[2][2];
		mtx.GetArray()[11] = glMat[2][3];

		mtx.GetArray()[12] = glMat[3][0];
		mtx.GetArray()[13] = glMat[3][1];
		mtx.GetArray()[14] = glMat[3][2];
		mtx.GetArray()[15] = glMat[3][3];
        return mtx;
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


//如果常数缓存为空则创建常数缓存
//因为不是所有图层都使用常数缓存，所以这样减少创建量
inline void CreateConstantBufferIfNull(SDL_GPUBuffer*& cbuffer, SDL_GPUTransferBuffer*& cbuffer_tb)
{
    if (!cbuffer)
    {
        {
            SDL_GPUBufferCreateInfo bufferDesc = {};
            bufferDesc.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
            bufferDesc.size = sizeof(CubismConstantBufferSDL3);
            auto pbuffer = SDL_CreateGPUBuffer(s_device, &bufferDesc);
            if (!pbuffer)
            {
                CubismLogError("ConstantBuffer create failed : %s", SDL_GetError());
            }
            else
            {
                //缓存有效，创建transferbuffer
                cbuffer = pbuffer;

                SDL_GPUTransferBufferCreateInfo createInfo = {};
                createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
                createInfo.size = sizeof(CubismConstantBufferSDL3);
                SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(s_device, &createInfo);
                cbuffer_tb = transferBuffer;

            }
        }
    }
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
    , _constantBuffersForDraw(NULL)
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
            if (_constantBuffersForDraw[buffer][drawAssign])
            {
                SDL_ReleaseGPUBuffer(s_device, _constantBuffersForDraw[buffer][drawAssign]);
                _constantBuffersForDraw[buffer][drawAssign] = NULL;
            }
            //if (_constantBuffers_tb[buffer][drawAssign])
            //{
            //    SDL_ReleaseGPUTransferBuffer(s_device, _constantBuffersForMask_tb[buffer][drawAssign]);
            //    _constantBuffersForMask_tb[buffer][drawAssign] = NULL;
            //}
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
            if (_vertexBuffers_tb[buffer][drawAssign])
            {
                SDL_ReleaseGPUTransferBuffer(s_device, _vertexBuffers_tb[buffer][drawAssign]);
                _vertexBuffers_tb[buffer][drawAssign] = NULL;
            }

            

        }
        for (auto x : _constantBuffersForDraw_tb)
        {
            SDL_ReleaseGPUTransferBuffer(s_device, x);
        }
        _constantBuffersForDraw_tb.clear();

        for (auto& bufferVec : _constantBuffersForMask)
        {
            //for (auto x : bufferVec)
            //{
            //    SDL_ReleaseGPUBuffer(s_device,x );
            //}
            SDL_ReleaseGPUBuffer(s_device, bufferVec);
        }

        for(auto& transferVec: _constantBuffersForMask_tb)
        for (auto x : transferVec)
        {
            SDL_ReleaseGPUTransferBuffer(s_device,x);
        }
        _constantBuffersForMask_tb.clear();

        _constantBuffersForMask_tb_index.clear();
        CSM_FREE(_constantBuffersForDraw[buffer]);
        CSM_FREE(_indexBuffers[buffer]);
        CSM_FREE(_vertexBuffers[buffer]);
        CSM_FREE(_vertexBuffers_tb[buffer]);
    }

    CSM_FREE(_constantBuffersForDraw);
    CSM_FREE(_indexBuffers);
    CSM_FREE(_vertexBuffers);
    CSM_FREE(_vertexBuffers_tb);

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
        //for (csmUint32 i = 0; i < s_bufferSetNum; i++)
        //{
        //    csmVector<CubismOffscreenSurface_SDL3> vector;
        //    _offscreenSurfaces.PushBack(vector);
        //    for (csmUint32 j = 0; j < maskBufferCount; j++)
        //    {
        //        CubismOffscreenSurface_SDL3 offscreenSurface;
        //        offscreenSurface.CreateOffscreenSurface(s_device, bufferWidth, bufferHeight);
        //        _offscreenSurfaces[i].PushBack(offscreenSurface);
        //    }
        //}
        
        //确保backbuffer
       
        for (csmUint32 i = 0; i < s_bufferSetNum; i++)
        {
            csmVector<CubismOffscreenSurface_SDL3> vector;
            _offscreenSurfaces.PushBack(vector);
            for (csmUint32 j = 0; j < model->GetDrawableCount(); j++)
            {
                

                CubismOffscreenSurface_SDL3 offscreenSurface;
                //需要时创建和绘制
                if (GetClippingContextBufferForDelayDraw(j))
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
    _constantBuffersForDraw = static_cast<SDL_GPUBuffer***>(CSM_MALLOC(sizeof(SDL_GPUBuffer**) * s_bufferSetNum));

    
    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    _drawableNum = drawableCount;

    for (csmUint32 buffer = 0; buffer < s_bufferSetNum; buffer++)
    {
        _vertexBuffers[buffer] = static_cast<SDL_GPUBuffer**>(CSM_MALLOC(sizeof(SDL_GPUBuffer*) * drawableCount));
        _vertexBuffers_tb[buffer] = static_cast<SDL_GPUTransferBuffer**>(CSM_MALLOC(sizeof(SDL_GPUTransferBuffer*) * drawableCount));
        _indexBuffers[buffer] = static_cast<SDL_GPUBuffer**>(CSM_MALLOC(sizeof(SDL_GPUBuffer*) * drawableCount));
        _constantBuffersForDraw[buffer] = static_cast<SDL_GPUBuffer**>(CSM_MALLOC(sizeof(SDL_GPUBuffer*) * drawableCount));


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
                    CubismLogError("Vertexbuffer create failed : %s", SDL_GetError());

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







            //创建对象的索引缓存
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
                    CubismLogError("Indexbuffer create failed : %s", SDL_GetError());
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
            // **********
            // ↑↑↑↑
    //已确认添加角色模型后桌子绘制出现问题是因为SDL每个slot、每个管线阶段只申请一块uniform buffer，
    //这个uniform大小为32468字节，并且数据以256字节对其
    //每次push uniform data即向uniform buffer的末尾添加新的数据
    //这导致在128个图层push了unifordata后，uniform buffer数据已满
    //第129个图层会创建一个新的uniform buffer，这导致了数据丢失，第1个图层会使用第129图层的数据
    //在这次出现的Shift框体消失的问题即是这种情况
    // 
    //因此 Live2D的每图层一个用于着色器的常数缓存不适合使用SDL uniform buffer
    // （图层会轻易上数百层， uniform buffer甚至会覆写两三次）
    //SDL UNIFORM buffer可能适合管理更少绑定次数的数据
    //
    // 将Live2D的缓存改用正常的常数缓存并自己管理.
            //***********

            //每个图层都需要一个常数缓存
            //蒙版所用的常数缓存在用到的时候才创建
            _constantBuffersForDraw[buffer][drawAssign] = NULL;
            {
                SDL_GPUBufferCreateInfo bufferDesc = {};
                bufferDesc.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
                bufferDesc.size = sizeof(CubismConstantBufferSDL3);
                auto pbuffer = SDL_CreateGPUBuffer(s_device, &bufferDesc);
                if (!pbuffer)
                {
                    CubismLogError("ConstantBuffer create failed : %s", SDL_GetError());
                }
                _constantBuffersForDraw[buffer][drawAssign] = pbuffer;
            }
            

        }

        _constantBuffersForDraw_tb.resize(drawableCount, 0);
        _constantBuffersForMask.resize(drawableCount,0);
        _constantBuffersForMask_tb.resize(drawableCount);
        _constantBuffersForMask_tb_index.resize(drawableCount,0);

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


    //重置一些状态
    SDL_memset(_constantBuffersForMask_tb_index.data(), 0, _constantBuffersForMask_tb_index.size() * sizeof(int));



    //在使用裁剪蒙版·缓冲区预处理方式的情况下
    if (_clippingManager != NULL)
    {
        for (csmInt32 i = 0; i < _offscreenSurfaces.GetSize(); ++i)
        {
            for (csmInt32 j = 0; j < _offscreenSurfaces[i].GetSize(); ++j)
            {
                if(_offscreenSurfaces[i][j].IsValid())
                if (_offscreenSurfaces[i][j].GetBufferWidth() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X) ||
                    _offscreenSurfaces[i][j].GetBufferHeight() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y))
                {
                    _offscreenSurfaces[i][j].CreateOffscreenSurface(s_device,
                        static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X), static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y));
                }
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

    //MIXDRAW
    if (bMixDraw)
    {
        _drawOrderList = Core::csmGetDrawableDrawOrders(GetModel()->GetModel());
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

        //绘制前的准备阶段
        auto readyFunc = [](void* _pthis, uint64_t drawableIndex)
            {
                CubismRenderer_SDL3* pthis = (CubismRenderer_SDL3*)_pthis;
                //设置剪贴蒙版
                CubismClippingContext_SDL3* clipContext = (pthis->_clippingManager != NULL)
                    ? (*pthis->_clippingManager->GetClippingContextListForDraw())[drawableIndex]
                    : NULL;

                if (clipContext != NULL && pthis->IsUsingHighPrecisionMask()) //绘制蒙版
                {
                    if (clipContext->_isUsing) //
                    {
                        
                        CubismRenderer_SDL3::GetRenderStateManager()->SetViewport(s_context,
                            0,
                            0,
                            static_cast<float>(pthis->_clippingManager->GetClippingMaskBufferSize().X),
                            static_cast<float>(pthis->_clippingManager->GetClippingMaskBufferSize().Y),
                            0.0f, 1.0f);

                        //调用具有正确渲染目标的离屏表面缓冲区
                        //CubismOffscreenSurface_SDL3* currentHighPrecisionMaskColorBuffer = &pthis->_offscreenSurfaces[pthis->_commandBufferCurrent][clipContext->_bufferIndex];
                        CubismOffscreenSurface_SDL3* currentHighPrecisionMaskColorBuffer = &pthis->_offscreenSurfaces[pthis->_commandBufferCurrent][drawableIndex];


                        currentHighPrecisionMaskColorBuffer->BeginDraw(s_context);
                        currentHighPrecisionMaskColorBuffer->Clear(s_context, 1.0f, 1.0f, 1.0f, 1.0f);

                        const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                        for (csmInt32 ctx = 0; ctx < clipDrawCount; ctx++)
                        {
                            const csmInt32 clipDrawIndex = clipContext->_clippingIdList[ctx];

                            // 如果顶点信息未更新，则跳过绘制
                            if (!pthis->GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
                            {
                                continue;
                            }

                            //是否裁剪背面（单面绘制）
                            pthis->IsCulling(pthis->GetModel()->GetDrawableCulling(clipDrawIndex) != 0);

                            // 今回専用の変換を適用して描く
                            // 频道也需要切换(A,R,G,B)
                            pthis->SetClippingContextBufferForMask(clipContext);
                            pthis->DrawMeshSDL3(*pthis->GetModel(), clipDrawIndex);
                        }

                        {
                            //后处理
                            currentHighPrecisionMaskColorBuffer->EndDraw(s_context);
                            pthis->SetClippingContextBufferForMask(NULL);

                            // 恢复viewport
                            GetRenderStateManager()->SetViewport(s_context,
                                0.0f,
                                0.0f,
                                static_cast<float>(s_viewportWidth),
                                static_cast<float>(s_viewportHeight),
                                0.0f, 1.0f);

                            pthis->PreDraw(); // バッファをクリアする
                        }
                    }
                }

                // クリッピングマスクをセットする
                pthis->SetClippingContextBufferForDraw(clipContext);



                pthis->IsCulling(pthis->GetModel()->GetDrawableCulling(drawableIndex) != 0);
            };
        if (!bMixDraw)
        {
            readyFunc(this, drawableIndex);
            DrawMeshSDL3(*GetModel(), drawableIndex);
        }
        else
        {
            DrawMeshSDL3(*GetModel(), drawableIndex, bMixDraw, readyFunc);
        }



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



    //这里不能和D3D11 example顺序一直，因为下面StartRender中用COPY CMD开启了新的render pass，（COPY用于离屏准备绘制、顶点和常数缓存的更新等）
    //所以要先处理常数

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


        auto testMat = GetClippingContextBufferForMask()->_matrixForMask;

        // Update
        UpdateConstantBufferForMask(cb, index);
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

    //在StartRender后再进行常数缓存的绑定
    BindConstantBufferForMask(index);


    //mask用的RenderState
    GetRenderStateManager()->SetBlend(s_context,
        CubismRenderState_SDL3::Blend_Mask,
        glm::vec4(0, 0, 0, 0),
        0xffffffff);



    //设置顶点几何布局为TRIANGLELIST
    s_context->SetTopology(SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
    DrawDrawableIndexed(model, index);

    s_context->EndRender();
}

void CubismRenderer_SDL3::ExecuteDrawForDraw(const CubismModel& model, const csmInt32 index)
{
    //if (bMixDraw);
    //return ExecuteDrawForMixDraw(model, index);

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
    int psCbufferSlot;
    if (!SetShader(model, index,&psCbufferSlot))
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
        UpdateConstantBufferForDraw(cb, index);
        BindConstantBufferForDraw(index);

    }


    //设置顶点几何布局为TRIANGLELIST
    s_context->SetTopology(SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
    DrawDrawableIndexed(model, index);


    s_context->EndRender();
}

void CubismRenderer_SDL3::ExecuteDrawForMixDraw(const CubismModel& model, const csmInt32 index, void(*beforeDrawCallback)(void* userData, uint64_t userData2))
{

    //MixDraw要注意不要设置了SDL渲染状态，而是把渲染数据传给MixDraw函数

    if (!mixDrawCallFunc)return;


    CubismShader_SDL3* shaderManager = Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::GetShaderManager();
    if (!shaderManager)
    {
        return;
    }


    //需要传递给MixDraw函数的数值
    SDL_GPUGraphicsPipeline* pipeLine;

    uint32_t vsStartSlot; 
    uint32_t vsBufferNum;
    SDL_GPUBufferBinding vsBuffers[1];

    SDL_GPUIndexElementSize indexElementSize;
    SDL_GPUBufferBinding indexBuffer;
    uint32_t indexCount;
    uint32_t indexStart;

    uint32_t texStartSlot;
    uint32_t texNum;
    SDL_GPUTextureSamplerBinding textures[2];//Live2D纹理数不会超过2

    //为了修复bug 不再使用uniform buffer 改用constant buffer
    //uint32_t vsUniformSlot;
    //void* vsUniformData; 
    //uint32_t vsUniformDataLength;

    //uint32_t psUniformSlot;
    //const void* psUniformData;
    //uint32_t psUniformDataLength;





    uint32_t vsConstantStartSlot;
    uint32_t vsConstantNum;
    SDL_GPUBuffer* vsConstantBuffers[1];//Live2D只使用1个常数缓存

    uint32_t psConstantStartSlot;
    uint32_t psConstantNum;
    SDL_GPUBuffer* psConstantBuffers[1];//Live2D只使用1个常数缓存


    

    SDL_GPUViewport viewPort = {};

    auto __clippingContextBufferForDelayDraw=GetClippingContextBufferForDelayDraw(index);
    //顶点
    {
        vsStartSlot = 0;
        vsBufferNum = 1;
        vsBuffers[0] = {_vertexBuffers[_commandBufferCurrent][index],0};
    }
    //索引
    {
        indexElementSize = SDL_GPU_INDEXELEMENTSIZE_16BIT;
        indexBuffer = { _indexBuffers[_commandBufferCurrent][index],0 };
        indexCount = model.GetDrawableVertexIndexCount(index);
        indexStart = 0;
    }
    //纹理和采样器
    {
    
        SetSamplerAccordingToAnisotropy();//根据各向异性数值设置采样器

        //根据状态设置着色器纹理资源
        //const csmBool masked = GetClippingContextBufferForDraw() != NULL;
        const csmBool masked = __clippingContextBufferForDelayDraw != NULL;;
        const csmBool drawing = !IsGeneratingMask();

        SDL_GPUTexture* textureView = GetTextureViewWithIndex(model, index);


        //SHIFT键框消失的BUG的原因应该是模型会共用蒙版。X
        //无法识别蒙版的原因是延迟绘制触发的时候_clippingContextBufferForDraw已经被恢复为空，而空值导致被判定为当前图层不使用mask
        //把mask绘制的过程也移动到mix里可能能解决这个问题
        //SDL_GPUTexture* maskView = (masked && drawing ? _offscreenSurfaces[_commandBufferCurrent][GetClippingContextBufferForDraw()->_bufferIndex].GetTextureView() : NULL);
        SDL_GPUTexture* maskView = (masked && drawing ? _offscreenSurfaces[_commandBufferCurrent][index].GetTextureView() : NULL);


        textures[0] = { textureView,textureView ? s_context->GetFragmentSampler() : NULL };
        textures[1] = { maskView ,maskView ? s_context->GetFragmentSampler() : NULL };

        texStartSlot = 0;
        texNum = 0;
        if (textureView)
        {
            if (maskView)
            {
                texNum = 2;
            }
            else
            {
                texNum = 1;
            }
        }
    }

    int psCbufferSlot;
    //PipeLile
    {
        CubismBlendMode colorBlendMode = model.GetDrawableBlendMode(index);
        SetBlendState(colorBlendMode);

        if (!SetShader(model, index,&psCbufferSlot))
        {
            return;
        }

        s_context->SetTopology(SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST);
    
        pipeLine=s_context->GetPipelineFromCurState();
    }

    //现在存在作为其他图像蒙版的图层绘制出现问题
    //原因是为了修复SHIFT按键消失的问题将常数缓存从uniform迁移到普通buffer
    //但CatTuber采用了SDL建议的双commandbuffer方案，即一个主CMD(command buffer),一个用于上传顶点图像、绘制离屏纹理的副CMD
    // 这种方案可以大幅减少RenderPass的重建次数以提高性能
    // 
    //但是在Live2D中，作为其他蒙版的图层的常数缓存存在多次修改的情况，（绘制蒙版的时候的数据和正常绘制的数据不同）
    // 这导致了正常绘制图像时GPU实际使用的常数缓存是绘制蒙版时的常数缓存（正常绘制和绘制蒙版时 并不在GPU内同步）
    // 
    //由于无论如何都是先将COPY 队列完全执行后在执行主队列，
    // 所以不管在哪修改这个buffer都会导致主队列中使用的是最后一次设置常数缓存
    //
    // 准备分开管理蒙版所用的常数缓存和绘制所用的常数缓存
    //
    // 
    // 
    //常数
    {
        CubismConstantBufferSDL3 cb;
        memset(&cb, 0, sizeof(cb));

        const csmBool masked = __clippingContextBufferForDelayDraw != NULL;
        if (masked)
        {
            //设置矩阵以将View坐标转换为ClippingContext坐标
            //cb.clipMatrix = glm::transpose(ConvertToGLM(GetClippingContextBufferForDraw()->_matrixForDraw));
            //cb.clipMatrix = glm::transpose(ConvertToGLM(_clipMatVec[index]));
            cb.clipMatrix = glm::transpose(ConvertToGLM(__clippingContextBufferForDelayDraw->_matrixForDraw));

            //设置要使用的颜色通道
            //CubismClippingContext_SDL3* contextBuffer = GetClippingContextBufferForDraw();
            SetColorChannel(cb, __clippingContextBufferForDelayDraw);
        }

        // 色
        CubismTextureColor baseColor = GetModelColorWithOpacity(model.GetDrawableOpacity(index));
        CubismTextureColor multiplyColor = model.GetMultiplyColor(index);
        CubismTextureColor screenColor = model.GetScreenColor(index);
        SetColorConstantBuffer(cb, model, index, baseColor, multiplyColor, screenColor);




        SetProjectionMatrix(cb, GetMvpMatrix());

        UpdateConstantBufferForDraw(cb, index);

        vsConstantStartSlot = 0;
        vsConstantNum = 1;
        vsConstantBuffers[0] = _constantBuffersForDraw[_commandBufferCurrent][index];

        //psConstantStartSlot = 0;
        //需要具体计算槽位 幽默SDL检测机制，复杂化、僵化开发.

        psConstantStartSlot = 0;
        psConstantNum = 1;
        psConstantBuffers[0] = _constantBuffersForDraw[_commandBufferCurrent][index];
    }


    //VIEWPORT
    {
        viewPort.x = 0;
        viewPort.y = 0;
        viewPort.w =static_cast<float>( s_viewportWidth);
        viewPort.h = static_cast<float>(s_viewportHeight);
        viewPort.min_depth = 0.f;
        viewPort.max_depth = 1.f;
    }

    //填充数据
    {
        Csm::Rendering::MixRenderData renderData = {};
        renderData.sizeOfThisStruct = sizeof(renderData);
        renderData.layerZ = _drawOrderList[index];
        renderData.pipeLine = pipeLine;

        renderData.vsStartSlot = vsStartSlot;
        renderData.vsBufferNum = vsBufferNum;
        renderData.vsBuffers = vsBuffers;

        renderData.indexElementSize = indexElementSize;
        renderData.indexBuffer = &indexBuffer;
        renderData.indexCount = indexCount;
        renderData.indexStart = indexStart;

        renderData.texStartSlot = texStartSlot;
        renderData.texNum = texNum;
        renderData.textures = textures;

        renderData.vsConstantSlot = vsConstantStartSlot;
        renderData.vsConstantNum = vsConstantNum;
        renderData.vsConstantBuffers = vsConstantBuffers;

        renderData.psConstantSlot = psConstantStartSlot;
        renderData.psConstantNum = psConstantNum;
        renderData.psConstantBuffers = psConstantBuffers;


        renderData.beforeDrawCallback = beforeDrawCallback;
        renderData.callbackUserData = this;
        renderData.callbackUserData2 = index;

        renderData.viewport = viewPort;
        mixDrawCallFunc(mixDrawFuncData,&renderData);
    
    }
    return;
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





void CubismRenderer_SDL3::DrawMeshSDL3(const CubismModel& model, const csmInt32 index, bool mixDraw, void(*beforeDrawCallback)(void* userData, uint64_t userData2))
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
        if(mixDraw)
            ExecuteDrawForMixDraw(model, index, beforeDrawCallback);
        else
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

CubismClippingContext_SDL3* CubismRenderer_SDL3::GetClippingContextBufferForDelayDraw(const csmInt32 index) const
{
    return (_clippingManager != NULL)
        ? (*_clippingManager->GetClippingContextListForDraw())[index]
        : NULL;
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
            SDL_GPUCommandBuffer* cmd = renderContext->GetCopyCommandBuffer();
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

Csm::csmBool CubismRenderer_SDL3::SetShader(const CubismModel& model, const csmInt32 index/*,int* cBufferStartSlotVS*/, int* cBufferStartSlotPS)
{
    //根据状态设置着色器
    //const csmBool masked = GetClippingContextBufferForDraw() != NULL;
    const csmBool masked = GetClippingContextBufferForDelayDraw(index) != NULL;
    const csmBool premult = IsPremultipliedAlpha();
    const csmBool invertedMask = model.GetDrawableInvertedMask(index);

    const ShaderNames vertexShaderNames = (masked ?
        ShaderNames_NormalMasked :
        ShaderNames_Normal);
    ShaderNames pixelShaderNames;

    int psCbufferSlot;
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
        psCbufferSlot = 2;
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
        psCbufferSlot = 1;
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

    if (cBufferStartSlotPS)*cBufferStartSlotPS = psCbufferSlot;


    s_context->SetVertexShader(vs);
    s_context->SetFragmentShader(ps);
    return true;
}

void CubismRenderer_SDL3::SetTextureView(const CubismModel& model, const csmInt32 index)
{
    //根据状态设置着色器纹理资源
    //const csmBool masked = GetClippingContextBufferForDraw() != NULL;
    const csmBool masked = GetClippingContextBufferForDelayDraw(index) != NULL;
    const csmBool drawing = !IsGeneratingMask();

    SDL_GPUTexture* textureView = GetTextureViewWithIndex(model, index);
    //SDL_GPUTexture* maskView = (masked && drawing ? _offscreenSurfaces[_commandBufferCurrent][GetClippingContextBufferForDraw()->_bufferIndex].GetTextureView() : NULL);
    SDL_GPUTexture* maskView = (masked && drawing ? _offscreenSurfaces[_commandBufferCurrent][index].GetTextureView() : NULL);
    SDL_GPUTextureSamplerBinding viewArray[2] = { 
        {textureView,textureView ? s_context->GetFragmentSampler():NULL },
        {maskView ,maskView ? s_context->GetFragmentSampler() : NULL}
    };
    uint32_t numViews = 0;
    if (textureView)
    {
        numViews++;
    }
    if (maskView)
    {
        numViews++;
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
    cb.channelFlag= glm::vec4(colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A);\
}
void CubismRenderer_SDL3::SetProjectionMatrix(CubismConstantBufferSDL3& cb, CubismMatrix44 matrix)
{
   cb.projectMatrix = glm::transpose(ConvertToGLM(matrix));
}

void CubismRenderer_SDL3::UpdateConstantBufferForDraw(CubismConstantBufferSDL3& cb, csmInt32 index)
{
    if (_constantBuffersForDraw[_commandBufferCurrent][index])
    {
        //获取最新的tb
        SDL_GPUTransferBuffer* transferBuffer= _constantBuffersForDraw_tb[index];
        if(!transferBuffer)
        {

            SDL_GPUTransferBufferCreateInfo createInfo = {};
            createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            createInfo.size = sizeof(CubismConstantBufferSDL3);
            transferBuffer = SDL_CreateGPUTransferBuffer(s_device, &createInfo);
            _constantBuffersForDraw_tb[index]=(transferBuffer);
        }


        //SDL_GPUTransferBuffer* transferBuffer = _constantBuffersForDraw_tb[_commandBufferCurrent][index];
        



        //映射并复制数据
        void* mapped = SDL_MapGPUTransferBuffer(s_device,transferBuffer,false);
        if (mapped)
        {
            SDL_memcpy(mapped,&cb,sizeof(cb));
            SDL_UnmapGPUTransferBuffer(s_device,transferBuffer);


            //创建命令缓冲区并开始复制
            SDL_GPUCommandBuffer* cmd = s_context->GetCopyCommandBuffer();

            SDL_GPUTransferBufferLocation bufferLocation = { transferBuffer,0 };
            SDL_GPUBufferRegion bufferRegion ;
            bufferRegion.buffer = _constantBuffersForDraw[_commandBufferCurrent][index];
            bufferRegion.offset = 0;
            bufferRegion.size = sizeof(CubismConstantBufferSDL3);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
            SDL_UploadToGPUBuffer(copyPass,&bufferLocation,&bufferRegion,false);
            SDL_EndGPUCopyPass(copyPass);
        }
    }
}

void CubismRenderer_SDL3::UpdateConstantBufferForMask(CubismConstantBufferSDL3& cb, csmInt32 index)
{
    //不是图层索引，每个图层需要很多缓存，这个是缓存的索引
    int curBufferIndex = _constantBuffersForMask_tb_index[index];


    if (!_constantBuffersForMask[index])
    {
        //for (int i = _constantBuffersForMask[index].size(); i <= curBufferIndex; i++)
        {
            SDL_GPUBufferCreateInfo bufferDesc = {};
            bufferDesc.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
            bufferDesc.size = sizeof(CubismConstantBufferSDL3);
            auto pbuffer = SDL_CreateGPUBuffer(s_device, &bufferDesc);
            if (!pbuffer)
            {
                CubismLogError("ConstantBuffer create failed : %s", SDL_GetError());
            }
            _constantBuffersForMask[index]=(pbuffer);
        }
    }
    //if (_constantBuffersForMask[index].size() <= curBufferIndex)
    //{
    //    for (int i = _constantBuffersForMask[index].size(); i <= curBufferIndex; i++)
    //    {
    //        SDL_GPUBufferCreateInfo bufferDesc = {};
    //        bufferDesc.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    //        bufferDesc.size = sizeof(CubismConstantBufferSDL3);
    //        auto pbuffer = SDL_CreateGPUBuffer(s_device, &bufferDesc);
    //        if (!pbuffer)
    //        {
    //            CubismLogError("ConstantBuffer create failed : %s", SDL_GetError());
    //        }
    //        _constantBuffersForMask[index].push_back(pbuffer);
    //    }
    //}


    if (_constantBuffersForMask_tb[index].size() <= curBufferIndex)
    {
        for (int i = _constantBuffersForMask_tb[index].size(); i <= curBufferIndex; i++)
        {
            SDL_GPUTransferBufferCreateInfo createInfo = {};
            createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            createInfo.size = sizeof(CubismConstantBufferSDL3);
            SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(s_device, &createInfo);
            _constantBuffersForMask_tb[index].push_back(transferBuffer);
        }
    }





    auto transferBuffer = _constantBuffersForMask_tb[index][curBufferIndex];
        //映射并复制数据
        void* mapped = SDL_MapGPUTransferBuffer(s_device, _constantBuffersForMask_tb[index][curBufferIndex], false);
        if (mapped)
        {
            SDL_memcpy(mapped,&cb,sizeof(cb));
            SDL_UnmapGPUTransferBuffer(s_device,transferBuffer);


            //创建命令缓冲区并开始复制
            SDL_GPUCommandBuffer* cmd = s_context->GetCopyCommandBuffer();

            SDL_GPUTransferBufferLocation bufferLocation = { transferBuffer,0 };
            SDL_GPUBufferRegion bufferRegion ;
            bufferRegion.buffer = _constantBuffersForMask[index]/*[curBufferIndex]*/;
            bufferRegion.offset = 0;
            bufferRegion.size = sizeof(CubismConstantBufferSDL3);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
            SDL_UploadToGPUBuffer(copyPass,&bufferLocation,&bufferRegion,false);
            SDL_EndGPUCopyPass(copyPass);
        }

        _constantBuffersForMask_tb_index[index]++;
}

//void CubismRenderer_SDL3::UpdateConstantBufferForMask(CubismConstantBufferSDL3& cb, csmInt32 index)
//{
    //CreateConstantBufferIfNull(_constantBuffersForMask[_commandBufferCurrent][index], _constantBuffersForMask_tb[_commandBufferCurrent][index]);

    //if (_constantBuffersForMask[_commandBufferCurrent][index])
    //{
    //    SDL_GPUTransferBuffer* transferBuffer = _constantBuffersForMask_tb[_commandBufferCurrent][index];

    //    //映射并复制数据
    //    void* mapped = SDL_MapGPUTransferBuffer(s_device,transferBuffer,false);
    //    if (mapped)
    //    {
    //        SDL_memcpy(mapped,&cb,sizeof(cb));
    //        SDL_UnmapGPUTransferBuffer(s_device,transferBuffer);


    //        //创建命令缓冲区并开始复制
    //        SDL_GPUCommandBuffer* cmd = s_context->GetCopyCommandBuffer();

    //        SDL_GPUTransferBufferLocation bufferLocation = { transferBuffer,0 };
    //        SDL_GPUBufferRegion bufferRegion ;
    //        bufferRegion.buffer = _constantBuffersForMask[_commandBufferCurrent][index];
    //        bufferRegion.offset = 0;
    //        bufferRegion.size = sizeof(CubismConstantBufferSDL3);
    //        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    //        SDL_UploadToGPUBuffer(copyPass,&bufferLocation,&bufferRegion,false);
    //        SDL_EndGPUCopyPass(copyPass);
    //    }
    //}
//}

void CubismRenderer_SDL3::BindConstantBufferForDraw(csmInt32 index)
{

    //在混合绘制的情况，不进行buffer绑定
    if (_constantBuffersForDraw[_commandBufferCurrent][index])
    {
        s_context->SetVertexConstantBuffer(0, 1, &_constantBuffersForDraw[_commandBufferCurrent][index]);
        s_context->SetFragmentConstantBuffer(0, 1, &_constantBuffersForDraw[_commandBufferCurrent][index]);
    }
}

void CubismRenderer_SDL3::BindConstantBufferForMask(csmInt32 index)
{

    ////在混合绘制的情况，不进行buffer绑定
    auto pcbuffer=_constantBuffersForMask[index]/*[_constantBuffersForMask_tb_index[index] - 1]*/;
    if (pcbuffer)
    {
        s_context->SetVertexConstantBuffer(0, 1, &pcbuffer);
        s_context->SetFragmentConstantBuffer(0, 1, &pcbuffer);
    }
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
