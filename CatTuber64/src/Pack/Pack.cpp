#include"Pack/Pack.h"
#include <SDL3/SDL.h>
#include"Util/Util.h"


#define PACKREADER_LIST(F) \
    F(Folder)        \
    F(Pack_ver0)         \
    F(Pack_ver1)


bool Pack::Open(const char* packFilePath)
{
	
#define PACK_CHECKPATH(reader) if(PackReader_##reader::CheckPack(packFilePath)){packPath = packFilePath;type=PackType_##reader;return true;};
	PACKREADER_LIST(PACK_CHECKPATH)
#undef PACK_CHECKPATH



	return false;
}




uint8_t* Pack::LoadFile(const char* path, size_t* size)
{
	switch (type)
	{
	case Pack::PackType_Unknown:
		break;

#define PACK_LOADFILE(reader) case PackType_##reader:return PackReader_##reader::LoadFile(packPath.c_str(),path,size);


		PACKREADER_LIST(PACK_LOADFILE)

#undef PACK_LOADFILE

	//case Pack::PackType_Folder:
	//	return PackReader_Folder::LoadFile(packPath.c_str(),path,size);
	//	break;
	//case Pack::PackType_Pack_ver0:
	//	return PackReader_Folder::LoadFile(packPath.c_str(), path, size);
	//	break;
	//case Pack::PackType_Pack_ver1:
	//	break;
	default:
		break;
	}


	return nullptr;
}



bool Pack::IsFileExist(const char* path)
{
	switch (type)
	{
	case Pack::PackType_Unknown:
		break;

#define PACK_FILEEXIST(reader) case PackType_##reader:return PackReader_##reader::IsFileExist(packPath.c_str(),path);
		PACKREADER_LIST(PACK_FILEEXIST)
#undef PACK_FILEEXIST

	default:
		break;
	}
	return false;
}

//fullPathInPack 输出文件在包内的路径，为否则只输出文件名
std::vector<std::string> Pack::GetFileList(const char* folerPath, bool fullPathInPack)
{
	switch (type)
	{
	case Pack::PackType_Unknown:
		break;

#define PACK_GETFILELIST(reader) case PackType_##reader:return PackReader_##reader::GetFileList(packPath.c_str(),folerPath,fullPathInPack);
		PACKREADER_LIST(PACK_GETFILELIST)
#undef PACK_GETFILELIST

	default:
		break;
	}
	return std::vector<std::string>();
}


void Pack::ReleaseMem(uint8_t* mem)
{
	switch (type)
	{
	case Pack::PackType_Unknown:
		break;

#define PACK_RELEASEMEM(reader) case PackType_##reader: PackReader_##reader::ReleaseMem(mem);return;
		PACKREADER_LIST(PACK_RELEASEMEM)
#undef PACK_RELEASEMEM

	default:
		break;
	}
	return;
}


bool PackReader_Folder::CheckPack(const char* packFilePath)
{
	//给定路径判断是否为文件夹即可
	SDL_PathInfo pathInfo;
	bool result = SDL_GetPathInfo(packFilePath, &pathInfo);
	if (result)
	{
		if (pathInfo.type == SDL_PathType::SDL_PATHTYPE_DIRECTORY)
		{
			
			return true;
		}
	}
	return false;
}

uint8_t* PackReader_Folder::LoadFile(const char* packPath, const char* path, size_t* size)
{
	std::string pathStr = packPath; 
	pathStr=pathStr +"/" + path;
	return util::SDL_LoadFileToMem(pathStr.c_str(), size);
}

bool PackReader_Folder::IsFileExist(const char* packPath, const char* path)
{
	std::string pathStr = packPath;
	pathStr = pathStr + "/" + path;
	return  SDL_GetPathInfo(pathStr.c_str(), NULL);
}

std::vector<std::string> PackReader_Folder::GetFileList(const char* packPath, const char* folerPath, bool fullPathInPack)
{
	//获取某个文件夹中的文件，不读取子文件夹
	std::vector<std::string> resultVec;

	struct _TEMSTRUCT
	{
		std::vector<std::string>* pVec;
	}tem = {&resultVec };

	//dirname以斜杠结尾
	SDL_EnumerateDirectoryCallback fileCallBack = [](void* userdata, const char* dirname, const char* fname) {

		_TEMSTRUCT* ptem = (_TEMSTRUCT*)userdata;

			ptem->pVec->push_back(fname);
		return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
		};



	//需要遍历的目录
	std::string targetPath = packPath;
	targetPath = targetPath + "/" + folerPath;

	if (!SDL_EnumerateDirectory(targetPath.c_str(), fileCallBack, &tem))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Can not enumerate directory : %s", targetPath.c_str());
	};
	
	if (fullPathInPack)
	{
		for (auto& x : resultVec)
		{
			x = UTIL_FOLDER_PATH_WITH_SEPARATOR(folerPath) + x;
		}
	}


	return resultVec;
}

void PackReader_Folder::ReleaseMem(uint8_t* mem)
{
	util::SDL_FreeMem(mem);
}
