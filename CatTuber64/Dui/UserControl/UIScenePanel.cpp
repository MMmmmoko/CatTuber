#include"DuiCommon.h"
#include"Form/MainUiForm.h"
#include"UIScenePanel.h"

#include"../Item/Scene.h"
#include"../Item/SceneManager.h"
#include"../Item/MainSceneItem.h"
#include"../Item/ClassicItem.h"
#include"../Item/CharacterObject.h"
#include"../Item/DeskObject.h"
#include"../Item/HandheldItemObject.h"
#include"Pack/Pack.h"
UIScenePanel::UIScenePanel(MainUiForm* pParent)
	:pParent(pParent),ui::VBox(pParent)
{
	ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UIScenePanel.xml"));


	btnSceneTitle =(ui::Button*)FindSubControl(L"scenePanel_btnSceneTitle");







	scenePanel_classic =(ui::VBox*)FindSubControl(L"scenePanel_classicItemPanel");

	classicPanel_slotCharacter = (ui::Button*)FindSubControl(L"classicSlot_Character");
	classicPanel_slotDesk = (ui::Button*)FindSubControl(L"classicSlot_Desk");
	classicPanel_slotHandheldItem = (ui::Button*)FindSubControl(L"classicSlot_HandheldItem");




	scenePanel_bongoCat =(ui::VBox*)FindSubControl(L"scenePanel_bongoCatPanel");

	bongoCatPanel_slot =(ui::Button*)FindSubControl(L"bongoCatSlot");



	//绑定事件
	btnSceneTitle->AttachClick(ui::UiBind(&UIScenePanel::OnSceneTitleClicked, this, std::placeholders::_1));

	classicPanel_slotCharacter->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));
	classicPanel_slotDesk->AttachClick(ui::UiBind(&UIScenePanel::OnMainItemSlotClicked, this, std::placeholders::_1));
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
		{
			//显示角色桌子面板
			scenePanel_classic->SetVisible(true);


			ClassicItem* pClassicItem = dynamic_cast<ClassicItem*>(mainItem);

#define __TOWSTRING(x) L#x
#define SET_SLOT_IMAGE(ItemType) \
			ItemType##Object* ItemType = pClassicItem->Get##ItemType();\
			if (ItemType)\
			{\
			const char* ItemType##PackPath = ItemType->GetPackPath();\
				Pack ItemType##Pack;\
				std::wstring imgFile;\
				if (ItemType##Pack.Open(ItemType##PackPath))\
				{\
					if (ItemType##Pack.IsFileExist("Slot.png"))imgFile = L"Slot.png";\
					else if (ItemType##Pack.IsFileExist("Slot.jpg"))imgFile = L"Slot.jpg";\
					else if (ItemType##Pack.IsFileExist("Slot.gif"))imgFile = L"Slot.gif";\
					if (!imgFile.empty())\
					{\
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(ItemType##PackPath), imgFile);\
					}\
				}\
				if (imgFile.empty())\
					imgFile =  __TOWSTRING(ClassicDefaultSlotImage_##ItemType##.png);\
				classicPanel_slot##ItemType->SetBkImage(imgFile);\
			classicPanel_slot##ItemType->SetBoxShadow(L"color='shadowColor' blurradius='4' offset='0,1' spreadradius='0'");\
			}\
			else\
			{\
				classicPanel_slot##ItemType->SetBkImage( __TOWSTRING(ClassicSlotNoItemBG_##ItemType##.png));\
classicPanel_slot##ItemType->SetBoxShadow(L"blurradius='0'");\
			}

			SET_SLOT_IMAGE(Character);
			SET_SLOT_IMAGE(Desk);
			SET_SLOT_IMAGE(HandheldItem);
#undef SET_SLOT_IMAGE


		}

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
	else if (classicPanel_slotDesk == msg.GetSender())
	{
		pParent->GoToPage(L"CLASSIC_DESK_SELECT_PAGE");
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
