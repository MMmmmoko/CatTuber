#include"Dui.h"
#include"SceneSelectPage.h"
#include"Dialog/UISceneItemRename_Dlg.h"
#include"Dialog/UISceneCreateNew_Dlg.h"
#include"Dialog/UISceneCoverCropper_Dlg.h"

#include"AppContext.h"
#include"Util.h"

#include"Item/SceneManager.h"
#include "UIModelItemSelect_Page.h"

//////////
//******预设所有场景文件后缀为.json!!!!
//////////



#define DEFAULT_COVER_PATH "UISceneItem_DefaultCover.png"



UISceneItem::UISceneItem(ui::Window* pWindow)
    :ui::ListBoxItemV(pWindow)
{
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/UISceneItem.xml"));
}

void UISceneItem::InitSubControls(const std::string& name, const std::string& img, size_t dataIndex,bool selected)
{
    if (nullptr == imgCover)
    {
        imgCover =dynamic_cast<ui::Control*>(FindSubControl(L"sceneItem_cover"));
        labelSceneName =dynamic_cast<ui::Label*>(FindSubControl(L"sceneItem_name"));


        //右键菜单
        AttachRClick(ui::UiBind(&UISceneItem::OnRightClick,this,std::placeholders::_1));

        //字体需要设置颜色
        auto textColorFunc=[this](const ui::EventArgs&)->bool {
            if (IsSelected())
            {
                labelSceneName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
                //this->SetStateColor(ui::kControlStateNormal,L"subjectColor");
                //this->SetSelectedStateColor(ui::);
            }
            else
            {
                labelSceneName->SetStateTextColor(ui::kControlStateNormal, L"textNormalColor");
                //this->SetStateColor(ui::kControlStateNormal, L"");
            }
            return true;
            };
        AttachSelect(textColorFunc);
        AttachUnSelect(textColorFunc);
        if (selected)
        {
            labelSceneName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
        }

    }
    auto testPtr = dynamic_cast<ui::VBox*> (this);
    imgCover->SetUTF8BkImage(img); 
    //imgCover->SetUTF8BkImage("file='"+img+"' adaptive_dest_rect='true' halign='center' valign='center'");
    labelSceneName->SetUTF8Text(name);
    //if (isSelected != selected)
    //{
    //    isSelected = selected;
    //    if(isSelected)this->SetStateColor(ui::kControlStateNormal, L"subjectColor");
    //    else this->SetStateColor(ui::kControlStateNormal, L"");
    //}


    index = dataIndex;
}

std::string UISceneItem::GetSceneName()
{
    auto provider=GetProvider();
    return provider->GetSceneInfo(index).name;
}

std::string UISceneItem::GetSceneFileName()
{
    auto provider = GetProvider();
    return provider->GetSceneInfo(index).fileName;
}

