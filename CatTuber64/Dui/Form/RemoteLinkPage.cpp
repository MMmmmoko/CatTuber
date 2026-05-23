#include"Dui.h"
#include"RemoteLinkPage.h"
#include"AppSettings.h"
#include"Net/SDL_net.h"

constexpr int toggleRound = 4;

void RemoteLinkPage::InitContents(uintptr_t userdata1, uintptr_t userdata2)
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
			((ui::Label*)labelContainer->GetItemAt(0))->EstimateText({9999,9999}).cx,
			((ui::Label*)labelContainer->GetItemAt(1))->EstimateText({9999,9999}).cx
		);
		maxTextWidth += 20;
		toggle_linkType->SetFixedWidth(ui::UiFixedInt(maxTextWidth*2),true,false);
		labelContainer->GetItemAt(0)->SetFixedWidth(ui::UiFixedInt(maxTextWidth),false,false);
		labelContainer->GetItemAt(1)->SetFixedWidth(ui::UiFixedInt(maxTextWidth),false,false);

		toggle_linkType->SetBorderRound({ toggleRound ,toggleRound });
		_SetTogleStyle(_currenType);
	}
	toggle_linkType->AttachClick(ui::UiBind(&RemoteLinkPage::OnLinkTypeToggleClicked, this, std::placeholders::_1));



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


