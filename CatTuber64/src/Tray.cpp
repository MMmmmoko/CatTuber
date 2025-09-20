#include"AppSettings.h"
#include "Tray.h"
#include"CatTuberApp.h"
#include"Dui.h"


void Tray::CreateTray()
{
	if (tray)
		return;
	
	//先用NULL图标测试
	tray = SDL_CreateTray(NULL,"CatTuber");
	if (!tray)return;

	// Create a context menu for the tray.
	menu = SDL_CreateTrayMenu(tray);
	if (!menu)return;

	item_openSettingPage=SDL_InsertTrayEntryAt(menu, -1, "Open Setting Window\tCtrl+Shift+S", SDL_TRAYENTRY_BUTTON);
	item_lockWindow=SDL_InsertTrayEntryAt(menu, -1, "Lock Window", SDL_TRAYENTRY_BUTTON);
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