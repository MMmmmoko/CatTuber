#ifndef _VertexBuffer_h
#define _VertexBuffer_h



#include"Bindable.h"





namespace rendering
{
	class VertexBuffer : public Bindable
	{


	public:
		virtual ~VertexBuffer();
		void Bind() override;

		void UpdateVertex(void* data, size_t dataSize);
		void PushLayout(SDL_GPUVertexElementFormat format);
		void ReleaseTransferBuffer();
	protected:
		SDL_GPUBuffer* pVertexBuffer = NULL;
		SDL_GPUTransferBuffer* pTransferBuffer = NULL;

		SDL_GPUVertexInputState inputlayout = {&_vertexBufferDescription ,1,_vertexAttr,0};//后方的_vertexBufferDescription和_vertexAttr均是_vertexInputState内部指向的内容
		SDL_GPUVertexBufferDescription _vertexBufferDescription = {0,0,SDL_GPUVertexInputRate::SDL_GPU_VERTEXINPUTRATE_VERTEX,0};//暂时应该每次渲染只需要一组顶点，后续再考虑这里改为数组
		SDL_GPUVertexAttribute _vertexAttr[4] = {};//针对live2D开发的，目前live2D顶点只有xy、uv两组(没有颜色)
	};









}





















#endif