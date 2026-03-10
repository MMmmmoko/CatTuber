#include "SceneManager.h"
#include<SDL3/SDL.h>
#include"Util.h"
#include"AppContext.h"
#include"AppSettings.h"
#include"RenderWindowManager.h"
bool SceneManager::CreateNewSceneJson(const char* sceneName, bool fillWithDefaultResource, bool createNewFile, Json::Value& outJson, std::string* outFileName)
{
    if (sceneName == nullptr || *sceneName == 0)
    {
        return false;
    }


    Json::Value root;


    //获取当前时间戳
    SDL_Time curtime=0;
    //不进行验证，源码中只有传入空指针的时候出错
    SDL_GetCurrentTime(&curtime);


    



    //填充基本的公共信息
    if(sceneName)
        root["SceneName"] = sceneName;

    root["CreateTime"] = curtime;
    root["Version"] = 1;
    //root["LastUsedTime"] = curtime;


    if (fillWithDefaultResource)
    {

        //向json中添加默认场景
        //abort();
    }
    else
    {
        root["Windows"]["Size"][0] = 400;
        root["Windows"]["Size"][1] = 300;
    }

    //如果不需要创建文件，直接返回
    if (!createNewFile)
    {
        outJson = root;
        return true;
    }






    //填充完json，进行文件写入
    //构建文件名
    SDL_DateTime date = {};
    SDL_TimeToDateTime(curtime, &date,true);

    //Scene_20250920133012333.json （年月日时分秒毫秒）
    char fileNameBuffer[100];
    SDL_snprintf(fileNameBuffer,sizeof(fileNameBuffer),"Scene_%04d%02d%02d%02d%02d%02d%03d.json", 
        date.year,date.month,date.day,date.hour,date.minute,date.second,date.nanosecond/1000'000);


    //std::string fileName="Scene_"+std::to_string(date.year)+ std::to_string(date.month)+ std::to_string(date.day)
    //    + std::to_string(date.hour)+ std::to_string(date.minute)+ std::to_string(date.second)

    std::string targetFilePath = AppContext::GetSceneFolderPath();
    targetFilePath+=fileNameBuffer;

    if (util::SaveJsonToFile(root, targetFilePath.c_str()))
    {
        if(outFileName) *outFileName=fileNameBuffer;
        outJson = root;
        return true;
    }

    return false;
}

std::vector<SceneInfo> SceneManager::GetSceneList()
{
    //从文件夹加载场景列表
//文件名示例：Scene_87B24C7F-5745-4DD8-B89D-BA95C8E8D6D2.json
//不使用UUID，改用时间戳，这样会自动按时间排序
//文件名示例：Scene_20250920133012333.json （年月日时分秒毫秒）
//封面文件：Scene_20250920133012333.png/jpg/gif
//创建时也不能使用重复的名字


    std::string sceneFoldPath = AppContext::GetSceneFolderPath();

    int matchCount = 0;
    char** sceneJsonFiles = SDL_GlobDirectory(sceneFoldPath.c_str(), "Scene_*.json", 0, &matchCount);


	std::vector<SceneInfo> result;
    for (int i = 0; i < matchCount; i++)
    {
        Json::Value curSceneJson = util::BuildJsonFromFile((sceneFoldPath + sceneJsonFiles[i]).c_str());



        if (!curSceneJson["Version"].isUInt())continue;


        if (curSceneJson["SceneName"].isString())
        {
            auto& curScene = result.emplace_back();
            curScene.name = curSceneJson["SceneName"].asString();
            curScene.fileName = sceneJsonFiles[i];

            //检查封面图片文件是否存在
            std::string baseFilePath = sceneFoldPath + curScene.fileName.substr(0, curScene.fileName.size() - 5);//去掉.json


            SDL_PathInfo info;
            if (SDL_GetPathInfo((baseFilePath + ".png").c_str(), &info))
            {
                curScene.imgPath = baseFilePath + ".png";
            }
            else if (SDL_GetPathInfo((baseFilePath + ".jpg").c_str(), &info))
            {
                curScene.imgPath = baseFilePath + ".jpg";
            }
            else if (SDL_GetPathInfo((baseFilePath + ".gif").c_str(), &info))
            {
                curScene.imgPath = baseFilePath + ".gif";
            }
            else
            {
                //不存在封面图片
                curScene.imgPath = "";
            }
        }
    }
    if (sceneJsonFiles)SDL_free(sceneJsonFiles);


	return result;
}

bool SceneManager::LoadScene(const char* sceneFileName)
{

	//加载场景前先保存当前场景
    SaveCurrentScene();


    SDL_PathInfo info;
	std::string sceneFilePath = std::string(AppContext::GetPrefPath()) + "Scenes" + "/" + sceneFileName;
    if(sceneFileName==nullptr||*sceneFileName=='\0'
        ||!SDL_GetPathInfo(sceneFilePath.c_str(),&info)
        )
    {
		//妹有指定场景文件，尝试寻找其他场景文件
        std::vector<SceneInfo> sceneList = GetSceneList();
        for (auto& x : sceneList)
        {
            if (LoadScene(x.fileName.c_str()))
            {
				return true;
            }
        }
        //未找到任何场景文件

		Json::Value sceneJson;
		std::string fileName;
        if (CreateNewSceneJson(nullptr, true, true, sceneJson, &fileName))
        {
            RenderWindowManager::GetIns()._BuildFromJson(sceneJson);

            pCurrentScene = {};
            pCurrentScene.fileName = fileName;
            AppSettings::GetIns().SetSceneLastExit(pCurrentScene.fileName);

            return true;
        }

        return false;
    }




	//场景文件存在
   //计算出场景文件路径后
    Json::Value json = util::BuildJsonFromFile(sceneFilePath.c_str());
    if (!json.empty())
    {
		RenderWindowManager::GetIns()._BuildFromJson(json);

        pCurrentScene = {};
		pCurrentScene.name = json["SceneName"].isString() ? json["SceneName"].asString() : "";
        pCurrentScene.fileName = sceneFileName;
        pCurrentScene.imgPath = "";//不需要提供封面
        AppSettings::GetIns().SetSceneLastExit(pCurrentScene.fileName);
        return true;
    }
    else
    {
        //场景文件有问题，按空读取
        LoadScene("");
        return true;
    }
    return true;
}

bool SceneManager::LoadLastQuitScene()
{
	//加载上次退出时选择的场景

    return LoadScene(AppSettings::GetIns().GetSceneLastExit().c_str());
}

void SceneManager::SaveCurrentScene()
{
    if (!pCurrentScene.fileName.empty())
    {
        Json::Value saveJson= RenderWindowManager::GetIns()._SaveToJson(GetCurrentScene().name.c_str());

        if (!saveJson.empty())
        {
            std::string savePath = AppContext::GetPrefPath();
            savePath = savePath + "Scenes"+"/" + pCurrentScene.fileName;

            //保存文件
            if (!util::SaveJsonToFile(saveJson, savePath.c_str()))
            {
                SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Save File Failed: %s", savePath.c_str());
            }
            
			AppSettings::GetIns().SetSceneLastExit(pCurrentScene.fileName);
        }
    }


}

MainSceneItem* SceneManager::GetCurrentMainSceneItem()
{
    auto window=RenderWindowManager::GetIns().GetWindowController(0);
    if (!window)return nullptr;
    return window->GetScene().GetMainItem();
}
