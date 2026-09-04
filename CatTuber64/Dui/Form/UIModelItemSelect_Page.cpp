
#include <algorithm> 
#include"Dui.h"
#include"UIModelItemSelect_Page.h"
#include"Dialog/UISceneItemRename_Dlg.h"
#include"Dialog/UISceneCreateNew_Dlg.h"
#include"Dialog/UISceneCoverCropper_Dlg.h"
#include"Dialog/BongoCatImport_Dlg.h"

#include"AppSettings.h"
#include"AppContext.h"
#include"Util.h"
#include"Pack/Pack.h"
#include"Item/SceneManager.h"
#include "Item/ClassicItem.h"
#include "Item/CharacterObject.h"
#include "Item/DeskObject.h"
#include "Item/HandheldItemObject.h"
#include "Item/BongoCatItem.h"
#include "Item/BongoCatObject.h"
#include "Item/DecorationItem.h"
#include "Item/DecorationObject.h"
#include "RenderWindowManager.h"
#include"RenderThread.h"
#include"MainUiForm.h"
#include"UserControl/UIScenePanel.h"
//////////
//******预设所有场景文件后缀为.json!!!!
//////////



#define DEFAULT_COVER_PATH "UISceneItem_DefaultCover.png"


static const wchar_t* itemTypeStr[UIModelItemType_Count] = 
{
L"Empty",
L"ClassicCharacter",
L"ClassicDesk",
L"ClassicHandheldItem",
L"BongoCat"
};







UIModelItem::UIModelItem(ui::Window* pWindow)
    :ui::ListBoxItemV(pWindow)
{
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UIModelItem.xml"));
}

void UIModelItem::InitSubControls(const std::string& name, const std::string& img, size_t dataIndex, UIModelItemType _itemType, bool selected, bool isFavorate, bool _isEmptyItem)
{
    if (nullptr == imgCover)
    {
        if (itemType != UIModelItemType_Empty)
        {
            SetGroup(itemTypeStr[itemType]);
        }
        itemType = _itemType;


        imgCover =dynamic_cast<ui::Control*>(FindSubControl(L"item_cover"));
        imgFavoriteIcon =dynamic_cast<ui::Control*>(FindSubControl(L"item_favorite"));
        labelItenName =dynamic_cast<ui::Label*>(FindSubControl(L"item_name"));


        //右键菜单
        AttachRClick(ui::UiBind(&UIModelItem::OnRightClick,this,std::placeholders::_1));

        //字体需要设置颜色
        //auto textColorFunc=[this](const ui::EventArgs&)->bool {
        //    if (IsSelected())
        //    {
        //        labelItenName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
        //        //this->SetStateColor(ui::kControlStateNormal,L"subjectColor");
        //        //this->SetSelectedStateColor(ui::);
        //    }
        //    else
        //    {
        //        labelItenName->SetStateTextColor(ui::kControlStateNormal, L"textNormalColor");
        //        //this->SetStateColor(ui::kControlStateNormal, L"");
        //    }
        //    return true;
        //    };
        //AttachSelect(textColorFunc);
        //AttachUnSelect(textColorFunc);

    }

    if (selected)
    {
        //labelItenName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
    }
    else
    {
        //labelItenName->SetStateTextColor(ui::kControlStateNormal, L"textNormalColor");
    }
    auto testPtr = dynamic_cast<ui::VBox*> (this);


    imgCover->SetUTF8BkImage(img);
    labelItenName->SetUTF8Text(name);
    //if (isSelected != selected)
    //{
    //    isSelected = selected;
    //    if(isSelected)this->SetStateColor(ui::kControlStateNormal, L"subjectColor");
    //    else this->SetStateColor(ui::kControlStateNormal, L"");
    //}

    imgFavoriteIcon->SetVisible(isFavorate);





    index = dataIndex;
    isEmptyItem = _isEmptyItem;




    if (isEmptyItem)
    {
        labelItenName->SetTextId(L"STRID_MODELITEM_ITEM_EMPTY");
        imgCover->SetBoxShadow(L"blurradius='0'");



        //imgCover->LoadImageInfo


    }





}



