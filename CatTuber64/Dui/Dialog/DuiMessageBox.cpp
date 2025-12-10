#include"DuiCommon.h"
#include"DuiMessageBox.h"


DuiSimpleMessageBox::Button DuiSimpleMessageBox::ShowModalDlg(ui::Window* parent,const std::wstring & title, const std::wstring& content, int buttonFlag)
{
	DuiSimpleMessageBox::Button returnButton = DuiSimpleMessageBox::BUTTON_CLOSE;
	DuiSimpleMessageBox* dialog = new DuiSimpleMessageBox(title, content, buttonFlag, &returnButton);
	dialog->CreateWnd(parent, ui::WindowCreateParam(title,true));
	dialog->PostQuitMsgWhenClosed(false);
	dialog->ShowModalFake();
	return returnButton;
}


DuiSimpleMessageBox::DuiSimpleMessageBox(const std::wstring& title, const std::wstring& content, int buttonFlag, Button* resultOut)
	:title(title), content(content), buttonFlag(buttonFlag), resultOut(resultOut)
{

}

void DuiSimpleMessageBox::OnInitWindow()
{

	__super::OnInitWindow();

	//寻找目标控件
	auto pTitle = static_cast<ui::Label*>(FindControl(L"text_Title"));
	auto pContent = static_cast<ui::Label*>(FindControl(L"text_Content"));

	pTitle->SetText(title);
	pContent->SetText(content);

	//根据buttonFlag设置按钮显示与否

	auto pBtnClose = static_cast<ui::Button*>(FindControl(L"btn_Close"));
	auto pBtnOk = static_cast<ui::Button*>(FindControl(L"btn_Ok"));
	auto pBtnCancel = static_cast<ui::Button*>(FindControl(L"btn_Cancel"));


	if (buttonFlag & BUTTON_OK)
	{
		pBtnOk->SetVisible(true);
	}

	if (buttonFlag & BUTTON_CANCEL)
	{
		pBtnCancel->SetVisible(true);
	}

	pBtnClose->AttachClick(UiBind(&DuiSimpleMessageBox::OnButtonClick, this, std::placeholders::_1));
	pBtnOk->AttachClick(UiBind(&DuiSimpleMessageBox::OnButtonClick, this, std::placeholders::_1));
	pBtnCancel->AttachClick(UiBind(&DuiSimpleMessageBox::OnButtonClick, this, std::placeholders::_1));

}


bool DuiSimpleMessageBox::OnButtonClick(const ui::EventArgs& args)
{
	auto btnname=args.GetSender()->GetName();

	if (btnname == L"btn_Close")
	{
		*resultOut = BUTTON_CLOSE;
		this->CloseWnd();
		return true;
	}

	if (btnname == L"btn_Cancel")
	{
		*resultOut = BUTTON_CANCEL;
		this->CloseWnd();
		return true;
	}

	if (btnname == L"btn_OK")
	{
		*resultOut = BUTTON_OK;
		this->CloseWnd();
		return true;
	}
	return false;
}

LRESULT DuiSimpleMessageBox::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
	if (ui::kVK_RETURN == vkCode&& (buttonFlag& BUTTON_OK))
	{
		*resultOut = BUTTON_OK;
		this->CloseWnd();
		bHandled = true;
		return true;
	}
	if (ui::kVK_ESCAPE == vkCode )
	{
		*resultOut = (buttonFlag & BUTTON_CANCEL)? BUTTON_CANCEL:BUTTON_CLOSE;
		this->CloseWnd();
		bHandled = true;
		return true;
	}


	return Window::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}
