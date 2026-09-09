#include"Dui.h"
#include"SettingsPage.h"


void SettingsPage_other::InitContents(class SettingsPage* parent)
{

	cb_taskBarIcon = (ui::CheckBox*)parent->FindSubControl(L"cb_taskBarIcon");
	cb_startOnBoot = (ui::CheckBox*)parent->FindSubControl(L"cb_startOnBoot");
	cb_startAsAdmin = (ui::CheckBox*)parent->FindSubControl(L"cb_startAsAdmin");


	cb_taskBarIcon->Selected(AppSettings::GetIns().GetOtherShowTaskBarIcon());
	cb_startOnBoot->Selected(AppSettings::GetIns().GetOtherStartOnBoot());
	cb_startAsAdmin->Selected(AppSettings::GetIns().GetOtherWindows_RunAsAdmin());

	cb_taskBarIcon->AttachSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));
	cb_taskBarIcon->AttachUnSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));
	cb_startOnBoot->AttachSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));
	cb_startOnBoot->AttachUnSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));
	cb_startAsAdmin->AttachSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));
	cb_startAsAdmin->AttachUnSelect(std::bind(&SettingsPage_other::OnCheckBoxClicked, this, std::placeholders::_1));


}

bool SettingsPage_other::OnCheckBoxClicked(const ui::EventArgs& msg)
{
	if (msg.GetSender() == cb_taskBarIcon)
	{
		AppSettings::GetIns().SetOtherShowTaskBarIcon(cb_taskBarIcon->IsSelected());
	}
	else if (msg.GetSender() == cb_startOnBoot)
	{
		AppSettings::GetIns().SetOtherStartOnBoot(cb_startOnBoot->IsSelected());
	}
	else if (msg.GetSender() == cb_startAsAdmin)
	{
		AppSettings::GetIns().SetOtherWindows_RunAsAdmin(cb_startAsAdmin->IsSelected());
	}
	return true;
}
