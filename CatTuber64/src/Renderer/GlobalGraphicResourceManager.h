#ifndef _GlobalGraphicResourceManager_h
#define _GlobalGraphicResourceManager_h


#include<SDL3/SDL.h>
#include<unordered_map>
#include<string>
#include<memory>
#include"Bindable/Bindables.h"
#include"Drawable/EmptyWindowSprite.h"

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


	bool SetUp(SDL_GPUDevice* device=nullptr);
	void CleanUp();

	//获取禁止渲染时所用的纹理
	SDL_GPUTexture* GetRenderDisabledTexture();
	//获取透明窗口且窗口里无内容的时候的绘制内容
	EmptyWindowSprite* GetEmptyWindowSprite() { if (!emptyWindowSprite.Avaliable()) emptyWindowSprite.SetUp(); return &emptyWindowSprite; };
	
	//TODO 考虑避免字符串？
	SDL_GPUShader* GetShaderByName(const char* shaderName);
	

	//void StoreBuffer(const char* bufferName, SDL_GPUBuffer* buffer);
	//SDL_GPUBuffer* GetBufferByName(const char* bufferName);

	SDL_GPUBuffer* CreateIndexBuffer(unsigned short* indices,int indicesCount);


	template<class T>
	static  std::shared_ptr<T> GetGlobalBindable(const char* bindableGlobalName,bool tryCreateIfNull=true)
	{
		auto it = GetIns().bindsMap.find(bindableGlobalName);
		if (it == GetIns().bindsMap.end())
		{
			if (tryCreateIfNull)
			{
				std::shared_ptr<T>  result = T::CreateFromStr(bindableGlobalName);
				if (result)
					StoreBindable<T>(bindableGlobalName, result);
				return result;
			}
			else
			{
				return nullptr;
			}
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
	bool releaseDeviceWhenQuit = false;


	EmptyWindowSprite emptyWindowSprite;

	SDL_GPUTexture* renderDisabledTexture=nullptr;
	SDL_GPUTexture* emptyWindowTexture=nullptr;

	//着色器池
	std::unordered_map<std::string, SDL_GPUShader*> shaderPool;
	std::unordered_map<std::string, std::shared_ptr<rendering::Bindable>>bindsMap;

};





























#endif