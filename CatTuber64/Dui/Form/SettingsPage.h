#ifndef _SettingsPage_h
#define _SettingsPage_h


#include"UIPageBase.h"
#include"SettingsPage_window.h"
#include"SettingsPage_sound.h"
#include"SettingsPage_input.h"
#include"SettingsPage_other.h"
#include"SettingsPage_aboutCatTuber.h"

#define UISETTINGSPAGE_TABOP_LIST(F) \
    F(window) \
    F(sound) \
    F(input) \
    F(other) \
    F(aboutCatTuber)


class SettingsPage :public UIPageBase
{

public:
	SettingsPage(ui::Window* pWindow) :UIPageBase(pWindow) {};
	virtual void InitContents(uintptr_t userdata1=0, uintptr_t userdata2=0)override;
	virtual MainUiForm::PageEnum GetPageType()override { return MainUiForm::PageEnum::PAGE_SETTINGS; };



private:
	bool OnTabOptionClicked(const ui::EventArgs& msg);
	bool OnScrollChange(const ui::EventArgs& msg);
	void _ClearTabOpStates();


	//Tab
	//ui::Option* tabop_window;
	//ui::Option* tabop_sound;
	//ui::Option* tabop_input;
	//ui::Option* tabop_other;
	//ui::Option* tabop_aboutCatTuber;

	ui::VListBox* vbox_container;
	bool _skip_scrollChangeEvent = false;//按按钮时跳过一次滚动数据
#define UISETTINGSPAGE_TABOP_Declarations(op) ui::Option* tabop_##op;ui::Box* header_##op;SettingsPage_##op settingsPage_##op;
	UISETTINGSPAGE_TABOP_LIST(UISETTINGSPAGE_TABOP_Declarations);
#undef UISETTINGSPAGE_TABOP_Declarations



		
	
	//音频
	bool OnSoundSliderValueChanged(const ui::EventArgs& msg);
	ui::Slider* slider_volume;


};
















#endif