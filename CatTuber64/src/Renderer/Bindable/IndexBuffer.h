#ifndef _IndexBuffer_h
#define _IndexBuffer_h



#include"Bindable.h"





namespace rendering
{
	class IndexBuffer : public Bindable
	{
	public:
		enum GlobalIndexBuffer
		{
			GlobalIndexBuffer_FullScreenFilter,//后期滤镜
			GlobalIndexBuffer_Count
		};
		static std::shared_ptr<IndexBuffer> GetGlobalIndexBuffer(GlobalIndexBuffer bufferType);
		static std::shared_ptr<IndexBuffer> CreateFromStr(const char*);



	public:
		virtual ~IndexBuffer();
		void Bind() override;

		void UpdateIndices(const void* data, SDL_GPUIndexElementSize index_element_size, uint32_t indicesCount);
		uint32_t GetCount() { return indexCount; }


	protected:
		SDL_GPUBuffer* pIndexBuffer = NULL;
		SDL_GPUIndexElementSize elementSize;
		uint32_t indexCount=0;






	};









}





















#endif