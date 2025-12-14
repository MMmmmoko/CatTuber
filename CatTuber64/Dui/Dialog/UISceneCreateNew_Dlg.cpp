#include"DuiCommon.h"
#include"Form/SceneSelectPage.h"
#include "UISceneCreateNew_Dlg.h"
#include "DuiMessageBox.h"
#include"AppSettings.h"

bool UISceneCreateNew_Dlg::ShowModalDlg(ui::Window* parentWindow, class SceneItemProvider* provider)
{
    bool result = false;
    UISceneCreateNew_Dlg* dialog = new UISceneCreateNew_Dlg(&result, provider);
    dialog->CreateWnd(parentWindow, ui::WindowCreateParam(L"CatTuber Scene Rename", true));
    dialog->PostQuitMsgWhenClosed(false);
    dialog->ShowModalFake();//草这里不会停的
    return result;
}





UISceneCreateNew_Dlg::UISceneCreateNew_Dlg(bool* resultOut, SceneItemProvider* provider)
	:pResult(resultOut), provider(provider)
{
}

UISceneCreateNew_Dlg::~UISceneCreateNew_Dlg()
{
}



void UISceneCreateNew_Dlg::OnInitWindow()
{
    __super::OnInitWindow();

    //寻找目标控件
    pEdit_NewName = static_cast<ui::RichEdit*>(FindControl(L"edit_sceneName"));

    ASSERT(pEdit_NewName);
    //设置为原始名称
    pEdit_NewName->SetFocus();

    pOp_Empty = static_cast<ui::Option*>(FindControl(L"op_empty"));
    pOp_FillDefault = static_cast<ui::Option*>(FindControl(L"op_filldefault"));
    //根据设置设置初始状态
    if (AppSettings::GetIns().GetUISceneCreateEmpty())
    {
        pOp_Empty->SetSelected(true);
    }
    else
    {
        pOp_FillDefault->SetSelected(true);
    }


    ui::Button* pBtn_Close = static_cast<ui::Button*>(FindControl(L"btn_Close"));
    pBtn_Close->AttachClick(UiBind(&UISceneCreateNew_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_Cancel = static_cast<ui::Button*>(FindControl(L"btn_Cancel"));
    pBtn_Cancel->AttachClick(UiBind(&UISceneCreateNew_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_OK = static_cast<ui::Button*>(FindControl(L"btn_OK"));
    pBtn_OK->AttachClick(UiBind(&UISceneCreateNew_Dlg::OnButtonClick, this, std::placeholders::_1));



}





bool UISceneCreateNew_Dlg::OnButtonClick(const ui::EventArgs& args)
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
        TryCreate();
        return true;
    }
    return true;
}

LRESULT UISceneCreateNew_Dlg::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (ui::kVK_RETURN == vkCode)
    {
        TryCreate();
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

void UISceneCreateNew_Dlg::TryCreate()
{
    //先获取当前对话框中的数据
    std::string getCurName = ui::StringConvert::WStringToUTF8(pEdit_NewName->GetText());



    //provider中进行重命名，并根据结果显示新对话框

    //获取选择状态

    auto renameResult = provider->CreateScene(getCurName, pOp_FillDefault->IsSelected());
    if (renameResult == SceneItemProvider::RESULT_STATUS_DONE)
    {
        if (pResult)*pResult = true;

        bool isCreateEmpty = pOp_Empty->IsSelected();
        if (isCreateEmpty != AppSettings::GetIns().GetUISceneCreateEmpty())
        {
            AppSettings::GetIns().SetUISceneCreateEmpty(isCreateEmpty);
        }


        this->CloseWnd();
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_EMPTY_NAME)
    {
        //提示存在同名场景
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_CREATE_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_NAME"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_DUPLICATE_SCENE_NAME)
    {
        //提示存在同名场景
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_CREATE_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_DUPLICATE_SCENE_NAME"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }
    else if (renameResult == SceneItemProvider::RESULT_STATUS_RENAME_WRITE_FILE_FAILED)
    {
        DuiSimpleMessageBox::ShowModalDlg(this,
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_TITLE_CREATE_FAILED"),
            ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_SCENEDLG_ERROR_WRITE_FILE_FAILED"),
            DuiSimpleMessageBox::BUTTON_OK);
        return;
    }





    if (pResult)
        *pResult = false;
    this->CloseWnd();
    return;




}

