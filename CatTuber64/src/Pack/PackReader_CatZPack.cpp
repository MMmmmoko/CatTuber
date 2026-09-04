#include"Pack/Pack.h"
#include <fstream>
#include <SDL3/SDL.h>
#include"Util/Util.h"


//使用duilib中的zip文件

#include"duilib/third_party/zlib/contrib/minizip/unzip.h"








bool PackReader_CatZPack::CheckPack(const char* packFilePath)
{
	//检查zip文件魔数

	std::ifstream file(packFilePath, std::ios::binary);
	if (!file) {
		return false; // 文件无法打开或不存在
	}

	// 读取文件的前4个字节
	std::vector<unsigned char> header(4);
	file.read(reinterpret_cast<char*>(header.data()), 4);

	// 检查是否读取了足够的字节，并且匹配 ZIP 的文件头魔数
	// ZIP 文件头的魔数为：PK\x03\x04 (即 0x50, 0x4B, 0x03, 0x04)
	return file.gcount() == 4 &&
		header[0] == 0x50 &&
		header[1] == 0x4B &&
		header[2] == 0x03 &&
		header[3] == 0x04;
}



std::vector<uint8_t> PackReader_CatZPack::LoadFile(const char* packPath, const char* path)
{
	// 1. 打开 ZIP 压缩包
	std::vector<uint8_t> result;
	unzFile uf = unzOpen(packPath);
	if (!uf) {
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not open zipfile : %s", packPath);
		return result;
	}

	// 2. 定位目标文件 (注意路径分隔符必须是 '/')
	if (unzLocateFile(uf, path, 0) != UNZ_OK) {
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not find file %s in zipfile %s", path, packPath);
		unzClose(uf);
		return result;
	}

	// 3. 打开当前文件
	if (unzOpenCurrentFile(uf) != UNZ_OK) {
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not open file %s in zipfile %s", path, packPath);
		unzClose(uf);
		return result;
	}

	// 4. 读取数据
	int bytesRead = 0;

	// 获取当前文件的元信息，拿到解压后的原始大小
	unz_file_info fileInfo;
	if (unzGetCurrentFileInfo(uf, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK) {
		// 预分配刚好够用的内存空间
		result.resize(fileInfo.uncompressed_size);


		// 一次性读取所有数据
		int bytesRead = unzReadCurrentFile(uf, result.data(), static_cast<unsigned int>(fileInfo.uncompressed_size));
		if (bytesRead < 0 || static_cast<unsigned long>(bytesRead) != fileInfo.uncompressed_size) {
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Error Occurred When Read file %s in zipfile %s", path, packPath);
			result.clear();
		}
	}

	// 5. 关闭资源
	unzCloseCurrentFile(uf);
	unzClose(uf);

	return result;

}


uint8_t* PackReader_CatZPack::LoadFile(const char* packPath, const char* path, size_t* size)
{
	// 1. 打开 ZIP 压缩包
	unzFile uf = unzOpen(packPath);
	if (!uf) {
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not open zipfile : %s", packPath);
		return nullptr;
	}

	// 2. 定位目标文件 (注意路径分隔符必须是 '/')
	if (unzLocateFile(uf, path, 0) != UNZ_OK) {
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not find file %s in zipfile %s", path,packPath);
		unzClose(uf);
		return nullptr;
	}

	// 3. 打开当前文件
	if (unzOpenCurrentFile(uf) != UNZ_OK) {
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not open file %s in zipfile %s", path, packPath);
		unzClose(uf);
		return nullptr;
	}

	// 4. 读取数据
	int bytesRead = 0;

	// 获取当前文件的元信息，拿到解压后的原始大小
	unz_file_info fileInfo;
	uint8_t* resultBuffer = nullptr;
	if (unzGetCurrentFileInfo(uf, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK) {
		// 预分配刚好够用的内存空间
		//result.resize(fileInfo.uncompressed_size);
		resultBuffer = new uint8_t[fileInfo.uncompressed_size];
		*size = fileInfo.uncompressed_size;

		// 一次性读取所有数据
		int bytesRead = unzReadCurrentFile(uf, resultBuffer, static_cast<unsigned int>(fileInfo.uncompressed_size));
		if (bytesRead < 0 || static_cast<unsigned long>(bytesRead) != fileInfo.uncompressed_size) {
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Error Occurred When Read file %s in zipfile %s", path, packPath);
			delete[] resultBuffer;
			resultBuffer = NULL;
		}
	}

	// 5. 关闭资源
	unzCloseCurrentFile(uf);
	unzClose(uf);

	return resultBuffer;
}

bool PackReader_CatZPack::IsFileExist(const char* packPath, const char* path)
{
	unzFile uf = unzOpen(packPath);
	if (!uf) return false;

	// 尝试定位文件。第三个参数传 1 表示忽略大小写，传 0 表示区分大小写
	bool exists = (unzLocateFile(uf, path, 1) == UNZ_OK);

	unzClose(uf);
	return exists;
}

std::vector<std::string> PackReader_CatZPack::GetFileList(const char* packPath, const char* folerPath, bool fullPathInPack)
{
	//获取某个文件夹中的文件，不读取子文件夹
	std::vector<std::string> allFiles;

	unzFile uf = unzOpen(packPath);
	if (!uf) return allFiles;

	unz_file_info fileInfo;
	char filename[512];

	// 遍历 ZIP 包内的所有文件
	int err = unzGoToFirstFile(uf);
	while (err == UNZ_OK) {
		// 获取当前文件名
		if (unzGetCurrentFileInfo(uf, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0) == UNZ_OK) {
			allFiles.push_back(std::string(filename));
		}
		err = unzGoToNextFile(uf);
	}

	unzClose(uf);


	return allFiles;
}

void PackReader_CatZPack::ReleaseMem(uint8_t* mem)
{
	delete[] mem;
}