bool UISceneItem::OnRightClick(const ui::EventArgs& args)
{
    //创建菜单



    //右键弹出菜单
    ui::Menu* menu = new ui::Menu(this->GetWindow(), this);
    menu->SetSkinFolder(L"CatTuber_default");
    DString xml(L"UISceneItemMenu.xml");


    ui::UiPoint curPoint(args.ptMouse.x, args.ptMouse.y + 4);
    this->GetWindow()->ClientToScreen(curPoint);
    menu->ShowMenu(xml, curPoint);



    ui::MenuItem* SceneItemMenu_Load = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Load"));
    ui::MenuItem* SceneItemMenu_Duplicate = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Duplicate"));
    ui::MenuItem* SceneItemMenu_Rename = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Rename"));
    ui::MenuItem* SceneItemMenu_UploadCover = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_UploadCover"));
    ui::MenuItem* SceneItemMenu_CaptureCover = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_CaptureCover"));
    ui::MenuItem* SceneItemMenu_Remove = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Remove"));

    ui::UiSize sizeMax(9999, 9999);
    int32_t maxW = 0;
    if (SceneItemMenu_Load)
    {
        int32_t curW = SceneItemMenu_Load->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
        if (IsSelected())
            SceneItemMenu_Load->SetEnabled(false);
    }
    if (SceneItemMenu_Duplicate)
    {
        int32_t curW = SceneItemMenu_Duplicate->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
    }
    if (SceneItemMenu_Rename)
    {
        int32_t curW = SceneItemMenu_Rename->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
    }
    if (SceneItemMenu_UploadCover)
    {
        int32_t curW = SceneItemMenu_UploadCover->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
    }
    if (SceneItemMenu_CaptureCover)
    {
        if (IsSelected())
        {
            //先判断是否有显示吧.
            int32_t curW = SceneItemMenu_CaptureCover->EstimateSize(sizeMax).cx.GetInt32();
            if (curW > maxW)maxW = curW;
        }
        else
        {
            SceneItemMenu_CaptureCover->SetVisible(false);
        }
    }
    if (SceneItemMenu_Remove)
    {
        int32_t curW = SceneItemMenu_Remove->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;

        //只有一个场景时不可remove
        ui::VirtualVTileListBox* parent = dynamic_cast<ui::VirtualVTileListBox*>(GetOwner());
        if (parent)
        {
            SceneItemProvider* provider = dynamic_cast<SceneItemProvider*>(parent->GetDataProvider());
            if (provider->GetElementCount() <= 1)
            {
                SceneItemMenu_Remove->SetEnabled(false);
            }
        }

    }
    SceneItemMenu_Load->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_Duplicate->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_Rename->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_UploadCover->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_CaptureCover->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_Remove->SetFixedWidth(ui::UiFixedInt(maxW), true, false);




    //添加按钮功能
    SceneItemMenu_Load->AttachClick(ui::UiBind(&UISceneItem::OnLoadClick, this, std::placeholders::_1));
    SceneItemMenu_Duplicate->AttachClick(ui::UiBind(&UISceneItem::OnDuplicateClick, this, std::placeholders::_1));
    SceneItemMenu_Rename->AttachClick(ui::UiBind(&UISceneItem::OnRenameClick, this, std::placeholders::_1));
    SceneItemMenu_UploadCover->AttachClick(ui::UiBind(&UISceneItem::OnUploadCoverClick, this, std::placeholders::_1));
    SceneItemMenu_CaptureCover->AttachClick(ui::UiBind(&UISceneItem::OnCaptureCoverClick, this, std::placeholders::_1));
    SceneItemMenu_Remove->AttachClick(ui::UiBind(&UISceneItem::OnRemoveClick, this, std::placeholders::_1));



    return true;
}

bool UISceneItem::OnLoadClick(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->LoadScene(index);

    return true;
}

bool UISceneItem::OnDuplicateClick(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->DuplicateItem(index);
    return true;
}

bool UISceneItem::OnRenameClick(const ui::EventArgs& args)
{
    //打开一个对话框，输入名称并判定是否成功
    //true表示数据有更新
    if (UISceneItemRename_Dlg::ShowModalDlg(this))
    {
        //auto provider = GetProvider();
        //provider->EmitDataChanged(index,index);
        //可能因为bug，等它刷新有时会等待一段时间才显示
        //确认原因是duilib的模式对话框是假的.. 会直接返回不会在这里停着
        
    }
    return true;
}

bool UISceneItem::OnUploadCoverClick(const ui::EventArgs& args)
{
    //ASSERT(false);


    //打开文件选择窗口
    //使用nimduilib封装
    ui::FileDialog dialog;
    ui::FilePath resultPath;
    std::vector<ui::FileDialog::FileType>fileTypes;
    auto& fileType=fileTypes.emplace_back();
    fileType.szName = L"Image File";
    fileType.szExt = L"*.png;*.jpg;*.gif";


    if (dialog.BrowseForFile(GetWindow(), resultPath, true, fileTypes))
    {
        //选择了一个图片文件
        //如果图片文件为gif则不进行裁剪直接拷贝?
        if (resultPath.GetFileExtension() == L".gif")
        {
            //将已有的图像文件移除
            auto& sceneInfo=GetProvider()->GetSceneInfo(index);

            std::string baseFilePath = AppContext::GetSceneFolderPath() + sceneInfo.fileName.substr(0, sceneInfo.fileName.size() - 5);
            SDL_RemovePath((baseFilePath + ".png").c_str());
            SDL_RemovePath((baseFilePath + ".jpg").c_str());
            SDL_RemovePath((baseFilePath + ".gif").c_str());
            if (SDL_CopyFile(resultPath.ToStringA().c_str(),(baseFilePath + ".gif").c_str()))
            {
                sceneInfo.imgPath = (baseFilePath + ".gif");
                GetProvider()->EmitDataChanged(index,index);
            }
        }
        else
        {
            //打开裁剪对话框
            UISceneCoverCropper_Dlg::ShowModalDlg(this, resultPath.ToStringA().c_str());
            
        }

        return true;
    }

    return true;
}