bool UIModelItem::OnRightClick(const ui::EventArgs& args)
{
	//空物体不需要右键菜单
    if (isEmptyItem)
        return true;



    //创建菜单



    //右键弹出菜单
    ui::Menu* menu = new ui::Menu(this->GetWindow(), this);
    menu->SetSkinFolder(L"CatTuber_default");
    DString xml(L"UIModelItemMenu.xml");


    ui::UiPoint curPoint(args.ptMouse.x, args.ptMouse.y + 4);
    this->GetWindow()->ClientToScreen(curPoint);
    menu->ShowMenu(xml, curPoint);



    ui::MenuItem* ItemMenu_Select = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"ModelItemMenu_Select"));
    ui::MenuItem* ItemMenu_Deselect = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"ModelItemMenu_Deselect"));
    ui::MenuItem* ItemMenu_Favorite = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"ModelItemMenu_Favorite"));
    ui::MenuItem* ItemMenu_Unfavorite = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"ModelItemMenu_Unfavorite"));

    ui::UiSize sizeMax(9999, 9999);
    int32_t maxW = 0;
    if (ItemMenu_Select)
    {
        int32_t curW = ItemMenu_Select->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
            ItemMenu_Select->SetVisible(!IsSelected());
    }


    if (ItemMenu_Deselect)
    {
        int32_t curW = ItemMenu_Deselect->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
            ItemMenu_Deselect->SetVisible(IsSelected());
    }
    if (ItemMenu_Favorite)
    {
        int32_t curW = ItemMenu_Favorite->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
        ItemMenu_Favorite->SetVisible(!isFavorite);
    }
    if (ItemMenu_Unfavorite)
    {
        int32_t curW = ItemMenu_Unfavorite->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
        ItemMenu_Unfavorite->SetVisible(isFavorite);
    }


    //部分类型不允许取消选择;
    switch (itemType)
    {
    case UIModelItemType_ClassicCharacter:
    case UIModelItemType_ClassicDesk:
    case UIModelItemType_ClassicHandheldItem:
        break;

    case UIModelItemType_BongoCat:
    case UIModelItemType_DecorationItem:
		ItemMenu_Deselect->SetVisible(false);
        break;
    default:
        assert(false);
        break;
    }



    ItemMenu_Select->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
    ItemMenu_Deselect->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
    ItemMenu_Favorite->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
    ItemMenu_Unfavorite->SetFixedWidth(ui::UiFixedInt(maxW), true, false);




    //添加按钮功能
    ItemMenu_Select->AttachClick(ui::UiBind(&UIModelItem::OnMenuSelectItem, this, std::placeholders::_1));
    ItemMenu_Deselect->AttachClick(ui::UiBind(&UIModelItem::OnMenuDeselectItem, this, std::placeholders::_1));
    ItemMenu_Favorite->AttachClick(ui::UiBind(&UIModelItem::OnFavoriteClick, this, std::placeholders::_1));
    ItemMenu_Unfavorite->AttachClick(ui::UiBind(&UIModelItem::OnUnfavoriteClick, this, std::placeholders::_1));



    return true;
}

bool UIModelItem::OnMenuSelectItem(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    this->Selected(true,true,0);

    return true;
}

bool UIModelItem::OnMenuDeselectItem(const ui::EventArgs& args)
{
    auto provider = GetProvider();

    provider->OnSetDeselect(index);
    return true;
}

bool UIModelItem::OnFavoriteClick(const ui::EventArgs& args)
{
    if (index == 0)return true;

    auto provider = GetProvider();
    provider->SetFavorite(index,true);
    return true;
}

bool UIModelItem::OnUnfavoriteClick(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->SetFavorite(index,false);
    return true;
}




UIModelItemProvider* UIModelItem::GetProvider()
{
    ui::VirtualVTileListBox* parent = dynamic_cast<ui::VirtualVTileListBox*>(GetOwner());
    if (parent)
    {
       return dynamic_cast<UIModelItemProvider*>(parent->GetDataProvider());
        
    }
    return nullptr;
}

void UIModelItem::UpdateUI()
{
    auto provider = GetProvider();
    provider->EmitDataChanged(index, index);
}





UIModelItemProvider::UIModelItemProvider(UIModelItemType itemType, UIModelItemSelect_Page* _parentPage)
	:itemType(itemType), _parentPage(_parentPage)
{
}

ui::Control* UIModelItemProvider::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
    //nimduilib的设计中，入参是用于获取窗口的，不要在这里进行添加进容器的操作
    
    UIModelItem* item = new UIModelItem(pVirtualListBox->GetWindow());
    return item;
}

bool UIModelItemProvider::FillElement(ui::Control* pControl, size_t nElementIndex)
{
    //示例中这里用了一个锁
    UIModelItem* pItem = dynamic_cast<UIModelItem*>(pControl);
	ASSERT(pItem != nullptr);
    if (pItem == NULL || nElementIndex >= itemList.size())
    {
        return false;
    }

    pItem->InitSubControls(itemList[itemView[nElementIndex]].name, itemList[itemView[nElementIndex]].imgPath, nElementIndex,
        itemType,
        itemList[itemView[nElementIndex]].selected, itemList[itemView[nElementIndex]].favorite, itemList[itemView[nElementIndex]].emptyItem);
    return true;
}

size_t UIModelItemProvider::GetElementCount() const
{
    return itemList.size();
}

void UIModelItemProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{

    if (nElementIndex>= itemList.size())return;
    itemList[itemView[nElementIndex]].selected = bSelected;
    if (bSelected)
    {
        for (size_t index = 0; index < itemList.size(); index++)
        {
            if (index != nElementIndex)
                itemList[itemView[index]].selected = false;
        
        }

		OnSetSelect(nElementIndex);
    }
    if (itemList[itemView[nElementIndex]].emptyItem)
        itemList[itemView[nElementIndex]].selected = false;
    EmitDataChanged(nElementIndex,nElementIndex);
    if (bSelected)
    {
        _parentPage->UpdateItemInfoBox();
    }
}

