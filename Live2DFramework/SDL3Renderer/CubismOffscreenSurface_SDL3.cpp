#include "CubismOffscreenSurface_SDL3.hpp"
#include"CubismRenderer_SDL3.hpp"


//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {



				CubismOffscreenSurface_SDL3::CubismOffscreenSurface_SDL3()
					: _texture(NULL)
					, _depthTexture(NULL)
					, _backupRender(NULL)
					, _backupDepth(NULL)
					, _device(NULL)
					, _bufferWidth(0)
					, _bufferHeight(0)
				{
				}


				void CubismOffscreenSurface_SDL3::BeginDraw(CubismRenderContext_SDL3* renderContext)
				{
					//将此纹理设置为渲染目标
					//保存以往的目标以恢复
					if (_texture == NULL || _depthTexture == NULL)
						return;


					renderContext->GetRenderTargets(&_backupRender,&_backupDepth);

					renderContext->SetRenderTarget(_texture,_depthTexture);

					//renderContext->StartRender();
				}

				void CubismOffscreenSurface_SDL3::EndDraw(CubismRenderContext_SDL3* renderContext)
				{
					//恢复之前保存的渲染目标
					if (_texture == NULL || _depthTexture == NULL)
						return;

					renderContext->SetRenderTarget(_backupRender, _backupDepth);
					//renderContext->EndRender();
				}

				void CubismOffscreenSurface_SDL3::Clear(CubismRenderContext_SDL3* renderContext, float r, float g, float b, float a)
				{
					//不要用于平时Clear，这个只在渲染管线中工作
					renderContext->ClearRenderTarget( r,  g,  b,  a);
					renderContext->ClearDepth(1.f);
				}


				csmBool CubismOffscreenSurface_SDL3::CreateOffscreenSurface(SDL_GPUDevice* device, csmUint32 displayBufferWidth, csmUint32 displayBufferHeight)
				{
					//如果已有纹理，删除它
					DestroyOffscreenSurface();
					_device = device;


					//并非循环
					//不用goto挑战吗？有点意思
					do
					{

						SDL_GPUTextureCreateInfo textureDesc = {};


						textureDesc.type = SDL_GPU_TEXTURETYPE_2D;
						textureDesc.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
						textureDesc.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET| SDL_GPU_TEXTUREUSAGE_SAMPLER;
						textureDesc.width = displayBufferWidth;
						textureDesc.height = displayBufferHeight;
						textureDesc.layer_count_or_depth = 1;
						textureDesc.num_levels = 1;


#ifdef SDL_PLATFORM_WINDOWS
						

						auto props = SDL_CreateProperties();
						SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT,1.f);
						SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT, 1.f);
						SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT, 1.f);
						SDL_SetFloatProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT, 1.f);
						
						textureDesc.props = props;

#endif // 
						_texture= SDL_CreateGPUTexture(device,&textureDesc);
						if (!_texture)
						{
							CubismLogError("Error : create offscreen texture");
							break;
						}


						//深度
						SDL_GPUTextureCreateInfo depthDesc = {};
						depthDesc.type = SDL_GPU_TEXTURETYPE_2D;
						depthDesc.format = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
						depthDesc.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
						depthDesc.width = displayBufferWidth;
						depthDesc.height = displayBufferHeight;
						depthDesc.layer_count_or_depth = 1;
						depthDesc.num_levels = 1;

#ifdef SDL_PLATFORM_WINDOWS
						//其实live2d好像不用深度也行
						depthDesc.props = SDL_CreateProperties();
						SDL_SetFloatProperty(depthDesc.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_DEPTH_FLOAT, 1.f);
						SDL_SetFloatProperty(depthDesc.props, SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_STENCIL_NUMBER, 0);


#endif // 

						_depthTexture = SDL_CreateGPUTexture(device, &depthDesc);
						if (!_depthTexture)
						{
							CubismLogError("Error : create offscreen depth texture");
							break;
						}


						_bufferWidth = displayBufferWidth;
						_bufferHeight = displayBufferHeight;

						// 成功
						return true;


					} while (0);


					//创建失败则进行清理
					DestroyOffscreenSurface();


					return false;
				}

				void CubismOffscreenSurface_SDL3::DestroyOffscreenSurface()
				{
					if (_texture)
					{
						SDL_ReleaseGPUTexture(_device, _texture);
						_texture = NULL;
					}
					if (_depthTexture)
					{
						SDL_ReleaseGPUTexture(_device, _depthTexture);
						_depthTexture = NULL;
					}


				}



				csmUint32 CubismOffscreenSurface_SDL3::GetBufferWidth() const
				{
					return _bufferWidth;
				}

				csmUint32 CubismOffscreenSurface_SDL3::GetBufferHeight() const
				{
					return _bufferHeight;
				}

				csmBool CubismOffscreenSurface_SDL3::IsValid() const
				{

					if (_texture == NULL || _depthTexture == NULL)
					{
						return false;
					}
					return true;

				}
			}
		}
	}
}