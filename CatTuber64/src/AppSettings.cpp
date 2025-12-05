
#include"AppContext.h"
#include"AppSettings.h"

#include"Util/Util.h"


#include"RenderThread.h"
#include"RenderWindowManager.h"
#include"Input/InputManager.h"

AppSettings AppSettings::ins;
SDL_Color AppSettings::_defaultBackgroundColor = {255,255,255,0};//低到高 rgba


//不要修改文件名？
#define _SETTINGS_FILE_NAME "AppConfig.json"





void AppSettings::Load()
{
	//重置默认
	*this = AppSettings();

	std::string settingsFilePath = AppContext::GetPrefPath();
	settingsFilePath +=_SETTINGS_FILE_NAME;

	Json::Value configJson= util::BuildJsonFromFile(settingsFilePath.c_str());






	
	//先检查项是否存在 如下:
	//后续如果有非基础的数据类型（如rect）
	//则对所有类型封装一层读写：
	//IsRectType(Json::Value& json){json.isArray()......}
	//SaveRect(Json::Value& json,Rect rect){json[i]=recr.topleft....};

	//子代码如下
	/*
	if (configJson.isMember("Window")&& configJson["Window"].isMember("Top")&& configJson["Window"]["Top"].isBool())
	{
		SetWindowTop(configJson["Window"]["Top"].asBool());
	}
	else
	{
		SetWindowTop(_WindowTop);
	}
	*/
	
#define APPSETTINGS_Load(settingGroup,setting,type,defaultValue) \
if(configJson.isMember(#settingGroup)&& configJson[#settingGroup].isMember(#setting)&&\
 _Is_##type##_Type(configJson[#settingGroup][#setting])) \
	{ \
	Set##settingGroup##setting(_Get_##type(configJson[#settingGroup][#setting])); \
} \
else \
{\
 Set##settingGroup##setting(_##settingGroup##setting);\
}

	{ CATTUBER_APPSETTINGS_LIST(APPSETTINGS_Load) }

#undef APPSETTINGS_Load



}


bool AppSettings::Save()
{

	//构建Json
	Json::Value root;

	//_Save_bool_Type(root["Window"]["Top"], _WindowTop);
#define APPSETTINGS_Save(settingGroup,setting,type,defaultValue) \
	_Save_##type##_Type(root[#settingGroup][#setting],_##settingGroup##setting);
	{ CATTUBER_APPSETTINGS_LIST(APPSETTINGS_Save) }
#undef APPSETTINGS_Save


	//特殊处理，如果不保存Lock状态的情况下，保存为false
	if (!root["Window"]["LockSave"].asBool())
	{
		root["Window"]["Lock"] = false;
	}

	//将json写入文件
	//需要判断是否可写
	std::string settingsFilePath = AppContext::GetPrefPath();
	settingsFilePath += _SETTINGS_FILE_NAME;

	if (!util::SaveJsonToFile(root,settingsFilePath.c_str()))
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"Save Settings Failed!");
		return false;
	}
	SDL_LogInfo(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Settings Saved.");
	return true;



}

const std::vector<std::string>& AppSettings::GetLocalLanguageFullbackVec()
{
	static std::vector<std::string> vec;
	if (!vec.empty())
		return vec;
	

	//构建fullback表
	int count;
	SDL_Locale** localeList=SDL_GetPreferredLocales(&count);
	if (NULL == localeList)
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "Can not get locale info! %s",SDL_GetError());
		//TODO/FIXME加入对steam语言的检测
		vec.push_back("english");
		return vec;
	}