bool UIModelItemProvider::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex >= itemList.size())return false;
    return itemList[itemView[nElementIndex]].selected;
}

void UIModelItemProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    selectedIndexs.reserve(itemList.size());

    for (size_t i=0;i< itemView.size();i++)
    {
        if(itemList[itemView[i]].selected)
        selectedIndexs.push_back(i);
    }
}






UIModelItemProvider::ItemInfo& UIModelItemProvider::GetItemInfo(size_t nElementIndex)
{
	ASSERT(nElementIndex < itemList.size());
	return itemList[itemView[nElementIndex]];
}

void UIModelItemProvider::Resort()
{
	//根据创建时间和加入收藏时间进行排序，时间较新的在前面
    std::sort(itemView.begin(), itemView.end(), [this](uint16_t a, uint16_t b)->bool {
		ItemInfo& ItemA = itemList[a];
		ItemInfo& ItemB = itemList[b];
        
        //bool result = true;
        int64_t featureA[3] = { ((int64_t)ItemA.emptyItem<<8u)+ (int64_t)ItemA.favorite ,(ItemA.favorite?ItemA.addFavoriteTime:0),ItemA.itemOrderTime };
        int64_t featureB[3] = { ((int64_t)ItemB.emptyItem<<8u)+ (int64_t)ItemB.favorite ,(ItemB.favorite?ItemB.addFavoriteTime:0),ItemB.itemOrderTime };
        for (int i = 0; i < 3; i++)
        {
            if (featureA[i] != featureB[i])
                return featureA[i] > featureB[i];
        }
		return a < b;
        });

    return ;
}

