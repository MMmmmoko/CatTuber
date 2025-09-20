#ifndef _Tray_h
#define _Tray_h

//基于SDL的托盘图标

#include<SDL3/SDL.h>

class Tray
{
	//AppSettings 修改设置时尝试修改Tray按钮文本
	friend class AppSettings;
public:
	static Tray& GetIns() { static Tray ref; return ref; };

	void CreateTray();

	void ReCreateMenuItemForCurrentLang();

	void DestroyTray();
private:
	SDL_Tray* tray=NULL;
	SDL_TrayMenu* menu=NULL;

	//方便设置文本和回调
	SDL_TrayEntry* item_openSettingPage;
	SDL_TrayEntry* item_lockWindow;
	SDL_TrayEntry* item_hideWindow;
	SDL_TrayEntry* item_resetWindowPos;
	SDL_TrayEntry* item_quit;








};













#endif