#include"Dui.h"
#include"RemoteLinkPage.h"
#include"AppSettings.h"
#include"Net/SDL_net.h"

constexpr int toggleRound = 4;

void RemoteLinkPage::InitContents(uintptr_t userdata1, uintptr_t userdata2)
{

	if(!tabBox_SettingContainer)
	{
		ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/RemoteLinkPage.xml"));


		//this->AddItem(Box);

		this->SetName(L"REMOTELINK_PAGE");




		tabBox_SettingContainer = (ui::TabBox*)FindSubControl(L"linkSettingsConatiner");


		toggle_linkType = (ui::ButtonBox*)FindSubControl(L"toggle_linkType");
		//切换按钮内部两个文本宽度对齐
		{
			ui::HBox* labelContainer = (ui::HBox*)toggle_linkType->GetItemAt(0);
			int32_t maxTextWidth = std::max(
				((ui::Label*)labelContainer->GetItemAt(0))->EstimateText({ 9999,9999 }).cx,
				((ui::Label*)labelContainer->GetItemAt(1))->EstimateText({ 9999,9999 }).cx
			);
			maxTextWidth += 20;
			toggle_linkType->SetFixedWidth(ui::UiFixedInt(maxTextWidth * 2), true, false);
			labelContainer->GetItemAt(0)->SetFixedWidth(ui::UiFixedInt(maxTextWidth), false, false);
			labelContainer->GetItemAt(1)->SetFixedWidth(ui::UiFixedInt(maxTextWidth), false, false);

			toggle_linkType->SetBorderRound({ toggleRound ,toggleRound });
			_SetTogleStyle(_currenType);
		}
		toggle_linkType->AttachClick(ui::UiBind(&RemoteLinkPage::OnLinkTypeToggleClicked, this, std::placeholders::_1));
	
	
	



		//输出端按钮
		{

			edit_remoteAddr = (ui::RichEdit*)FindSubControl(L"edit_remoteAddr");
			edit_remotePort = (ui::RichEdit*)FindSubControl(L"edit_remotePort");
			cb_StopUpdate = (ui::CheckBox*)FindSubControl(L"cb_StopUpdate");
			cb_AutoReconnect = (ui::CheckBox*)FindSubControl(L"cb_AutoReconnect");
			cb_OutputWhenStart = (ui::CheckBox*)FindSubControl(L"cb_OutputWhenStart");

			btn_StartOutput = (ui::Button*)FindSubControl(L"btn_StartOutput");
			btn_StopOutput = (ui::Button*)FindSubControl(L"btn_StopOutput");
		


			

		}



	//接受页按钮
		{
			btn_copyIP = (ui::Button*)FindSubControl(L"btn_copyIP");
			btn_copyName = (ui::Button*)FindSubControl(L"btn_copyName");
			btn_randomPort = (ui::Button*)FindSubControl(L"btn_randomPort");

			edit_localPort = (ui::RichEdit*)FindSubControl(L"edit_localPort");
			cb_BlockLocalInput = (ui::CheckBox*)FindSubControl(L"cb_BlockLocalInput");
			cb_ReceiveWhenStart = (ui::CheckBox*)FindSubControl(L"cb_ReceiveWhenStart");

			btn_StartReceive = (ui::Button*)FindSubControl(L"btn_StartReceive");
			btn_StopReceive = (ui::Button*)FindSubControl(L"btn_StopReceive");
		}
	
	
	
	
	
	//填充保存的信息
		{
			edit_remoteAddr->SetText(AppSettings::GetIns().GetRemoteLinkRemoteAddr());
			edit_remotePort->SetText(std::to_wstring(AppSettings::GetIns().GetRemoteLinkRemotePort()));
			cb_StopUpdate->Selected(AppSettings::GetIns().GetRemoteLinkStopUpdateWhenOutput(), false);
			cb_AutoReconnect->Selected(AppSettings::GetIns().GetRemoteLinkAutoReconnect(), false);
			cb_OutputWhenStart->Selected(AppSettings::GetIns().GetRemoteLinkOutputWhenStart(), false);


			edit_localPort->SetText(std::to_wstring(AppSettings::GetIns().GetRemoteLinkLocalPort()));
			cb_BlockLocalInput->Selected(AppSettings::GetIns().GetRemoteLinkBlockInputWhenReceive(), false);

			if (cb_OutputWhenStart->IsSelected())
				cb_ReceiveWhenStart->Selected(false,true);
			else
				cb_ReceiveWhenStart->Selected(AppSettings::GetIns().GetRemoteLinkReceiveWhenStart(), false);
	
		}
	
		//挂接函数
		{
		//输出端
			edit_remoteAddr->AttachKillFocus(ui::UiBind(&RemoteLinkPage::OnEditLoseFocus, this, std::placeholders::_1));
			edit_remotePort->AttachKillFocus(ui::UiBind(&RemoteLinkPage::OnEditLoseFocus, this, std::placeholders::_1));

			
			edit_remoteAddr->AttachReturn(ui::UiBind(&RemoteLinkPage::OnEditReturn, this, std::placeholders::_1));
			edit_remotePort->AttachReturn(ui::UiBind(&RemoteLinkPage::OnEditReturn, this, std::placeholders::_1));

			cb_StopUpdate->AttachSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_AutoReconnect->AttachSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_OutputWhenStart->AttachSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_StopUpdate->AttachUnSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_AutoReconnect->AttachUnSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_OutputWhenStart->AttachUnSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));



			//接受页
			btn_copyIP->AttachClick(ui::UiBind(&RemoteLinkPage::OnReceivePageButtonClicked, this, std::placeholders::_1));
			btn_copyName->AttachClick(ui::UiBind(&RemoteLinkPage::OnReceivePageButtonClicked, this, std::placeholders::_1));
			btn_randomPort->AttachClick(ui::UiBind(&RemoteLinkPage::OnReceivePageButtonClicked, this, std::placeholders::_1));

			edit_localPort->AttachKillFocus(ui::UiBind(&RemoteLinkPage::OnEditLoseFocus, this, std::placeholders::_1));
			edit_localPort->AttachReturn(ui::UiBind(&RemoteLinkPage::OnEditReturn, this, std::placeholders::_1));
		
			cb_BlockLocalInput->AttachSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_ReceiveWhenStart->AttachSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_BlockLocalInput->AttachUnSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));
			cb_ReceiveWhenStart->AttachUnSelect(ui::UiBind(&RemoteLinkPage::OnCheckBoxClick, this, std::placeholders::_1));

		
		}
	
	
	
	
	
	
	
	
	
	}


	//接收页
	{
		//设置本机信息
		char localIpStr[128];
		if (NET_GetLocalLANAddressStr(localIpStr,sizeof(localIpStr)))
		{
			((ui::Label*)FindSubControl(L"text_loaclIP"))->SetUTF8Text(localIpStr);
		}
		if (NET_GetLocalComputerName(localIpStr, sizeof(localIpStr)))
		{
			((ui::Label*)FindSubControl(L"text_loaclName"))->SetUTF8Text(localIpStr);
		}
		//获取计算机名
	}
}

