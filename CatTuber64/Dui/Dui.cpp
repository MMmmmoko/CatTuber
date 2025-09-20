#include"Dui.h"
#include"UiThread.h"
#include"Form/MainUiForm.h"
//噙怓Е
static bool duiInited = false;
static std::unique_ptr<UIMainThread> uiMainThread;
static MainUiForm* uiMainForm;



void Dui::Init()
{
	if (duiInited)return;
	uiMainThread.reset(new UIMainThread);
	uiMainThread->StartWithoutLoop();
	duiInited = true;

}

void Dui::ShutDown()
{
	if (uiMainForm)uiMainForm->Close();
	uiMainForm = NULL;
	if (uiMainThread)
	{
		uiMainThread->OnCleanup();
	}
	uiMainThread.reset();
}

void Dui::OpenMainUiWindow()
{
	if (uiMainForm)
	{
		uiMainForm->ShowWindow(ui::ShowWindowCommands::kSW_SHOW);
	}
	else
	{
		uiMainForm = new MainUiForm;
		uiMainForm->CreateWnd(nullptr, ui::WindowCreateParam(L"CatTuber Settings",true));
		uiMainForm->ShowWindow(ui::kSW_SHOW_NORMAL);
	}
}