#define PUSHLANG(_language,_lang) else if (0 == SDL_strcmp(localeList[i]->language, #_lang))vec.push_back(#_language)
	for (int i = 0; i < count; i++)
	{
		if (0 == SDL_strcmp(localeList[i]->language, "bg"))
		{
			vec.push_back("bulgarian");
		}
		else if (0 == SDL_strcmp(localeList[i]->language, "zh"))
		{
			if(0 == SDL_strcmp(localeList[i]->country, "CN")
				|| 0 == SDL_strcmp(localeList[i]->country, "SG")
				|| NULL==localeList[i]->country)
				vec.push_back("schinese");
			else
				vec.push_back("tchinese");
		}
		else if (0 == SDL_strcmp(localeList[i]->language, "cs"))
		{
			vec.push_back("czech");
		}
		PUSHLANG(czech, cs);
		PUSHLANG(danish, da);
		PUSHLANG(dutch, nl);
		PUSHLANG(english, en);
		PUSHLANG(finnish, fi);
		PUSHLANG(french, fr);
		PUSHLANG(german, de);
		PUSHLANG(greek, el);
		PUSHLANG(hungarian, hu);
		PUSHLANG(indonesian, id);
		PUSHLANG(italian, it);
		PUSHLANG(japanese, ja);
		PUSHLANG(koreana, ko);
		PUSHLANG(norwegian, no);
		PUSHLANG(polish, pl);
		else if (0 == SDL_strcmp(localeList[i]->language, "pt"))
		{
			if (0 == SDL_strcmp(localeList[i]->country, "BR"))
				vec.push_back("brazilian");
			else
				vec.push_back("portuguese");
		}
		PUSHLANG(romanian, ro);
		PUSHLANG(russian, ru);
		else if (0 == SDL_strcmp(localeList[i]->language, "es"))
		{
			if (0 == SDL_strcmp(localeList[i]->country, "ES"))
				vec.push_back("spanish");
			else
				vec.push_back("latam");
		}
		PUSHLANG(swedish, sv);
		PUSHLANG(thai, th);
		PUSHLANG(turkish, tr);
		PUSHLANG(ukrainian, uk);
		PUSHLANG(vietnamese, vi);
	}

	//对简体繁体互相添加一个Fullback
	for (auto it = vec.begin(); it != vec.end(); it++)
	{
		if (*it == "schinese")
		{
			vec.insert(it + 1, "tchinese");
			break;
		}
		if (*it == "tchinese")
		{
			vec.insert(it + 1, "schinese");
			break;
		}
	}
	//总以english结尾
	vec.push_back("english");

	SDL_free(localeList);




	return vec;
}













//************************************************
//实现_OnXXXXXChange()
//注意设置项的修改应该是发生在主线程中的
//与渲染线程有关的需要进行转发
//************************************************

void AppSettings::_OnWindowTopChange(const bool& value)
{
	RenderWindowManager::GetIns().SetWindowTop(value);
}

void AppSettings::_OnWindowTransparentChange(const bool& value)
{
	RenderWindowManager::GetIns().SetWindowTransparent(value);
}

void AppSettings::_OnWindowLockChange(const bool& value)
{
	RenderWindowManager::GetIns().SetWindowLock(value);
}

void AppSettings::_OnWindowLockSaveChange(const bool& value)
{
	//应该不用进行任何动作..
}

void AppSettings::_OnWindowVisibleChange(const bool& value)
{
	RenderWindowManager::GetIns().SetWindowVisible(value);
}

void AppSettings::_OnWindowFrameLimitChange(const int64_t& value)
{
	RenderThread::GetIns().PostTask([](void* data,uint64_t taskParam) {
		int64_t frameLimit = (int64_t)(&data);
	RenderWindowManager::GetIns().SetFrameLimit(static_cast<int>(frameLimit));
		}, *(void**)(&value));
}


void AppSettings::_OnWindowBackgroundColorChange(const SDL_Color& value)
{
	//SDL_Color4字节，可以直接传值
	RenderThread::GetIns().PostTask([](void* data, uint64_t taskParam) {
		SDL_Color color = *(SDL_Color*)(&data);
		RenderWindowManager::GetIns().SetWindowBackgroundColor(color);
		}, *(void**)(&value));
}


void AppSettings::_OnVolumeValueChange(const double& value)
{
	//音量等待sdl_mix3库
	SDL_assert(false);
}

