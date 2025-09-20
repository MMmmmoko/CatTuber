#include <SDL3/SDL.h>



#include"Model/IModel.h"
#include"Model/Live2DModelBase.h"

//
//
//bool IModel::LoadFromFile(const char* path)
//{
//
//    //加载着色器源码
//
////通过SDL加载文件
//    SDL_IOStream* modelFileStream = SDL_IOFromFile(path, "r");
//    if (!modelFileStream)
//    {
//        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Open Model File Failed: %s",path);
//        return false;
//    }
//    uint64_t size = SDL_GetIOSize(modelFileStream);
//    void* buffer = SDL_malloc(size);
//    SDL_ReadIO(modelFileStream, buffer, size);
//    bool result=LoadFromMemFile((unsigned char*)buffer);
//    SDL_free(buffer);
//    SDL_CloseIO(modelFileStream); // 关闭文件流
//	return result;
//}
IModel* IModel::CreateFromFolder(const char* packPath, const char* folderInpack)
{
	IModel* pModel = NULL;
	pModel = Live2DModelBase::LoadFromFolder(packPath, folderInpack);
	if (pModel)return pModel;
	return nullptr;
}