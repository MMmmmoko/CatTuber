#include"DuiCommon.h"
#include"Form/SceneSelectPage.h"
#include "BongoCatImport_Dlg.h"
#include "DuiMessageBox.h"
#include"AppSettings.h"
#include"AppContext.h"
#include"Util/Util.h"
bool BongoCatImport_Dlg::ShowModalDlg(ui::Window* parentWindow)
{

    //先打开一个文件选择对话框
    //使用nimduilib封装
    ui::FileDialog dialog;
    ui::FilePath resultPath;

    if (dialog.BrowseForFolder(parentWindow, resultPath))
    {
   


        //路径不能在CatTuber的BongoCat资源路径内 
        //todo对创意工坊路径进行检测？
        auto parentPath= resultPath.GetParentPath();
        parentPath.FormatPathAsDirectory();
        auto bongoCatPath= ui::FilePath(AppContext::GetBongoCatFolderPath());//这个路径结尾自带斜杠

        //选择的路径结果最后没有斜杠符号
    resultPath.FormatPathAsDirectory();

#if 0
        //bongoCatPath.FormatPathAsDirectory();
        if (parentPath == bongoCatPath)
        {
			DuiSimpleMessageBox::ShowModalDlg(parentWindow, 
                GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_TITLE"), 
                GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_CATTUBERFOLDER"), 
                DuiSimpleMessageBox::BUTTON_OK);
            return false;
        }
        
        //检测内容是否是bongo cat mver 0.1.6以上的文件夹
        {
            //检测此路劲内是否存在config.json文件与img文件夹，img文件夹内至少存在一种模式
            auto configFilePath=resultPath.JoinFilePath(ui::FilePath(L"config.json"));
            auto imgFolderPath=resultPath.JoinFilePath(ui::FilePath(L"img"));
            auto modelFolderPath_gamepad=resultPath.JoinFilePath(ui::FilePath(L"img/gamepad"));
            auto modelFolderPath_keyboard=resultPath.JoinFilePath(ui::FilePath(L"img/keyboard"));
            auto modelFolderPath_standard=resultPath.JoinFilePath(ui::FilePath(L"img/standard"));
            if(!configFilePath.IsExistsFile())
                {
                DuiSimpleMessageBox::ShowModalDlg(parentWindow, 
                    GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_TITLE"), 
                    GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_WRONGFOLDER_CONFIG"), 
                    DuiSimpleMessageBox::BUTTON_OK);
                return false;
			}
            if (!configFilePath.IsExistsFile() || !imgFolderPath.IsExistsDirectory() ||
                !(modelFolderPath_gamepad.IsExistsDirectory() || modelFolderPath_keyboard.IsExistsDirectory() || modelFolderPath_standard.IsExistsDirectory()))
            {
                    DuiSimpleMessageBox::ShowModalDlg(parentWindow,
                        GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_TITLE"),
                        GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_WRONGFOLDER_CONTENT"),
                        DuiSimpleMessageBox::BUTTON_OK);
                    return false;
            }
        }

#endif

        BongoCatImport_Dlg* dialog = new BongoCatImport_Dlg(resultPath.ToStringA().c_str());
        if (!dialog->isInited)
        {
            DuiSimpleMessageBox::ShowModalDlg(parentWindow,
                GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_TITLE"),
                GETDUISTRING(dialog->errStrID),
                DuiSimpleMessageBox::BUTTON_OK);
            delete dialog;
            return false;
        }


        dialog->DoModal(parentWindow, ui::WindowCreateParam(GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT"), true), false, false);
        bool result = dialog->dlgResult;
        if (!result)
        {
            DuiSimpleMessageBox::ShowModalDlg(parentWindow,
                GETDUISTRING(L"STRID_MODELITEM_BONGOCAT_IMPORT_FILEDDLG_TITLE"),
                GETDUISTRING(dialog->errStrID),
                DuiSimpleMessageBox::BUTTON_OK);
        }
        delete dialog;
        return result;
    }








    //BongoCatImport_Dlg* dialog = new BongoCatImport_Dlg();
    ////dialog->CreateWnd(parentWindow, ui::WindowCreateParam(L"CatTuber Scene Rename", true));
    ////dialog->PostQuitMsgWhenClosed(false);
    ////dialog->ShowModalFake();//草这里不会停的
    return false;
}





BongoCatImport_Dlg::BongoCatImport_Dlg(const char* u8FolderPath)
    :u8FolderPath(u8FolderPath)
{
    isInited = true;
}

BongoCatImport_Dlg::~BongoCatImport_Dlg()
{
}



void BongoCatImport_Dlg::OnInitWindow()
{
    __super::OnInitWindow();

    //刷新一下界面。以TextID显示的情况下窗口大小会出现问题
    auto pTitle = static_cast<ui::Label*>(FindControl(L"selectWarningText"));
    pTitle->SetText(pTitle->GetText());
    pTitle->SetText(L"");
    pTitle->SetTextId(pTitle->GetTextId());



    /*ui::Button* */btn_modeselect = static_cast<ui::Button*>(FindControl(L"btn_modeselect"));
    btn_modeselect->AttachClick(UiBind(&BongoCatImport_Dlg::OnButtonClick, this, std::placeholders::_1));
    ui::Button* pBtn_Cancel = static_cast<ui::Button*>(FindControl(L"btn_Cancel"));
    pBtn_Cancel->AttachClick(UiBind(&BongoCatImport_Dlg::OnButtonClick, this, std::placeholders::_1));
    ui::Button* pBtn_OK = static_cast<ui::Button*>(FindControl(L"btn_OK"));
    pBtn_OK->AttachClick(UiBind(&BongoCatImport_Dlg::OnButtonClick, this, std::placeholders::_1));


    //计算模式选择按钮宽度
    {
        //下面不可行，因为获取不到正确的字符长度

        ui::UiSize sizeMax(9999, 9999);
        int32_t maxW = 0;
        //btn_modeselect->SetTextId(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_L2D");
        //int32_t curW = btn_modeselect->EstimateSize(sizeMax).cx.GetInt32(); if (curW > maxW)maxW = curW;
        //btn_modeselect->EstimateText(sizeMax).c;
//#define MAXWCALC(ID){btn_modeselect->SetTextId(ID); int32_t curW = btn_modeselect->EstimateText(sizeMax).cx; if (curW > maxW)maxW = curW; }

#define MAXWCALC(ID){btn_modeselect->SetTextId(ID); int32_t curW = btn_modeselect->EstimateSize(sizeMax).cx.GetInt32(); if (curW > maxW)maxW = curW; }
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_L2D");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_L2D_PEN");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_PEN");

        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_KEYBOARD_L2D");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_KEYBOARD");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_GAMEPAD_L2D");
        MAXWCALC(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_GAMEPAD");
#undef MAXWCALC
        btn_modeselect->SetFixedWidth(ui::UiFixedInt(maxW+16), true, false);
        btn_modeselect->SetTextId(L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD");

    }
}

bool BongoCatImport_Dlg::OnButtonClick(const ui::EventArgs& args)
{
    DString controlName = args.GetSender()->GetName();
    if (controlName == L"btn_modeselect")
    {
        //打开一个选择菜单

        ui::Menu* menu = new ui::Menu(this);
        menu->SetSkinFolder(L"CatTuber_default");
        DString xml(L"BongoCatImport_ModeSelectMenu.xml");
        ui::UiPoint cursorPos;
        GetCursorPos(cursorPos);
        menu->ShowMenu(xml, cursorPos);

        ui::MenuItem* menu_standard = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_standard"));
        ui::MenuItem* menu_standard_pen = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_standard_pen"));
        ui::MenuItem* menu_standard_l2d = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_standard_l2d"));
        ui::MenuItem* menu_standard_l2d_pen = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_standard_l2d_pen"));

        ui::MenuItem* menu_keyboard = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_keyboard"));
        ui::MenuItem* menu_keyboard_l2d = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_keyboard_l2d"));

        ui::MenuItem* menu_gamepad = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_gamepad"));
        ui::MenuItem* menu_gamepad_l2d = dynamic_cast<ui::MenuItem*>(menu->FindControl(L"menu_gamepad_l2d"));




        ui::UiSize sizeMax(9999, 9999);
        int32_t maxW = 0;
#define MAXWCALC(ctl) if (ctl) { int32_t curW = ctl->EstimateSize(sizeMax).cx.GetInt32(); if (curW > maxW)maxW = curW; }
        MAXWCALC(menu_standard);
        MAXWCALC(menu_standard_pen);
        MAXWCALC(menu_standard_l2d);
        MAXWCALC(menu_standard_l2d_pen);
        MAXWCALC(menu_keyboard);
        MAXWCALC(menu_keyboard_l2d);
        MAXWCALC(menu_gamepad);
        MAXWCALC(menu_gamepad_l2d);
#undef MAXWCALC


        menu_standard->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_standard_pen->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_standard_l2d->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_standard_l2d_pen->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_keyboard->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_keyboard_l2d->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_gamepad->SetFixedWidth(ui::UiFixedInt(maxW), false, false);
        menu_gamepad_l2d->SetFixedWidth(ui::UiFixedInt(maxW), true, false);


        auto clickFunc = [](const ui::EventArgs& msg)->bool {
            ui::Button* ptargetButton=(ui::Button*)msg.GetSender()->GetWindow()->GetParentWindow()->FindControl(L"btn_modeselect");
            if (ptargetButton)
            {
                ui::Label* pLabel = (ui::Label*)((ui::MenuItem*)(msg.GetSender()))->GetItemAt(0);
                ptargetButton->SetTextId(pLabel->GetTextId());
            }
            return true; };
        
        menu_standard->AttachClick(clickFunc);
        menu_standard_pen->AttachClick(clickFunc);
        menu_standard_l2d->AttachClick(clickFunc);
        menu_standard_l2d_pen->AttachClick(clickFunc);
        menu_keyboard->AttachClick(clickFunc);
        menu_keyboard_l2d->AttachClick(clickFunc);
        menu_gamepad->AttachClick(clickFunc);
        menu_gamepad_l2d->AttachClick(clickFunc);

        return true;
    }


    if (controlName == L"btn_Close" || controlName == L"btn_Cancel")
    {
        this->CloseWnd();
        return true;
    }

    if (controlName == L"btn_OK")
    {
        //TryCreate();
        dlgResult=DoImport();
        this->CloseWnd();
        return true;
    }
    return true;
}

bool BongoCatImport_Dlg::DoImport()
{
    //发送到其他线程？
    std::wstring modelId=btn_modeselect->GetTextId();


    //先尝试在软件自身相对路径中新建文件夹，如果无法新建文件夹说明无写入权限
    std::string targetFolderPath=AppContext::GetBongoCatFolderPath();
    //targetFolderPath+=

        //获取当前时间戳
    SDL_Time curtime = 0;
    //不进行验证，源码中只有传入空指针的时候出错
    SDL_GetCurrentTime(&curtime);    
    SDL_DateTime date = {};
    SDL_TimeToDateTime(curtime, &date, true);
    char folderNameBuffer[100];
    SDL_snprintf(folderNameBuffer, sizeof(folderNameBuffer), "BongoCat_%04d%02d%02d%02d%02d%02d%03d",
        date.year, date.month, date.day, date.hour, date.minute, date.second, date.nanosecond / 1000'000);
    targetFolderPath += folderNameBuffer;

    if (!SDL_CreateDirectory(targetFolderPath.c_str()))
    {
        targetFolderPath = AppContext::GetPrefPath();
        targetFolderPath = targetFolderPath + "UserModel/BongoCatMver/" + folderNameBuffer;
        //软件没有写入权限
        if (!SDL_CreateDirectory(targetFolderPath.c_str()))
        {
            errStrID = L"STRID_MODELITEM_BONGOCAT_IMPORT_ERROR_NOWRITEPERMISSION";
            return false;
        }

    }

    targetFolderPath += "/";


    
    if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_L2D")
    {
        SDL_CreateDirectory((targetFolderPath+"img/standard").c_str());
        //键鼠L2D
        //SDL_CopyFile((u8FolderPath+"config.json").c_str(), (targetFolderPath + "config.json").c_str());
#define _COPYBCMFILE(file) SDL_CopyFile((u8FolderPath+file).c_str(), (targetFolderPath + file).c_str())
#define _COPYBCMFOLDER(file) util::SDL_CopyFolder((u8FolderPath+file).c_str(), (targetFolderPath + file).c_str())
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/standard/mousebg.png");
        _COPYBCMFOLDER("img/standard/face");
        _COPYBCMFOLDER("img/standard/hand");
        _COPYBCMFOLDER("img/standard/keyboard");
        _COPYBCMFOLDER("img/standard/sounds");
        _COPYBCMFOLDER("img/standard/cat_model");
        //_COPYBCMFILE("img/standard/up.png");

    }
    if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD")
    {
        SDL_CreateDirectory((targetFolderPath + "img/standard").c_str());
        //键鼠非L2D
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/standard/mousebg.png");
        _COPYBCMFOLDER("img/standard/face");
        _COPYBCMFOLDER("img/standard/hand");
        _COPYBCMFOLDER("img/standard/keyboard");
        _COPYBCMFOLDER("img/standard/sounds");
        //_COPYBCMFOLDER("img/standard/cat_model");

        _COPYBCMFILE("img/standard/up.png");
        _COPYBCMFILE("img/standard/arm.png");
        _COPYBCMFILE("img/standard/mouse.png");
        _COPYBCMFILE("img/standard/mouse_left.png");
        _COPYBCMFILE("img/standard/mouse_right.png");
        _COPYBCMFILE("img/standard/mouse_side.png");




    }
    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_L2D_PEN")
    {
        SDL_CreateDirectory((targetFolderPath + "img/standard").c_str());
        //键鼠 (Live2D + 笔)
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/standard/tabletbg.png");
        _COPYBCMFOLDER("img/standard/face");
        _COPYBCMFOLDER("img/standard/hand");
        _COPYBCMFOLDER("img/standard/keyboard");
        _COPYBCMFOLDER("img/standard/sounds");
        _COPYBCMFOLDER("img/standard/cat_model");
        //_COPYBCMFILE("img/standard/up.png");

    }
    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_STANDARD_PEN")
    {
        SDL_CreateDirectory((targetFolderPath + "img/standard").c_str());
        //键鼠 (笔)
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/standard/tabletbg.png");
        _COPYBCMFOLDER("img/standard/face");
        _COPYBCMFOLDER("img/standard/hand");
        _COPYBCMFOLDER("img/standard/keyboard");
        _COPYBCMFOLDER("img/standard/sounds");
        //_COPYBCMFOLDER("img/standard/cat_model");

        _COPYBCMFILE("img/standard/up.png");
        _COPYBCMFILE("img/standard/arm.png");
        _COPYBCMFILE("img/standard/tablet.png");
        _COPYBCMFILE("img/standard/tablet_left.png");
        _COPYBCMFILE("img/standard/tablet_right.png");
        _COPYBCMFILE("img/standard/tablet_side.png");
    }



    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_KEYBOARD_L2D")
    {
        SDL_CreateDirectory((targetFolderPath + "img/keyboard").c_str());
        //键盘 (Live2D)
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/keyboard/bg.png");

        _COPYBCMFOLDER("img/keyboard/face");
        _COPYBCMFOLDER("img/keyboard/lefthand");
        _COPYBCMFOLDER("img/keyboard/righthand");
        _COPYBCMFOLDER("img/keyboard/keyboard");
        _COPYBCMFOLDER("img/keyboard/sounds");
        _COPYBCMFOLDER("img/keyboard/cat_model");


    }
    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_KEYBOARD")
    {
        SDL_CreateDirectory((targetFolderPath + "img/keyboard").c_str());
        //键盘

        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/keyboard/bg.png");
        _COPYBCMFILE("img/keyboard/cat.png");

        _COPYBCMFOLDER("img/keyboard/face");
        _COPYBCMFOLDER("img/keyboard/lefthand");
        _COPYBCMFOLDER("img/keyboard/righthand");
        _COPYBCMFOLDER("img/keyboard/keyboard");
        _COPYBCMFOLDER("img/keyboard/sounds");
        //_COPYBCMFOLDER("img/keyboard/cat_model");
    }

    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_GAMEPAD_L2D")
    {
        SDL_CreateDirectory((targetFolderPath + "img/gamepad").c_str());
        //手柄 (Live2D)
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/gamepad/bg.png");

        _COPYBCMFOLDER("img/gamepad/face");
        _COPYBCMFOLDER("img/gamepad/lefthand");
        _COPYBCMFOLDER("img/gamepad/righthand");
        _COPYBCMFOLDER("img/gamepad/keyboard");
        _COPYBCMFOLDER("img/gamepad/sounds");
        _COPYBCMFOLDER("img/gamepad/cat_model");

    }
    else if (modelId == L"STRID_MODELITEM_BONGOCAT_IMPORT_DLG_MODE_GAMEPAD")
    {
        SDL_CreateDirectory((targetFolderPath + "img/gamepad").c_str());
        //手柄
        _COPYBCMFILE("config.json");
        _COPYBCMFILE("img/gamepad/bg.png");

        _COPYBCMFOLDER("img/gamepad/face");
        _COPYBCMFOLDER("img/gamepad/lefthand");
        _COPYBCMFOLDER("img/gamepad/righthand");
        _COPYBCMFOLDER("img/gamepad/keyboard");
        _COPYBCMFOLDER("img/gamepad/sounds");
        //_COPYBCMFOLDER("img/gamepad/cat_model");

        _COPYBCMFILE("img/gamepad/cat.png");
        _COPYBCMFILE("img/gamepad/arm_L.png");
        _COPYBCMFILE("img/gamepad/arm_R.png");
        _COPYBCMFILE("img/gamepad/left_stick.png");
        _COPYBCMFILE("img/gamepad/left_stick_down.png");
        _COPYBCMFILE("img/gamepad/right_stick.png");
        _COPYBCMFILE("img/gamepad/right_stick_down.png");

    }
    return true;
}
