#include"Dui.h"
#include"SettingsPage.h"
#include"AppContext.h"
void SettingsPage_aboutCatTuber::InitContents(class SettingsPage* parent)
{
	ui::Label* text_currentVersion=(ui::Label*)parent->FindSubControl(L"text_currentVersion");
	text_currentVersion->SetUTF8Text(AppContext::GetCatTueberVersionStr());

	ui::Label* text_live2DSDKVersion =(ui::Label*)parent->FindSubControl(L"text_live2DSDKVersion");
	text_live2DSDKVersion->SetUTF8Text(AppContext::GetLive2DVersionStr());

}