void UIModelItemProvider::LoadItemList()
{
    //两个加载路径，一个是软件的固有路径，一个是创意工坊路径
    //Resources/Character






	//从文件夹加载场景列表
    //文件名示例：Scene_87B24C7F-5745-4DD8-B89D-BA95C8E8D6D2.json
    //不使用UUID，改用时间戳，这样会自动按时间排序
    //文件名示例：Scene_20250920133012333.json （年月日时分秒毫秒）
    //封面文件：Scene_20250920133012333.png/jpg/gif
    //创建时也不能使用重复的名字
    itemList.clear();
    itemView.clear();

	//bool enableEmptyItem = false;
    enableEmptyItem = false;

    std::string itemFoldPath;
    switch (itemType)
    {
    case UIModelItemType_ClassicCharacter:
		itemFoldPath = AppContext::GetClassicCharacterFolderPath();
        enableEmptyItem = true;
        break;
    case UIModelItemType_ClassicDesk:
		itemFoldPath = AppContext::GetClassicDeskFolderPath();
        enableEmptyItem = true;
        break;
    case UIModelItemType_ClassicHandheldItem:
		itemFoldPath = AppContext::GetClassicHandheldItemFolderPath();
        enableEmptyItem = true;
        break;
    case UIModelItemType_BongoCat:
		itemFoldPath = AppContext::GetBongoCatFolderPath();
        enableEmptyItem = false;
        break;
    case UIModelItemType_DecorationItem:
		itemFoldPath = AppContext::GetDecorationItemFolderPath();
        enableEmptyItem = false;
        break;
    default:
		assert(false&&"ERROR itemType");
        break;
    }
    
    if (enableEmptyItem)
    {
        auto& emptyEmpty = itemList.emplace_back();
        emptyEmpty.emptyItem = true;
        emptyEmpty.imgPath = "ModelItem_Empty.svg";
        itemView.push_back((uint16_t)itemView.size());
    }

    //std::string curSelectItemPath;
    //获取实际选择的物品路径
    {
        switch (itemType)
        {
        case UIModelItemType_ClassicCharacter:
        {
            UIScenePanel* scenePanel = ((MainUiForm*)_parentPage->GetWindow())->GetScenePanel();
           
            auto mainItem = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene().GetMainItem();
            ClassicItem* pClassicItem = dynamic_cast<ClassicItem*>(mainItem);
            if (!pClassicItem)break;
            if(!pClassicItem->GetCharacter())break;
            curSelectItemPath=pClassicItem->GetCharacter()->GetPackPath();
            break;
        }
        case UIModelItemType_ClassicDesk:
        {
            UIScenePanel* scenePanel = ((MainUiForm*)_parentPage->GetWindow())->GetScenePanel();
            auto mainItem = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene().GetMainItem();
            ClassicItem* pClassicItem = dynamic_cast<ClassicItem*>(mainItem);
            if (!pClassicItem)break;
            if (!pClassicItem->GetDesk())break;
            curSelectItemPath = pClassicItem->GetDesk()->GetPackPath();
            break;
        }
        case UIModelItemType_ClassicHandheldItem:
        {
            UIScenePanel* scenePanel = ((MainUiForm*)_parentPage->GetWindow())->GetScenePanel();
            auto mainItem = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene().GetMainItem();
            ClassicItem* pClassicItem = dynamic_cast<ClassicItem*>(mainItem);
            if (!pClassicItem)break;
            if (!pClassicItem->GetHandheldItem())break;
            curSelectItemPath = pClassicItem->GetHandheldItem()->GetPackPath();
            break;
        }
        case UIModelItemType_BongoCat:
        {
            UIScenePanel* scenePanel = ((MainUiForm*)_parentPage->GetWindow())->GetScenePanel();
            auto mainItem = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene().GetMainItem();
            BongoCatItem* pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);
            if (!pBongoCatItem||!pBongoCatItem->GetObj())break;
            curSelectItemPath = pBongoCatItem->GetObj()->GetPackPath();
            break;
        }
        default:
            break;
        }
    
    
    
    
    }




    SDL_EnumerateDirectoryCallback fileCallback = [](void* userdata, const char* dirname, const char* fname) -> SDL_EnumerationResult
    {
            char pathBuf[4096];
            int pathBuflen=SDL_snprintf(pathBuf, sizeof(pathBuf), "%s%s", dirname, fname);
            Pack pack;
            if (pack.Open(pathBuf))
            {

                ItemInfo info;

                std::vector<uint8_t> descJsonBytes = pack.LoadFile("desc.json");
                if (!descJsonBytes.empty())
                {
                    Json::Value descJson= util::BuildJsonFromMem((const char*)descJsonBytes.data(), descJsonBytes.size());

                    info.name=util::GetStringFromMultiLangJsonNode(descJson["Name"]);
                    info.uploader= util::GetStringFromMultiLangJsonNode(descJson["Uploader"]);
					info.description = util::GetStringFromMultiLangJsonNode(descJson["Description"]);
                }



                if (info.name.empty())info.name = fname;
                
                //预览图像路径,使用Preview/Cover
                const char* imgFile = nullptr;
                if(pack.IsFileExist("Preview.png"))imgFile= "Preview.png";
                else if(pack.IsFileExist("Preview.jpg"))imgFile= "Preview.jpg";
                else if(pack.IsFileExist("Preview.gif"))imgFile= "Preview.gif";
                else if(pack.IsFileExist("Cover.png"))imgFile= "Cover.png";
                else if(pack.IsFileExist("Cover.jpg"))imgFile= "Cover.jpg";
                else if(pack.IsFileExist("Cover.gif"))imgFile= "Cover.gif";
                if (imgFile)
                {
					info.imgPath = std::string("[") + pathBuf + "]" + "["+imgFile+"]";


                    //ui::ImageLoadParam imgLoad;
                    //ui::ImageLoadPath imgLoadPath;
                    //imgLoadPath.m_imageFullPath = ui::StringConvert::UTF8ToWString(info.imgPath);
                    //imgLoadPath.m_pathType = ui::ImageLoadPathType::kVirtualPath;
                    //imgLoad.SetImageLoadPath(imgLoadPath);
                    //bool isLoadFromCache = false;
                    //auto imgResult = ui::GlobalManager::Instance().Image().GetImage(imgLoad, isLoadFromCache);
                    //if(imgResult)
                    //static_cast<UIModelItemProvider*>(userdata)->imageCache.push_back(imgResult
                    //    );
                    //ui::Control::ClearImageCache
                    //ui::Control::SetBkImage
                    uint32_t nIconId= ui::GlobalManager::Instance().Icon().AddIcon(ui::StringConvert::UTF8ToWString(info.imgPath));
                    info.imgPath  = ui::StringConvert::WStringToUTF8(   ui::GlobalManager::Instance().Icon().GetIconString(nIconId));
                    

                    
                }
                else
                {
                    info.imgPath = "ModelItem_DefaultCover.png";
                }

                info.filePath = pathBuf;




                auto& itemFilePath = static_cast<UIModelItemProvider*>(userdata)->curSelectItemPath;
                

                if (!itemFilePath.empty()&& itemFilePath.size()== pathBuflen)
                {
                    //对比路径
                    bool success = true;
                    for (int i = 0; i < pathBuflen; i++)
                    {
                        if (itemFilePath[i] == pathBuf[i])
                            continue;
                        if ((itemFilePath[i] == '/' || itemFilePath[i] == '\\')
                            && (pathBuf[i] == '/' || pathBuf[i] == '\\'))
                            continue;
                        success = false;
                        break;
                    }
                    info.selected = success;
                }



                auto& itemList = static_cast<UIModelItemProvider*>(userdata)->itemList;
                itemList.push_back(std::move(info));
                auto& itemView = static_cast<UIModelItemProvider*>(userdata)->itemView;
                itemView.push_back((uint16_t)itemView.size());
            }




            return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
		};



    SDL_EnumerateDirectory(itemFoldPath.c_str(), fileCallback,this);

    //按需要重新排序
    Resort();


    return;


}




