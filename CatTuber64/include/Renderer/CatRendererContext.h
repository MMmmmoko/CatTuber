#ifndef _CatRendererContext_h
#define _CatRendererContext_h


//对SDL渲染进行一次封装


#include<SDL3/SDL.h>

#include<unordered_map>


struct CatRendererPipelineState
{
	//CatTuber先使用的是Live2D模型，所以这里参考live2d定义，后续拓展spine/3D模型再按需拓展或映射
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
	enum Depth
	{
		Depth_Origin,   ///< 原始设置
		Depth_Disable,  ///< Zoff
		Depth_Enable,   ///< Zon
		Depth_Max,
	};

	Blend _blend;
	Cull _cullmode;
	Depth _depthEnable;
	SDL_GPUPrimitiveType _topology;
	SDL_GPUShader* _vertexShader = NULL;
	SDL_GPUShader* _fragmentShader = NULL;


	//输入布局
	SDL_GPUVertexInputState _vertexInputState;
	SDL_GPUVertexBufferDescription _vertexBufferDescription;
	SDL_GPUVertexAttribute _vertexAttr[2];//针对live2D开发的，目前live2D顶点只有xy、uv两组(没有颜色)
	//^^^后续可能会修改封装以适应3D

	//用于hash的函数
	//size_t operator()(_PipelineState const&) const noexcept;
	bool operator==(const CatRendererPipelineState& o) const noexcept
	{

		return memcmp(this, &o,
			sizeof(_blend) + sizeof(_cullmode) + sizeof(_depthEnable) + sizeof(_topology)
			+ sizeof(_vertexShader) + sizeof(_fragmentShader)
		) == 0;
	}
};

struct CatRenderPassState
{
	SDL_GPUColorTargetInfo colorTargetInfo;
	SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo;
};

namespace std
{
	inline void _hash_combine(size_t& seed, size_t h) {
		// 0x9e3779b97f4a7c15 是 64 位黄金分割常量
		seed ^= h + 0x9e3779b97f4a7c15ULL
			+ (seed << 6)
			+ (seed >> 2);
	};
	template<>
	struct hash<CatRendererPipelineState>
	{
		size_t operator() (const CatRendererPipelineState& s) const noexcept
		{

			size_t seed = 0;
			_hash_combine(seed, std::hash<int>()(int(s._blend)));
			_hash_combine(seed, std::hash<int>()(int(s._cullmode)));
			_hash_combine(seed, std::hash<int>()(int(s._depthEnable)));
			_hash_combine(seed, std::hash<void*>()(s._vertexShader));
			_hash_combine(seed, std::hash<void*>()(s._fragmentShader));

			//剩下的参数应该是不需要参与哈希计算的（输入布局与顶点着色器强绑定）
			//_topology可以计算一下（但在live2D中没有用到过三角形list以外的topology，所以这里忽略）
			//_hash_combine(seed, std::hash<int>()(int(s._topology)));
			return seed;
		}
	}; // 间接调用原生Hash.

	template<>
	struct hash<CatRenderPassState>
	{
		size_t operator() (const CatRenderPassState& s) const noexcept
		{
			size_t seed = 0;
			_hash_combine(seed, std::hash<void*>()(s.colorTargetInfo.texture));
			_hash_combine(seed, std::hash<void*>()(s.depthStencilTargetInfo.texture));
			_hash_combine(seed, std::hash<int>()(s.colorTargetInfo.load_op));
			_hash_combine(seed, std::hash<int>()(s.depthStencilTargetInfo.load_op));

			return seed;
		}
	};
}

//对SDL顶点绑定所需参数进行一下封装
//optimize: needRebinding可以改进为同时判断是否需要绑定、如果需要则进行绑定、如果数值改变则重新赋值
struct CatVertexBufferBinding
{
	uint32_t startSlot;
	uint32_t numBuffers;
	SDL_GPUBufferBinding buffers[4];

	bool needRebinding(const CatVertexBufferBinding* anotherBinding)
	{
		if (startSlot == anotherBinding->startSlot&& numBuffers== anotherBinding->numBuffers)
		{
			for (uint32_t i = 0; i < numBuffers; i++)
			{
				if (buffers[i].buffer != anotherBinding->buffers[i].buffer
					|| buffers[i].offset!= anotherBinding->buffers[i].offset)
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
};
struct CatIndexBufferBinding
{
	SDL_GPUIndexElementSize index_element_size;
	SDL_GPUBufferBinding buffer;

	bool needRebinding(const CatIndexBufferBinding* anotherBinding)
	{
		if (index_element_size == anotherBinding->index_element_size
			&& buffer.buffer == anotherBinding->buffer.buffer
			&& buffer.offset == anotherBinding->buffer.offset)
		{
			return false;
		}
		return true;
	}
};
struct CatTextureBinding
{
	uint32_t startSlot;
	uint32_t numTextures;
	SDL_GPUTextureSamplerBinding textures[8];

	bool needRebinding(const CatTextureBinding* anotherBinding)
	{
		if (numTextures == anotherBinding->numTextures)
		{
			for (uint32_t i = 0; i < numTextures; i++)
			{
				if (textures[i].texture != anotherBinding->textures[i].texture|| textures[i].sampler!= anotherBinding->textures[i].sampler)
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
};
//需要修改Live2D渲染相关部分,让UniformData数据持久化？
struct CatUniformDataBinding
{
	uint32_t slot;
	const void* data=NULL;
	uint32_t datalength;

	bool needRebinding(const CatUniformDataBinding* anotherBinding)
	{
		return SDL_memcmp(this, anotherBinding,sizeof(CatUniformDataBinding))!=0;
	}
};













#endif