bool UISceneItem::OnCaptureCoverClick(const ui::EventArgs& args)
{
    ASSERT(false);
    return true;
}

bool UISceneItem::OnRemoveClick(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->RemoveItem(index);
    return true;
}

SceneItemProvider* UISceneItem::GetProvider()
{
    ui::VirtualVTileListBox* parent = dynamic_cast<ui::VirtualVTileListBox*>(GetOwner());
    if (parent)
    {
       return dynamic_cast<SceneItemProvider*>(parent->GetDataProvider());
        
    }
    return nullptr;
}

void UISceneItem::UpdateUI()
{
    auto provider = GetProvider();
    provider->EmitDataChanged(index, index);
}





SceneItemProvider::SceneItemProvider()
{
}

ui::Control* SceneItemProvider::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
    //nimduilib的设计中，入参是用于获取窗口的，不要在这里进行添加进容器的操作
    
	UISceneItem* item = new UISceneItem(pVirtualListBox->GetWindow());
    return item;
}

bool SceneItemProvider::FillElement(ui::Control* pControl, size_t nElementIndex)
{
    //示例中这里用了一个锁
	UISceneItem* pItem = dynamic_cast<UISceneItem*>(pControl);
	ASSERT(pItem != nullptr);
    if (pItem == NULL || nElementIndex >= sceneList.size())
    {
        return false;
    }

    pItem->InitSubControls(sceneList[nElementIndex].name, sceneList[nElementIndex].imgPath, nElementIndex, sceneList[nElementIndex].selected);
    return true;
}

size_t SceneItemProvider::GetElementCount() const
{
    return sceneList.size();
}

void SceneItemProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{
    if (nElementIndex>=sceneList.size())return;
    sceneList[nElementIndex].selected = bSelected;
    if(bSelected)
    for (size_t index = 0; index < sceneList.size(); index++)
    {
        if(index!= nElementIndex)
            sceneList[index].selected = false;
    }
    //EmitDataChanged();
}

bool SceneItemProvider::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex >= sceneList.size())return false;
    return sceneList[nElementIndex].selected;
}

void SceneItemProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    selectedIndexs.reserve(sceneList.size());
    for (size_t i=0;i<sceneList.size();i++)
    {
        if(sceneList[i].selected)
        selectedIndexs.push_back(i);
    }
}

bool SceneItemProvider::LoadScene(size_t index)
{
	SDL_Log("SceneItemProvider::LoadScene index=%zu", index);
    return true;
}

SceneItemProvider::RESULT_STATUS SceneItemProvider::RenameScene(size_t index, std::string name)
{
    if (index >= sceneList.size())return RESULT_STATUS_INDEX_OUT_OF_RANGE;

    //判断是否为空
    if (name.empty())
        return RESULT_STATUS_RENAME_EMPTY_NAME;


    //判断是否有同名场景
    for (size_t i = 0; i < sceneList.size(); i++)
    {
        if (i != index && sceneList[i].name == name)
        {
            return RESULT_STATUS_RENAME_DUPLICATE_SCENE_NAME;
		}
    }
    sceneList[index].name = name;
    std::string filePath = AppContext::GetSceneFolderPath() + sceneList[index].fileName;
    Json::Value curSceneJson = util::BuildJsonFromFile(filePath.c_str());
	//curSceneJson["Version"] = 1;
	curSceneJson["SceneName"] = name;
    //std::string filepath=
    if (util::SaveJsonToFile(curSceneJson, filePath.c_str()))
    {
        EmitDataChanged(index,index);
		return RESULT_STATUS_DONE;
    }
    else
    {
        //写入文件失败
		return RESULT_STATUS_RENAME_WRITE_FILE_FAILED;
    }


    //return RESULT_STATUS();
}

SceneItemProvider::RESULT_STATUS SceneItemProvider::CreateScene(std::string name, bool fillSceneWithDefaultResource)
{

    //判断是否为空
    if (name.empty())
        return RESULT_STATUS_CREATE_EMPTY_NAME;


    //判断是否有同名场景
    for (size_t i = 0; i < sceneList.size(); i++)
    {
        if (sceneList[i].name == name)
        {
            return RESULT_STATUS_CREATE_DUPLICATE_SCENE_NAME;
        }
    }

    Json::Value sceneJson;
    std::string fileName;
    if (SceneManager::CreateNewSceneJson(name.c_str(), fillSceneWithDefaultResource, true, sceneJson, &fileName))
    {
        //成功创建了场景json文件
        //构建新的item
        SceneUIInfo itemInfo;
        itemInfo.fileName = fileName;
        itemInfo.imgPath = DEFAULT_COVER_PATH;
        itemInfo.name = name;
        itemInfo.selected = false;
        sceneList.push_back(itemInfo);
        EmitCountChanged();

        return RESULT_STATUS_DONE;
    }

    //创建文件失败

    return RESULT_STATUS_CREATE_WRITE_FILE_FAILED;


}

