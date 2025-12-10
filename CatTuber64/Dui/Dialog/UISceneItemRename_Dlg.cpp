#include"DuiCommon.h"
#include"Form/SceneSelectPage.h"
#include "UISceneItemRename_Dlg.h"
#include "DuiMessageBox.h"
bool UISceneItemRename_Dlg::ShowModalDlg(UISceneItem* item)
{
    bool result = false;
    UISceneItemRename_Dlg* dialog = new UISceneItemRename_Dlg(&result, item);
    dialog->CreateWnd(item->GetWindow(), ui::WindowCreateParam(L"CatTuber Scene Rename", true));
    dialog->PostQuitMsgWhenClosed(false);
    dialog->ShowModalFake();//草这里不会停的
    return result;
}





UISceneItemRename_Dlg::UISceneItemRename_Dlg(bool* resultOut, UISceneItem* item)
	:pResult(resultOut), pItem(item)
{
	originalName = item->GetSceneName();
}

UISceneItemRename_Dlg::~UISceneItemRename_Dlg()
{
}



void UISceneItemRename_Dlg::OnInitWindow()
{
    __super::OnInitWindow();

    //寻找目标控件
    pEdit_NewName = static_cast<ui::RichEdit*>(FindControl(L"edit_sceneName"));

    ASSERT(pEdit_NewName);
    //设置为原始名称
    pEdit_NewName->SetText(ui::StringConvert::UTF8ToWString(originalName));
    pEdit_NewName->SetFocus();
    pEdit_NewName->SetSelAll();


    ui::Button* pBtn_Close = static_cast<ui::Button*>(FindControl(L"btn_Close"));
    pBtn_Close->AttachClick(UiBind(&UISceneItemRename_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_Cancel = static_cast<ui::Button*>(FindControl(L"btn_Cancel"));
    pBtn_Cancel->AttachClick(UiBind(&UISceneItemRename_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_OK = static_cast<ui::Button*>(FindControl(L"btn_OK"));
    pBtn_OK->AttachClick(UiBind(&UISceneItemRename_Dlg::OnButtonClick, this, std::placeholders::_1));



}





bool UISceneItemRename_Dlg::OnButtonClick(const ui::EventArgs& args)
{
    DString controlName = args.GetSender()->GetName();
    if (controlName == L"btn_Close" || controlName == L"btn_Cancel")
    {
        if (pResult)*pResult = false;
        this->CloseWnd();
        return true;
    }

    if (controlName == L"btn_OK")
    {
        TryRename();
#if 0
        //先获取当前对话框中的数据
		std::string getCurName = ui::StringConvert::WStringToUTF8(pEdit_NewName->GetText());
        if (originalName == getCurName)
        {
            if (pResult)*pResult = false;
            this->CloseWnd();
			return true;
        }


        //provider中进行重命名，并根据结果显示新对话框
        auto provider=pItem->GetProvider();
        auto renameResult=provider->RenameScene(pItem->GetElementIndex(), getCurName);
        if (renameResult == SceneItemProvider::RESULT_STATUS_DONE)
        {
            if (pResult)*pResult = true;
            this->CloseWnd();
            return true;
        }
        else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_DUPLICATE_SCENE_NAME)
        {
            //提示存在同名场景
			DuiSimpleMessageBox::ShowModalDlg(this,
                ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENESELECT_RENAMEDLG_TITLE"),
                ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENESELECT_RENAMEDLG_DUPLICATE_SCENE_NAME"),
                DuiSimpleMessageBox::BUTTON_OK);
            return true;
        }
        else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_WRITE_FILE_FAILED)
        {
			DuiSimpleMessageBox::ShowModalDlg(this,
                ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENESELECT_RENAMEDLG_TITLE"),
                ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENESELECT_RENAMEDLG_WRITE_FILE_FAILED"),
                DuiSimpleMessageBox::BUTTON_OK);
            return true;
        }


        


        if (pResult)
            *pResult = false;
        this->CloseWnd();
#endif
        return true;
    }
    return true;
}

LRESULT UISceneItemRename_Dlg::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (ui::kVK_RETURN == vkCode)
    {
        TryRename();
        bHandled = true;
        return true;
    }
    if (ui::kVK_ESCAPE == vkCode)
    {
        if (pResult)*pResult = false;
        this->CloseWnd();
        bHandled = true;
        return true;
    }

    return Window::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

void UISceneItemRename_Dlg::TryRename()
{
    //先获取当前对话框中的数据
    std::string getCurName = ui::StringConvert::WStringToUTF8(pEdit_NewName->GetText());
    if (originalName == getCurName)
    {
        if (pResult)*pResult = false;
        this->CloseWnd();
        return;
    }


    //provider中进行重命名，并根据结果显示新对话框
    auto provider = pItem->GetProvider();
    auto renameResult = provider->RenameScene(pItem->GetElementIndex(), getCurName);
    if (renameResult == SceneItemProvider::RESULT_STATUS_DONE)
    {
        if (pResult)*pResult = true;
        this->CloseWnd();
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_EMPTY_NAME)
    {
        //提示存在同名场景
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_RENAME_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_NAME"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_DUPLICATE_SCENE_NAME)
    {
        //提示存在同名场景
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_RENAME_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_DUPLICATE_SCENE_NAME"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_WRITE_FILE_FAILED)
    {
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_RENAME_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_WRITE_FILE_FAILED"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }





    if (pResult)
        *pResult = false;
    this->CloseWnd();
    return;




}

