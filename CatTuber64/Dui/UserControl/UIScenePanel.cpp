#include"DuiCommon.h"
#include"Dialog/DuiMessageBox.h"
#include"Form/MainUiForm.h"
#include"UIScenePanel.h"

#include"../Item/Scene.h"
#include"../Item/SceneManager.h"
#include"../Item/MainSceneItem.h"
#include"../Item/ClassicItem.h"
#include"../Item/CharacterObject.h"
#include"../Item/DeskObject.h"
#include"../Item/HandheldItemObject.h"
#include"../Item/BongoCatItem.h"
#include"../Item/BongoCatObject.h"
#include"../Item/DecorationItem.h"
#include"../Item/DecorationObject.h"

#include"Pack/Pack.h"
#include"RenderThread.h"
#include "RenderWindowManager.h"





UISceneContentListItem::UISceneContentListItem(UIScenePanel* pParent)
	//:ui::ListBoxItem(pWindow)
	//:ui::ControlDragable(pWindow)
	:ui::ControlDragableT<ui::ListBoxItem>(pParent->GetWindow()), pParent(pParent)
{
	ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UISceneContentListItem.xml"));


	ui::ControlDragableT<ui::ListBoxItem>::SetEnableDragOut(false);
	
}

void UISceneContentListItem::InitSubControls(
	const std::wstring& imgstr1,
	const std::wstring& imgstr2,
	const std::wstring& imgstr3,
	size_t dataIndex)
{
	if (index == MAXSIZE_T)
	{
		//img1 = dynamic_cast<ui::Control*>(FindSubControl(L"listitem_img"));
		//img2 = dynamic_cast<ui::Control*>(FindSubControl(L"listitem_img2"));
		//img3 = dynamic_cast<ui::Control*>(FindSubControl(L"listitem_img3"));


		//右键菜单
		AttachRClick(ui::UiBind(&UISceneContentListItem::OnRightClick, this, std::placeholders::_1));

	

		

		AttachSelect(ui::UiBind(&UISceneContentListItem::OnLeftClick, this, std::placeholders::_1));
		AttachUnSelect(ui::UiBind(&UISceneContentListItem::OnLeftClick, this, std::placeholders::_1));


		AttachSetFocus( [this](const ui::EventArgs&)->bool {
			pParent->UpdatePanelButtonEnable(); return true;
			});

		AttachKillFocus([this](const ui::EventArgs&)->bool {

			ui::VirtualVListBox* parent = dynamic_cast<ui::VirtualVListBox*>(GetOwner());
			
			//parent->SetSelectNone();
			this->Selected(false,true,0);
			pParent->UpdatePanelButtonEnable(); return true;
			});
	}








	//adaptive_dest_rect_fill;

	{
		if (!imgstr1.empty()) {
			if (img1 == nullptr) {
				img1 = std::make_unique<ui::Image>();
				img1->SetControl(this);
			}
		}
		if (!imgstr2.empty()) {
			if (img2 == nullptr) {
				img2 = std::make_unique<ui::Image>();
				img2->SetControl(this);
			}
		}
		if (!imgstr3.empty()) {
			if (img3 == nullptr) {
				img3 = std::make_unique<ui::Image>();
				img3->SetControl(this);
			}
		}
		bool bChanged = false;
		if (img1 != nullptr) {
			if (img1->GetImageString() != imgstr1) {
				bChanged = true;
				img1->SetImageString(imgstr1, Dpi());
			}
		}
		if (img2 != nullptr) {
			if (img2->GetImageString() != imgstr2) {
				bChanged = true;
				img2->SetImageString(imgstr2, Dpi());
			}
		}
		if (img3 != nullptr) {
			if (img3->GetImageString() != imgstr3) {
				bChanged = true;
				img3->SetImageString(imgstr3, Dpi());
			}
		}
		if (bChanged) {
			RelayoutOrRedraw();
		}
	}
	index = dataIndex;
}




void UISceneContentListItem::PaintBkImage(ui::IRender* pRender)
{
	__super::PaintBkImage(pRender);
	if (img1 != nullptr)PaintImage(pRender, img1.get());
	if (img2 != nullptr)PaintImage(pRender, img2.get());
	if (img3 != nullptr)PaintImage(pRender, img3.get());

}