void SceneItemProvider::DuplicateItem(size_t index)
{

    std::string filePath = AppContext::GetSceneFolderPath() + sceneList[index].fileName;
    Json::Value curSceneJson = util::BuildJsonFromFile(filePath.c_str());

    //AAAA
    //AAAA_2
    //AAAA_3
    //AAAA_4
    //...





    std::string realSceneName;
    std::string realFileName;

    {
        //创建一个不重名的
        std::string curItemName = sceneList[index].name;
        auto pos = curItemName.find_last_of('_');
        std::string nameBase;
        int endNum;
        if (pos != std::string::npos)
        {
            std::string endStr = curItemName.substr(pos + 1);
            if (util::StringIsNumber(endStr))
            {

                //末尾是数字
                endNum = std::atoi(endStr.c_str());
                nameBase = curItemName.substr(0, pos + 1);
            }
            else
            {

                nameBase = curItemName + "_";
                endNum = 2;
            }
        }
        else
        {
            endNum = 2;
            nameBase = curItemName + "_";
        }
        while (true)
        {
            //根据endnum构建字符串
            realSceneName = nameBase + std::to_string(endNum);
            //判断是否存在名为realName的物品
            bool hasSameName = false;
            for (auto& x : sceneList)
            {
                if (x.name == realSceneName)
                {
                    hasSameName = true;
                    break;
                }
            }
            if (hasSameName)
            {
                endNum++;
                continue;
            }
            break;
        }
    }
    //按同样的方法获取文件名
    {
        std::string curFileName = sceneList[index].fileName;
        curFileName = curFileName.substr(0, curFileName.size()-5);
        auto pos = curFileName.find_last_of('_');
        std::string nameBase;
        int endNum;
        //防止pos为时间戳前的 '_'
        if (pos != std::string::npos&& pos> curFileName.size()-4)
        {
            std::string endStr = curFileName.substr(pos + 1);
            if (util::StringIsNumber(endStr))
            {

                //末尾是数字
                endNum = std::atoi(endStr.c_str());
                nameBase = curFileName.substr(0, pos + 1);
            }
            else
            {

                nameBase = curFileName + "_";
                endNum = 2;
            }
        }
        else
        {
            endNum = 2;
            nameBase = curFileName + "_";
        }
        while (true)
        {
            //根据endnum构建字符串
            realFileName = nameBase + std::to_string(endNum);
            //判断是否存在名为realName的物品
            bool hasFileName = false;
            for (auto& x : sceneList)
            {
                if (0 == SDL_strncmp(x.fileName.c_str(), realFileName.c_str(), realFileName.size()))
                {
                    hasFileName = true;
                    break;
                }
            }
            if (hasFileName)
            {
                endNum++;
                continue;
            }
            realFileName += ".json";
            break;
        }
    }

    //
    curSceneJson["SceneName"] = realSceneName;
    if (util::SaveJsonToFile(curSceneJson, (AppContext::GetSceneFolderPath() + realFileName).c_str()))
    {

        SceneUIInfo info;


        //如果有封面文件那么也复制封面
        //封面不是默认图片则复制
        if (sceneList[index].imgPath != DEFAULT_COVER_PATH)
        {
            //确认图片类型
            auto pos=sceneList[index].imgPath.find_last_of(".");
            std::string extension = sceneList[index].imgPath.substr(pos);

            std::string newImagePath = AppContext::GetSceneFolderPath()+ realFileName.substr(0, realFileName.size()-5)+ extension;
            //尝试复制
            if (SDL_CopyFile(sceneList[index].imgPath.c_str(), newImagePath.c_str()))
            {
                info.imgPath = newImagePath;
            }


        }



        info.fileName = realFileName;
        if(info.imgPath.empty())info.imgPath = sceneList[index].imgPath;
        info.name = realSceneName;
        info.selected = false;
        sceneList.insert(sceneList.begin() + index + 1, info);
        EmitCountChanged();
    }




}

