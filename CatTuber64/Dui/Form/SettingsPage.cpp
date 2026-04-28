#include"Dui.h"
#include"SettingsPage.h"
#include"AppSettings.h"




void SettingsPage::InitContents(uintptr_t userdata1, uintptr_t userdata2)
{


    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    //ui::Box* Box=NULL;

    //{
    //    ui::WindowBuilder builder;
    //    if (builder.ParseXmlFile(ui::FilePath(L"CatTuber_default/SettingsPage.xml"))) {
    //        Control* pControl = builder.CreateControls(ui::CreateControlCallback(),GetWindow());
    //        ASSERT(pControl != nullptr);
    //        Box= builder.ToBox(pControl);
    //    }
    //}
    //ASSERT(Box&&"Can not be nullptr.");

    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(L"CatTuber_default/SettingsPage.xml"));


	//this->AddItem(Box);

	this->SetName(L"SETTINGS_PAGE");

    


#define TABOP_TO_TABOPSTR(x) L#x
#define UISETTINGSPAGE_TABOP_InitControl(op) \
tabop_##op = static_cast<ui::Option*>(FindSubControl(TABOP_TO_TABOPSTR(tabop_##op)));\
tabop_##op->AttachSelect(ui::UiBind(&SettingsPage::OnTabOptionClicked, this, std::placeholders::_1));\
tabop_##op->AttachUnSelect(ui::UiBind(&SettingsPage::OnTabOptionClicked, this, std::placeholders::_1));\
header_##op = static_cast<ui::Box*>(FindSubControl(TABOP_TO_TABOPSTR(header_##op)));\
\
settingsPage_##op.InitContents(this);
    UISETTINGSPAGE_TABOP_LIST(UISETTINGSPAGE_TABOP_InitControl);
#undef UISETTINGSPAGE_TABOP_InitControl
    

    //header_input->getsc
    //tabop_window = static_cast<ui::Option*>(FindSubControl(L"tabop_window"));
    //tabop_window->AttachSelect(ui::UiBind(&SettingsPage::OnTabOptionClicked, this, std::placeholders::_1));
    
    vbox_container= static_cast<ui::VListBox*>(FindSubControl(L"box_Container"));
    vbox_container->AttachScrollPosChanged(ui::UiBind(&SettingsPage::OnScrollChange, this, std::placeholders::_1));



    tabop_window->Selected(true,true);





    //音频
    {
        double volumevalue=AppSettings::GetIns().GetVolumeValue();
        slider_volume= static_cast<ui::Slider*>(FindSubControl(L"slider_volume"));
        slider_volume->SetValue(volumevalue*100.);
        slider_volume->AttachValueChanged(ui::UiBind(&SettingsPage::OnSoundSliderValueChanged, this, std::placeholders::_1));
    }




}





bool SettingsPage::OnTabOptionClicked(const ui::EventArgs& msg)
{
    //if (msg.GetSender() == tabop_window)
    //{
    //    if (tabop_window->IsSelected())
    //    {
    //        tabop_window->SetBorderSize(ui::UiRectF(0, 0, 0, 2), false);
    //        tabop_window->SetFontId(L"system_bold_16");
    //        //让vbox_container滚动到tabop_window所在的位置
    //        vbox_container->ScrollItemToTop(tabop_window->GetName());
    //    }
    //    else
    //    {
    //        tabop_window->SetBorderSize(ui::UiRectF(0, 0, 0, 0), false);
    //        tabop_window->SetFontId(L"system_16");
    //    }
    //}



#define UISETTINGSPAGE_TABOP_OnClickControl(opname) \
    if (msg.GetSender() == tabop_##opname) \
    {\
        if (tabop_##opname->IsSelected()) \
        {\
            tabop_##opname->SetBorderSize(ui::UiRectF(0, 0, 0, 2), false);\
            tabop_##opname->SetFontId(L"system_bold_16");\
_skip_scrollChangeEvent=true;\
 vbox_container->ScrollItemToTop( TABOP_TO_TABOPSTR(header_##opname));\
        }\
        else\
        {\
            tabop_##opname->SetBorderSize(ui::UiRectF(0, 0, 0, 0), false);\
            tabop_##opname->SetFontId(L"system_16");\
        }\
    }
    UISETTINGSPAGE_TABOP_LIST(UISETTINGSPAGE_TABOP_OnClickControl);

#undef UISETTINGSPAGE_TABOP_OnClickControl


    return true;
}

bool SettingsPage::OnScrollChange(const ui::EventArgs& msg)
{
    if (!_skip_scrollChangeEvent)return false;
    _skip_scrollChangeEvent = false;
    //SDL_Log("scrollPos:%llu", vbox_container->GetScrollPos().cy);
    Control* topItem = vbox_container->GetTopItem();
    //if (topItem == header_input)
    //{
    //    tabop_input->Selected(true,false);
    //    tabop_input->SetBorderSize(ui::UiRectF(0, 0, 0, 2), false);
    //    tabop_input->SetFontId(L"system_bold_16");
    //}
#define UISETTINGSPAGE_ScroolChange(headerName) \
    if (topItem == header_##headerName)\
    {\
_ClearTabOpStates();\
    tabop_##headerName->Selected(true,false);\
    tabop_##headerName->SetBorderSize(ui::UiRectF(0, 0, 0, 2), false);\
    tabop_##headerName->SetFontId(L"system_bold_16");\
    }
    UISETTINGSPAGE_TABOP_LIST(UISETTINGSPAGE_ScroolChange);
#undef UISETTINGSPAGE_ScroolChange

    return true;
}

void SettingsPage::_ClearTabOpStates()
{
#define UISETTINGSPAGE_ClearTabOpStates(opName) \
    {\
    tabop_##opName->Selected(false,false);\
    tabop_##opName->SetBorderSize(ui::UiRectF(0, 0, 0, 0), false);\
    tabop_##opName->SetFontId(L"system_16");\
    }
    UISETTINGSPAGE_TABOP_LIST(UISETTINGSPAGE_ClearTabOpStates);
#undef UISETTINGSPAGE_ScroolChange
}

bool SettingsPage::OnSoundSliderValueChanged(const ui::EventArgs& msg)
{
    //音频只有一个滑块
    
    double value=slider_volume->GetValue();
    //100
    AppSettings::GetIns().SetVolumeValue(value/100.);
    return true;
}