bool UIModelItemProvider::OnSetSelect(size_t index)
{
    //this->SetSelect(index);

    //if (itemList[itemView[index]].emptyItem)return true;

    UIScenePanel* scenePanel = ((MainUiForm*)_parentPage->GetWindow())->GetScenePanel();
    
    auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();

    //ASSERT(false);
    //UI中选择了指定的物品
    switch (itemType)
    {
    case UIModelItemType_Empty:
        return true;
        break;
    case UIModelItemType_ClassicCharacter:
    case UIModelItemType_ClassicDesk:
    case UIModelItemType_ClassicHandheldItem:
    {
        uint64_t userdata2 = ((uint64_t)itemView[index]) | ((uint64_t)itemType << 32u);

        RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
            UIModelItemProvider* pthis = (UIModelItemProvider*)userdata;
            uint32_t itemIndex = ((uint32_t*)(&userdata2))[0];
            UIModelItemType objType = (UIModelItemType)((uint32_t*)(&userdata2))[1];

            UIScenePanel* scenePanel= ((MainUiForm*)(pthis->_parentPage)->GetWindow())->GetScenePanel();
            auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();

            auto mainItem = targetScene.GetMainItem();
            auto pClassicItem = dynamic_cast<ClassicItem*>(mainItem);

            int originalIndex = -1;
            //不是BongoCatItem的时候要移除
            if (!pClassicItem)
            {
                return;
            }

            switch (objType)
            {
            case UIModelItemType_ClassicCharacter:
                pClassicItem->ResetCharacterObj(CharacterObject::CreateFromPath(pthis->itemList[itemIndex].filePath.c_str()));
                break;
            case UIModelItemType_ClassicDesk:
                pClassicItem->ResetDeskObj(DeskObject::CreateFromPath(pthis->itemList[itemIndex].filePath.c_str()));
                break;
            case UIModelItemType_ClassicHandheldItem:
                pClassicItem->ResetHandheldItemObj(HandheldItemObject::CreateFromPath(pthis->itemList[itemIndex].filePath.c_str()));
                break;
            }


            ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [pthis]() {

                ((MainUiForm*)pthis->_parentPage->GetWindow())->GetScenePanel()->InitContents();
                });

            }, this, userdata2);
    }
        break;
    case UIModelItemType_BongoCat:

        //为了保持流畅性，先直接当作成功加载更新UI，如果加载失败则刷新一次ui
    {



        Pack bongoCatPack;
        if (bongoCatPack.Open(itemList[itemView[index]].filePath.c_str()))
        {
            scenePanel->scenePanel_classic->SetVisible(false);
            scenePanel->scenePanel_bongoCat->SetVisible(true);
            {
                std::wstring imgFile;
                if (bongoCatPack.IsFileExist("Slot.png"))imgFile = L"Slot.png";
                else if (bongoCatPack.IsFileExist("Slot.jpg"))imgFile = L"Slot.jpg";
                else if (bongoCatPack.IsFileExist("Slot.gif"))imgFile = L"Slot.gif";
                if (!imgFile.empty())
                {
                    imgFile = Pack::PackPath_BuildPackFullPath(ui::StringConvert::UTF8ToWString(bongoCatPack.GetPath()), imgFile);
                }
                if (imgFile.empty())
                    imgFile = L"DefaultBongoCatSlot.png";
                scenePanel->bongoCatPanel_slot->SetBkImage(imgFile);
            }

            {
                std::wstring imgFile;
                int originalIndex = targetScene.GetItemIndex_TopToBottom(targetScene.GetMainItem());

                if (bongoCatPack.IsFileExist("List.png"))imgFile = L"List.png";
                else if (bongoCatPack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
                else if (bongoCatPack.IsFileExist("List.gif"))imgFile = L"List.gif";
                if (!imgFile.empty())
                {
                    scenePanel->providers[scenePanel->currentWindowIndex]->SetListItemImg(originalIndex, BongoCatItem::_GetType(), bongoCatPack.GetPath());
                    
                }
            }

        }
        
    }



    



        RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
			UIModelItemProvider* pthis = (UIModelItemProvider*)userdata;
			uint64_t itemIndex = userdata2;

            UIScenePanel* scenePanel = ((MainUiForm*)(pthis->_parentPage)->GetWindow())->GetScenePanel();
        auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();

        auto mainItem = targetScene.GetMainItem();
        auto pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);

		int originalIndex = -1;
        //不是BongoCatItem的时候要移除
        if (mainItem && !pBongoCatItem)
        {
            originalIndex=targetScene.RemoveItem(mainItem);
            mainItem = nullptr;
            pBongoCatItem = nullptr;
        }

        if (!mainItem)
        {
            targetScene.CreateNewItem("BongoCatItem", originalIndex);
            mainItem = targetScene.GetMainItem();
        }
        pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);
        if (pBongoCatItem)
        {
            BongoCatObject* bongocatObj=BongoCatObject::CreateFromPath(pthis->itemList[itemIndex].filePath.c_str());
            if (bongocatObj)
            {
                pBongoCatItem->ResetObj(bongocatObj);
            }
        }

        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [pthis]() {
            
            ( (MainUiForm*)pthis->_parentPage->GetWindow())->GetScenePanel()->InitContents();
            });


            },this, itemView[index]);
        break;

    case UIModelItemType_DecorationItem:
    {

        Pack decorationPack;
        if (decorationPack.Open(itemList[itemView[index]].filePath.c_str()))
        {

            {
                std::wstring imgFile;
                int originalIndex = targetScene.GetItemIndex_TopToBottom(_parentPage->pageStates.decorationPageStates.targetItem);

                //if (decorationPack.IsFileExist("List.png"))imgFile = L"List.png";
                //else if (decorationPack.IsFileExist("List.jpg"))imgFile = L"List.jpg";
                //else if (decorationPack.IsFileExist("List.gif"))imgFile = L"List.gif";
                //if (!imgFile.empty())
                {
                    scenePanel->providers[scenePanel->currentWindowIndex]->SetListItemImg(originalIndex, DecorationItem::_GetType(), decorationPack.GetPath());

                }
            }




            RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
                UIModelItemProvider* pthis = (UIModelItemProvider*)userdata;
                uint64_t itemIndex = userdata2;

                UIScenePanel* scenePanel = ((MainUiForm*)(pthis->_parentPage)->GetWindow())->GetScenePanel();
                auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();



                if (!pthis->_parentPage->pageStates.decorationPageStates.targetItem)
                {
                    if (targetScene.CreateNewItem(DecorationItem::_GetType(), -1))
                    {
                        pthis->_parentPage->pageStates.decorationPageStates.targetItem=dynamic_cast<DecorationItem*>( targetScene.GetItemList().back());
                    }
                }
                auto& pTargetItem = pthis->_parentPage->pageStates.decorationPageStates.targetItem;
                if (pTargetItem)
                {
                    DecorationObject* decorationObj = DecorationObject::CreateFromPath(pthis->itemList[itemIndex].filePath.c_str());
                    if (decorationObj)
                    {
                        pTargetItem->ResetObj(decorationObj);
                    }
                }

                ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, [pthis]() {

                    ((MainUiForm*)pthis->_parentPage->GetWindow())->GetScenePanel()->InitContents();
                    });


                }, this, itemView[index]);

        }
        break;
    }
    default:
        assert(false);
        break;
    }
    //成功执行了操作，回发UI刷新的事件？？？








    return true;
}

