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









	//场景内容列表控件
	scenePanel_btnItemAdd= (ui::Button*)FindSubControl(L"scenePanel_btnItemAdd");
	scenePanel_btnItemRemove = (ui::Button*)FindSubControl(L"scenePanel_btnItemRemove");
	scenePanel_btnItemMoveUp = (ui::Button*)FindSubControl(L"scenePanel_btnItemMoveUp");
	scenePanel_btnItemMoveDown = (ui::Button*)FindSubControl(L"scenePanel_btnItemMoveDown");
	scenePanel_btnItemAdd->AttachClick(ui::UiBind(&UIScenePanel::OnSceneContentListControlButtonClicked, this, std::placeholders::_1));
	scenePanel_btnItemRemove->AttachClick(ui::UiBind(&UIScenePanel::OnSceneContentListControlButtonClicked, this, std::placeholders::_1));
	scenePanel_btnItemMoveUp->AttachClick(ui::UiBind(&UIScenePanel::OnSceneContentListControlButtonClicked, this, std::placeholders::_1));
	scenePanel_btnItemMoveDown->AttachClick(ui::UiBind(&UIScenePanel::OnSceneContentListControlButtonClicked, this, std::placeholders::_1));
	


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

bool UIScenePanel::OnSceneContentListControlButtonClicked(const ui::EventArgs& msg)
{
	if (scenePanel_btnItemAdd == msg.GetSender())
	{
		//创建一个添加物品的菜单

		ui::Menu* menu = new ui::Menu(GetWindow());
		menu->SetSkinFolder(L"CatTuber_default");
		DString xml(L"UIScenePanelItemAddMenu.xml");
		ui::UiPoint cursorPos;
		GetWindow()->GetCursorPos(cursorPos);
		menu->ShowMenu(xml, cursorPos);

		ui::MenuItem* itemAddMenu_ClassicItem = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"itemAddMenu_ClassicItem"));
		ui::MenuItem* itemAddMenu_BongoCatItem = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"itemAddMenu_BongoCatItem"));

		ui::MenuItem* itemAddMenu_DecorationItem = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"itemAddMenu_DecorationItem"));




		//计算控件宽度
		//因为套了一层HBox以绘制图标，这里计算的是内部的子HBox控件的宽度
		int menuItemWidth=std::max(std::max(
			itemAddMenu_ClassicItem->GetItemAt(0)->EstimateSize({ 9999,9999 }).cx.value,
			itemAddMenu_BongoCatItem->GetItemAt(0)->EstimateSize({ 9999,9999 }).cx.value),
			itemAddMenu_DecorationItem->GetItemAt(0)->EstimateSize({ 9999,9999 }).cx.value);
		//设置的是内部子控件的宽度
		itemAddMenu_ClassicItem->GetItemAt(0)-> SetFixedWidth(ui::UiFixedInt(menuItemWidth),false,false);
		itemAddMenu_BongoCatItem->GetItemAt(0)->SetFixedWidth(ui::UiFixedInt(menuItemWidth),false,false);
		itemAddMenu_DecorationItem->GetItemAt(0)->SetFixedWidth(ui::UiFixedInt(menuItemWidth),false,true);
		
		//计算控件可用性
		auto mainItem = SceneManager::GetInstance().GetCurrentMainSceneItem();
		if (mainItem)
		{
			switch (mainItem->GetMainItemType())
			{
			case MainSceneItem::MainSceneItemType_Classic:
				itemAddMenu_ClassicItem->SetEnabled(false);
				break;
			case MainSceneItem::MainSceneItemType_IntegralModel:
				//itemAddMenu_ClassicItem->SetEnabled(false);
				ASSERT(false&&"todo");
				break;
			case MainSceneItem::MainSceneItemType_BongoCat:
				itemAddMenu_BongoCatItem->SetEnabled(false);
				break;
			}
		}




		//添加按钮功能
		itemAddMenu_ClassicItem->AttachClick(ui::UiBind(&UIScenePanel::OnSceneItemAddMenuClicked, this, std::placeholders::_1));
		itemAddMenu_BongoCatItem->AttachClick(ui::UiBind(&UIScenePanel::OnSceneItemAddMenuClicked, this, std::placeholders::_1));
		itemAddMenu_DecorationItem->AttachClick(ui::UiBind(&UIScenePanel::OnSceneItemAddMenuClicked, this, std::placeholders::_1));
	}
	else if (scenePanel_btnItemRemove == msg.GetSender())
	{
	}
	else if (scenePanel_btnItemMoveUp == msg.GetSender())
	{
	}
	else if (scenePanel_btnItemMoveDown == msg.GetSender())
	{
	}

	return true;
}

bool UIScenePanel::OnSceneItemAddMenuClicked(const ui::EventArgs& msg)
{
	std::wstring senderName=msg.GetSender()->GetName();
	if (senderName == L"itemAddMenu_ClassicItem")
	{



	}
	else if (senderName == L"itemAddMenu_BongoCatItem")
	{



	}
	else if (senderName == L"itemAddMenu_DecorationItem")
	{



	}



	return true;
}
