#ifndef _GlobalGraphicResourceManager_h
#define _GlobalGraphicResourceManager_h


#include<SDL3/SDL.h>
#include<unordered_map>
#include<string>
#include<memory>
#include"Bindable/Bindables.h"


//struct SDL_GPUIndexBuffer
//{
//	SDL_GPUBuffer* buffer;
//	SDL_GPUIndexElementSize index_element_size;
//	uint32_t count;
//};

#define SDLGPUCHECK(condition) 






class GlobalGraphicResourceManager
{
public:
	static GlobalGraphicResourceManager& GetIns() { static GlobalGraphicResourceManager ref; return ref; }


	void SetUp(SDL_GPUDevice* device);
	void CleanUp();



	
	SDL_GPUShader* GetShaderByName(const char* shaderName);
	

	//void StoreBuffer(const char* bufferName, SDL_GPUBuffer* buffer);
	//SDL_GPUBuffer* GetBufferByName(const char* bufferName);

	SDL_GPUBuffer* CreateIndexBuffer(unsigned short* indices,int indicesCount);


	template<class T>
	static  std::shared_ptr<T> GetGlobalBindable(const char* bindableGlobalName)
	{
		auto it = GetIns().bindsMap.find(bindableGlobalName);
		if (it == GetIns().bindsMap.end())
		{
			return nullptr;
		}
		else
		{
			return std::move(std::static_pointer_cast<T>(it->second));
		}

	}
	template<class T>
	static void StoreBindable(const char* bindableGlobalName, std::shared_ptr<T>& target)
	{
		GetIns().bindsMap[bindableGlobalName] = target;
	}

private:
	SDL_GPUDevice* pDevice = nullptr;



	//着色器池
	std::unordered_map<std::string, SDL_GPUShader*> shaderPool;
	std::unordered_map<std::string, std::shared_ptr<rendering::Bindable>>bindsMap;

};





























#endif