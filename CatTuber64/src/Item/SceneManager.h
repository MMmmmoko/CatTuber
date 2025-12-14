#ifndef _SceneManager_h
#define _SceneManager_h

#include <string>
#include <vector>
#include"json/json.h"

//场景管理器，目前主要负责场景列表信息读取


struct SceneInfo
{
	std::string name;
	std::string coverPath;//场景文件路径
};

class SceneManager
{



public:
	static bool GenerateSceneJson(const char* sceneName,bool fillWithDefaultResource,bool createNewFile,Json::Value& outJson,std::string* outFileName);
	


	//获取场景列表
	std::vector<SceneInfo> GetSceneList();
	//打开场景文件夹
	static void OpenSceneFileFolder();
















};


















#endif