void AppSettings::_OnMouseRelativeMoveChange(const bool& value)
{
	RenderThread::GetIns().PostTask([](void* data, uint64_t taskParam) {
		InputManager::GetIns().isMousePosMove=((bool)data);
		}, (void*)value);
}

void AppSettings::_OnMouseSpeedChange(const double& value)
{
	float fvalue = static_cast<float>(value);
	RenderThread::GetIns().PostTask([](void* data, uint64_t taskParam) {
		InputManager::GetIns().mouseInput.SetMoveSpeed( *(float*)(&data));
		}, *(void**)(&fvalue));
}

void AppSettings::_OnMouseInputAreaChange(const std::string& value)
{
	//value是基于显示器的字符串
	//"AllDisplays" 或者显示器名+部分其他标识
	//同名显示器后缀##0\##1
	SDL_Rect rect = {};
	//先判断"AllDisplays"
	if (value == "AllDisplays")
	{
		//计算屏幕总范围
		int displayCount=0;
		SDL_DisplayID* displays=SDL_GetDisplays(&displayCount);
		if(!displays|| displayCount==0)
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,"MouseInputArea: Can not get display list! %s",SDL_GetError());
			SDL_free(displays);
			return;
		}
		for (int i = 1; i < displayCount; i++)
		{
			SDL_Rect temRect;
			if (SDL_GetDisplayBounds(displays[i], &temRect))
			{
				if (i == 0)
				{
					rect = temRect;
				}
				else
				{
					//left
					int left = SDL_min(rect.x, temRect.x);
					//right
					int right = SDL_max(rect.x+ rect.w, temRect.x+ temRect.w);
					//top
					int top = SDL_min(rect.y, temRect.y);
					//bottom
					int bottom = SDL_max(rect.y+rect.h, temRect.y+ temRect.h);

					rect.x = left;
					rect.w = right- left;
					rect.y = top;
					rect.h = bottom-top;
				}
			}
			else
			{
				SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "MouseInputArea: Can not get display bounds! %s", SDL_GetError());
				SDL_free(displays);
				return;
			}
		}

		SDL_free(displays);
	}
	else
	{
		//这段代码后续会用在UI中
		//先构建一个显示器display和显示器名后缀的对照表.
		//然后查字符串
		int displayCount = 0;
		SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
		if (!displays || displayCount == 0)
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "MouseInputArea: Can not get display list! %s", SDL_GetError());
			SDL_free(displays);
			return;
		}
		std::vector<std::string> nameVec;
		for (int i = 1; i < displayCount; i++)
		{
			const char* displayName=SDL_GetDisplayName(displays[i]);
			if (NULL == displayName)
			{
				nameVec.push_back("Display "+std::to_string(i+1));
			}
			else
			{
				nameVec.push_back(displayName);
			}
		}
		for (int i = 0; i < displayCount; i++)
		{
			const std::string& curDisplayName = nameVec[i];
			std::vector<int> sameNameIndexVec;//同名显示器索引
			for (int j = i + 1; j < displayCount; j++)
			{
				if (curDisplayName == nameVec[j])
				{
					sameNameIndexVec.push_back(j);
				}
			}
			if (!sameNameIndexVec.empty())
			{
				//检测到同名显示器,重构命名
				nameVec[i] = nameVec[i]+"##1";

				for (int k = 0; k < sameNameIndexVec.size(); k++)
				{
					nameVec[sameNameIndexVec[k]] = nameVec[sameNameIndexVec[k]] + "##"+std::to_string(k+2);//##2开始
				}
			}
		}
		//对比字符串
		bool success = false;
		for (int i = 0; i < displayCount; i++)
		{
			if (value == nameVec[i])
			{
				success = true;
				if (!SDL_GetDisplayBounds(displays[i],&rect))
				{
					SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "MouseInputArea: Can not get display bounds! %s", SDL_GetError());
					SDL_free(displays);
					return;
				}
				break;
			}
		}
		//未成功匹配显示器，移除##再进行匹配
		{
			std::string curDisplay = value;
			curDisplay=curDisplay.substr(0,curDisplay.find("##"));
			for (int i = 0; i < displayCount; i++)
			{
				nameVec[i] = nameVec[i].substr(0,nameVec[i].find("##"));
				if (curDisplay == nameVec[i])
				{
					if (!SDL_GetDisplayBounds(displays[i], &rect))
					{
						SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "MouseInputArea: Can not get display bounds! %s", SDL_GetError());
						SDL_free(displays);
						return;
					}
					break;
				}
			}
		}

		SDL_free(displays);
	}

	if (rect.w == 0 || rect.h == 0)
	{
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, "MouseInputArea: Rect Area is 0!");
		return;
	}

	//构造好了rect
	SDL_Rect* rectData = new SDL_Rect(rect);
	RenderThread::GetIns().PostTask([](void* data, uint64_t taskParam) {
		SDL_Rect* pRect = (SDL_Rect*)data;
		InputManager::GetIns().SetMouseInputArea(pRect);
		delete pRect;
		}, rectData);



}

