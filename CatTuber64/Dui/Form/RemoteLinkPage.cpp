#include"Dui.h"
#include"Input/RemoteInputLink.h"
#include"RemoteLinkPage.h"
#include"AppSettings.h"
#include"AppContext.h"
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
	
	
	



		//输出端控件
		{

			edit_remoteAddr = (ui::RichEdit*)FindSubControl(L"edit_remoteAddr");
			edit_remotePort = (ui::RichEdit*)FindSubControl(L"edit_remotePort");
			cb_StopUpdate = (ui::CheckBox*)FindSubControl(L"cb_StopUpdate");
			cb_AutoReconnect = (ui::CheckBox*)FindSubControl(L"cb_AutoReconnect");
			cb_OutputWhenStart = (ui::CheckBox*)FindSubControl(L"cb_OutputWhenStart");

			btn_StartOutput = (ui::Button*)FindSubControl(L"btn_StartOutput");
			btn_StopOutput = (ui::Button*)FindSubControl(L"btn_StopOutput");
		

			text_Output = (ui::Label*)FindSubControl(L"text_Output");
			

		}



	//接受页控件
		{
			btn_copyIP = (ui::Button*)FindSubControl(L"btn_copyIP");
			btn_copyName = (ui::Button*)FindSubControl(L"btn_copyName");
			btn_randomPort = (ui::Button*)FindSubControl(L"btn_randomPort");

			edit_localPort = (ui::RichEdit*)FindSubControl(L"edit_localPort");
			cb_BlockLocalInput = (ui::CheckBox*)FindSubControl(L"cb_BlockLocalInput");
			cb_ReceiveWhenStart = (ui::CheckBox*)FindSubControl(L"cb_ReceiveWhenStart");

			btn_StartReceive = (ui::Button*)FindSubControl(L"btn_StartReceive");
			btn_StopReceive = (ui::Button*)FindSubControl(L"btn_StopReceive");

			text_Receive = (ui::Label*)FindSubControl(L"text_Receive");
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


			btn_StartOutput->AttachClick(ui::UiBind(&RemoteLinkPage::OnSendReceiveButtonClicked, this, std::placeholders::_1));
			btn_StopOutput->AttachClick(ui::UiBind(&RemoteLinkPage::OnSendReceiveButtonClicked, this, std::placeholders::_1));


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


			btn_StartReceive->AttachClick(ui::UiBind(&RemoteLinkPage::OnSendReceiveButtonClicked, this, std::placeholders::_1));
			btn_StopReceive->AttachClick(ui::UiBind(&RemoteLinkPage::OnSendReceiveButtonClicked, this, std::placeholders::_1));
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




	AppContext::SetRemoteLinkUIPagePointer(this);
}

