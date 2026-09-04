#ifndef _UIPageBase_h
#define _UIPageBase_h

#include"MainUiForm.h"



class UIPageBase :public ui::VBox
{
public:
	UIPageBase(ui::Window* pwindow) : ui::VBox(pwindow) {};
	virtual void InitContents(uintptr_t userdata1=0, uintptr_t userdata2=0)=0;
	virtual MainUiForm::PageEnum GetPageType() = 0;

	enum PageEnterFlag
	{
		PageEnter_New,
		PageEnter_NavigationLeft,//left/back
		PageEnter_NavigationRight,//right/forward

	};
	virtual void OnEnterThisPage(PageEnterFlag enterFlag) {};
	virtual void OnLeaveThisPage() {};

	virtual bool IsValid() { return true; };
};













#endif