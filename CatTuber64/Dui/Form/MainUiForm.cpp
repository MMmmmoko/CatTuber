#include"DuiCommon.h"


#include"MainUiForm.h"
#include"SettingsPage.h"
#include"SceneSelectPage.h"
MainUiForm::MainUiForm()
{
}

MainUiForm::~MainUiForm()
{
}

void MainUiForm::OnInitWindow()
{
	__super::OnInitWindow();


	pageContainer= dynamic_cast<ui::Box*>(FindControl(L"box_workSpaceContainer"));

	//总是以场景选择页开始
	GoToPage(L"SCENESELECT_PAGE");


	//基础控件区
	auto baseControl_btn_settings = dynamic_cast<ui::ButtonBox*>( FindControl(L"baseControl_btn_settings"));
	if (baseControl_btn_settings)
	{
		baseControl_btn_settings->AttachClick([this](const ui::EventArgs&) ->bool{
			GoToPage(L"SETTINGS_PAGE");
			return true;
			});
	}
}

bool MainUiForm::OnLanguageBtnClicked(ui::EventArgs* msg)
{
	return false;
}

bool MainUiForm::OnLanguageMenuBtnClicked(ui::EventArgs* msg)
{
	return false;
}

void MainUiForm::GoToPage(const std::wstring& pageName)
{
	//先从pagecontainer里检索

	if(pageVisitList.empty()==false&& pageName == pageVisitList.back())return;





	auto childCount = pageContainer->GetItemCount();

	bool hasTarget = false;
	for (size_t i = 0; i < childCount; i++)
	{
		auto curItem=pageContainer->GetItemAt(i);
		curItem->SetVisible(false);
		if (curItem->GetName() == pageName)
		{
			hasTarget = true;
			curItem->SetVisible(true);
			pageVisitList.push_back(pageName);
		}
		
	}

	if (hasTarget)return;


	//没有找到目标
	auto page = BuildPage(pageName);
	if (page)
	{
		pageContainer->AddItem(page);
		pageVisitList.push_back(pageName);
	}



}

ui::Box* MainUiForm::BuildPage(const std::wstring& pageName)
{
	if (pageName == L"SCENESELECT_PAGE")
	{
		auto page = new SceneSelectPage(this);
		page->InitContents();
		return page;
	}
	if (pageName == L"SETTINGS_PAGE")
	{
		auto page = new SettingsPage(this);
		page->InitContents();
		return page;
	}

	return nullptr;
}
