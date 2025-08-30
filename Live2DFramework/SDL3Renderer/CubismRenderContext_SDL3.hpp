#pragma once
#include<SDL3/SDL.h>
#include<vector>
#include<unordered_map>
#include"CubismRenderState_SDL3.hpp"





namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {
                struct _CubismRenderSDL3PipelineState
                {
                    CubismRenderState_SDL3::Blend _blend;
                    CubismRenderState_SDL3::Cull _cullmode;
                    CubismRenderState_SDL3::Depth _depthEnable;
                    SDL_GPUPrimitiveType _topology;
                    SDL_GPUShader* _vertexShader = NULL;
                    SDL_GPUShader* _fragmentShader = NULL;

                    //输入布局
                    SDL_GPUVertexInputState _vertexInputState;
                    SDL_GPUVertexBufferDescription _vertexBufferDescription;
                    SDL_GPUVertexAttribute _vertexAttr[2];//针对live2D开发的，目前live2D顶点只有xy、uv两组(没有颜色)


                    //用于hash的函数
                    //size_t operator()(_PipelineState const&) const noexcept;
                    bool operator==(const _CubismRenderSDL3PipelineState & o) const noexcept
                    {

                        return memcmp(this,&o,
                            sizeof(_blend)+ sizeof(_cullmode) + sizeof(_depthEnable) + sizeof(_topology) 
                            +sizeof(_vertexShader) + sizeof(_fragmentShader)
                        )==0;
                    }
                    //bool operator()(const _PipelineState& a, const _PipelineState& b);
                };


                struct _RenderPassState
                {
                    SDL_GPUColorTargetInfo colorTargetInfo;
                    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo;
                };
            }
        }
    }
}



namespace std
{
    inline void hash_combine(size_t& seed, size_t h) {
        // 0x9e3779b97f4a7c15 是 64 位黄金分割常量
        seed ^= h + 0x9e3779b97f4a7c15ULL
            + (seed << 6)
            + (seed >> 2);
    };
    template<>
    struct hash<Live2D::Cubism::Framework::Rendering::_CubismRenderSDL3PipelineState>
    {



        size_t operator() (const Live2D::Cubism::Framework::Rendering::_CubismRenderSDL3PipelineState& s) const noexcept
        {

            size_t seed = 0;
            hash_combine(seed, std::hash<int>()(int(s._blend)));
            hash_combine(seed, std::hash<int>()(int(s._cullmode)));
            hash_combine(seed, std::hash<int>()(int(s._depthEnable)));
            hash_combine(seed, std::hash<void*>()(s._vertexShader));
            hash_combine(seed, std::hash<void*>()(s._fragmentShader));

            //剩下的参数应该是不需要参与哈希计算的（输入布局与顶点着色器强绑定）
            //_topology可以计算一下（但在live2D中没有用到过三角形list以外的topology，所以这里忽略）
            //hash_combine(seed, std::hash<int>()(int(s._topology)));
            return seed;
        }
    }; // 间接调用原生Hash.


    template<>
    struct hash<Csm::Rendering::_RenderPassState>
    {
        size_t operator() (const Csm::Rendering::_RenderPassState& s) const noexcept
        {
            size_t seed = 0;
            hash_combine(seed, std::hash<void*>()(s.colorTargetInfo.texture));
            hash_combine(seed, std::hash<void*>()(s.depthStencilTargetInfo.texture));
            hash_combine(seed, std::hash<int>()(s.colorTargetInfo.load_op));
            hash_combine(seed, std::hash<int>()(s.depthStencilTargetInfo.load_op));

            return seed;
        }
    };



}







namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {

                
                //SDL3逻辑类似Dx12，这里尝试封装一个类似Dx11context的东西
                //仅用于Live2D
                class CubismRenderContext_SDL3
                {

                public:
                    SDL_GPUDevice* GetDevice() { return _device; }

                    static CubismRenderContext_SDL3* CreateSDLGpuRenderContext(SDL_GPUDevice* device);
                    static void ReleaseSDLGpuRenderContext(CubismRenderContext_SDL3* context);


                    //为了减少数据复制，额外增加BeforeDrawCall和EndDrawCall来适应SDL3的commandbuffer
                    //是在每次DrawCall前后使用，将绑定管线资源的代码包含在Start和End之间
                    //这个是SDL3的commandbuffer的生命周期，不是类似imgui那种一帧的渲染函数的前后

                    //不能从外面传入pass，因为无法同时存在多个pass
                    // 在live2d内部会存在切换渲染目标的情况（离屏）
                    //所以这里改为传入创建一个pass所必须的信息，在pass失效后由渲染器自行重建pass
                    void StartFrame(SDL_GPUCommandBuffer* cmdCurFrame, SDL_GPUColorTargetInfo* colorTargetInfo, SDL_GPUDepthStencilTargetInfo* depthStencilTargetInfo);//每帧数传入当前帧

                    SDL_GPUCommandBuffer* GetCommandBuffer() { return _cmd; };



                    void StartRender();
                    void EndRender();
                    void EndFrame();

                    void SetRenderTarget(SDL_GPUTexture* renderTex, SDL_GPUTexture* _depthTex);
                    void GetRenderTargets(SDL_GPUTexture** renderTex, SDL_GPUTexture** depthTex);
                    void SetBlendState(CubismRenderState_SDL3::Blend blend);
                    void SetCullMode(CubismRenderState_SDL3::Cull cullmode);
                    void SetViewport(SDL_GPUViewport* pviewPort);
                    void SetZEnable(CubismRenderState_SDL3::Depth depthEnable);
                    void SetTopology(SDL_GPUPrimitiveType topology);


                    void SetVertexShader(SDL_GPUShader* vs);
                    void SetVertexBuffers(uint32_t startSlot, uint32_t numBuffers, SDL_GPUBufferBinding* buffers);
                    void SetIndexBuffer(SDL_GPUBuffer* indexBuffer, SDL_GPUIndexElementSize index_element_size,uint32_t offset);
                    void SetVertexUniformData(uint32_t slot, const void* data, uint32_t datalength);

                    void SetInputLayout(SDL_GPUVertexInputState* inputstate);

                    void SetFragmentShader(SDL_GPUShader* fs);
                    void SetFragmentTextureResources(uint32_t startSlot, uint32_t numViews, const SDL_GPUTextureSamplerBinding* textures);
                    void SetFragmentUniformData(uint32_t slot,const void* data,uint32_t datalength);
                    void SetFragmentSampler(CubismRenderState_SDL3::Sampler sampler) { _sampler = sampler; };
                    SDL_GPUSampler* GetFragmentSampler();


                    

                    void ClearRenderTarget(float r, float g,float b,float a);
                    void ClearDepth(float depth);
                    void ClearStencil(uint8_t stencil);
                    
                    void DrawIndexed(uint32_t count,uint32_t startIndex);



                private:


                    //各种渲染状态
                    //渲染目标与Clear标记
                    SDL_GPUTexture* _renderTarget = NULL;
                    SDL_GPUTexture* _depthStencil = NULL;
                    CubismRenderState_SDL3::Sampler _sampler;
                    SDL_GPUViewport viewPort;
                    SDL_FColor _clearColor;
                    float _clearDepth=1.f;
                    uint8_t _clearStencil=0;
                    bool _needClearColor=false;
                    bool _needClearDepth =false;
                    bool _needClearStencil =false;

                    SDL_GPUColorTargetInfo _colorTargetInfo;
                    SDL_GPUDepthStencilTargetInfo _depthStencilTargetInfo;
                    void _RebuildRenderPass(bool isStartFrame=false);

                    //资源
                    //SDL_GPUBufferBinding _vertexBuffer[8];
                    //bool _vertexBufferUpdateFlag[8] = {};
                    //SDL_GPUBuffer* _indexBuffer = NULL;
                    //SDL_GPUIndexElementSize _index_element_size;
                    //uint32_t _indexBufferOffset = 0;
                    
                                        //管线

                    _CubismRenderSDL3PipelineState _pipelineState;

                    //SDL3 GPU
                    SDL_GPUCommandBuffer* _cmd = NULL;
                    SDL_GPURenderPass* _pass_orig = NULL;
                    SDL_GPURenderPass* _pass_offscreen = NULL;
                    //SDL_GPURenderPass* _pass_working = NULL;
                    SDL_GPURenderPass* _pass = NULL;//当_pass_working用

                    SDL_GPUDevice* _device=NULL;


                    //PIPELINE POOL
                    std::unordered_map<_CubismRenderSDL3PipelineState,SDL_GPUGraphicsPipeline*> pipelinePool;

                    //std::unordered_map<_RenderPassState, SDL_GPURenderPass*>renderPassPool;

                    //需要RENDERPASS POOL吗？Live2D的绘制逻辑每个离屏目标应该每帧不会渲染多次
                    


                };
            }
        }
    }
}

