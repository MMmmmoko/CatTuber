#ifndef _SceneManager_h
#define _SceneManager_h

#include <string>
#include <vector>
#include"json/json.h"

//场景管理器，目前主要负责场景列表信息读取
//这里的场景主要指示整体的所有画面
//窗口类的场景成员指窗口内的场景内容

struct SceneInfo
{
	std::string name;
	std::string fileName;
	std::string imgPath;//场景封面
};

class SceneManager
{



public:
	static SceneManager& GetInstance(){static SceneManager instance;return instance;}
	static bool CreateNewSceneJson(const char* sceneName,bool fillWithDefaultResource,bool createNewFile,Json::Value& outJson,std::string* outFileName);
	


	//获取场景列表
	std::vector<SceneInfo> GetSceneList(); 
	//打开场景文件夹
	static void OpenSceneFileFolder();

	
	bool LoadScene(const char* sceneFileName);
	bool LoadLastQuitScene();
	void SaveCurrentScene();


	//bool GetCurrentSceneInfo(SceneInfo* infoOut);


	const SceneInfo& GetCurrentScene() { return pCurrentScene; };
	//class MainSceneItem* GetCurrentMainSceneItem();
	//const std::vector<class ISceneItem*>& GetCurrentSceneItemList();





private:
	SceneInfo pCurrentScene;


};


















#endif