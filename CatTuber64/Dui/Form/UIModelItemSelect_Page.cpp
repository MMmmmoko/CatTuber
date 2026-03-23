#include"Dui.h"
#include"UIModelItemSelect_Page.h"
#include"Dialog/UISceneItemRename_Dlg.h"
#include"Dialog/UISceneCreateNew_Dlg.h"
#include"Dialog/UISceneCoverCropper_Dlg.h"

#include"AppContext.h"
#include"Util.h"
#include"Pack/Pack.h"
#include"Item/SceneManager.h"

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

void UIModelItem::InitSubControls(const std::string& name, const std::string& img, size_t dataIndex, UIModelItemType itemType, bool selected, bool isFavorate, bool _isEmptyItem)
{
    if (nullptr == imgCover)
    {
        if (itemType != UIModelItemType_Empty)
        {
            SetGroup(itemTypeStr[itemType]);
        }



        imgCover =dynamic_cast<ui::Control*>(FindSubControl(L"item_cover"));
        imgFavoriteIcon =dynamic_cast<ui::Control*>(FindSubControl(L"item_favorite"));
        labelItenName =dynamic_cast<ui::Label*>(FindSubControl(L"item_name"));


        //右键菜单
        AttachRClick(ui::UiBind(&UIModelItem::OnRightClick,this,std::placeholders::_1));

        //字体需要设置颜色
        auto textColorFunc=[this](const ui::EventArgs&)->bool {
            if (IsSelected())
            {
                labelItenName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
                //this->SetStateColor(ui::kControlStateNormal,L"subjectColor");
                //this->SetSelectedStateColor(ui::);
            }
            else
            {
                labelItenName->SetStateTextColor(ui::kControlStateNormal, L"textNormalColor");
                //this->SetStateColor(ui::kControlStateNormal, L"");
            }
            return true;
            };
        AttachSelect(textColorFunc);
        AttachUnSelect(textColorFunc);
        if (selected)
        {
            labelItenName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
        }

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


    ItemMenu_Select->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    ItemMenu_Deselect->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    ItemMenu_Favorite->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    ItemMenu_Unfavorite->SetFixedWidth(ui::UiFixedInt(maxW), true, false);




    //添加按钮功能
    ItemMenu_Select->AttachClick(ui::UiBind(&UIModelItem::OnSelectItem, this, std::placeholders::_1));
    ItemMenu_Deselect->AttachClick(ui::UiBind(&UIModelItem::OnDeselectItem, this, std::placeholders::_1));
    ItemMenu_Favorite->AttachClick(ui::UiBind(&UIModelItem::OnFavoriteClick, this, std::placeholders::_1));
    ItemMenu_Unfavorite->AttachClick(ui::UiBind(&UIModelItem::OnUnfavoriteClick, this, std::placeholders::_1));



    return true;
}

bool UIModelItem::OnSelectItem(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->SetSelect(index);

    return true;
}

bool UIModelItem::OnDeselectItem(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->SetSelect(0);
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





UIModelItemProvider::UIModelItemProvider(UIModelItemType itemType)
    :itemType(itemType)
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

    pItem->InitSubControls(itemList[nElementIndex].name, itemList[nElementIndex].imgPath, nElementIndex, 
        ((nElementIndex == 0)?UIModelItemType_Empty:itemType),
        itemList[nElementIndex].selected, itemList[nElementIndex].favorite, nElementIndex==0);
    return true;
}

size_t UIModelItemProvider::GetElementCount() const
{
    return itemList.size();
}

void UIModelItemProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{

    if (nElementIndex>= itemList.size())return;
    itemList[nElementIndex].selected = bSelected;
    if(bSelected)
    for (size_t index = 0; index < itemList.size(); index++)
    {
        if(index!= nElementIndex)
            itemList[index].selected = false;
    }
    //EmitDataChanged();
}

bool UIModelItemProvider::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex >= itemList.size())return false;
    return itemList[nElementIndex].selected;
}

void UIModelItemProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    selectedIndexs.reserve(itemList.size());
    for (size_t i=0;i< itemList.size();i++)
    {
        if(itemList[i].selected)
        selectedIndexs.push_back(i);
    }
}






UIModelItemProvider::ItemInfo& UIModelItemProvider::GetItemInfo(size_t nElementIndex)
{
	ASSERT(nElementIndex < itemList.size());
	return itemList[nElementIndex];
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

    //无论如何塞个空物体
    auto& emptyEmpty=itemList.emplace_back();
    emptyEmpty.emptyItem = true;
    emptyEmpty.imgPath = "ModelItem_Empty.svg";



    std::string itemFoldPath;
    switch (itemType)
    {
    case UIModelItemType_ClassicCharacter:
		itemFoldPath = AppContext::GetClassicCharacterFolderPath();
        break;
    case UIModelItemType_ClassicDesk:
		itemFoldPath = AppContext::GetClassicDeskFolderPath();
        break;
    case UIModelItemType_ClassicHandheldItem:
		itemFoldPath = AppContext::GetClassicHandheldItemFolderPath();
        break;
    case UIModelItemType_BongoCat:
		itemFoldPath = AppContext::GetBongoCatFolderPath();
        break;
    default:
		ASSERT(false&&"ERROR itemType");
        break;
    }
    



    SDL_EnumerateDirectoryCallback fileCallback = [](void* userdata, const char* dirname, const char* fname) -> SDL_EnumerationResult
    {
            char pathBuf[1024];
            SDL_snprintf(pathBuf, 1024,"%s%s", dirname, fname);
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


				auto& itemList = static_cast<UIModelItemProvider*>(userdata)->itemList;
				itemList.push_back(std::move(info));
            }




            return SDL_EnumerationResult::SDL_ENUM_CONTINUE;
		};



    SDL_EnumerateDirectory(itemFoldPath.c_str(), fileCallback,this);




    return;


}











UIModelItemSelect_Page::UIModelItemSelect_Page(ui::Window* pWindow, UIModelItemType itemType)
    : ui::VBox(pWindow), itemType(itemType)
{
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/ItemSelectPage.xml"));
    ui::Label* title = (ui::Label*)FindSubControl(L"itemSelectPageTitle");

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
        this->SetName(L"BONGOCAT_SELECT_PAGE");
        title->SetTextId(L"STRID_MODELITEM_TITLE_BONGOCAT");
        break;
    default:
        break;
    }

}

void UIModelItemSelect_Page::InitContents()
{

    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    
    




    ui::VirtualVTileListBox* container= (ui::VirtualVTileListBox*)FindSubControl(L"itemContainer");
    container->GetLayout()->SetChildHAlignType(ui::HorAlignType::kAlignLeft);

    provider = new UIModelItemProvider(itemType);
    container->SetDataProvider(provider);
    provider->LoadItemList();


    


}









