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
	bool OnReceivePageButtonClicked(const ui::EventArgs& args);


	//Edit空间失去焦点的时候填充信息
	bool OnEditLoseFocus(const ui::EventArgs& args);
	bool OnEditReturn(const ui::EventArgs& args);//回车时失去交点，触发保存
	bool OnCheckBoxClick(const ui::EventArgs& args);



	void _SetTogleStyle(LinkType link);

	void _StartSend();
	void _StopSend();
	LinkType _currenType = LinkType_InputSource;

	ui::TabBox* tabBox_SettingContainer = nullptr;
	ui::ButtonBox* toggle_linkType = nullptr;

	//发送端
	ui::RichEdit* edit_remoteAddr = nullptr;
	ui::RichEdit* edit_remotePort = nullptr;
	ui::CheckBox* cb_StopUpdate = nullptr;
	ui::CheckBox* cb_AutoReconnect = nullptr;
	ui::CheckBox* cb_OutputWhenStart = nullptr;

	ui::Button* btn_StartOutput = nullptr;
	ui::Button* btn_StopOutput = nullptr;




	//接收端
	ui::Button* btn_copyIP = nullptr;
	ui::Button* btn_copyName = nullptr;
	ui::Button* btn_randomPort = nullptr;
	ui::RichEdit* edit_localPort = nullptr;
	ui::CheckBox* cb_BlockLocalInput = nullptr;
	ui::CheckBox* cb_ReceiveWhenStart = nullptr;

	ui::Button* btn_StartReceive = nullptr;
	ui::Button* btn_StopReceive = nullptr;


};
















#endif