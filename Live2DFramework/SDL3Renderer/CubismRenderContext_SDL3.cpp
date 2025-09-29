#include"CubismRenderContext_SDL3.hpp"
#include"CubismRenderer_SDL3.hpp"





namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {


				static CubismRenderContext_SDL3 rc = {};
				CubismRenderContext_SDL3* CubismRenderContext_SDL3::CreateSDLGpuRenderContext(SDL_GPUDevice* device)
				{
					//Live2D内部使用的话，这样就够了，以后有需要再改这里
					//static CubismRenderContext_SDL3 rc = {};
					rc._device = device;
					return &rc;
				}

				void CubismRenderContext_SDL3::ReleaseSDLGpuRenderContext(CubismRenderContext_SDL3* context)
				{
					
					//释放RenderContext,将不再可用
					if (!rc._device)return;
					for (auto& x : rc.pipelinePool)
					{
						SDL_GPUGraphicsPipeline* curPipeline = x.second;
						if (curPipeline)
						{
							SDL_ReleaseGPUGraphicsPipeline(rc._device, curPipeline);
						}
					}
					rc.pipelinePool.clear();

				}


				void CubismRenderContext_SDL3::StartFrame(SDL_GPUCommandBuffer* cmdCurFrame, SDL_GPURenderPass* mainRenderPass, SDL_GPUCommandBuffer* copyBuffer)
				{

					_cmd = cmdCurFrame;
					_cmd_copy = copyBuffer;

					_pass_orig = mainRenderPass;
				}

				void CubismRenderContext_SDL3::EndFrame()
				{
					//每帧重设渲染目标
					_renderTarget = NULL;
					_depthStencil = NULL;


					if(_pass_orig)
						SDL_EndGPURenderPass(_pass_orig);

					//RenderPass和CommandBuffer是外面传进来的，在外面提交
					//SDL_EndGPURenderPass(_pass_orig);

					//// 提交命令缓冲区
					//SDL_SubmitGPUCommandBuffer(_cmd);
					_pass_orig = NULL;
				}



				void CubismRenderContext_SDL3::StartRender()
				{

					//
					if (_renderTarget || _depthStencil)
					{
						SDL_GPUColorTargetInfo colorTargetInfo = {};
						colorTargetInfo.texture = _renderTarget;
						colorTargetInfo.load_op = _needClearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
						colorTargetInfo.clear_color = _clearColor;
						colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

						if (!_depthStencil)
						{

							//_pass = SDL_BeginGPURenderPass(_cmd, &colorTargetInfo, 1, NULL);
							//离屏渲染用copycmd
							_pass = SDL_BeginGPURenderPass(_cmd_copy, &colorTargetInfo, 1, NULL);
						}
						else
						{
							SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {};
							depthStencilTargetInfo.texture = _depthStencil;
							depthStencilTargetInfo.load_op = _needClearDepth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
							depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
							depthStencilTargetInfo.clear_depth = _clearDepth;
							depthStencilTargetInfo.stencil_load_op = _needClearStencil ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
							depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;
							depthStencilTargetInfo.clear_stencil = _clearStencil;

							_pass = SDL_BeginGPURenderPass(_cmd_copy, &colorTargetInfo, 1, &depthStencilTargetInfo);
						}
					}
					else
					{
						//_renderTarget和_depthStencil同时为空时表明在主RenderPass上渲染
						_pass = _pass_orig;
					}






					//if (_renderTarget || _depthStencil)
					//{
					//	if (_pass_orig)
					//	{
					//		SDL_EndGPURenderPass(_pass_orig);
					//		_pass_orig = 0;
					//	}
					//	SDL_GPUColorTargetInfo colorTargetInfo = {};
					//	colorTargetInfo.texture = _renderTarget;
					//	colorTargetInfo.load_op = _needClearColor ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
					//	colorTargetInfo.clear_color = _clearColor;
					//	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

					//	if (!_depthStencil)
					//	{

					//		_pass = SDL_BeginGPURenderPass(_cmd, &colorTargetInfo, 1, NULL);
					//	}
					//	else
					//	{
					//		SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = {};
					//		depthStencilTargetInfo.texture = _depthStencil;
					//		depthStencilTargetInfo.load_op = _needClearDepth ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
					//		depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
					//		depthStencilTargetInfo.clear_depth = _clearDepth;
					//		depthStencilTargetInfo.stencil_load_op = _needClearStencil ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
					//		depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_STORE;
					//		depthStencilTargetInfo.clear_stencil = _clearStencil;

					//		_pass = SDL_BeginGPURenderPass(_cmd, &colorTargetInfo, 1, &depthStencilTargetInfo);
					//	}
					//}
					//else
					//{
					//	if (!_pass_orig)
					//		_RebuildRenderPass();

					//	//_renderTarget和_depthStencil为空说明在原目标上渲染
					//	_pass = _pass_orig;
					//}




				}

				void CubismRenderContext_SDL3::EndRender()
				{
					//仅end离屏渲染
					//if (_pass && !_pass_orig)
					//{
					//	SDL_EndGPURenderPass(_pass);
					//	_RebuildRenderPass();
					//}
					////_pass = NULL;
					////_cmd = NULL;
					//_needClearColor = false;
					//_needClearDepth = false;
					//_needClearStencil = false;

					//仅end离屏渲染

					if (_renderTarget || _depthStencil)
					{
						SDL_EndGPURenderPass(_pass);
						_pass = NULL;
					}
				}

				void CubismRenderContext_SDL3::SetRenderTarget(SDL_GPUTexture* renderTex, SDL_GPUTexture* _depthTex)
				{
					_renderTarget = renderTex;
					_depthStencil = _depthTex;
				}

				void CubismRenderContext_SDL3::GetRenderTargets(SDL_GPUTexture** renderTex, SDL_GPUTexture** depthTex)
				{
					if (renderTex)*renderTex = _renderTarget;
					if (depthTex)*depthTex = _depthStencil;
				}

				void CubismRenderContext_SDL3::SetBlendState(CubismRenderState_SDL3::Blend blend)
				{
					_pipelineState._blend = blend;
				}

				void CubismRenderContext_SDL3::SetCullMode(CubismRenderState_SDL3::Cull cullmode)
				{
					_pipelineState._cullmode = cullmode;
				}

				void CubismRenderContext_SDL3::SetViewport(SDL_GPUViewport* pviewPort)
				{
					viewPort = *pviewPort;
				}

				void CubismRenderContext_SDL3::SetZEnable(CubismRenderState_SDL3::Depth depthEnable)
				{
					_pipelineState._depthEnable = depthEnable;
				}

				void CubismRenderContext_SDL3::SetTopology(SDL_GPUPrimitiveType topology)
				{
					_pipelineState._topology = _pipelineState._topology;
				}

				void CubismRenderContext_SDL3::SetVertexShader(SDL_GPUShader* vs)
				{
					_pipelineState._vertexShader = vs;
				}

				void CubismRenderContext_SDL3::SetVertexBuffers(uint32_t startSlot, uint32_t numBuffers, SDL_GPUBufferBinding* buffers)
				{
					SDL_BindGPUVertexBuffers(_pass, startSlot, buffers, numBuffers);
				}

				void CubismRenderContext_SDL3::SetIndexBuffer(SDL_GPUBuffer* indexBuffer, SDL_GPUIndexElementSize index_element_size, uint32_t offset)
				{
					SDL_GPUBufferBinding binding;
					binding.buffer = indexBuffer;
					binding.offset = offset;
					SDL_BindGPUIndexBuffer(_pass, &binding, index_element_size);
				}

				//void CubismRenderContext_SDL3::SetVertexUniformData(uint32_t slot, const void* data, uint32_t datalength)
				//{
				//	//SDL_assert(_cmd&&"command buffer should not be null");
				//	//修改为多缓冲后，需要区分缓冲区.....
				//	if(_renderTarget || _depthStencil)
				//		SDL_PushGPUVertexUniformData(_cmd_copy, slot, data, datalength);
				//	else
				//		SDL_PushGPUVertexUniformData(_cmd, slot, data, datalength);
				//}

				void CubismRenderContext_SDL3::SetVertexConstantBuffer(uint32_t startSlot, uint32_t numBuffers, SDL_GPUBuffer** buffers)
				{

					SDL_BindGPUVertexStorageBuffers(_pass,startSlot,buffers, numBuffers);
				}

				void CubismRenderContext_SDL3::SetInputLayout(SDL_GPUVertexInputState* inputstate)
				{
					//深复制
					_pipelineState._vertexInputState.num_vertex_attributes = inputstate->num_vertex_attributes;
					_pipelineState._vertexInputState.num_vertex_buffers = inputstate->num_vertex_buffers;
					for (int i = 0; i < inputstate->num_vertex_attributes; i++)
					{
						_pipelineState._vertexAttr[i] = inputstate->vertex_attributes[i];
					}

					//为防止检索混乱（unordered map可能会因为_vertexAttr后多余的无效数据而检索到不同位置）对无效的数据均进行清除
					for (int i = inputstate->num_vertex_attributes; i < 2; i++)
					{
						_pipelineState._vertexAttr[i] = {};
					}

					_pipelineState._vertexBufferDescription = *inputstate->vertex_buffer_descriptions;


					_pipelineState._vertexInputState.vertex_attributes = _pipelineState._vertexAttr;
					_pipelineState._vertexInputState.vertex_buffer_descriptions = &_pipelineState._vertexBufferDescription;
				}

				void CubismRenderContext_SDL3::SetFragmentShader(SDL_GPUShader* fs)
				{

					_pipelineState._fragmentShader = fs;
				}

				void CubismRenderContext_SDL3::SetFragmentTextureResources(uint32_t startSlot, uint32_t numViews, const SDL_GPUTextureSamplerBinding* textures)
				{
					SDL_BindGPUFragmentSamplers(_pass, startSlot, textures, numViews);
				}

				//void CubismRenderContext_SDL3::SetFragmentUniformData(uint32_t slot, const void* data, uint32_t datalength)
				//{
				//	if (_renderTarget || _depthStencil)
				//		SDL_PushGPUFragmentUniformData(_cmd_copy, slot, data, datalength);
				//	else
				//		SDL_PushGPUVertexUniformData(_cmd, slot, data, datalength);
				//}
				void CubismRenderContext_SDL3::SetFragmentConstantBuffer(uint32_t startSlot, uint32_t numBuffers, SDL_GPUBuffer** buffers)
				{

					SDL_BindGPUFragmentStorageBuffers(_pass, startSlot, buffers, numBuffers);
				}

				SDL_GPUSampler* CubismRenderContext_SDL3::GetFragmentSampler()
				{
					CubismRenderState_SDL3* rsm = CubismRenderer_SDL3::GetRenderStateManager();
					return rsm->_samplerState[rsm->_stored._sampler];
				}

				void CubismRenderContext_SDL3::ClearRenderTarget(float r, float g, float b, float a)
				{
					_clearColor = { r,g,b,a };
					_needClearColor = true;
				}

				void CubismRenderContext_SDL3::ClearDepth(float depth)
				{
					_clearDepth = depth;
					_needClearDepth = true;
				}

				void CubismRenderContext_SDL3::ClearStencil(uint8_t stencil)
				{
					_clearStencil = stencil;
					_needClearStencil = true;
				}

				void CubismRenderContext_SDL3::DrawIndexed(uint32_t count, uint32_t startIndex)
				{
					if (count == 0)return;

					//根据_PipelineState检索Pipeline

					auto& targetPipeline = pipelinePool[_pipelineState];
					if (targetPipeline == NULL)
					{
						//为空，根据pipelineStates创建

						SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {};
						pipelineCreateInfo.vertex_shader = _pipelineState._vertexShader;
						pipelineCreateInfo.fragment_shader = _pipelineState._fragmentShader;
						pipelineCreateInfo.vertex_input_state = _pipelineState._vertexInputState;
						pipelineCreateInfo.primitive_type = _pipelineState._topology;


						CubismRenderState_SDL3* rsm = CubismRenderer_SDL3::GetRenderStateManager();
						pipelineCreateInfo.rasterizer_state = rsm->_rasterizeStateObjects[_pipelineState._cullmode];
						pipelineCreateInfo.depth_stencil_state = rsm->_depthStencilState[_pipelineState._depthEnable];

						SDL_GPUColorTargetDescription colorDesc = {};
						colorDesc.blend_state = rsm->_blendStateObjects[_pipelineState._blend];
						colorDesc.format =(_renderTarget==NULL)? SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM : SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
						pipelineCreateInfo.target_info.color_target_descriptions = &colorDesc;
						pipelineCreateInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
						pipelineCreateInfo.target_info.num_color_targets = 1;
						pipelineCreateInfo.target_info.has_depth_stencil_target = _pipelineState._depthEnable == CubismRenderState_SDL3::Depth_Enable;

						SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(_device, &pipelineCreateInfo);
						if (pipeline == NULL) {
							SDL_LogError(SDL_LOG_CATEGORY_RENDER, "pipeline create failed! %s",SDL_GetError());
							//SDL_assert(_cmd && "pipeline create failed");
							return;
						}
						targetPipeline = pipeline;
					}



					SDL_SetGPUViewport(_pass, &viewPort);

					SDL_BindGPUGraphicsPipeline(_pass, targetPipeline);
					SDL_DrawGPUIndexedPrimitives(_pass,count,1, startIndex,0,0 );
				}

				SDL_GPUGraphicsPipeline* CubismRenderContext_SDL3::GetPipelineFromCurState()
				{
					auto& targetPipeline = pipelinePool[_pipelineState];
					if (targetPipeline == NULL)
					{
						//为空，根据pipelineStates创建

						SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {};
						pipelineCreateInfo.vertex_shader = _pipelineState._vertexShader;
						pipelineCreateInfo.fragment_shader = _pipelineState._fragmentShader;
						pipelineCreateInfo.vertex_input_state = _pipelineState._vertexInputState;
						pipelineCreateInfo.primitive_type = _pipelineState._topology;


						CubismRenderState_SDL3* rsm = CubismRenderer_SDL3::GetRenderStateManager();
						pipelineCreateInfo.rasterizer_state = rsm->_rasterizeStateObjects[_pipelineState._cullmode];
						pipelineCreateInfo.depth_stencil_state = rsm->_depthStencilState[_pipelineState._depthEnable];

						SDL_GPUColorTargetDescription colorDesc = {};
						colorDesc.blend_state = rsm->_blendStateObjects[_pipelineState._blend];
						colorDesc.format = (_renderTarget == NULL) ? SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM : SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
						pipelineCreateInfo.target_info.color_target_descriptions = &colorDesc;
						pipelineCreateInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
						pipelineCreateInfo.target_info.num_color_targets = 1;
						pipelineCreateInfo.target_info.has_depth_stencil_target = _pipelineState._depthEnable == CubismRenderState_SDL3::Depth_Enable;

						SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(_device, &pipelineCreateInfo);
						if (pipeline == NULL) {
							SDL_LogError(SDL_LOG_CATEGORY_RENDER, "pipeline create failed");
							//SDL_assert(_cmd && "pipeline create failed");
							return NULL;
						}
						targetPipeline = pipeline;
					}
					return targetPipeline;

				}









				//inline void hash_combine(size_t& seed, size_t h) {
				//    // 0x9e3779b97f4a7c15 是 64 位黄金分割常量
				//    seed ^= h + 0x9e3779b97f4a7c15ULL
				//        + (seed << 6)
				//        + (seed >> 2);
				//}


				//size_t CubismRenderContext_SDL3::_PipelineState::operator()(_PipelineState const& s) const noexcept
				//{
				//    size_t seed = 0;
				//    hash_combine(seed, std::hash<int>()(int(s._blend)));
				//    hash_combine(seed, std::hash<int>()(int(s._cullmode)));
				//    hash_combine(seed, std::hash<int>()(int(s._depthEnable)));
				//    hash_combine(seed, std::hash<void*>()(s._vertexShader));
				//    hash_combine(seed, std::hash<void*>()(s._fragmentShader));

				//    //剩下的参数应该是不需要参与哈希计算的（输入布局与顶点着色器强绑定）
				//    //_topology可以计算一下（但在live2D中没有用到过三角形list以外的topology，所以这里忽略）
				//    //hash_combine(seed, std::hash<int>()(int(s._topology)));
				//    return seed;
				//}

				//bool CubismRenderContext_SDL3::_PipelineState::operator==(_PipelineState const& o) const noexcept
				//{
				//    return false;
				//}

				//bool CubismRenderContext_SDL3::_PipelineState::operator()(const _PipelineState& a, const _PipelineState& b)
				//{
				//    return false;
				//}

			}
		}
	}
}