bool UISceneContentListItem::OnRightClick(const ui::EventArgs& args)
{


	//创建菜单

	//右键弹出菜单
	ui::Menu* menu = new ui::Menu(this->GetWindow(), this);
	menu->SetSkinFolder(L"CatTuber_default");
	DString xml(L"UISceneListItemMenu.xml");


	ui::UiPoint curPoint(args.ptMouse.x, args.ptMouse.y + 4);
	this->GetWindow()->ClientToScreen(curPoint);
	menu->ShowMenu(xml, curPoint);


	ui::MenuItem* ItemMenu_Settings = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_Settings"));

	ui::MenuItem* ItemMenu_MoveUp = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_MoveUp"));
	ui::MenuItem* ItemMenu_MoveDown = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_MoveDown"));
	ui::MenuItem* ItemMenu_MoveTop = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_MoveTop"));
	ui::MenuItem* ItemMenu_MoveBottom = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_MoveBottom"));
	
	ui::MenuItem* ItemMenu_Remove = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneListItemMenu_Remove"));




	ui::UiSize sizeMax(9999, 9999);
	int32_t maxW = 0;
#define MAXWCALC(ctl) if (ctl) { int32_t curW = ctl->EstimateSize(sizeMax).cx.GetInt32(); if (curW > maxW)maxW = curW; }
	MAXWCALC(ItemMenu_Settings);
	MAXWCALC(ItemMenu_MoveUp);
	MAXWCALC(ItemMenu_MoveDown);
	MAXWCALC(ItemMenu_MoveTop);
	MAXWCALC(ItemMenu_MoveBottom);
	MAXWCALC(ItemMenu_Remove);


	ItemMenu_Settings->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
	ItemMenu_MoveUp->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
	ItemMenu_MoveDown->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
	ItemMenu_MoveTop->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
	ItemMenu_MoveBottom->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
	ItemMenu_Remove->SetFixedWidth(ui::UiFixedInt(maxW), true, false);



	//添加按钮功能
	ItemMenu_Settings->AttachClick([this](const ui::EventArgs& args)->bool {
		assert(false);
		return true;
		});
	ItemMenu_MoveUp->AttachClick([this](const ui::EventArgs& args)->bool {
		GetProvider()->ItemMoveUp(index);
		return true;
		});
	ItemMenu_MoveDown->AttachClick([this](const ui::EventArgs& args)->bool {
		GetProvider()->ItemMoveDown(index);
		return true;
		});
	ItemMenu_MoveTop->AttachClick([this](const ui::EventArgs& args)->bool {
		GetProvider()->ItemMoveTop(index);
		return true;
		});
	ItemMenu_MoveBottom->AttachClick([this](const ui::EventArgs& args)->bool {
		GetProvider()->ItemMoveBottom(index);
		return true;
		});
	ItemMenu_Remove->AttachClick([this](const ui::EventArgs& args)->bool {
		GetProvider()->RemoveItem(index);
		return true;
		});


	return true;
}



bool UISceneContentListItem::OnLeftClick(const ui::EventArgs& args)
{
	
	//if (IsSelected())SetBorderColor(L"subjectColor_push");
	if (IsSelected())SetBorderSize({1,1,1,1},false);
	else {
		SetBorderSize({ }, false);
		ui::VirtualVListBox* parent = dynamic_cast<ui::VirtualVListBox*>(GetOwner());
		parent->SetSelectNone();
	}
	pParent->UpdatePanelButtonEnable();
	return true;
}







UISceneItemListProvider* UISceneContentListItem::GetProvider()
{
	ui::VirtualVListBox* parent = dynamic_cast<ui::VirtualVListBox*>(GetOwner());
	if (parent)
	{
		return dynamic_cast<UISceneItemListProvider*>(parent->GetDataProvider());
	}
	return nullptr;
}

void UISceneContentListItem::OnItemOrdersChanged(size_t nOldItemIndex, size_t nNewItemIndex)
{
	GetProvider()->ItemOrderChange(nOldItemIndex, nNewItemIndex);
}



