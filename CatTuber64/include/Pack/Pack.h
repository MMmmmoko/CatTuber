#ifndef _Pack_h
#define _Pack_h


#include<vector>
#include<iostream>

//统一文件夹和资源包读取

class Pack
{
	enum PackType
	{
		PackType_Unknown = 0,
		PackType_Folder,
		PackType_Pack_ver0,//旧CatTuber所使用的Pack，除文件外不再读取其他数据
		PackType_Pack_ver1

	};





public:
	//合法性检查
	bool Open(const char* packFilePath);
	
	//packPath中无反斜杠，这里会组合成packPath+/+path
	uint8_t* LoadFile(const char* path,size_t* size);
	bool IsFileExist(const char* path);
	//无反斜杠,如果fullPath为true则返回结果为相对于packroot的路径，否则只包含文件名
	//获取资源包中某个文件夹中的文件，不读取子文件夹
	std::vector<std::string> GetFileList(const char* folerPath,bool fullPath=true);


	void ReleaseMem(uint8_t* mem);
	const char* GetPath() { return packPath.c_str(); };
private:
	PackType type;
	std::string packPath;//文件夹或者pack文件
};


//
class PackReader_Folder
{
public:
	static bool CheckPack(const char* packFilePath);
	static uint8_t* LoadFile(const char* packPath, const char* path, size_t* size);
	static bool IsFileExist(const char* packPath, const char* path);
	static std::vector<std::string> GetFileList(const char* packPath, const char* folerPath, bool fullPath = true);
	static void ReleaseMem(uint8_t* mem);
};

class PackReader_Pack_ver0
{
public:
	static bool CheckPack(const char* packFilePath) { return false;  };
	static uint8_t* LoadFile(const char* packPath, const char* path, size_t* size) { return nullptr; };
	static bool IsFileExist(const char* packPath, const char* path) { return false; };
	static std::vector<std::string> GetFileList(const char* packPath, const char* folerPath, bool fullPath = true) { return std::vector<std::string>(); };
	static void ReleaseMem(uint8_t* mem) {};
};

class PackReader_Pack_ver1
{
public:
	static bool CheckPack(const char* packFilePath) { return false; };
	static uint8_t* LoadFile(const char* packPath, const char* path, size_t* size) { return nullptr; };
	static bool IsFileExist(const char* packPath, const char* path) { return false; };
	static std::vector<std::string> GetFileList(const char* packPath, const char* folerPath, bool fullPath = true) { return std::vector<std::string>(); };
	static void ReleaseMem(uint8_t* mem) {};
};








#endif