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



	//自定义颜色
	auto& colorManager=ui::GlobalManager::Instance().Color();
	colorManager.AddColor(L"panelBkColor",ui::UiColor( 0xFFF7F7FC));// 高到低 ARGB    低到高BGRA
	colorManager.AddColor(L"panelBorderColor", ui::UiColor(0xFFDBDDE1));

	colorManager.AddColor(L"textNormalColor", ui::UiColor(0xFF414A5E));
	colorManager.AddColor(L"textLightColor", ui::UiColor(0xFF7C8291));
	colorManager.AddColor(L"textActiveColor", ui::UiColor(0xFF283248));
	colorManager.AddColor(L"itemHoverColor", ui::UiColor(0xFFE5E6E7));
	colorManager.AddColor(L"itemPushColor", ui::UiColor(0xFFCACDCF));

	colorManager.AddColor(L"itemSeparatorColor", ui::UiColor(0xFFEBEDF1));
	colorManager.AddColor(L"subjectColor", ui::UiColor(0xFFff7bb8));//主题色
	colorManager.AddColor(L"subjectColor_hot", ui::UiColor(0xFFFF95D0));//主题色
	colorManager.AddColor(L"subjectColor_push", ui::UiColor(0xFFF159AE));//主题色
	colorManager.AddColor(L"subjectColor_content", ui::UiColor(0xFFFFFFFF));//主题色下的内容颜色


	//设置默认文字颜色减少xml编辑量
	colorManager.SetDefaultTextColor(L"textNormalColor");







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
		uiMainForm->AttachWindowClose([](const ui::EventArgs&) ->bool{
			uiMainForm = NULL;
			return true;
			});
		uiMainForm->ShowWindow(ui::kSW_SHOW_NORMAL);
	}




}