void UISceneItemListProvider::LoadItemList(int windowIndex)
{

	itemList.clear();

	RenderWindowController* curWindow = RenderWindowManager::GetIns().GetWindowController(windowIndex);
	if (!curWindow)return;
	targetWindow = curWindow;



	auto& sceneItemList= curWindow->GetScene().GetItemList();
	for (auto it = sceneItemList.rbegin(); it != sceneItemList.rend(); ++it)
	{
		auto& item = *it;
		auto& curItem=itemList.emplace_back();
		curItem.item = item;

		//根据控件计算图片
		{
			//ISceneItem* item = GetProvider()->GetItemInfo(dataIndex).item;

			if (item->GetType() == ClassicItem::_GetType())
			{
				ClassicItem* classicItem = (ClassicItem*)item;
				auto pCharacterObj = classicItem->GetCharacter();
				auto pDeskObj = classicItem->GetDesk();
				auto pHandheldObj = classicItem->GetHandheldItem();

				
				//img1->SetVisible(pDeskObj);
				//img2->SetVisible(pCharacterObj);
				//img3->SetVisible(pHandheldObj);


				if (pCharacterObj)
				{
					Pack pack;
					std::wstring imgFile;
					pack.Open(pCharacterObj->GetPackPath());
					if (pack.IsFileExist("List.png"))imgFile = L"List.png";
					else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
					else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

					if (!imgFile.empty())
					{
						if (!pDeskObj && !pHandheldObj)
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);

							curItem.imgPath2 = imgFile;

						}
						else if ((pDeskObj && !pHandheldObj) )
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//3:4    429
								//L"fadeH:\"400,500,1000,1000\" destSize:\"260,40\" centerPos:\"0.725\""
								L"fadeH:\"379,479,1000,1000\" destSize:\"260,40\" centerPos:\"0.714\""
							);
							curItem.imgPath2 = imgFile;
						}
						else if ((!pDeskObj && pHandheldObj) )
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//2:4    过渡由鼠标提供，因为鼠标所在的图层更高
								//
								//L"fadeH:\"400,500,1000,1000\" destSize:\"260,40\" centerPos:\"0.725\""
								L"destSize:\"260,40\" centerPos:\"0.667\""
							);
							curItem.imgPath2 = imgFile;
						}
						else if (pDeskObj && pHandheldObj)
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//2:3:5 250
								//2:3:4 556
								//L"fadeH:\"450,550,1000,1000\" destSize:\"260,40\" centerPos:\"0.75\""
								L"fadeH:\"506,606,1000,1000\" destSize:\"260,40\" centerPos:\"0.778\""
							);

							curItem.imgPath2 = imgFile;
						}
					}
				}
				if (pDeskObj)
				{
					Pack pack;
					std::wstring imgFile;
					pack.Open(pDeskObj->GetPackPath());
					if (pack.IsFileExist("List.png"))imgFile = L"List.png";
					else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
					else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

					if (!imgFile.empty())
					{
						//桌子均在下方
						if (!pCharacterObj && !pHandheldObj)
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
							curItem.imgPath1 = imgFile;
						}
						else if ((pCharacterObj && !pHandheldObj))
						{
							//桌子和角色3:4
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//3:4    429
								L"destSize:\"260,40\" centerPos:\"0.2143\""
							);
							curItem.imgPath1 = imgFile;
						}
						else if ((!pCharacterObj && pHandheldObj))
						{
							//鼠2:3桌
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//2:3    400
								L"destSize:\"260,40\" centerPos:\"0.7\""
							);
							curItem.imgPath1 = imgFile;
						}
						else if ((pCharacterObj && pHandheldObj))
						{
							//2:3:4
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//2:3    400
								L"destSize:\"260,40\" centerPos:\"0.389\""
							);
							curItem.imgPath1 = imgFile;
						}
					}

				}

				if (pHandheldObj)
				{
					Pack pack;
					std::wstring imgFile;
					pack.Open(pHandheldObj->GetPackPath());
					if (pack.IsFileExist("List.png"))imgFile = L"List.png";
					else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
					else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

					if (!imgFile.empty())
					{

						if (!pDeskObj && !pCharacterObj)
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
							curItem.imgPath3 = imgFile;
						}
						else if ((pDeskObj && !pCharacterObj))
						{

							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//鼠2:3桌
								L"fadeH:\"0,0,350,450\" destSize:\"260,40\" centerPos:\"0.2\""
							); 
							curItem.imgPath3 = imgFile;
						}
						else if ((!pDeskObj && pCharacterObj))
						{

							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//鼠2:4角色 333
								//过渡由鼠标提供，因为鼠标图层高
								L"fadeH:\"0,0,283,383\" destSize:\"260,40\" centerPos:\"0.167\""
							); 
							curItem.imgPath3 = imgFile;
						}
						else if (pDeskObj && pCharacterObj)
						{

							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
								//2:3:4    222
								L"fadeH:\"0,0,172,272\" destSize:\"260,40\" centerPos:\"0.111\""
							);
							curItem.imgPath3 = imgFile;
						}
					}

				}


				if (!pCharacterObj && !pDeskObj && !pHandheldObj)
				{
					ASSERT(false);
					//默认图片
					//curItem.imgPath1 = "";
				}
			}


			//BONGO CAT
			else if (item->GetType() == BongoCatItem::_GetType())
			{
				auto pBongoCatObj = ((BongoCatItem*)item)->GetObj();

				Pack pack;
				std::wstring imgFile;
				pack.Open(pBongoCatObj->GetPackPath());
				if (pack.IsFileExist("List.png"))imgFile = L"List.png";
				else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
				else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";
				if (!imgFile.empty())
				{
					imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
					curItem.imgPath1 = imgFile;
				}
				else
				{

				}
			}

			//DecorationItem
			else if (item->GetType() == DecorationItem::_GetType())
			{
				auto pDecorationObj = ((DecorationItem*)item)->GetObj();

				Pack pack;
				std::wstring imgFile;
				pack.Open(pDecorationObj->GetPackPath());
				if (pack.IsFileExist("List.png"))imgFile = L"List.png";
				else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
				else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";
				if (!imgFile.empty())
				{
					imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
					curItem.imgPath1 = imgFile;
				}
				else
				{

				}
			}









			//img->SetUTF8BkImage(imgstr);

			if (!curItem.imgPath1.empty())
			{
				curItem.imgPath1 = L"file='" + curItem.imgPath1 + L"' adaptive_dest_rect_fill='true'";
			}
			if (!curItem.imgPath2.empty())
			{
				curItem.imgPath2 = L"file='" + curItem.imgPath2 + L"' adaptive_dest_rect_fill='true'";
			}
			if (!curItem.imgPath3.empty())
			{
				curItem.imgPath3 = L"file='" + curItem.imgPath3 + L"' adaptive_dest_rect_fill='true'";
			}


		}


	}
	return;


}