void RemoteLinkPage::UpdateLinkStates()
{
	RemoteInputLink::_Link_States linkStates = RemoteInputLink::GetIns().GetLinkStates();



	switch (linkStates)
	{
	case RemoteInputLink::_Link_States_None:
		_SetControlsEnableState(true);
		btn_StartOutput->SetVisible(true);
		btn_StopOutput->SetVisible(false);
		btn_StartReceive->SetVisible(true);
		btn_StopReceive->SetVisible(false);

		if (text_Output->GetTextId() == L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RUNNING")
		{
			text_Output->SetTextId(L"");
			text_Output->SetText(L"");
		}
		if (text_Receive->GetTextId() == L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_RUNNING")
		{
			text_Receive->SetTextId(L"");
			text_Receive->SetText(L"");
		}

		break;
	case RemoteInputLink::_Link_States_InputSource_Resolving:
		_SetControlsEnableState(false);
		btn_StartOutput->SetVisible(false);
		btn_StopOutput->SetVisible(true);

		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RESOLVING");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");


		//页面回到发送端
		_SetLinkPage(LinkType_InputSource);
		break;
	case RemoteInputLink::_Link_States_InputSource_Connecting:
		_SetControlsEnableState(false);
		btn_StartOutput->SetVisible(false);
		btn_StopOutput->SetVisible(true);


		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_CONNECTING");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");

		//页面回到发送端
		_SetLinkPage(LinkType_InputSource);
		break;
	case RemoteInputLink::_Link_States_InputSource_Reconnecting:
		_SetControlsEnableState(false);
		btn_StartOutput->SetVisible(false);
		btn_StopOutput->SetVisible(true);


		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RECONNECTING");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");

		//页面回到发送端
		_SetLinkPage(LinkType_InputSource);
		break;
	case RemoteInputLink::_Link_States_InputSource_Running:
	{
		_SetControlsEnableState(false);
		btn_StartOutput->SetVisible(false);
		btn_StopOutput->SetVisible(true);

		std::string addrStr = RemoteInputLink::GetIns().GetSocketAddrStr();
		//"***** [addr]"
		std::wstring formatStr = GETDUISTRING(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_RUNNING");
		ui::StringUtil::ReplaceAll(L"[addr]", ui::StringConvert::UTF8ToWString(addrStr), formatStr);
		text_Output->SetText(formatStr);
		text_Output->SetTextId(L"");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");


		//页面回到发送端
		_SetLinkPage(LinkType_InputSource);
		break;
	}




	case RemoteInputLink::_Link_States_OutputTarget_ReceivePreparing:
		_SetControlsEnableState(false);



		btn_StartReceive->SetVisible(false);
		btn_StopReceive->SetVisible(true);

		text_Receive->SetText(L"");
		text_Receive->SetTextId(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_RECEIVEPREPARING");
		text_Receive->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");
		//页面回到接收端
		_SetLinkPage(LinkType_OutputTarget);

		break;
	case RemoteInputLink::_Link_States_OutputTarget_WaitConnect:
		_SetControlsEnableState(false);

		btn_StartReceive->SetVisible(false);
		btn_StopReceive->SetVisible(true);

		text_Receive->SetText(L"");
		text_Receive->SetTextId(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_WAITCONNECTIION");
		text_Receive->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");
		_SetLinkPage(LinkType_OutputTarget);

		break;
	case RemoteInputLink::_Link_States_OutputTarget_Running:
	{
		_SetControlsEnableState(false);

		btn_StartReceive->SetVisible(false);
		btn_StopReceive->SetVisible(true);

		std::string addrStr = RemoteInputLink::GetIns().GetSocketAddrStr();

		//"***** [addr]"
		std::wstring formatStr = GETDUISTRING(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_RUNNING");
		//std::wstring resultStr = ui::StringUtil::Printf(formatStr.c_str(), addrStr.c_str());
		ui::StringUtil::ReplaceAll(L"[addr]", ui::StringConvert::UTF8ToWString(addrStr), formatStr);


		text_Receive->SetText(formatStr);
		text_Receive->SetTextId(L"");
		text_Receive->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"textNormalColor");
		//页面回到接收端
		_SetLinkPage(LinkType_OutputTarget);


		break;
	}

	default:
		break;
	}
	


}

void RemoteLinkPage::UpdateLoaclPort()
{
	edit_localPort->SetText(std::to_wstring(AppSettings::GetIns().GetRemoteLinkLocalPort()));
}

void RemoteLinkPage::UpdateLinkEndStates()
{
	//连接出现问题时，显示红色文字提示

	RemoteInputLink::_UI_EndStates endState = RemoteInputLink::GetIns().GetUIEndStates();

	switch (endState)
	{
	case RemoteInputLink::_UI_EndStates_None:
		break;
	case RemoteInputLink::_UI_EndStates_ResolveEmptyAddrStr:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RESOLVEEMPTYADDRSTR");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ResolveFailed:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RESOLVEFAILED");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ResolveOvertime:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_RESOLVEOVERTIME");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ConnectFailed:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_CONNECTFAILED");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ConnectionInterruption:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"STRID_REMOTELINK_AS_INPUT_SOURCE_TEXT_CONNECTIONINTERRUPTION");
		text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ManuallyStopOutput:
		text_Output->SetText(L"");
		text_Output->SetTextId(L"");
		//text_Output->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;



	case RemoteInputLink::_UI_EndStates_ServerCreateFailed:
		text_Receive->SetText(L"");
		text_Receive->SetTextId(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_PREPARATIONFAILED");
		text_Receive->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ErrorOccurredWhileWaiting:
		text_Receive->SetText(L"");
		text_Receive->SetTextId(L"STRID_REMOTELINK_AS_OUTPUT_TARGET_TEXT_ERROROCCURREDWHILEWAITING");
		text_Receive->SetStateTextColor(ui::ControlStateType::kControlStateNormal, L"warningColor");
		break;
	case RemoteInputLink::_UI_EndStates_ManuallyStopReceive:
		text_Receive->SetText(L"");
		text_Receive->SetTextId(L"");
		break;
	default:
		break;
	}
}


void RemoteLinkPage::_SetLinkPage(LinkType link)
{
	_currenType = link;	
	_SetTogleStyle(_currenType);
	tabBox_SettingContainer->SelectItem(_currenType);
}
bool RemoteLinkPage::OnLinkTypeToggleClicked(const ui::EventArgs& args)
{
	_SetLinkPage((LinkType)(1 - _currenType));
	//_currenType =(LinkType)( 1 - _currenType);
	//_SetTogleStyle(_currenType);
	//tabBox_SettingContainer->SelectItem(_currenType);
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

bool RemoteLinkPage::OnSendReceiveButtonClicked(const ui::EventArgs& args)
{
	if (args.GetSender() == btn_StartOutput)
	{
		RemoteInputLink::GetIns().StartSend(
			ui::StringConvert::WStringToUTF8(edit_remoteAddr->GetText()).c_str(),
			(unsigned short)edit_remotePort->GetTextNumber(),
			cb_StopUpdate->IsSelected()
		);
	}
	else if (args.GetSender() == btn_StopOutput)
	{
		RemoteInputLink::GetIns().StopSend();
	}
	else if (args.GetSender() == btn_StartReceive)
	{
		RemoteInputLink::GetIns().StartReceive(cb_BlockLocalInput->IsSelected());
	}
	else if (args.GetSender() == btn_StopReceive)
	{
		RemoteInputLink::GetIns().StopReceive();
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

void RemoteLinkPage::_SetControlsEnableState(bool enable)
{
	toggle_linkType->SetEnabled(enable);
	edit_remoteAddr->SetEnabled(enable);
	edit_remotePort->SetEnabled(enable);
	//cb_StopUpdate->SetEnabled(enable);
	//cb_AutoReconnect->SetEnabled(enable);
	//cb_OutputWhenStart->SetEnabled(enable);

	//btn_copyIP;
	//btn_copyName;
	btn_randomPort->SetEnabled(enable);
	edit_localPort->SetEnabled(enable);
	cb_BlockLocalInput->SetEnabled(enable);
	//cb_ReceiveWhenStart->SetEnabled(enable);







	//if (enable)
	//{
	//	toggle_linkType->SetEnabled(enable);
	//}
}





