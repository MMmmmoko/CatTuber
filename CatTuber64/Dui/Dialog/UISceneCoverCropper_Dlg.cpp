#include"DuiCommon.h"
#include "UISceneCoverCropper_Dlg.h"
#include "DuiMessageBox.h"
#include"AppContext.h"
#include"Form/SceneSelectPage.h"
bool UISceneCoverCropper_Dlg::ShowModalDlg(UISceneItem* item, const char* imgPath)
{
    bool result = false;
    UISceneCoverCropper_Dlg* dialog = new UISceneCoverCropper_Dlg( item, imgPath);
    dialog->CreateWnd(item->GetWindow(), ui::WindowCreateParam(L"CatTuber Scene Cover Cropper", true));
    dialog->PostQuitMsgWhenClosed(false);
    dialog->ShowModalFake();//草这里不会停的
    return result;
}





UISceneCoverCropper_Dlg::UISceneCoverCropper_Dlg(class UISceneItem* item, const char* imgPath)
    :pItem(item), imgPath(imgPath)
{
}

UISceneCoverCropper_Dlg::~UISceneCoverCropper_Dlg()
{
}



void UISceneCoverCropper_Dlg::OnInitWindow()
{
    __super::OnInitWindow();

    

    imageCropperControl.InitControls(this);
    imageCropperControl.SetImageFromFile(imgPath.c_str());
    imageCropperControl.SetAspectRatio(4,3);

    ui::Button* pBtn_Close = static_cast<ui::Button*>(FindControl(L"btn_Close"));
    pBtn_Close->AttachClick(UiBind(&UISceneCoverCropper_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_Cancel = static_cast<ui::Button*>(FindControl(L"btn_Cancel"));
    pBtn_Cancel->AttachClick(UiBind(&UISceneCoverCropper_Dlg::OnButtonClick, this, std::placeholders::_1));

    ui::Button* pBtn_OK = static_cast<ui::Button*>(FindControl(L"btn_OK"));
    pBtn_OK->AttachClick(UiBind(&UISceneCoverCropper_Dlg::OnButtonClick, this, std::placeholders::_1));


    //预览场景
    ui::CheckBoxBox* pPreviewItem= static_cast<ui::CheckBoxBox*>(FindControl(L"cbb_previewItem"));
    pPreviewItem->AttachSelect(UiBind(&UISceneCoverCropper_Dlg::OnPreviewItemClick, this, std::placeholders::_1));
    pPreviewItem->AttachUnSelect(UiBind(&UISceneCoverCropper_Dlg::OnPreviewItemClick, this, std::placeholders::_1));



    //滑块
    ui::Slider* pZoomSlider = static_cast<ui::Slider*>(FindControl(L"slider_imgScale"));
    pZoomSlider->AttachValueChange(UiBind(&UISceneCoverCropper_Dlg::OnSliderValueChange, this, std::placeholders::_1));


}





bool UISceneCoverCropper_Dlg::OnButtonClick(const ui::EventArgs& args)
{
    DString controlName = args.GetSender()->GetName();
    if (controlName == L"btn_Close" || controlName == L"btn_Cancel")
    {
        this->CloseWnd();
        return true;
    }

    if (controlName == L"btn_OK")
    {
        if (DoSaveCover())
        {
            this->CloseWnd();
        }
        return true;
    }
    return true;
}

bool UISceneCoverCropper_Dlg::OnPreviewItemClick(const ui::EventArgs& args)
{
    ui::CheckBoxBox* pPreviewItem = static_cast<ui::CheckBoxBox*>(args.GetSender());
    ui::Label* pItemName = static_cast<ui::Label*>(pPreviewItem->FindSubControl(L"label_previewItemName"));
    if (!pItemName)return true;
    if (pPreviewItem->IsSelected())
    {
        pItemName->SetStateTextColor(ui::kControlStateNormal, L"subjectColor_content");
    }
    else
    {
        
        pItemName->SetStateTextColor(ui::kControlStateNormal, L"");
    }
    



    return true;
}

bool UISceneCoverCropper_Dlg::OnSliderValueChange(const ui::EventArgs& args)
{
    double value=((ui::Slider*)args.GetSender())->GetValue();
    //SDL_Log("Slider Value %f,event data: %d, wparam:%llu",value, args.eventData, args.wParam);
    //value范围是滑块最小值到最大值，wParam和value一致，但是是整数，eventData未使用
    //设置的值范围是0~65535  滑块为从scaleMin为基础，以指数的形式放大20倍

    //计算ln20
    static const double areaMax = std::log(20);
    double mapValue = areaMax * value / 65535;
    float newScale = static_cast<float>(std::exp(mapValue));

    imageCropperControl.SetZoomScale(newScale);

    return true;
}

bool UISceneCoverCropper_Dlg::DoSaveCover()
{
    auto fileNameStr = pItem->GetSceneFileName();
    std::string outPath = (imageCropperControl.WriteImage(AppContext::GetSceneFolderPath(), fileNameStr.substr(0,fileNameStr.size() - 5).c_str()));
    std::string basePath = AppContext::GetSceneFolderPath() + fileNameStr.substr(0,fileNameStr.size() - 5);
    if (!outPath.empty())
    {
        pItem->GetProvider()->OnCoverSetted(pItem->GetSceneIndex(), outPath.c_str());
        return true;
    }
    return false;
}

LRESULT UISceneCoverCropper_Dlg::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (ui::kVK_RETURN == vkCode)
    {

        if(DoSaveCover())
        {
            this->CloseWnd();
            return true;
        }
        


        bHandled = true;
        return true;
    }
    return Window::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}