ui::Control* UISceneItemListProvider::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
	UISceneContentListItem* item = new UISceneContentListItem(pParent);
	//item->SetFixedHeight(ui::UiFixedInt(30),true,true);
	return item;
}



bool UISceneItemListProvider::FillElement(ui::Control* pControl, size_t nElementIndex)
{
	//示例中这里用了一个锁
	UISceneContentListItem* pItem = dynamic_cast<UISceneContentListItem*>(pControl);
	ASSERT(pItem != nullptr);
	if (pItem == NULL || nElementIndex >= itemList.size())
	{
		return false;
	}

	//pItem->InitSubControls(itemList[nElementIndex].imgPath, nElementIndex);
	pItem->InitSubControls(
		itemList[nElementIndex].imgPath1,
		itemList[nElementIndex].imgPath2,
		itemList[nElementIndex].imgPath3,
		nElementIndex);
	return true;
}

size_t UISceneItemListProvider::GetElementCount() const
{
	return itemList.size();
}

void UISceneItemListProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{

	if (nElementIndex >= itemList.size())return;
	itemList[nElementIndex].selected = bSelected;
	if (bSelected)
		for (size_t index = 0; index < itemList.size(); index++)
		{
			if (index != nElementIndex)
				itemList[index].selected = false;
		}
}

bool UISceneItemListProvider::IsElementSelected(size_t nElementIndex) const
{
	if (nElementIndex >= itemList.size())return false;
	return itemList[nElementIndex].selected;
}

void UISceneItemListProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
	selectedIndexs.clear();
	selectedIndexs.reserve(itemList.size());
	for (size_t i = 0; i < itemList.size(); i++)
	{
		if (itemList[i].selected)
			selectedIndexs.push_back(i);
	}
}



UISceneItemListProvider::ItemInfo& UISceneItemListProvider::GetItemInfo(size_t nElementIndex)
{
	ASSERT(nElementIndex < itemList.size());
	return itemList[nElementIndex];
}

void UISceneItemListProvider::ItemOrderChange(size_t nOldItemIndex, size_t nNewItemIndex)
{

	ASSERT(nOldItemIndex < itemList.size());
	ASSERT(nNewItemIndex < itemList.size());

	auto element_to_move = std::move(itemList[nOldItemIndex]);


	itemList.erase(itemList.begin() + nOldItemIndex);

	itemList.emplace(itemList.begin() + nNewItemIndex, std::move(element_to_move));


	//不进行全局刷新
	EmitDataChanged(SDL_min(nOldItemIndex, nNewItemIndex), SDL_max(nOldItemIndex, nNewItemIndex));







	RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
		
		RenderWindowController* targetWindow = (RenderWindowController*)userdata;
		uint32_t nOldItemIndex = ((uint32_t*)&userdata2)[0];
		uint32_t nNewItemIndex = ((uint32_t*)&userdata2)[1];

		auto& targetScene = targetWindow->GetScene();
		targetScene.ItemOrderChange_TopToBottom(nOldItemIndex, nNewItemIndex);



		}, targetWindow/*window*/, nOldItemIndex + (nNewItemIndex << 32));




}

void UISceneItemListProvider::ItemMoveUp(size_t nItemIndex)
{
	if (0 == nItemIndex)return;
	ItemOrderChange(nItemIndex, nItemIndex-1);
	this->pParent->GetSceneItemListBox()->GetItemAt(nItemIndex - 1)->SetFocus();
	this->pParent->UpdatePanelButtonEnable();
}

