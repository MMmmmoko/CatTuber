#include"AppSettings.h"
#include"AppContext.h"
#include "Tray.h"
#include"CatTuberApp.h"
#include"Dui.h"
#include"Util/Util.h"
#include"SDL3_image/SDL_image.h"
void Tray::CreateTray()
{
	if (tray)
		return;

	std::string trayTips = "CatTuber";

	//从资源文件中加载icon
	std::vector<unsigned char> iconFileData;
	{
		auto& m_zipManager = ui::GlobalManager::Instance().Zip();
		if (m_zipManager.IsUseZip()) 
		{
			ui::FilePath filePath = ui::FilePath(L"DuiResource/themes/default/CatTuber_default/CatTuberLogo256px.png");
			if (m_zipManager.GetZipData(filePath, iconFileData)) {

			}
			else {
				SDL_assert(!"GetZipData failed!");
			}
		}
		else {
			//没有使用资源包
			std::string pathstr=AppContext::GetAppBasePath();
			pathstr +="Dui/themes/default/CatTuber_default/CatTuberLogo256px.png";
			iconFileData =util::SDL_LoadFileToMem(pathstr.c_str());
		}
	}
	
	SDL_Surface* iconSurface = nullptr;


	{
		SDL_IOStream* io = SDL_IOFromConstMem(iconFileData.data(), iconFileData.size());
		iconSurface = IMG_LoadPNG_IO(io);
		SDL_CloseIO(io);
	}


	//参数
	//SDL_PropertiesID prop = SDL_CreateProperties();
	//TODO: SDL3.6.0 通过SDL_CreateTrayWithProperties实现跨平台左右键点击回调，并DUILIB自己实现菜单

	tray=SDL_CreateTray(iconSurface, trayTips.c_str());

	SDL_DestroySurface(iconSurface);








	if (!tray)return;

	// Create a context menu for the tray.
	menu = SDL_CreateTrayMenu(tray);
	if (!menu)return;
	
	item_openSettingPage=SDL_InsertTrayEntryAt(menu, -1, GETDUISTRING_UTF8(L"STRID_WINDOWMENU_OPENSETTINGWINDOW").c_str(), SDL_TRAYENTRY_BUTTON);
	item_lockWindow=SDL_InsertTrayEntryAt(menu, -1, GETDUISTRING_UTF8(L"STRID_WINDOWMENU_LOCKWINDOW").c_str(), SDL_TRAYENTRY_BUTTON);
	item_hideWindow=SDL_InsertTrayEntryAt(menu, -1, "Hide/Show Window", SDL_TRAYENTRY_BUTTON);
	item_resetWindowPos=SDL_InsertTrayEntryAt(menu, -1, "Reset Window Pos", SDL_TRAYENTRY_BUTTON);
	item_quit= SDL_InsertTrayEntryAt(menu, -1, "Quit", SDL_TRAYENTRY_BUTTON);


	//设置按钮功能
	{
		if (item_openSettingPage)
		{
			//打开设置页面
			SDL_SetTrayEntryCallback(item_openSettingPage,
			[](void* userdata, SDL_TrayEntry * entry)
			{
				Dui::OpenMainUiWindow();
			}
			,NULL);
		}

		if (item_lockWindow)
		{
			SDL_SetTrayEntryCallback(item_lockWindow,
			[](void* userdata, SDL_TrayEntry * entry)
			{
				auto& appSettings=AppSettings::GetIns();
				appSettings.SetWindowLock(!appSettings.GetWindowLock());
			}
			,NULL);
		}

		if (item_hideWindow)
		{
			SDL_SetTrayEntryCallback(item_hideWindow,
			[](void* userdata, SDL_TrayEntry * entry)
			{
				auto& appSettings=AppSettings::GetIns();
				appSettings.SetWindowVisible(!appSettings.GetWindowVisible());
			}
			,NULL);
		}

		if (item_resetWindowPos)
		{
			SDL_SetTrayEntryCallback(item_hideWindow,
			[](void* userdata, SDL_TrayEntry * entry)
			{
					//RenderWindowManager Reset Pos

			}
			,NULL);
		}

		if (item_quit)
		{
			SDL_SetTrayEntryCallback(item_quit,
			[](void* userdata, SDL_TrayEntry * entry)
			{
					SDL_Event e;
					e.type = SDL_EVENT_QUIT;
					SDL_PushEvent(&e);
			}
			,NULL);
		}
	
	}




	ReCreateMenuItemForCurrentLang();




}

void Tray::ReCreateMenuItemForCurrentLang()
{
	if (!menu)
		return;

	if (item_openSettingPage)
	{
		//SDL_SetTrayEntryLabel(,);
	}





}

void Tray::DestroyTray()
{
	if (tray)
	{
		SDL_DestroyTray(tray);
		tray = NULL;
		menu = NULL;
	}
}