bool UIModelItemProvider::SetFavorite(size_t index, bool bFavorite)
{
    //itemList[index].favorite = bFavorite;
    itemList[itemView[index]].favorite = bFavorite;


    if (bFavorite)
    {
        auto originIndex = itemView[index];
        itemView.erase(itemView.begin() + index);
        itemView.insert(itemView.begin(), originIndex);
        SDL_Time currentTime;
        SDL_GetCurrentTime(&currentTime);
        itemList[originIndex].addFavoriteTime = currentTime;
        EmitDataChanged(0, index);
    }
    else
    {
        //auto originIndex = itemView[index];
        //itemView.erase(itemView.begin() + index);
        //itemView.insert(itemView.begin()+ originIndex, originIndex);

        auto originIndex = itemView[index];
        Resort();
        for (int i = 0; i < itemView.size(); i++)
        {
            if (itemView[i] == originIndex)
            {
                EmitDataChanged(index, i);
            }
        }


    }


    EmitCountChanged();
    return true;
}



void UIModelItemProvider::OnSetDeselect(size_t index)
{
	//当前物品由选中变为未选中
    //对应为UI场景中移除目标对象

    switch (itemType)
    {
    case UIModelItemType_Empty:
        return;
        break;
    case UIModelItemType_ClassicCharacter:
    case UIModelItemType_ClassicDesk:
    case UIModelItemType_ClassicHandheldItem:
    {
        uint64_t userdata2 = ((uint64_t)itemView[index]) | ((uint64_t)itemType << 32u);

        RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
            UIModelItemProvider* pthis = (UIModelItemProvider*)userdata;
            uint32_t itemIndex = ((uint32_t*)(&userdata2))[0];
            UIModelItemType objType = (UIModelItemType)((uint32_t*)(&userdata2))[1];

            UIScenePanel* scenePanel = ((MainUiForm*)(pthis->_parentPage)->GetWindow())->GetScenePanel();

            auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();

            auto mainItem = targetScene.GetMainItem();
            auto pClassicItem = dynamic_cast<ClassicItem*>(mainItem);

            int originalIndex = -1;
            //不是BongoCatItem的时候要移除
            if (!pClassicItem)
            {
                return;
            }

            switch (objType)
            {
            case UIModelItemType_ClassicCharacter:
                pClassicItem->ResetCharacterObj(nullptr);
                break;
            case UIModelItemType_ClassicDesk:
                pClassicItem->ResetDeskObj(nullptr);
                break;
            case UIModelItemType_ClassicHandheldItem:
                pClassicItem->ResetHandheldItemObj(nullptr);
                break;
            }


            }, this, userdata2);
    }
    break;
    case UIModelItemType_BongoCat:

        RenderThread::GetIns().PostTask([](void* userdata, uint64_t userdata2) {
            UIModelItemProvider* pthis = (UIModelItemProvider*)userdata;
            uint64_t itemIndex = userdata2;

            UIScenePanel* scenePanel = ((MainUiForm*)(pthis->_parentPage)->GetWindow())->GetScenePanel();
            auto& targetScene = RenderWindowManager::GetIns().GetWindowController(scenePanel->currentWindowIndex)->GetScene();

            auto mainItem = targetScene.GetMainItem();
            auto pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);

            int originalIndex = -1;
            //不是BongoCatItem的时候要移除
            if (mainItem && !pBongoCatItem)
            {
                originalIndex = targetScene.RemoveItem(mainItem);
                mainItem = nullptr;
                pBongoCatItem = nullptr;
            }

            if (!mainItem)
            {
                return;
            }
            pBongoCatItem = dynamic_cast<BongoCatItem*>(mainItem);
            if (pBongoCatItem)
            {
                pBongoCatItem->ResetObj(nullptr);
            }
            }, this, itemView[index]);
        break;
    default:
        //错误类型
        assert(false);
        break;




    }


}




