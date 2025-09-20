
#include "Type/csmVector.hpp"

#include "CubismShader_SDL3.hpp"
#include "CubismRenderer_SDL3.hpp"
#include"CubismRenderContext_SDL3.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {




                CubismRenderState_SDL3::CubismRenderState_SDL3()
                {
                    //
                    memset(_stored._valid, 0, sizeof(_stored._valid));
                    Create(CubismRenderer_SDL3::GetCurrentDevice());
                }

                CubismRenderState_SDL3::~CubismRenderState_SDL3()
                {
                    _pushed.Clear();
                    // 释放samplerstates
                    for (csmUint32 i = 0; i < _samplerState.GetSize(); i++)
                    {
                        SDL_GPUSampler* sampl = _samplerState[i];
                        if (sampl)
                        {
                            SDL_ReleaseGPUSampler(_device,sampl);
                        }
                    }
                    _samplerState.Clear();

                    // Z解放
                    //for (csmUint32 i = 0; i < _depthStencilState.GetSize(); i++)
                    //{
                    //    ID3D11DepthStencilState* depth = _depthStencilState[i];
                    //    if (depth)
                    //    {
                    //        depth->Release();
                    //    }
                    //}
                    _depthStencilState.Clear();

                    // ラスタライザ解放
                    //for (csmUint32 i = 0; i < _rasterizeStateObjects.GetSize(); i++)
                    //{
                    //    ID3D11RasterizerState* raster = _rasterizeStateObjects[i];
                    //    if (raster)
                    //    {
                    //        raster->Release();
                    //    }
                    //}
                    _rasterizeStateObjects.Clear();

                    // ブレンドステート解放
                    //for (csmUint32 i = 0; i < _blendStateObjects.GetSize(); i++)
                    //{
                    //    ID3D11BlendState* state = _blendStateObjects[i];
                    //    if (state)
                    //    {
                    //        state->Release();
                    //    }
                    //}
                    _blendStateObjects.Clear();
                }

                void CubismRenderState_SDL3::Create(SDL_GPUDevice* device)
                {
                    if (!device)
                    {
                        return;
                    }
                    _device = device;



                    SDL_GPUColorTargetBlendState state = {};
                    // origin
                    _blendStateObjects.PushBack(state);
                    //zero
                    state.enable_blend = true;
                    state.enable_color_write_mask = false;//设置为false即写入所有颜色通道

                    state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.color_blend_op = SDL_GPU_BLENDOP_ADD;
                    state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
                    _blendStateObjects.PushBack(state);


                    // 通常
                    state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                    state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                    _blendStateObjects.PushBack(state);

                    //加算
                    state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    _blendStateObjects.PushBack(state);

                    //乘算
                    state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR;
                    state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                    state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                    _blendStateObjects.PushBack(state);

                    //mask
                    state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
                    state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
                    state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                    _blendStateObjects.PushBack(state);



                    //RasterizerState
                    SDL_GPURasterizerState rasterizer = {};
                    _rasterizeStateObjects.PushBack(rasterizer);

                    //None
                    rasterizer.fill_mode = SDL_GPU_FILLMODE_FILL;
                    rasterizer.cull_mode = SDL_GPU_CULLMODE_NONE;
                    rasterizer.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
                    _rasterizeStateObjects.PushBack(rasterizer);

                    rasterizer.cull_mode = SDL_GPU_CULLMODE_BACK;
                    _rasterizeStateObjects.PushBack(rasterizer);



                    //Depth
                    SDL_GPUDepthStencilState depthDesc = {};
                    _depthStencilState.PushBack(depthDesc);

                    //无效深度
                    depthDesc.enable_depth_test = false;
                    depthDesc.enable_stencil_test = false;
                    depthDesc.write_mask = 0xFF;
                    depthDesc.compare_op = SDL_GPU_COMPAREOP_LESS;
                    _depthStencilState.PushBack(depthDesc);

                    //启用深度
                    depthDesc.enable_depth_test = true;
                    depthDesc.enable_stencil_test = false;
                    _depthStencilState.PushBack(depthDesc);


                    //采样器
                    SDL_GPUSampler* sampler = NULL;
                    SDL_GPUSamplerCreateInfo samplerDesc = {};

                    _samplerState.PushBack(NULL);

                    samplerDesc.min_filter = SDL_GPU_FILTER_LINEAR;
                    samplerDesc.mag_filter = SDL_GPU_FILTER_LINEAR;
                    samplerDesc.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
                    samplerDesc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                    samplerDesc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                    samplerDesc.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                    samplerDesc.max_anisotropy = 0;
                    samplerDesc.enable_compare = false;
                    samplerDesc.min_lod = -FLT_MAX;
                    samplerDesc.max_lod = FLT_MAX;
                    sampler=SDL_CreateGPUSampler(device,&samplerDesc);
                    _samplerState.PushBack(sampler);


                }


                void CubismRenderState_SDL3::StartFrame()
                {
                    memset(_stored._valid, 0, sizeof(_stored._valid));
                    _pushed.Clear();
                }

                void CubismRenderState_SDL3::Save()
                {
                    //Save和Restore应该是用于D3d11恢复渲染状态的，但是SDL3渲染不依赖状态（用非常多的管线实例），所以这两个函数应该不需要实现
                }

                void CubismRenderState_SDL3::Restore(CubismRenderContext_SDL3* renderContext)
                {
                    //Save和Restore应该是用于D3d11恢复渲染状态的，但是SDL3渲染不依赖状态（用非常多的管线实例），所以这两个函数应该不需要实现
                }

                void CubismRenderState_SDL3::SetBlend(CubismRenderContext_SDL3* renderContext, Blend blendState, glm::vec4 blendFactor, uint32_t mask,
                    csmBool force)
                {
                    //检测参数异常
                    if (!renderContext || blendState < 0 || Blend_Max <= blendState)
                    {
                        return;
                    }

                    if (!_stored._valid[State_Blend] || force ||
                        _stored._blendFactor.x != blendFactor.x || _stored._blendFactor.y != blendFactor.y || _stored._blendFactor.z != blendFactor.z || _stored._blendFactor.w != blendFactor.w ||
                        _stored._blendMask != mask ||
                        _stored._blendState != blendState)
                    {
                        //OMSetBlendState的blend mask和factor在Live2d中不使用（实际上SDL3中我也不知道该怎么弄就是了..）
                        renderContext->SetBlendState(blendState);
                    }

                    _stored._blendState = blendState;
                    _stored._blendFactor = blendFactor;//SDL3未使用
                    _stored._blendMask = mask;//SDL3未使用

                    _stored._valid[State_Blend] = true;
                }

                void CubismRenderState_SDL3::SetCullMode(CubismRenderContext_SDL3* renderContext, Cull cullFace, csmBool force)
                {
                    if (!renderContext || cullFace < 0 || Cull_Max <= cullFace)
                    {// パラメータ異常チェック
                        return;
                    }
                    if (!_stored._valid[State_CullMode] || force ||
                        _stored._cullMode != cullFace)
                    {
                        renderContext->SetCullMode(cullFace);
                    }
                    _stored._cullMode = cullFace;
                    _stored._valid[State_CullMode] = true;
                }

                void CubismRenderState_SDL3::SetViewport(CubismRenderContext_SDL3* renderContext, float left, float top, float width, float height, float zMin, float zMax, csmBool force)
                {
                    //感觉可以移除不少地方的条件判断，毕竟库里一般情况下不会出现非法参数（出现就是live2d的锅）
                    if (!_stored._valid[State_Blend] || force ||
                        _stored._viewportX != left || _stored._viewportY != top || _stored._viewportWidth != width || _stored._viewportHeight != height ||
                        _stored._viewportMinZ != zMin || _stored._viewportMaxZ != zMax)
                    {
                        SDL_GPUViewport setViewport;
                        setViewport.x = left;
                        setViewport.y = top;
                        setViewport.w = width;
                        setViewport.h = height;
                        setViewport.min_depth = zMin;
                        setViewport.max_depth = zMax;
                        renderContext->SetViewport(&setViewport);
                    }
                    _stored._viewportX = left;
                    _stored._viewportY = top;
                    _stored._viewportWidth = width;
                    _stored._viewportHeight = height;
                    _stored._viewportMinZ = zMin;
                    _stored._viewportMaxZ = zMax;

                    _stored._valid[State_Viewport] = true;

                }

                void CubismRenderState_SDL3::SetZEnable(CubismRenderContext_SDL3* renderContext, Depth enable, uint32_t stelcilRef, csmBool force)
                {
                    if (!renderContext || enable < 0 || Depth_Max <= enable)
                    {// パラメータ異常チェック
                        return;
                    }

                    if (!_stored._valid[State_ZEnable] || force ||
                        _stored._depthEnable != enable)
                    {
                        //renderContext->OMSetDepthStencilState(_depthStencilState[enable], stelcilRef);
                        renderContext->SetZEnable(enable);
                    }

                    _stored._depthEnable = enable;
                    _stored._depthRef = stelcilRef;

                    _stored._valid[State_ZEnable] = true;
                }

                void CubismRenderState_SDL3::SetSampler(CubismRenderContext_SDL3* renderContext, Sampler sample, csmFloat32 anisotropy, SDL_GPUDevice* device, csmBool force)
                {
                    if (!renderContext || sample < 0 || Sampler_Max <= sample)
                    {// パラメータ異常チェック
                        return;
                    }

                    if (!_stored._valid[State_ZEnable] || force ||
                        _stored._sampler != sample)
                    {
                        if (anisotropy > 0.0f && sample == Sampler_Anisotropy && device != NULL&& _samplerState.GetSize()< Sampler_Max)
                        {
                            // Sampler
                            
                            SDL_GPUSampler* sampler = NULL;
                            SDL_GPUSamplerCreateInfo samplerDesc = {};

                            samplerDesc.min_filter = SDL_GPU_FILTER_LINEAR;
                            samplerDesc.mag_filter = SDL_GPU_FILTER_LINEAR;
                            samplerDesc.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
                            samplerDesc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                            samplerDesc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                            samplerDesc.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
                            samplerDesc.max_anisotropy = anisotropy;
                            samplerDesc.enable_compare = false;
                            samplerDesc.min_lod = -FLT_MAX;
                            samplerDesc.max_lod = FLT_MAX;
                            sampler=SDL_CreateGPUSampler(device, &samplerDesc);
                            _samplerState.PushBack(sampler);
                            //有问题吗这里原sample中没有限制只创建一次，我还没摸清楚实际执行逻辑，但正常来说要是不加限制的话，
                            // 这里每次引用各向异性采样的时候都会额外pushback一个用不到的采样器  


          

                        }


                        renderContext->SetFragmentSampler(sample);

                        
                    }
                    _stored._sampler = sample;
                    _stored._valid[State_Sampler] = true;
                }









			}
		}
	}
}