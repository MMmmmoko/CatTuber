#pragma once

#include<SDL3/SDL.h>
#include"CubismRenderContext_SDL3.hpp"
#include "Math/CubismMatrix44.hpp"


//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {


				class CubismRenderContext_SDL3;
				class CubismOffscreenSurface_SDL3
				{
				public:

					CubismOffscreenSurface_SDL3();

					//应该不需要Clear，Clear发生在renderpass的loadop中
					//void Clear(ID3D11DeviceContext* renderContext, float r, float g, float b, float a);

					void BeginDraw(CubismRenderContext_SDL3*);
					void EndDraw(CubismRenderContext_SDL3*);
					//void SetAsRenderTarget();

					//SDL3不直接Clear而是在管钱的起始阶段Clear，所以这个Clear不会实时生效，并且不能独立使用
					void Clear(CubismRenderContext_SDL3* renderContext, float r, float g, float b, float a);



					csmBool CreateOffscreenSurface(SDL_GPUDevice* device, csmUint32 displayBufferWidth, csmUint32 displayBufferHeight);
					void DestroyOffscreenSurface();

					SDL_GPUTexture* GetTextureView() const { return _texture; };

					csmUint32 GetBufferWidth() const;
					csmUint32 GetBufferHeight() const;

					csmBool IsValid() const;


				private:
					SDL_GPUTexture* _texture;
					SDL_GPUTexture* _depthTexture;
					SDL_GPUTexture* _backupRender;//离屏渲染完需要回退到原渲染目标，即窗口或Live2D外的渲染目标，不回退的话会渲染错乱
					SDL_GPUTexture* _backupDepth;

					SDL_GPUDevice* _device;

					csmUint32                   _bufferWidth;     
					csmUint32                   _bufferHeight;   
				};
			}
		}
	}
}