UIModelItemSelect_Page::UIModelItemSelect_Page(ui::Window* pWindow, UIModelItemType itemType)
    :UIPageBase(pWindow), itemType(itemType)
{
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/ItemSelectPage.xml"));
    ui::Label* title = (ui::Label*)FindSubControl(L"itemSelectPageTitle");
    ui::HBox* titleContainer = (ui::HBox*)FindSubControl(L"itemSelectPageTitleHBox");
   
    switch (itemType)
    {
    case UIModelItemType_ClassicCharacter:
        this->SetName(L"CLASSIC_CHARACTER_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_CLASSIC_CHARACTER");
        break;
    case UIModelItemType_ClassicDesk:
        this->SetName(L"CLASSIC_DESK_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_CLASSIC_DESK");
        break;
    case UIModelItemType_ClassicHandheldItem:
        this->SetName(L"CLASSIC_HANDHELDITEM_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_CLASSIC_HANDHELDITEM");
        break;
    case UIModelItemType_BongoCat:
    {
        this->SetName(L"BONGOCAT_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_BONGOCAT");



        //Bongo Cat选择页内部添加选择按钮
		ui::Button* importBongoCatBtn = new ui::Button(pWindow);
        importBongoCatBtn->SetFixedHeight(ui::UiFixedInt(32),true,true);
		importBongoCatBtn->SetVerAlignType(ui::VerAlignType::kAlignCenter);
        importBongoCatBtn->SetBkImage(L"file='itemImport.svg' dest='8,8,24,24'");
        importBongoCatBtn->SetTextId(L"STRID_MODELITEM_BONGOCAT_IMPORT");
        importBongoCatBtn->SetTextPadding({32,0,8,0},true);
        //importBongoCatBtn->SetTextStyle
        importBongoCatBtn->SetStateColor(ui::ControlStateType::kControlStateHot,L"itemHoverColor");
        importBongoCatBtn->AttachClick([this](const ui::EventArgs& msg)->bool {
            if (BongoCatImport_Dlg::ShowModalDlg(msg.GetSender()->GetWindow()))
            {
                //如果导入成功则刷新页面
                provider->LoadItemList();
                provider->EmitCountChanged();

            }
            return true;
            });
        

        titleContainer->AddItemAt(importBongoCatBtn,1);
    }
        break;
    case UIModelItemType_DecorationItem:
    {
        this->SetName(L"DECORATIONITEM_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_DECORATIONITEM");
    }
        break;
    default:
        break;
    }



    //挂接详细信息按钮
    cb_showDetailedInfo = (ui::CheckBoxBox*)FindSubControl(L"cb_showDetailedInfo");
    cb_showDetailedInfo->AttachSelect(ui::UiBind(&UIModelItemSelect_Page::OnCheckBoxSelected, this, std::placeholders::_1));
    cb_showDetailedInfo->AttachUnSelect(ui::UiBind(&UIModelItemSelect_Page::OnCheckBoxSelected, this, std::placeholders::_1));

    itemInfoBox = (ui::VScrollBox*)FindSubControl(L"itemInfoBox");
    itemInfo_cover = (ui::Control*)FindSubControl(L"itemInfo_cover");
    itemInfo_name = (ui::Label*)FindSubControl(L"itemInfo_name");
    itemInfo_desc = (ui::RichText*)FindSubControl(L"itemInfo_desc");
}

UIModelItemSelect_Page::~UIModelItemSelect_Page()
{
    if (provider)delete provider;
    provider = nullptr;
}

MainUiForm::PageEnum UIModelItemSelect_Page::GetPageType()
{
    switch (itemType)
    {
    case UIModelItemType_ClassicCharacter:
		return MainUiForm::PageEnum::PAGE_CLASSIC_CHARACTER_SELECT;
        break;
    case UIModelItemType_ClassicDesk:
        return MainUiForm::PageEnum::PAGE_CLASSIC_DESK_SELECT;
        break;
    case UIModelItemType_ClassicHandheldItem:
        return MainUiForm::PageEnum::PAGE_CLASSIC_HANDHELDITEM_SELECT;
        break;
    case UIModelItemType_BongoCat:
        return MainUiForm::PageEnum::PAGE_BONGOCAT_SELECT;
        break;
    default:
        break;
    }

    return MainUiForm::PageEnum();
}


