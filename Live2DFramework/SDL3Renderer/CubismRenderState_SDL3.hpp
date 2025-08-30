#pragma once
#include<SDL3/SDL.h>
#include<glm/glm.hpp>

#include "Type/csmVector.hpp"
#include "Type/csmMap.hpp"



//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {





				class CubismRenderer_SDL3;

				class CubismRenderState_SDL3
				{
					friend class CubismRenderer_SDL3;
					friend class CubismRenderContext_SDL3;



				public:
					enum
					{
						State_None,
						State_Blend,        ///< ブレンドモード
						State_Viewport,     ///< ビューポート
						State_ZEnable,      ///< Z有効無効
						State_CullMode,     ///< カリングモード
						State_Sampler,      ///< テクスチャフィルター
						State_Max,
					};

					// ブレンドステート D3D11ではオブジェクト単位での管理
					enum Blend
					{
						Blend_Origin,
						Blend_Zero,
						Blend_Normal,
						Blend_Add,
						Blend_Mult,
						Blend_Mask,
						Blend_Max,
					};

					enum Cull
					{
						Cull_Origin,///< 元々の設定
						Cull_None,  ///< カリング無し
						Cull_Ccw,   ///< CCW表示
						Cull_Max,
					};

					// Z D3D11ではオブジェクト単位での管理
					enum Depth
					{
						Depth_Origin,   ///< 原始设置
						Depth_Disable,  ///< Zoff
						Depth_Enable,   ///< Zon
						Depth_Max,
					};

					// サンプラーステート D3D11ではオブジェクト単位での管理
					enum Sampler
					{
						Sampler_Origin,     ///< 元々の設定
						Sampler_Normal,     ///< 使用ステート
						Sampler_Anisotropy, ///< 異方性フィルタリング使用
						Sampler_Max,
					};


					struct Stored
					{
						Stored()
						{
							_blendState = Blend_Zero;
							_blendFactor.x = _blendFactor.y = _blendFactor.z = _blendFactor.w = 0.0f;
							_blendMask = 0xffffffff;

							_cullMode = Cull_None;

							_depthEnable = Depth_Disable;
							_depthRef = 0;

							_viewportX = 0;
							_viewportY = 0;
							_viewportWidth = 0;
							_viewportHeight = 0;
							_viewportMinZ = 0.0f;
							_viewportMaxZ = 0.0f;

							_sampler = Sampler_Normal;
							_anisotropy = 0.0f;

							memset(_valid, 0, sizeof(_valid));
						}

						// State_Blend
						Blend _blendState;
						glm::vec4 _blendFactor;
						uint32_t _blendMask;

						// State_CullMode
						Cull _cullMode;

						// State_Viewport
						float _viewportX;
						float _viewportY;
						float _viewportWidth;
						float _viewportHeight;
						float _viewportMinZ;
						float _viewportMaxZ;

						// State_ZEnable
						Depth _depthEnable;
						uint32_t _depthRef;

						// State_Sampler
						Sampler _sampler;
						float _anisotropy;

						csmBool _valid[State_Max];    ///< 是否已设置。現在はStartFrameで一通りは呼んでいる
					};


					void StartFrame();
					void Save();
					void Restore(CubismRenderContext_SDL3* renderContext);
					void SetBlend(CubismRenderContext_SDL3* renderContext, Blend blendState, glm::vec4 blendFactor, uint32_t mask, csmBool force = false);
					void SetCullMode(CubismRenderContext_SDL3* renderContext, Cull cullFace, csmBool force = false);
					void SetViewport(CubismRenderContext_SDL3* renderContext, float left, float top, float width, float height, float zMin, float zMax, csmBool force = false);
					void SetZEnable(CubismRenderContext_SDL3* renderContext, Depth enable, uint32_t stelcilRef, csmBool force = false);
					void SetSampler(CubismRenderContext_SDL3* renderContext, Sampler sample, csmFloat32 anisotropy = 0.0f, SDL_GPUDevice* device = NULL, csmBool force = false);




				private:


					CubismRenderState_SDL3();
					~CubismRenderState_SDL3();

					//创建各种对象?
					void Create(SDL_GPUDevice* device);

					//好像用不上
					//void SaveCurrentNativeState(ID3D11Device* device, ID3D11DeviceContext* renderContext);
					//void RestoreNativeState(ID3D11Device* device, ID3D11DeviceContext* renderContext);

					Stored  _stored;

					csmVector<Stored> _pushed;

					csmVector<SDL_GPUColorTargetBlendState>  _blendStateObjects;
					csmVector<SDL_GPURasterizerState>   _rasterizeStateObjects;
					csmVector<SDL_GPUDepthStencilState> _depthStencilState;
					csmVector<SDL_GPUSampler*>   _samplerState;

					SDL_GPUDevice* _device = NULL;
				};






			}

			}
		}
	}




