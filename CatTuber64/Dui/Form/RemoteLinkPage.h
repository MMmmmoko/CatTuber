#ifndef _RemoteLinkPage_h
#define _RemoteLinkPage_h


#include"UIPageBase.h"



class RemoteLinkPage :public UIPageBase
{

public:
	RemoteLinkPage(ui::Window* pWindow) :UIPageBase(pWindow) {};
	virtual void InitContents(uintptr_t userdata1=0, uintptr_t userdata2=0)override;
	virtual MainUiForm::PageEnum GetPageType()override { return MainUiForm::PageEnum::PAGE_REMOTELINK; };



private:
	enum LinkType
	{
		LinkType_InputSource=0,
		LinkType_OutputTarget=1,
	};
	bool OnLinkTypeToggleClicked(const ui::EventArgs& args);
	void _SetTogleStyle(LinkType link);

	void _StartSend();
	void _StopSend();
	LinkType _currenType = LinkType_InputSource;

	ui::TabBox* tabBox_SettingContainer = nullptr;
	ui::ButtonBox* toggle_linkType = nullptr;

	//发送端
	ui::RichEdit* edit_remoteAddr = nullptr;
	ui::RichEdit* edit_remotePort = nullptr;
};
















#endif