void UIModelItemSelect_Page::InitContents(uintptr_t userdata1,uintptr_t userdata2)
{

    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    
    

    pageStates.decorationPageStates = {};



    ui::VirtualVTileListBox* container= (ui::VirtualVTileListBox*)FindSubControl(L"itemContainer");
    container->GetLayout()->SetChildHAlignType(ui::HorAlignType::kAlignLeft);


    if (provider)delete provider;
    provider = new UIModelItemProvider(itemType,this);
    container->SetDataProvider(provider);
    provider->LoadItemList();



    switch (itemType)
    {
    case UIModelItemType_Empty:
        break;
    case UIModelItemType_ClassicCharacter:
        cb_showDetailedInfo->Selected(AppSettings::GetIns().GetUIItemShowDetailedInfo_ClassicCharacter(), true);
        break;
    case UIModelItemType_ClassicDesk:
        cb_showDetailedInfo->Selected(AppSettings::GetIns().GetUIItemShowDetailedInfo_ClassicDesk(), true);
        break;
    case UIModelItemType_ClassicHandheldItem:
        cb_showDetailedInfo->Selected(AppSettings::GetIns().GetUIItemShowDetailedInfo_ClassicHandheldItem(), true);
        break;
    case UIModelItemType_BongoCat:
        cb_showDetailedInfo->Selected(AppSettings::GetIns().GetUIItemShowDetailedInfo_BongoCat(), true);
        break;
    case UIModelItemType_DecorationItem:
        cb_showDetailedInfo->Selected(AppSettings::GetIns().GetUIItemShowDetailedInfo_DecorationItem(), true);
        break;
    case UIModelItemType_Count:
        break;
    default:
        break;
    }

    

    
}



bool UIModelItemSelect_Page::OnCheckBoxSelected(const ui::EventArgs& args)
{
    if (args.GetSender() == cb_showDetailedInfo)
    {
        switch (itemType)
        {
        case UIModelItemType_Empty:
            break;
        case UIModelItemType_ClassicCharacter:
            AppSettings::GetIns().SetUIItemShowDetailedInfo_ClassicCharacter(cb_showDetailedInfo->IsSelected());
            break;
        case UIModelItemType_ClassicDesk:
            AppSettings::GetIns().SetUIItemShowDetailedInfo_ClassicDesk(cb_showDetailedInfo->IsSelected());

            break;
        case UIModelItemType_ClassicHandheldItem:
            AppSettings::GetIns().SetUIItemShowDetailedInfo_ClassicHandheldItem(cb_showDetailedInfo->IsSelected());
            break;
        case UIModelItemType_BongoCat:
            AppSettings::GetIns().SetUIItemShowDetailedInfo_BongoCat(cb_showDetailedInfo->IsSelected());
            break;
        case UIModelItemType_DecorationItem:
            AppSettings::GetIns().SetUIItemShowDetailedInfo_DecorationItem(cb_showDetailedInfo->IsSelected());
            break;
        case UIModelItemType_Count:
            break;
        default:
            break;
        }

        itemInfoBox->SetVisible(cb_showDetailedInfo->IsSelected());
        if (cb_showDetailedInfo->IsSelected())
            UpdateItemInfoBox();

    }


    return true;
}

void UIModelItemSelect_Page::OnEnterThisPage(PageEnterFlag enterFlag)
{
    {
       //返回到此页面时设置物品显隐
        switch (itemType)
        {
        case UIModelItemType_Empty:
            break;
        case UIModelItemType_ClassicCharacter:
            break;
        case UIModelItemType_ClassicDesk:
            break;
        case UIModelItemType_ClassicHandheldItem:
            break;
        case UIModelItemType_BongoCat:
            break;
        case UIModelItemType_DecorationItem:
        {
         
            pageStates.decorationPageStates = {};
            for (int i = 0; i < provider->GetElementCount();i++)
            {
				provider->SetElementSelected(i, false);
            }
            
            //Invalidate();
            
            break;
        }
        default:
            assert(false);
            break;
        }






    }





}

void UIModelItemSelect_Page::UpdateItemInfoBox()
{
    if (cb_showDetailedInfo->IsSelected() == false)return;
    
    std::vector<size_t> selectedIndexs;
    provider->GetSelectedElements(selectedIndexs);
    if (selectedIndexs.empty())
    {
        itemInfo_cover->SetBkImage(L"ModelItem_DefaultCover.png");
        itemInfo_name->SetText(L"");
        itemInfo_name->SetTextId(L"STRID_MODELITEM_DETAILEDINFOBOX_NOITEMSELECTED");
        itemInfo_desc->SetText(L"");
    }
    else
    {
        itemInfo_cover->SetUTF8BkImage(provider->GetItemInfo(selectedIndexs[0]).imgPath);
        itemInfo_name->SetTextId(L"");
        itemInfo_name->SetUTF8Text(provider->GetItemInfo(selectedIndexs[0]).name);
        //itemInfo_desc->SetUTF8Text(provider->GetItemInfo(selectedIndexs[0]).description);
        itemInfo_desc->SetText(ui::StringConvert::UTF8ToWString((provider->GetItemInfo(selectedIndexs[0]).description)));
    
        //itemInfo_desc->SetText(L" <b>ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ....ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS ABSADAS .... </b>");
    }
}









