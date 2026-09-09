#include"Util/Util.h"
#include"AppSettings.h"
#include "AppContext.h"

#ifdef SDL_PLATFORM_WINDOWS
#include <windows.h>
#endif

AppContext::AppContext()
{
	//_commonProperties = SDL_CreateProperties();
	//if (0 == _commonProperties)
	//{
	//	SDL_Log("SDL_CreateProperties failed: %s", SDL_GetError());
	//	SDL_assert(false&& "SDL_CreateProperties failed");
	//}


}

AppContext::~AppContext()
{
	if (_prefPath)
	{
		SDL_free(_prefPath);
		_prefPath = NULL;
	}
}

std::string AppContext::GetSystemVersion()
{
#ifdef SDL_PLATFORM_WINDOWS
	{
		typedef struct {
			DWORD dwOSVersionInfoSize;
			DWORD dwMajorVersion;
			DWORD dwMinorVersion;
			DWORD dwBuildNumber;
			DWORD dwPlatformId;
			WCHAR szCSDVersion[128];
		} NT_OSVERSIONINFOW;
		typedef LONG(WINAPI* RtlGetVersion_t)(NT_OSVERSIONINFOW*);


		// 动态加载 ntdll.dll
		HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
		if (!ntdll) return "Unknown";

		// 获取 RtlGetVersion 函数地址
		RtlGetVersion_t RtlGetVersionFunc = (RtlGetVersion_t)GetProcAddress(ntdll, "RtlGetVersion");
		if (!RtlGetVersionFunc) {
			FreeLibrary(ntdll);
			return "Unknown";
		}

		NT_OSVERSIONINFOW os_info = { 0 };
		os_info.dwOSVersionInfoSize = sizeof(os_info);

		// 调用函数获取真实版本
		if (RtlGetVersionFunc(&os_info) == 0) { // 0 表示成功
			DWORD  major = os_info.dwMajorVersion;
			DWORD minor = os_info.dwMinorVersion;
			DWORD build = os_info.dwBuildNumber;
			FreeLibrary(ntdll);
			char buf[128];
			SDL_snprintf(buf,sizeof(buf),"%d.%d.%d", major, minor,build);
			return buf;
		}

		FreeLibrary(ntdll);
		return "Unknown";
	}
#endif


}

const char* AppContext::GetCatTueberVersionStr()
{
	return CATTUBER_VER_STR;
}

std::string AppContext::GetLive2DVersionStr()
{
	const Live2D::Cubism::Core::csmVersion version = Live2D::Cubism::Core::csmGetVersion();
	const uint32_t major = static_cast<uint32_t>((version & 0xFF000000) >> 24);
	const uint32_t minor = static_cast<uint32_t>((version & 0x00FF0000) >> 16);
	const uint32_t patch = static_cast<uint32_t>((version & 0x0000FFFF));
	const uint32_t versionNumber = version;
	
	char buf[32];
	snprintf(buf,sizeof(buf),"%d.%d.%d (%d)", major, minor, patch, versionNumber);
	return buf;
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

const char* AppContext::GetAppLang()
{
	return AppSettings::GetIns().GetMiscLanguage().c_str();
}

const char* AppContext::GetClassicCharacterFolderPath()
{
	if (_ref()._classicCharacterFolderPath.empty())
	{
		_ref()._classicCharacterFolderPath = std::string(GetAppBasePath()) + "Resources/Character/";
	}
	return _ref()._classicCharacterFolderPath.c_str();
}

const char* AppContext::GetClassicDeskFolderPath()
{
	if (_ref()._classicDeskFolderPath.empty())
	{
		_ref()._classicDeskFolderPath = std::string(GetAppBasePath()) + "Resources/Desk/";
	}
	return _ref()._classicDeskFolderPath.c_str();
}

const char* AppContext::GetClassicHandheldItemFolderPath()
{
	if (_ref()._classicHandheldItemFolderPath.empty())
	{
		_ref()._classicHandheldItemFolderPath = std::string(GetAppBasePath()) + "Resources/HandheldItem/";
	}
	return _ref()._classicHandheldItemFolderPath.c_str();
}

const char* AppContext::GetBongoCatFolderPath()
{
	if (_ref()._BongoCatFolderPath.empty())
	{
		_ref()._BongoCatFolderPath = std::string(GetAppBasePath()) + "Resources/BongoCatMver/";
	}
	return _ref()._BongoCatFolderPath.c_str();
}

const char* AppContext::GetDecorationItemFolderPath()
{
	if (_ref()._BongoCatFolderPath.empty())
	{
		_ref()._BongoCatFolderPath = std::string(GetAppBasePath()) + "Resources/Decoration/";
	}
	return _ref()._BongoCatFolderPath.c_str();
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
