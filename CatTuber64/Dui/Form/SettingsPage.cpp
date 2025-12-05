#include"Dui.h"
#include"SettingsPage.h"



void SettingsPage::InitContents()
{

    //auto Box = ui::GlobalManager::Instance().CreateBox(ui::FilePath(L"CatTuber_default/SettingsPage.xml"));
    ui::Box* Box=NULL;

    {
        ui::WindowBuilder builder;
        if (builder.ParseXmlFile(ui::FilePath(L"CatTuber_default/SettingsPage.xml"))) {
            Control* pControl = builder.CreateControls(ui::CreateControlCallback(),GetWindow());
            ASSERT(pControl != nullptr);
            Box= builder.ToBox(pControl);
        }
    }
    ASSERT(Box&&"Can not be nullptr.");

	this->AddItem(Box);

	this->SetName(L"SETTINGS_PAGE");





}
