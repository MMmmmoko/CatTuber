#include<SDL3/SDL.h>
#include "CatSteam.h"
#include"Util/Util.h"

//不修改steam头文件的情况下屏蔽警告
#ifdef SDL_PLATFORM_WINDOWS
#define strncpy strncpy_s
#include"steam_api.h"
#undef strncpy
#else
#include"steam_api.h"
#endif // SDL_PLATFORM_WINDOWS


//非steam生成版本注释掉下行
#define STEAM_CATTUBER


#pragma comment(lib,"../../ThirdPart/steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.lib")


namespace cat::steam
{
	static bool b_steamInited = false;





	//删除txt有关的代码，应付检查，通过检查后使用上方代码
	bool CheckSteam(bool isRunAsAdmin)
	{
#ifndef STEAM_CATTUBER
		return true;
#else

//		if (isRunAsAdmin)
//		{
//			//创建并写入steam_appid.txt文件
//			std::ofstream ofs;
//			//3.指定打开方式
//			ofs.open("steam_appid.txt", std::ios::out | std::ios::trunc);
//			ofs.write("1337970", sizeof("1337970") - 1);
//			ofs.close();
//
//			if (SteamAPI_RestartAppIfNecessary(1337970)) // Replace with your App ID
//			{
//				return false;
//			}
//
//
//			if (!SteamAPI_Init())
//			{
//				CATLOG_ERROR_("Fatal Error - Failed to connect Steam client.");
//				return false;
//			}
//			b_steamInited = true;
//			SteamInput()->Init(false);
//			SteamInput()->Shutdown();
//
//#ifndef _DEBUG
//			DeleteFile(L"steam_appid.txt");
//#endif
//
//			return true;
//
//		}







		//创建并写入steam_appid.txt文件


		SDL_SaveFile("steam_appid.txt","1337970", sizeof("1337970") - 1);
		SteamErrMsg errMsg;
		if (SteamAPI_InitEx(&errMsg) != k_ESteamAPIInitResult_OK)
		{
#ifndef _DEBUG
			SDL_RemovePath("steam_appid.txt");
#endif
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Fatal Error - Failed to connect Steam client : %s", errMsg);
			return false;
		}
		b_steamInited = true;
		//SteamInput()->Init(false);
		//SteamInput()->Shutdown();

#ifndef _DEBUG
		SDL_RemovePath("steam_appid.txt");
#endif

		return true;



		//if (SteamAPI_RestartAppIfNecessary(1337970)) // Replace with your App ID
		//{
		//	CATLOG_ERROR_("Exit due to call to SteamAPI_RestartAppIfNecessary");
		//	return false;
		//}


#endif // STEAM_CATTUBER
	}

	std::string GetSteamUserLanguage()
	{
#ifndef STEAM_CATTUBER
		return "zh_CN";
#else
		////查询Steam语言表
		////https://partner.steamgames.com/doc/store/localization


		std::string langstr = SteamUtils()->GetSteamUILanguage();
		//转为UI可用语言？
		//if (langstr == "schinese" || langstr == "tchinese")
		//	return "zh_CN";
		//return "en_US";
		return std::move(langstr);

#endif // STEAM_CATTUBER
	}

	void ShutDownAPI()
	{
#ifndef STEAM_CATTUBER
		//无需执行任何工作
#else
		SteamAPI_Shutdown();
#endif // STEAM_CATTUBER
	}

	//仅32位
#ifdef WIN32
	void WriteMiniDump()
	{
#ifndef STEAM_CATTUBER
		//无需执行任何工作
#else

		//SteamAPI_SetMiniDumpComment("Minidump comment:Steam错误报告测试\n");
		//EXCEPTION_POINTERS exceptionptrs;
		//exceptionptrs.ContextRecord;
		//SteamAPI_WriteMiniDump(10737,&exceptionptrs,301009);


#endif // STEAM_CATTUBER
	}
#endif // 32

	bool IsSteamInited()
	{
		return b_steamInited;
	}



	std::string GetSteamUILanguage()
	{
#if 0
		//查询Steam语言表
		//https://partner.steamgames.com/doc/store/localization

		if (b_steamInited)
		{
			const char* lang = SteamUtils()->GetSteamUILanguage();
			cat::app::AppSettingsSystem::GetInstance().SetSettingValue<std::string>("Language_ItemInfo", lang);
			return lang;
		}

		std::string lang = cat::app::AppSettingsSystem::GetInstance().GetSettingValue<std::string>("Language_ItemInfo");
		if (lang != "UNKNOW")
			return lang;
		else
			return "english";
#endif
		return "english";
	}

}