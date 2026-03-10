#include"DuiCommon.h"
#include"../Form/MainUiForm.h"
#include"UISceneMainItemSlot_Classic.h"

void UISceneMainItemSlot_Classic::InitControls(MainUiForm* pWindow, ClassicItem* pClassicItem)
{
	mainForm = pWindow;

	ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UISceneMainItemSlot_Classic.xml"));


	slot_Character=static_cast <ui::Option*>(FindSubControl(L"classicSlot_Character"));
	slot_Table = static_cast <ui::Option*>(FindSubControl(L"classicSlot_Table"));
	slot_HandheldItem = static_cast <ui::Option*>(FindSubControl(L"classicSlot_HandheldItem"));

	slot_Character->AttachSelect(ui::UiBind(&UISceneMainItemSlot_Classic::OnSlotOptionClicked, this, std::placeholders::_1));
	slot_Table->AttachSelect(ui::UiBind(&UISceneMainItemSlot_Classic::OnSlotOptionClicked, this, std::placeholders::_1));
	slot_HandheldItem->AttachSelect(ui::UiBind(&UISceneMainItemSlot_Classic::OnSlotOptionClicked, this, std::placeholders::_1));
}







bool UISceneMainItemSlot_Classic::OnSlotOptionClicked(const ui::EventArgs& pEventArgs)
{
	if (pEventArgs.GetSender() == slot_Character)
	{
		mainForm->GoToPage(L"CLASSIC_CHARACTER_SELECT_PAGE");
	}
	else if (pEventArgs.GetSender() == slot_Table)
	{
		mainForm->GoToPage(L"CLASSIC_TABLE_SELECT_PAGE");
	}
	else if (pEventArgs.GetSender() == slot_HandheldItem)
	{
		mainForm->GoToPage(L"CLASSIC_HANDHELDITEM_SELECT_PAGE");
	}



	return true;
}
