#include "SceneManager.h"
#include<SDL3/SDL.h>
#include"Util.h"
#include"AppContext.h"
bool SceneManager::GenerateSceneJson(const char* sceneName, bool fillWithDefaultResource, bool createNewFile, Json::Value& outJson, std::string* outFileName)
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
    root["SceneName"] = sceneName;

    root["CreateTime"] = curtime;
    root["Version"] = 1;
    //root["LastUsedTime"] = curtime;


    if (fillWithDefaultResource)
    {

        //向json中添加默认场景
        abort();

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