void UISceneItemListProvider::ItemMoveDown(size_t nItemIndex)
{
	if (itemList.size()-1 == nItemIndex)return;
	ItemOrderChange(nItemIndex, nItemIndex+1);
	this->pParent->GetSceneItemListBox()->GetItemAt(nItemIndex + 1)->SetFocus();	
	this->pParent->UpdatePanelButtonEnable();
}

void UISceneItemListProvider::ItemMoveTop(size_t nItemIndex)
{
	if (0 == nItemIndex)return;
	ItemOrderChange(nItemIndex, 0);
	this->pParent->GetSceneItemListBox()->GetItemAt(0)->SetFocus();
	this->pParent->UpdatePanelButtonEnable();
}

void UISceneItemListProvider::ItemMoveBottom(size_t nItemIndex)
{
	if (itemList.size() - 1 == nItemIndex)return;
	ItemOrderChange(nItemIndex, itemList.size() - 1);
	this->pParent->GetSceneItemListBox()->GetItemAt(itemList.size() - 1)->SetFocus();
	this->pParent->UpdatePanelButtonEnable();
}

void UISceneItemListProvider::RemoveItem(size_t nItemIndex)
{
	ASSERT(nItemIndex < itemList.size());
	itemList.erase(itemList.begin() + nItemIndex);

	
	uint64_t userdata2= nItemIndex;
	RenderThread::GetIns().PostTask(
		[](void* userdata,uint64_t userdata2) {
		
			RenderWindowController* window = (RenderWindowController*)userdata;
			uint64_t itemIndex = userdata2;

			auto& targetScene = window->GetScene();
			targetScene.RemoveItem(targetScene.GetItemAt_TopToBottom((int)itemIndex));
		},targetWindow, userdata2
	
	);

	

	EmitCountChanged();
	if (nItemIndex < itemList.size())
	{
		this->pParent->GetSceneItemListBox()->GetItemAt(nItemIndex)->SetFocus();
	}
	this->pParent->UpdatePanelButtonEnable();
}

