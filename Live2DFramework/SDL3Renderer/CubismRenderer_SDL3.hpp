/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <SDL3/SDL.h>

#include "Rendering/CubismRenderer.hpp"
#include "Rendering/CubismClippingManager.hpp"
#include "CubismFramework.hpp"
#include "CubismType_SDL3.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"
#include "Math/CubismVector2.hpp"
#include "Type/csmMap.hpp"
#include "CubismOffscreenSurface_SDL3.hpp"
#include "CubismRenderContext_SDL3.hpp"
//#include "CubismRenderState_D3D11.hpp"

 //------------ LIVE2D NAMESPACE ------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {
				extern glm::mat4x4 ConvertToGLM(CubismMatrix44& mtx);
				extern CubismMatrix44 ConvertToCsmMat(glm::mat4x4& glMat);


				//  前方宣言
				class CubismRenderer_SDL3;
				class CubismShader_SDL3;
				class CubismClippingContext_SDL3;


				//有点丑陋...
				//typedef void(*MixDrawCall)(void* pThis, int layerZ, SDL_GPUGraphicsPipeline* pipeLine,
				//	uint32_t vsStartSlot, uint32_t vsBufferNum, SDL_GPUBufferBinding* vsBuffers,
				//	SDL_GPUIndexElementSize indexElementSize, SDL_GPUBufferBinding* indexBuffer, uint32_t indexCount, uint32_t indexStart,
				//	uint32_t texStartSlot, uint32_t texNum, SDL_GPUTextureSamplerBinding* textures,
				//	uint32_t vsUniformSlot, const void* vsUniformData, uint32_t vsUniformDataLength,
				//	uint32_t psUniformSlot, const void* psUniformData, uint32_t psUniformDataLength
				//);
				struct MixRenderData
				{
					uint32_t sizeOfThisStruct; int layerZ; SDL_GPUGraphicsPipeline* pipeLine;
					uint32_t vsStartSlot; uint32_t vsBufferNum; SDL_GPUBufferBinding* vsBuffers;
					SDL_GPUIndexElementSize indexElementSize; SDL_GPUBufferBinding* indexBuffer; uint32_t indexCount; uint32_t indexStart;
					uint32_t texStartSlot; uint32_t texNum; SDL_GPUTextureSamplerBinding* textures;
					uint32_t vsUniformSlot; const void* vsUniformData; uint32_t vsUniformDataLength;
					uint32_t psUniformSlot; const void* psUniformData; uint32_t psUniformDataLength;
					SDL_GPUViewport viewport;
					void(*beforeDrawCallback)(void* userData, uint64_t userData2)=NULL; void* callbackUserData; uint64_t callbackUserData2;
				};					//由于一些绘制需要额外绘制蒙版，这里也提供函数让一些动作在绘制前执行
				typedef void(*MixDrawCall)(void* pMix,void* pRenderData);

				class CubismClippingManager_SDL3 : public CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>
				{
				public:

					/**
					 * @brief   クリッピングコンテキストを作成する。モデル描画時に実行する。
					 *
					 * @param[in]   model       ->  モデルのインスタンス
					 * @param[in]   renderer    ->  レンダラのインスタンス
					 */
					 void SetupClippingContext(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext, CubismModel& model, CubismRenderer_SDL3* renderer, csmInt32 offscreenCurrent);
				};


				class CubismClippingContext_SDL3 : public CubismClippingContext
				{
					friend class CubismClippingManager_SDL3;
					friend class CubismShader_SDL3;
					friend class CubismRenderer_SDL3;

				public:
					CubismClippingContext_SDL3(CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>* manager, CubismModel& model, const csmInt32* clippingDrawableIndices, csmInt32 clipCount);
					virtual ~CubismClippingContext_SDL3();

					CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>* GetClippingManager();
					CubismClippingManager<CubismClippingContext_SDL3, CubismOffscreenSurface_SDL3>* _owner;
				};







				class CubismRenderer_SDL3 :public CubismRenderer
				{
					friend class CubismRenderer;
					friend class CubismShader_SDL3;
					friend class CubismClippingManager_SDL3;
				public:

					static void InitializeConstantSettings(csmUint32 bufferSetNum, SDL_GPUDevice* device);
					static void SetDefaultRenderState();
					static void StartFrame(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext, csmUint32 viewportWidth, csmUint32 viewportHeight);
					static void EndFrame(SDL_GPUDevice* device);

					static CubismRenderState_SDL3* GetRenderStateManager();
					static void DeleteRenderStateManager();
					static CubismShader_SDL3* GetShaderManager();
					static void DeleteShaderManager();

					static void OnDeviceLost();//貌似example没有使用这个

					static void GenerateShader(SDL_GPUDevice* device);


					static SDL_GPUDevice* GetCurrentDevice();

					virtual void Initialize(Framework::CubismModel* model)override;
					virtual void Initialize(Framework::CubismModel* model, csmInt32 maskBufferCount)override;

					//不是绑定到渲染管线，是建立模型纹理索引与具体GPU纹理之间的映射
					void BindTexture(csmUint32 modelTextureAssign, SDL_GPUTexture* textureView);
					const csmMap<csmInt32, SDL_GPUTexture*>& GetBindedTextures() const;

					void SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height);
					csmInt32 GetRenderTextureCount() const;
					CubismVector2 GetClippingMaskBufferSize() const;
					CubismOffscreenSurface_SDL3* GetMaskBuffer(csmUint32 backbufferNum, csmInt32 offscreenIndex);


					//CatTuber某些功能需要新的Draw函数
					void SetMvpMatrix(glm::mat4x4& mvpMat) { auto mat = ConvertToCsmMat(mvpMat); CubismRenderer::SetMvpMatrix(&mat); };
					void SetMixDraw(bool mixDraw) { bMixDraw= mixDraw; };
					void SetMixCallback(MixDrawCall mixDrawFunc, void* _mixDrawFuncData) {
						mixDrawCallFunc = mixDrawFunc; mixDrawFuncData = _mixDrawFuncData;
					};
				protected:
					CubismRenderer_SDL3();
					virtual ~CubismRenderer_SDL3();



					



					virtual void DoDrawModel() override;//实际的绘制动作

					void DrawMeshSDL3(const CubismModel& model, const csmInt32 index,bool mixDraw=false, void(*beforeDrawCallback)(void* userData, uint64_t userData2)=NULL);


				private:
					//设定着色器等各种渲染管线资源并绘制
					void ExecuteDrawForMask(const CubismModel& model, const csmInt32 index);
					void ExecuteDrawForDraw(const CubismModel& model, const csmInt32 index);
					void ExecuteDrawForMixDraw(const CubismModel& model, const csmInt32 index, void(*beforeDrawCallback)(void* userData, uint64_t userData2));

					void DrawDrawableIndexed(const CubismModel& model, const csmInt32 index);


					static void DoStaticRelease();
					static void ReleaseShader();


					//禁用拷贝
					CubismRenderer_SDL3(const CubismRenderer_SDL3&);
					CubismRenderer_SDL3& operator=(const CubismRenderer_SDL3&);

					//模型绘制前后要执行的代码
					void PreDraw();
					void PostDraw();

					virtual void SaveProfile() override;//绘制前保存渲染器状态
					virtual void RestoreProfile() override;//绘制后恢复渲染器状态


					void SetClippingContextBufferForMask(CubismClippingContext_SDL3* clip);
					CubismClippingContext_SDL3* GetClippingContextBufferForMask() const;
					void SetClippingContextBufferForDraw(CubismClippingContext_SDL3* clip);
					CubismClippingContext_SDL3* GetClippingContextBufferForDraw() const;
					CubismClippingContext_SDL3* GetClippingContextBufferForDelayDraw(const csmInt32 index) const;



					void CopyToBuffer(CubismRenderContext_SDL3* renderContext, csmInt32 drawAssign, const csmInt32 vcount, const csmFloat32* varray, const csmFloat32* uvarray);
					SDL_GPUTexture* GetTextureViewWithIndex(const CubismModel& model, const csmInt32 index);

					void SetBlendState(const CubismBlendMode blendMode);
					Csm::csmBool SetShader(const CubismModel& model, const csmInt32 index);

					void SetTextureView(const CubismModel& model, const csmInt32 index);

					//设置像素着色器uniform参数（constbufffer
					void SetColorConstantBuffer(CubismConstantBufferSDL3& cb, const CubismModel& model, const csmInt32 index,
						CubismTextureColor& baseColor, CubismTextureColor& multiplyColor, CubismTextureColor& screenColor);

					void SetColorChannel(CubismConstantBufferSDL3& cb, CubismClippingContext_SDL3* contextBuffer);

					void SetProjectionMatrix(CubismConstantBufferSDL3& cb, CubismMatrix44 matrix);
					void UpdateConstantBuffer(CubismConstantBufferSDL3& cb, csmInt32 index);

					void SetSamplerAccordingToAnisotropy();

					const csmBool inline IsGeneratingMask() const;

					SDL_GPUBuffer*** _vertexBuffers;         ///< 绘制buffer，没有颜色，位置+UV
					SDL_GPUTransferBuffer*** _vertexBuffers_tb=NULL;         ///< SDL3需要中转buffer来更新vertexBuffer,为了防止频繁创建，这里复用缓冲区
					SDL_GPUBuffer*** _indexBuffers;          ///< インデックスのバッファ
					//SDL_GPUBuffer*** _indexBuffers_tb;          ///< indexbuffer不会每帧更新，不复用transfer buffer
					CubismConstantBufferSDL3*** _constantBuffers;       ///< 定数のバッファ
					//虽然SDL库能实时绑定uniform data，但CatTuber中的混合绘制不是当场绘制，需要有地方保持常数缓存的数据，所以这个结构保留

					csmUint32 _drawableNum;           ///< _vertexBuffers, _indexBuffersの確保数

					csmInt32 _commandBufferNum;
					csmInt32 _commandBufferCurrent;


					csmVector<csmInt32> _sortedDrawableIndexList;       ///< 按绘制顺序排序后的绘图对象索引
					
					const int* _drawOrderList;       ///< 用于MixDraw的绘制顺序索引



					csmMap<csmInt32, SDL_GPUTexture*> _textures;

					csmVector<csmVector<CubismOffscreenSurface_SDL3>> _offscreenSurfaces;//用于蒙版绘制的帧缓冲区


					CubismClippingManager_SDL3* _clippingManager;               ///< 剪贴蒙版管理对象
					CubismClippingContext_SDL3* _clippingContextBufferForMask;  ///< 用于在遮罩纹理上绘制的剪裁上下文
					CubismClippingContext_SDL3* _clippingContextBufferForDraw;  ///< 用于在画面上绘制的裁剪上下文




					MixDrawCall mixDrawCallFunc=NULL;
					void* mixDrawFuncData = NULL;
					bool bMixDraw=false;
				};













				//------------ LIVE2D NAMESPACE ------------
			}
		}
	}
}
//------------ LIVE2D NAMESPACE ------------