bool RemoteLinkPage::OnLinkTypeToggleClicked(const ui::EventArgs& args)
{
	_currenType =(LinkType)( 1 - _currenType);
	_SetTogleStyle(_currenType);
	tabBox_SettingContainer->SelectItem(_currenType);
	return true;
}

bool RemoteLinkPage::OnReceivePageButtonClicked(const ui::EventArgs& args)
{
	if (args.GetSender()== btn_copyIP)
	{
		ui::Label* target=((ui::Label*)FindSubControl(L"text_loaclIP"));
		if (target)
		{
			ui::Clipboard::SetClipboardText(target->GetText());
		}
	}
	else if (args.GetSender()== btn_copyName)
	{
		ui::Label* target=((ui::Label*)FindSubControl(L"text_loaclName"));
		if (target)
		{
			ui::Clipboard::SetClipboardText(target->GetText());
		}
	}
	else if (args.GetSender()== btn_randomPort)
	{
		//随机可用的tcp端口
		//NET_Server* server=NET_CreateServer(nullptr,50001);
		//if (server)
		{
			unsigned short port = NET_GetAvailablePort();
			edit_localPort->SetText(std::to_wstring(port));
			//NET_DestroyServer(server);
		}
	}


	return true;
}

bool RemoteLinkPage::OnEditLoseFocus(const ui::EventArgs& args)
{
	//将edit内容保存指APPSETTINGS
	if(edit_remoteAddr == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkRemoteAddr(ui::StringConvert::WStringToUTF8(edit_remoteAddr->GetText()));

	else if (edit_remotePort == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkRemotePort(edit_remotePort->GetTextNumber());

	else if (edit_localPort == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkLocalPort(edit_localPort->GetTextNumber());
	return true;
}

bool RemoteLinkPage::OnEditReturn(const ui::EventArgs& args)
{
	//DUILIB好像没有直接的失焦方案，这里通过将焦点设为父控件来让目标控件失焦
	((ui::RichEdit*)args.GetSender())->GetParent()->SetFocus();
	return true;
}

bool RemoteLinkPage::OnCheckBoxClick(const ui::EventArgs& args)
{

	if (cb_StopUpdate == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkStopUpdateWhenOutput(cb_StopUpdate->IsSelected());
	else if (cb_AutoReconnect == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkAutoReconnect(cb_AutoReconnect->IsSelected());
	else if (cb_OutputWhenStart == args.GetSender())
	{
		//此项目和接收冲突
		AppSettings::GetIns().SetRemoteLinkOutputWhenStart(cb_OutputWhenStart->IsSelected());
		if(cb_OutputWhenStart->IsSelected())
			cb_ReceiveWhenStart->Selected(false,true);
	}


	else if (cb_BlockLocalInput == args.GetSender())
		AppSettings::GetIns().SetRemoteLinkBlockInputWhenReceive(cb_BlockLocalInput->IsSelected());
	else if (cb_ReceiveWhenStart == args.GetSender())
	{
		//此项目和发送冲突
		AppSettings::GetIns().SetRemoteLinkReceiveWhenStart(cb_ReceiveWhenStart->IsSelected());
		if (cb_ReceiveWhenStart->IsSelected())
			cb_OutputWhenStart->Selected(false, true);
	}

	return true;
}

void RemoteLinkPage::_SetTogleStyle(LinkType link)
{
	//文本颜色
	ui::HBox* labelContainer = (ui::HBox*)toggle_linkType->GetItemAt(0);
	((ui::Label*)labelContainer->GetItemAt(_currenType))->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
	((ui::Label*)labelContainer->GetItemAt(size_t(1 - _currenType)))->SetStateTextColor(ui::kControlStateNormal, L"textNormalColor");

	{
		toggle_linkType->SetStateColor(ui::kControlStateNormal, L"subjectColor");

		toggle_linkType->SetStateColorRound(ui::kControlStateNormal, { toggleRound,toggleRound },true);

		int controlWidth=toggle_linkType->GetFixedSize().cx.GetInt32();

		if (link == LinkType_InputSource)
			toggle_linkType->SetStateColorMargin(ui::kControlStateNormal, { 0,0,controlWidth/2,0 }, true);
		else
			toggle_linkType->SetStateColorMargin(ui::kControlStateNormal, { controlWidth / 2,0,0,0 }, true);
	}
}

void RemoteLinkPage::_StartSend()
{
	//解析地址
	std::string remoteAddrStr = ui::StringConvert::WStringToUTF8(edit_remoteAddr->GetText());
	//NET_ResolveHostname(remoteAddrStr.c_str());

}