void UISceneItemListProvider::SetListItemImg(size_t nItemIndex, const char* type, const char* packPath, const char* packPath2, const char* packPath3)
{
	//ASSERT(nItemIndex <= itemList.size());
	bool dataCountChanged = false;
	if (nItemIndex >= itemList.size())
	{
		nItemIndex = 0;
		//itemList.emplace_back();
		itemList.insert(itemList.begin(),ItemInfo());
		dataCountChanged = true;
	}


	//根据控件计算图片
	{
		//ISceneItem* item = GetProvider()->GetItemInfo(dataIndex).item;
		auto& curItem = itemList[nItemIndex];
		curItem.imgPath1 = L"";
		curItem.imgPath2 = L"";
		curItem.imgPath3 = L"";
		if ( type == ClassicItem::_GetType()||SDL_strcmp(type, ClassicItem::_GetType())==0)
		{
			auto pCharacterObj = packPath;
			auto pDeskObj = packPath2;
			auto pHandheldObj = packPath3;


			//img1->SetVisible(pDeskObj);
			//img2->SetVisible(pCharacterObj);
			//img3->SetVisible(pHandheldObj);


			if (pCharacterObj)
			{
				Pack pack;
				std::wstring imgFile;
				pack.Open(pCharacterObj);
				if (pack.IsFileExist("List.png"))imgFile = L"List.png";
				else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
				else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

				if (!imgFile.empty())
				{
					if (!pDeskObj && !pHandheldObj)
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);

						curItem.imgPath2 = imgFile;

					}
					else if ((pDeskObj && !pHandheldObj))
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//3:4    429
							//L"fadeH:\"400,500,1000,1000\" destSize:\"260,40\" centerPos:\"0.725\""
							L"fadeH:\"379,479,1000,1000\" destSize:\"260,40\" centerPos:\"0.714\""
						);
						curItem.imgPath2 = imgFile;
					}
					else if ((!pDeskObj && pHandheldObj))
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//2:4    过渡由鼠标提供，因为鼠标所在的图层更高
							//
							//L"fadeH:\"400,500,1000,1000\" destSize:\"260,40\" centerPos:\"0.725\""
							L"destSize:\"260,40\" centerPos:\"0.667\""
						);
						curItem.imgPath2 = imgFile;
					}
					else if (pDeskObj && pHandheldObj)
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//2:3:5 250
							//2:3:4 556
							//L"fadeH:\"450,550,1000,1000\" destSize:\"260,40\" centerPos:\"0.75\""
							L"fadeH:\"506,606,1000,1000\" destSize:\"260,40\" centerPos:\"0.778\""
						);

						curItem.imgPath2 = imgFile;
					}
				}
			}
			if (pDeskObj)
			{
				Pack pack;
				std::wstring imgFile;
				pack.Open(pDeskObj);
				if (pack.IsFileExist("List.png"))imgFile = L"List.png";
				else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
				else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

				if (!imgFile.empty())
				{
					//桌子均在下方
					if (!pCharacterObj && !pHandheldObj)
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
						curItem.imgPath1 = imgFile;
					}
					else if ((pCharacterObj && !pHandheldObj))
					{
						//桌子和角色3:4
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//3:4    429
							L"destSize:\"260,40\" centerPos:\"0.2143\""
						);
						curItem.imgPath1 = imgFile;
					}
					else if ((!pCharacterObj && pHandheldObj))
					{
						//鼠2:3桌
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//2:3    400
							L"destSize:\"260,40\" centerPos:\"0.7\""
						);
						curItem.imgPath1 = imgFile;
					}
					else if ((pCharacterObj && pHandheldObj))
					{
						//2:3:4
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//2:3    400
							L"destSize:\"260,40\" centerPos:\"0.389\""
						);
						curItem.imgPath1 = imgFile;
					}
				}

			}

			if (pHandheldObj)
			{
				Pack pack;
				std::wstring imgFile;
				pack.Open(pHandheldObj);
				if (pack.IsFileExist("List.png"))imgFile = L"List.png";
				else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
				else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";

				if (!imgFile.empty())
				{

					if (!pDeskObj && !pCharacterObj)
					{
						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
						curItem.imgPath3 = imgFile;
					}
					else if ((pDeskObj && !pCharacterObj))
					{

						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//鼠2:3桌
							L"fadeH:\"0,0,350,450\" destSize:\"260,40\" centerPos:\"0.2\""
						);
						curItem.imgPath3 = imgFile;
					}
					else if ((!pDeskObj && pCharacterObj))
					{

						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//鼠2:4角色 333
							//过渡由鼠标提供，因为鼠标图层高
							L"fadeH:\"0,0,283,383\" destSize:\"260,40\" centerPos:\"0.167\""
						);
						curItem.imgPath3 = imgFile;
					}
					else if (pDeskObj && pCharacterObj)
					{

						imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile,
							//2:3:4    222
							L"fadeH:\"0,0,172,272\" destSize:\"260,40\" centerPos:\"0.111\""
						);
						curItem.imgPath3 = imgFile;
					}
				}

			}


			if (!pCharacterObj && !pDeskObj && !pHandheldObj)
			{
				ASSERT(false);
				//默认图片
				//curItem.imgPath1 = "";
			}
		}


		//BONGO CAT
		else if (type== BongoCatItem::_GetType()||SDL_strcmp(type, BongoCatItem::_GetType())==0)
		{
			auto pBongoCatObj = packPath;

			Pack pack;
			std::wstring imgFile;
			pack.Open(pBongoCatObj);
			if (pack.IsFileExist("List.png"))imgFile = L"List.png";
			else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
			else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";
			if (!imgFile.empty())
			{
				imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
				curItem.imgPath1 = imgFile;
			}
			else
			{
				assert(false);
			}
		}

		//DecorationItem
		else if (type == DecorationItem::_GetType()|| SDL_strcmp(type, DecorationItem::_GetType())==0)
		{
			auto pDecorationObj = packPath;

			Pack pack;
			std::wstring imgFile;
			pack.Open(pDecorationObj);
			if (pack.IsFileExist("List.png"))imgFile = L"List.png";
			else if (pack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
			else if (pack.IsFileExist("List.gif"))imgFile = L"List.gif";
			if (!imgFile.empty())
			{
				imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(pack.GetPath()), imgFile);
				curItem.imgPath1 = imgFile;
			}
			else
			{
				assert(false);
			}
		}









		//img->SetUTF8BkImage(imgstr);

		if (!curItem.imgPath1.empty())
		{
			curItem.imgPath1 = L"file='" + curItem.imgPath1 + L"' adaptive_dest_rect_fill='true'";
		}
		if (!curItem.imgPath2.empty())
		{
			curItem.imgPath2 = L"file='" + curItem.imgPath2 + L"' adaptive_dest_rect_fill='true'";
		}
		if (!curItem.imgPath3.empty())
		{
			curItem.imgPath3 = L"file='" + curItem.imgPath3 + L"' adaptive_dest_rect_fill='true'";
		}


	}

	if (dataCountChanged)
	{
		EmitCountChanged();
	}
	else
	EmitDataChanged(nItemIndex, nItemIndex);

}


















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
	
	auto sceneItemListTabBox = (ui::TabBox*)FindSubControl(L"sceneItemListTabBox");
	//窗口上限为4
	for (int i = 0; i < 4; i++)
	{
		containers[i]=(ui::VirtualVListBox*)sceneItemListTabBox->GetItemAt(i);
	}






	


}

