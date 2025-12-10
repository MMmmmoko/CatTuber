#include"Dui.h"
#include"SceneSelectPage.h"
#include"Dialog/UISceneItemRename_Dlg.h"
#include"AppContext.h"
#include"Util.h"
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


    }
    auto testPtr = dynamic_cast<ui::VBox*> (this);
    imgCover->SetUTF8BkImage(img);
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
    ui::MenuItem* SceneItemMenu_Rename = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Rename"));
    ui::MenuItem* SceneItemMenu_Remove = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"SceneItemMenu_Remove"));

    ui::UiSize sizeMax(9999, 9999);
    int32_t maxW = 0;
    if (SceneItemMenu_Load)
    {
        int32_t curW = SceneItemMenu_Load->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
        if (IsSelected())
            SceneItemMenu_Load->SetVisible(false);
    }
    if (SceneItemMenu_Rename)
    {
        int32_t curW = SceneItemMenu_Rename->EstimateSize(sizeMax).cx.GetInt32();
        if (curW > maxW)maxW = curW;
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
                SceneItemMenu_Remove->SetVisible(false);
            }
        }

    }
    SceneItemMenu_Load->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_Rename->SetFixedWidth(ui::UiFixedInt(maxW), true, false);
    SceneItemMenu_Remove->SetFixedWidth(ui::UiFixedInt(maxW), true, false);




    //添加按钮功能
    SceneItemMenu_Load->AttachClick(ui::UiBind(&UISceneItem::OnLoadClick, this, std::placeholders::_1));
    SceneItemMenu_Rename->AttachClick(ui::UiBind(&UISceneItem::OnRenameClick, this, std::placeholders::_1));
    SceneItemMenu_Remove->AttachClick(ui::UiBind(&UISceneItem::OnRemoveClick, this, std::placeholders::_1));



    return true;
}

bool UISceneItem::OnLoadClick(const ui::EventArgs& args)
{
    auto provider = GetProvider();
    provider->LoadScene(index);

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
    Json::Value curSceneJson = util::BuildJsonFromFile(sceneList[index].filePath.c_str());
	curSceneJson["SceneName"] = name;
    if (util::SaveJsonToFile(curSceneJson, sceneList[index].filePath.c_str()))
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

bool SceneItemProvider::RemoveItem(size_t index)
{
    if (index >= sceneList.size())return false;

    auto it = sceneList.begin() + index;
    //删除目标文件
    {
        std::string filePath = it->filePath;
        std::filesystem::remove(filePath);
		std::string basePath = filePath.substr(0, filePath.size() - 5);//去掉.json
        std::filesystem::remove(basePath+".png");
        std::filesystem::remove(basePath+".jpg");
        std::filesystem::remove(basePath+".gif");
    }
    //删除数据
    sceneList.erase(it);
    EmitCountChanged();
    return true;
}







const SceneItemProvider::SceneInfo& SceneItemProvider::GetSceneInfo(size_t nElementIndex)
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
	std::string sceneFoldPath=AppContext::GetSceneFolderPath();
    
    int matchCount = 0;
	char** sceneJsonFiles=SDL_GlobDirectory(sceneFoldPath.c_str(), "Scene_*.json", 0, &matchCount);
    for (int i = 0; i < matchCount; i++)
    {
        Json::Value curSceneJson = util::BuildJsonFromFile((sceneFoldPath+sceneJsonFiles[i]).c_str());



        if (!curSceneJson["Version"].isUInt())continue;


        if (curSceneJson["SceneName"].isString())
        {
            auto& curScene=sceneList.emplace_back();
			curScene.name = curSceneJson["SceneName"].asString();
			curScene.filePath = sceneJsonFiles[i];

			//检查封面图片文件是否存在
			std::string baseFilePath = sceneFoldPath+ curScene.filePath.substr(0, curScene.filePath.size() - 5);//去掉.json
			
            if (std::filesystem::exists(baseFilePath + ".png"))
            {
                curScene.imgPath = baseFilePath + ".png";
            }
            else if (std::filesystem::exists(baseFilePath + ".jpg"))
            {
                curScene.imgPath = baseFilePath + ".jpg";
            }
            else if (std::filesystem::exists(baseFilePath + ".gif"))
            {
                curScene.imgPath = baseFilePath + ".gif";
            }
            else
            {
				//不存在封面图片，使用默认图片
				curScene.imgPath = "UISceneItem_DefaultCover.png";
            }
        }
    }
    if (sceneJsonFiles)SDL_free(sceneJsonFiles);

    EmitCountChanged();


    return;


}











void SceneSelectPage::InitContents()
{

    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    
    
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/SceneSelectPage.xml"));
    this->SetName(L"SCENESELECT_PAGE");




    ui::VirtualVTileListBox* container= (ui::VirtualVTileListBox*)FindSubControl(L"sceneItemContainer");
    
    auto provider = new SceneItemProvider;
    container->SetDataProvider(provider);
    provider->LoadSceneList();

}