void AppSettings::_OnMouseInvertXChange(const bool& value)
{
	RenderThread::GetIns().PostTask([](void* data, uint64_t taskParam) {
		InputManager::GetIns().mouseInput.SetInverX((bool)data);
		}, (void*)value);
}



void AppSettings::_OnMiscLanguageChange(const std::string& value)
{
	//语言采用SteamAPI的形式
//https://partner.steamgames.com/doc/store/localization/languages
	SDL_assert(false);
}

void AppSettings::_OnOtherShowTaskBarIconChange(const bool& value)
{
	//TODO
	SDL_assert(false);
}

void AppSettings::_OnOtherWindows_RunAsAdminChange(const bool& value)
{
	//TODO
	SDL_assert(false);
}

































































bool AppSettings::_Is_double_Type(Json::Value& json)
{
	return json.isDouble();
}
void AppSettings::_Save_double_Type(Json::Value& json,double value)
{
	json=value;
}
double AppSettings::_Get_double(Json::Value& json)
{
	return json.asDouble();
}



bool AppSettings::_Is_int64_t_Type(Json::Value& json)
{
	return json.isInt64();
}
void AppSettings::_Save_int64_t_Type(Json::Value& json, int64_t  value)
{
	json=value;
}
int64_t AppSettings::_Get_int64_t(Json::Value& json)
{
	return json.asInt64();
}


bool AppSettings::_Is_bool_Type(Json::Value& json)
{
	return json.isBool();
}
void AppSettings::_Save_bool_Type(Json::Value& json, bool value)
{
	json=value;
}
bool AppSettings::_Get_bool(Json::Value& json)
{
	return json.asBool();
}

bool AppSettings::_Is__CatString_Type(Json::Value& json)
{
	return json.isString();
}
void AppSettings::_Save__CatString_Type(Json::Value& json, const std::string& value)
{
	json=value;
}
std::string AppSettings::_Get__CatString(Json::Value& json)
{
	return json.asString();
}



bool AppSettings::_Is_SDL_Color_Type(Json::Value& json)
{
	//颜色由4个整数数值组成数组//不面向用户所以不对3元的颜色进行判断
	//json[0]
	if (json.isArray() && json.size() == 4)
	{
		for (int i = 0; i < 4; i++)
		{
			if (!(json[i].isUInt64() && json[i].asUInt64() <= 255))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}
void AppSettings::_Save_SDL_Color_Type(Json::Value& json, const SDL_Color& value)
{
	json[0] = value.r;
	json[1] = value.g;
	json[2] = value.b;
	json[3] = value.a;
}
SDL_Color AppSettings::_Get_SDL_Color(Json::Value& json)
{
	SDL_Color result;
	result.r = static_cast<uint8_t>( json[0].asUInt64());
	result.g = static_cast<uint8_t>( json[1].asUInt64());
	result.b = static_cast<uint8_t>( json[2].asUInt64());
	result.a = static_cast<uint8_t>( json[3].asUInt64());
	return result;
}


