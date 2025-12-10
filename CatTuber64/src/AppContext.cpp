#include"Util/Util.h"
#include "AppContext.h"

AppContext::~AppContext()
{
	if (_prefPath)
	{
		SDL_free(_prefPath);
		_prefPath = NULL;
	}
}

const char* AppContext::GetPrefPath()
{
	if (!_ref()._prefPath)
	{
		_ref()._prefPath = SDL_GetPrefPath(CATTUBER_ORGNAME,CATTUBER_APPNAME);
	}
	return _ref()._prefPath;
}

const char* AppContext::GetAppBasePath()
{
	return SDL_GetBasePath();
}

const char* AppContext::GetSceneFolderPath()
{
	if (_ref()._sceneFolderPath.empty())
	{
		_ref()._sceneFolderPath = std::string(GetPrefPath()) + "Scenes/";
	}
	return _ref()._sceneFolderPath.c_str();
}

const char* AppContext::GetWorkShopPath()
{
	//TODD/Fixme:创意工坊
	SDL_assert(false);
	return nullptr;
}

std::string AppContext::ResolvePathToAbsolute(const std::string& pathToResolve)
{
	std::string path = pathToResolve;

	std::string prefpath = GetPrefPath();
	if (!prefpath.empty())
	{
		prefpath.pop_back();
		util::ReplaceString(path, "[AppPrefPath]", prefpath.c_str());
	}

	std::string basePath = GetAppBasePath();
	if (!basePath.empty())
	{
		basePath.pop_back();
		util::ReplaceString(path, "[AppBasePath]", basePath.c_str());
	}

	//TODO/FIXEME 添加创意工坊功能时取消注释
	//std::string workShopPath = GetWorkShopPath();
	//if (!workShopPath.empty())
	//{
	//	workShopPath.pop_back();
	//	util::ReplaceString(path, "[WorkshopPath]", workShopPath.c_str());
	//}

	return path;
}

std::string AppContext::ResolvePathToRelative(const std::string& pathToResolve)
{

	std::string pathStr = pathToResolve;
	util::ReplaceString(pathStr, "\\", "/");
	{
		std::string prefpath = GetPrefPath();
		if (!prefpath.empty())
		{
			util::ReplaceString(prefpath, "\\", "/");
			prefpath.pop_back();

			if (pathStr.find(prefpath) == 0)
			{
				pathStr = "[AppPrefPath]" + pathStr.substr(prefpath.size());
				return pathStr;
			}
		}
	}
	{
		std::string basePath = GetAppBasePath();
		if (!basePath.empty())
		{
			util::ReplaceString(basePath, "\\", "/");
			basePath.pop_back();
			if (pathStr.find(basePath) == 0)
			{
				pathStr = "[AppBasePath]" + pathStr.substr(basePath.size());
				return pathStr;
			}
		}
	}
	{
		std::string workShopPath = GetWorkShopPath();
		if (!workShopPath.empty())
		{
			util::ReplaceString(workShopPath, "\\", "/");
			workShopPath.pop_back();
			if (pathStr.find(workShopPath) == 0)
			{
				pathStr = "[WorkshopPath]" + pathStr.substr(workShopPath.size());
				return pathStr;
			}
		}
	}
	return pathToResolve;

}

void AppContext::LogFunc(const char* log)
{
	SDL_Log("%s", log);
}

unsigned char* AppContext::LoadFileFunc(const std::string filePath, size_t* outSize)
{
	//后续Fullback
	return util::SDL_LoadFileToMem(filePath.c_str(), outSize);;
}

void AppContext::ReleaseBytesFunc(unsigned char* byteData)
{
	SDL_free(byteData);
}

void* AppContext::AllocFunc(size_t size)
{
	return SDL_malloc(size);
}

void AppContext::DeallocFunc(void* memory)
{
	SDL_free(memory);
}

void* AppContext::AllocFunc_Aligned(size_t size, uint32_t alignment)
{
	return SDL_aligned_alloc( alignment,  size);;
}

void AppContext::DeallocFunc_Aligned(void* alignedMemory)
{
	SDL_aligned_free(alignedMemory);
}
