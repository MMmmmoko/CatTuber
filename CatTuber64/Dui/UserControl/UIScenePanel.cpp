#include"DuiCommon.h"
#include"Form/MainUiForm.h"
#include"UIScenePanel.h"

#include"../Item/Scene.h"
#include"../Item/SceneManager.h"
#include"../Item/MainSceneItem.h"

UIScenePanel::UIScenePanel(MainUiForm* pParent)
	:pParent(pParent),ui::VBox(pParent)
{
	ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UIScenePanel.xml"));


	btnSceneTitle =(ui::Button*)FindSubControl(L"scenePanel_btnSceneTitle");







	scenePanel_classic =(ui::VBox*)FindSubControl(L"scenePanel_classicItemPanel");

	classicPanel_slotCharacter = (ui::Button*)FindSubControl(L"classicSlot_Character");
	classicPanel_slotTable = (ui::Button*)FindSubControl(L"classicSlot_Table");
	classicPanel_slotHandheldItem = (ui::Button*)FindSubControl(L"classicSlot_HandheldItem");




	scenePanel_bongoCat =(ui::VBox*)FindSubControl(L"scenePanel_bongoCatPanel");

	bongoCatPanel_slot =(ui::Button*)FindSubControl(L"bongoCatSlot");



	//绑定事件
	btnSceneTitle->AttachClick(ui::UiBind(&UIScenePanel::OnSceneTitleClicked, this, std::placeholders::_1));

	classicPanel_slotCharacter->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));
	classicPanel_slotTable->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));
	classicPanel_slotHandheldItem->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));
	bongoCatPanel_slot->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));



}



void UIScenePanel::InitContents()
{
	std::string sceneName=  SceneManager::GetInstance().GetCurrentScene().name  ;
	if (sceneName.empty())
	{
		btnSceneTitle->SetTextId(L"STRID_SCENEPANEL_UNNAMEDSCENE");
	}
	else
	{
		btnSceneTitle->SetUTF8Text(sceneName);
	}


	auto mainItem= SceneManager::GetInstance().GetCurrentMainSceneItem();


	scenePanel_classic->SetVisible(false);
	scenePanel_bongoCat->SetVisible(false);


	if (mainItem)
	{
		switch (mainItem->GetMainItemType())
		{
			case MainSceneItem::MainSceneItemType_Classic:
				//显示角色桌子面板
				scenePanel_classic->SetVisible(true);

				break;
			case MainSceneItem::MainSceneItemType_IntegralModel:
				//显示角色桌子面板
				break;
			case MainSceneItem::MainSceneItemType_BongoCat:
				//显示BongoCat面板
				scenePanel_bongoCat->SetVisible(true);
				break;
		default:
			break;
		}




	}
	else
	{
		//不显示主物件面板
	}





}

bool UIScenePanel::OnSceneTitleClicked(const ui::EventArgs& msg)
{
	pParent->GoToPage(L"SCENESELECT_PAGE");
	return true;
}

bool UIScenePanel::OnMainItemSlotClicked(const ui::EventArgs& msg)
{
	if (classicPanel_slotCharacter == msg.GetSender())
	{
		pParent->GoToPage(L"CLASSIC_CHARACTER_SELECT_PAGE");
	}
	else if (classicPanel_slotTable == msg.GetSender())
	{
		pParent->GoToPage(L"CLASSIC_TABLE_SELECT_PAGE");
	}
	else if (classicPanel_slotHandheldItem == msg.GetSender())
	{
		pParent->GoToPage(L"CLASSIC_HANDHELDITEM_SELECT_PAGE");
	}
	else if (bongoCatPanel_slot == msg.GetSender())
	{
		pParent->GoToPage(L"BONGOCAT_SELECT_PAGE");
	}

	return true;
}