bool SceneItemProvider::RemoveItem(size_t index)
{
    if (index >= sceneList.size())return false;

    auto it = sceneList.begin() + index;
    //删除目标文件
    {
        std::string filePath = it->fileName;
        std::string baseFilePath = AppContext::GetSceneFolderPath() + filePath.substr(0,filePath.size() - 5);
        SDL_RemovePath((baseFilePath+".json").c_str());
        SDL_RemovePath((baseFilePath +".png").c_str());
        SDL_RemovePath((baseFilePath +".jpg").c_str());
        SDL_RemovePath((baseFilePath +".gif").c_str());
    }
    //删除数据

    if (sceneList[index].selected == true)
    {
        //自动选择为下一个场景
        size_t nextIndex = index + 1;
        if (nextIndex == sceneList.size())nextIndex = 0;
        sceneList[nextIndex].selected = true;
    }
    sceneList.erase(it);

  

    EmitCountChanged();
    return true;
}



void SceneItemProvider::OnCoverSetted(size_t index, const char* imageFileInSceneFolder)
{
    std::string basePath = imageFileInSceneFolder;
    std::string extension = basePath.substr(basePath.find_first_of('.'));
    basePath = basePath.substr(basePath.size()- extension.size());

    if (extension == ".png")
    {
        SDL_RemovePath((basePath + ".jpg").c_str());
        SDL_RemovePath((basePath + ".gif").c_str());
    }
    else if (extension == ".jpg")
    {
        SDL_RemovePath((basePath + ".png").c_str());
        SDL_RemovePath((basePath + ".gif").c_str());
    }
    else if (extension == ".gif")
    {
        SDL_RemovePath((basePath + ".png").c_str());
        SDL_RemovePath((basePath + ".jpg").c_str());
    }

    sceneList[index].imgPath = imageFileInSceneFolder;
    EmitDataChanged(index, index);

}







SceneItemProvider::SceneUIInfo& SceneItemProvider::GetSceneInfo(size_t nElementIndex)
{
	ASSERT(nElementIndex < sceneList.size());
	return sceneList[nElementIndex];
}

void SceneItemProvider::LoadSceneList()
{
	//从文件夹加载场景列表
    //文件名示例：Scene_87B24C7F-5745-4DD8-B89D-BA95C8E8D6D2.json
    //不使用UUID，改用时间戳，这样会自动按时间排序
    //文件名示例：Scene_20250920133012333.json （年月日时分秒毫秒）
    //封面文件：Scene_20250920133012333.png/jpg/gif
    //创建时也不能使用重复的名字
	sceneList.clear();

    std::vector<SceneInfo> scenes= SceneManager::GetInstance().GetSceneList();
	SceneInfo selectScene = SceneManager::GetInstance().GetCurrentScene();
    for (auto& s : scenes)
    {
        SceneUIInfo& sceneUIInfo=sceneList.emplace_back();
		sceneUIInfo.name = s.name;
        sceneUIInfo.imgPath = s.imgPath;
		if(sceneUIInfo.imgPath.empty()) sceneUIInfo.imgPath = DEFAULT_COVER_PATH;
		sceneUIInfo.fileName = s.fileName;
		sceneUIInfo.selected = selectScene.fileName == s.fileName;
    }

    EmitCountChanged();


    return;


}











SceneSelectPage::~SceneSelectPage()
{
	if (provider) delete provider;
    provider = nullptr;
}

void SceneSelectPage::InitContents(uintptr_t userdata1, uintptr_t userdata2)
{

    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    
    if (!inited)
    {
        ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/SceneSelectPage.xml"));
        this->SetName(L"SCENESELECT_PAGE");




        ui::VirtualVTileListBox* container = (ui::VirtualVTileListBox*)FindSubControl(L"sceneItemContainer");
        container->GetLayout()->SetChildHAlignType(ui::HorAlignType::kAlignLeft);



        if (provider) delete provider;
        provider = new SceneItemProvider;
        container->SetDataProvider(provider);
        provider->LoadSceneList();



        auto btn_createScene = static_cast<ui::Button*>(FindSubControl(L"btn_createScene"));

        btn_createScene->AttachClick(ui::UiBind(&SceneSelectPage::OnBtnClicked, this, std::placeholders::_1));
    

        inited = true;
    }
}

bool SceneSelectPage::OnBtnClicked(const ui::EventArgs& args)
{
    
    if (UISceneCreateNew_Dlg::ShowModalDlg(GetWindow(), provider))
    {


    }

    return true;
}