UIScenePanel::~UIScenePanel()
{
	//for (auto x : providers)
	//{
	//	if (x)delete x;
	//}
	providers.clear();
	//if (provider)delete provider;
	//provider = nullptr;
}



void UIScenePanel::InitContents()
{

	//创建与窗口数量相等的provider

	{
		size_t windowCount = RenderWindowManager::GetIns().GetWindowControllerCount();
		providers.resize(windowCount);
		for (size_t i = 0; i < windowCount; i++)
		{
			if (!providers[i])
				providers[i].reset(new UISceneItemListProvider(this));

			auto vlayout = dynamic_cast<ui::VirtualVLayout*>(containers[i]->GetLayout());
			ui::UiSize itemSize;
			itemSize.cx = 260;
			itemSize.cy = 40;
			//vlayout->SetAutoCalcItemWidth(true);
			vlayout->SetItemSize(itemSize, true);
			containers[i]->SetDataProvider(providers[i].get());
			providers[i]->LoadItemList(0);
		}
		currentWindowIndex = 0;
	}


	std::string sceneName=  SceneManager::GetInstance().GetCurrentScene().name  ;
	if (sceneName.empty())
	{
		btnSceneTitle->SetTextId(L"STRID_SCENEPANEL_UNNAMEDSCENE");
	}
	else
	{
		btnSceneTitle->SetUTF8Text(sceneName);
	}


	auto mainItem= RenderWindowManager::GetIns().GetWindowController(currentWindowIndex)->GetScene().GetMainItem();


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
			{
				scenePanel_bongoCat->SetVisible(true);
				BongoCatItem* pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);
				BongoCatObject* pBongoCatObj = pBongoCatItem->GetObj();
				if (pBongoCatObj)
				{
					const char* bongoCatPackPath = pBongoCatObj->GetPackPath();
					Pack bongoCatPack;
					std::wstring imgFile;
					if (bongoCatPack.Open(bongoCatPackPath))
					{
						if (bongoCatPack.IsFileExist("Slot.png"))imgFile = L"Slot.png";
						else if (bongoCatPack.IsFileExist("Slot.jpg"))imgFile = L"Slot.jpg";
						else if (bongoCatPack.IsFileExist("Slot.gif"))imgFile = L"Slot.gif";
						if (!imgFile.empty())
						{
							imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(bongoCatPackPath), imgFile);
						}
					}
					if (imgFile.empty())
						imgFile = L"DefaultBongoCatSlot.png";
					bongoCatPanel_slot->SetBkImage(imgFile);
					bongoCatPanel_slot->SetBoxShadow(L"color='shadowColor' blurradius='4' offset='0,1' spreadradius='0'");
				}
				else
				{
					bongoCatPanel_slot->SetBkImage(L"DefaultBongoCatSlot.png");
					bongoCatPanel_slot->SetBoxShadow(L"blurradius='0'");
				}

			}



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

//void UIScenePanel::UpdatePanelButtonEnable(ui::Control* pChild)
//{
//	if()
//}

void UIScenePanel::UpdatePanelButtonEnable(/*UISceneContentListItem* pChild*/)
{
	//panelFocusedButton = pChild;
	auto& provider = providers[currentWindowIndex];
	auto& container = containers[currentWindowIndex];

	std::vector<size_t> selectElement;
	provider->GetSelectedElements(selectElement);
	if (!selectElement.empty())
	{
		container->GetItemAt(selectElement[0])->SetFocus();
		scenePanel_btnItemRemove->SetEnabled(true);
		scenePanel_btnItemMoveUp->SetEnabled(true);
		scenePanel_btnItemMoveDown->SetEnabled(true);

	}
	else
	{
		scenePanel_btnItemRemove->SetEnabled(false);
		scenePanel_btnItemMoveUp->SetEnabled(false);
		scenePanel_btnItemMoveDown->SetEnabled(false);
	}

}

bool UIScenePanel::OnSceneTitleClicked(const ui::EventArgs& msg)
{
	pParent->GoToPage(MainUiForm::PAGE_SCENESELECT);
	return true;
}

bool UIScenePanel::OnMainItemSlotClicked(const ui::EventArgs& msg)
{
	if (classicPanel_slotCharacter == msg.GetSender())
	{
		pParent->GoToPage(MainUiForm::PAGE_CLASSIC_CHARACTER_SELECT);
	}
	else if (classicPanel_slotDesk == msg.GetSender())
	{
		pParent->GoToPage(MainUiForm::PAGE_CLASSIC_DESK_SELECT);
	}
	else if (classicPanel_slotHandheldItem == msg.GetSender())
	{
		pParent->GoToPage(MainUiForm::PAGE_CLASSIC_HANDHELDITEM_SELECT);
	}
	else if (bongoCatPanel_slot == msg.GetSender())
	{
		pParent->GoToPage(MainUiForm::PAGE_BONGOCAT_SELECT);
	}

	return true;
}

