#include"Dui.h"
#include"SettingsPage.h"

void SettingsPage_window::InitContents(class SettingsPage* parent)
{
	cb_windowTop =(ui::CheckBox*) parent->FindSubControl(L"cb_windowTop");
	cb_windowTransparent =(ui::CheckBox*) parent->FindSubControl(L"cb_windowTransparent");
	cb_windowLock =(ui::CheckBox*) parent->FindSubControl(L"cb_windowLock");
	cb_windowLockVisibleSave =(ui::CheckBox*) parent->FindSubControl(L"cb_windowLockVisibleSave");
	cb_windowVisible =(ui::CheckBox*) parent->FindSubControl(L"cb_windowVisible");

	cb_windowTop->Selected(AppSettings::GetIns().GetWindowTop());
	cb_windowTransparent->Selected(AppSettings::GetIns().GetWindowTransparent());
	cb_windowLock->Selected(AppSettings::GetIns().GetWindowLock());
	cb_windowLockVisibleSave->Selected(AppSettings::GetIns().GetWindowLockVisibleSave());
	cb_windowVisible->Selected(AppSettings::GetIns().GetWindowVisible());

	cb_windowTop->AttachSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowTop->AttachUnSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowTransparent->AttachSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowTransparent->AttachUnSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowLock->AttachSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowLock->AttachUnSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowLockVisibleSave->AttachSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowLockVisibleSave->AttachUnSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowVisible->AttachSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));
	cb_windowVisible->AttachUnSelect(std::bind(&SettingsPage_window::OnCheckBoxSelected, this, std::placeholders::_1));

}



bool SettingsPage_window::OnCheckBoxSelected(const ui::EventArgs& msg)
{
	if (msg.GetSender() == cb_windowTop)
	{
		AppSettings::GetIns().SetWindowTop(cb_windowTop->IsSelected());
	}
	else if (msg.GetSender() == cb_windowTransparent)
	{
		AppSettings::GetIns().SetWindowTransparent(cb_windowTransparent->IsSelected());
	}
	else if (msg.GetSender() == cb_windowLock)
	{
		AppSettings::GetIns().SetWindowLock(cb_windowLock->IsSelected());
	}
	else if (msg.GetSender() == cb_windowLockVisibleSave)
	{
		AppSettings::GetIns().SetWindowLockVisibleSave(cb_windowLockVisibleSave->IsSelected());
	}
	else if (msg.GetSender() == cb_windowVisible)
	{
		AppSettings::GetIns().SetWindowVisible(cb_windowVisible->IsSelected());
	}






	return true;
}