bool UIScenePanel::OnSceneContentListControlButtonClicked(const ui::EventArgs& msg)
{
	auto& provider = providers[currentWindowIndex];
	auto& container = containers[currentWindowIndex];


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
		auto mainItem = RenderWindowManager::GetIns().GetWindowController(currentWindowIndex)->GetScene().GetMainItem();
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

		std::vector<size_t> selectElement;
		provider->GetSelectedElements(selectElement);
		if (!selectElement.empty())
		{
			provider->RemoveItem(selectElement[0]);
		}

	}
	else if (scenePanel_btnItemMoveUp == msg.GetSender())
	{
		std::vector<size_t> selectElement;
		provider->GetSelectedElements(selectElement);
		if (!selectElement.empty())
		{
			provider->ItemMoveUp(selectElement[0]);
		}
	}
	else if (scenePanel_btnItemMoveDown == msg.GetSender())
	{
		std::vector<size_t> selectElement;
		provider->GetSelectedElements(selectElement);
		if (!selectElement.empty())
		{
			provider->ItemMoveDown(selectElement[0]);
		}
	}

	return true;
}

bool UIScenePanel::OnSceneItemAddMenuClicked(const ui::EventArgs& msg)
{
	std::wstring senderName=msg.GetSender()->GetName();


	if (senderName == L"itemAddMenu_ClassicItem")
	{
		//检查是否已经有主物品
		auto mainItem = RenderWindowManager::GetIns().GetWindowController(currentWindowIndex)->GetScene().GetMainItem();
		bool showPage = true;
		if (mainItem)
		{
			switch (mainItem->GetMainItemType())
			{
			case MainSceneItem::MainSceneItemType_Classic:
				break;
			case MainSceneItem::MainSceneItemType_IntegralModel:
			case MainSceneItem::MainSceneItemType_BongoCat:
			{
				//显示一个是否继续的对话框
				//title  添加%s...

				std::wstring title = ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEITEMADD_DLG_MAINITEMWARNING_TITLE");
				ui::StringUtil::ReplaceAll(L"[itemType]", GETDUISTRING(L"STRID_SCENEITEMADDMENU_CLASSIC"), title);

				auto button = DuiSimpleMessageBox::ShowModalDlg(GetWindow(), title, GETDUISTRING(L"STRID_SCENEITEMADD_DLG_MAINITEMWARNING_CONTENT_CLASSIC"),
					DuiSimpleMessageBox::BUTTON_OK | DuiSimpleMessageBox::BUTTON_CANCEL
				);
				showPage = (button == DuiSimpleMessageBox::BUTTON_OK);
			}
			break;
			}
		}
		//不进行页面切换
		//if (showPage)
		//{
		//	pParent->GoToPage(L"BONGOCAT_SELECT_PAGE");
		//}


	}
	else if (senderName == L"itemAddMenu_BongoCatItem")
	{
		//检查是否已经有主物品
		auto mainItem = RenderWindowManager::GetIns().GetWindowController(currentWindowIndex)->GetScene().GetMainItem();
		bool showPage = true;
		if (mainItem)
		{
			switch (mainItem->GetMainItemType())
			{
			case MainSceneItem::MainSceneItemType_Classic:
			case MainSceneItem::MainSceneItemType_IntegralModel:
			{
				//显示一个是否继续的对话框
				//title  添加%s...

				std::wstring title = ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEITEMADD_DLG_MAINITEMWARNING_TITLE");
				ui::StringUtil::ReplaceAll(L"[itemType]", GETDUISTRING(L"STRID_SCENEITEMADDMENU_BONGOCAT"),title);

				auto button=DuiSimpleMessageBox::ShowModalDlg(GetWindow(),title, GETDUISTRING(L"STRID_SCENEITEMADD_DLG_MAINITEMWARNING_CONTENT"),
					DuiSimpleMessageBox::BUTTON_OK| DuiSimpleMessageBox::BUTTON_CANCEL
					);
				showPage = (button == DuiSimpleMessageBox::BUTTON_OK);
			}
				break;
			case MainSceneItem::MainSceneItemType_BongoCat:
				break;
			}
		}
		if (showPage)
		{
			pParent->GoToPage(MainUiForm::PAGE_BONGOCAT_SELECT);
		}




	}
	else if (senderName == L"itemAddMenu_DecorationItem")
	{
		//添加新的装饰物

		pParent->GoToPage(MainUiForm::PAGE_DECORATIONITEM_SELECT);

	}



	return true;
}

