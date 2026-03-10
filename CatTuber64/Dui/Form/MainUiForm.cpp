#include"DuiCommon.h"


#include"MainUiForm.h"
#include"SettingsPage.h"
#include"SceneSelectPage.h"
#include"UIModelItemSelect_Page.h"
#include"UserControl/UIScenePanel.h"
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


	//基础控件区
	auto baseControl_btn_settings = dynamic_cast<ui::ButtonBox*>( FindControl(L"baseControl_btn_settings"));
	if (baseControl_btn_settings)
	{
		baseControl_btn_settings->AttachClick([this](const ui::EventArgs&) ->bool{
			GoToPage(L"SETTINGS_PAGE");
			return true;
			});
	}


	//导航按钮
	pBtnLeft= dynamic_cast<ui::Button*>(FindControl(L"btn_pageBack"));
	pBtnRight= dynamic_cast<ui::Button*>(FindControl(L"btn_pageFront"));
	pBtnLeft->AttachClick(ui::UiBind(&MainUiForm::OnNavigationBtnClicked, this, std::placeholders::_1));
	pBtnRight->AttachClick(ui::UiBind(&MainUiForm::OnNavigationBtnClicked, this, std::placeholders::_1));





	//场景区域
	ui::Box* box_scenePanelContainer= dynamic_cast<ui::Box*>(FindControl(L"box_scenePanelContainer"));
	scenePanel = new UIScenePanel(this);
	scenePanel->InitContents();
	box_scenePanelContainer->AddItem(scenePanel);








	//总是以场景选择页开始
	GoToPage(L"SCENESELECT_PAGE");




	//初始化主页数据，当前场景










}

bool MainUiForm::OnLanguageBtnClicked(ui::EventArgs* msg)
{
	return false;
}

bool MainUiForm::OnLanguageMenuBtnClicked(ui::EventArgs* msg)
{
	return false;
}

bool MainUiForm::OnNavigationBtnClicked(const ui::EventArgs& msg)
{
	if (msg.GetSender() == pBtnLeft)
	{
		//返回
		if (nextVisitIndex >= 2)
		{
			nextVisitIndex--;

			auto childCount = pageContainer->GetItemCount();
			for (size_t i = 0; i < childCount; i++)
			{
				auto curItem = pageContainer->GetItemAt(i);
				curItem->SetVisible(false);
				if (curItem->GetName() == pageVisitList[nextVisitIndex-1])
				{
					curItem->SetVisible(true);
				}
			}
		}


		_UpdateNavigateButton();
	}
	else if (msg.GetSender() == pBtnRight)
	{
		//向右
		if (nextVisitIndex< pageVisitList.size())
		{
			auto childCount = pageContainer->GetItemCount();
			for (size_t i = 0; i < childCount; i++)
			{
				auto curItem = pageContainer->GetItemAt(i);
				curItem->SetVisible(false);
				if (curItem->GetName() == pageVisitList[nextVisitIndex])
				{
					curItem->SetVisible(true);
				}
			}

			nextVisitIndex++;
		}


		_UpdateNavigateButton();
	}


	return true;
}

void MainUiForm::GoToPage(const std::wstring& pageName)
{
	//先从pagecontainer里检索

	if(pageVisitList.empty()==false&& pageName == pageVisitList[nextVisitIndex-1])return;





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

			pageVisitList.resize(nextVisitIndex);
			pageVisitList.push_back(pageName);
			nextVisitIndex++;




			//根据名称进行特殊处理？
			{
			
			
			
			
			}



			_UpdateNavigateButton();
		}
		
	}

	if (hasTarget)return;


	//没有找到目标
	auto page = BuildPage(pageName);
	if (page)
	{
		pageContainer->AddItem(page);

		pageVisitList.resize(nextVisitIndex);
		pageVisitList.push_back(pageName);
		nextVisitIndex++;



		_UpdateNavigateButton();
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
	else if (pageName == L"SETTINGS_PAGE")
	{
		auto page = new SettingsPage(this);
		page->InitContents();
		return page;
	}








	else if (pageName == L"CLASSIC_CHARACTER_SELECT_PAGE")
	{
		auto page = new UIModelItemSelect_Page(this, UIModelItemType_ClassicCharacter);
		page->InitContents();
		return page;
	}
	else if (pageName == L"CLASSIC_TABLE_SELECT_PAGE")
	{
		auto page = new UIModelItemSelect_Page(this, UIModelItemType_ClassicTable);
		page->InitContents();
		return page;
	}
	else if (pageName == L"CLASSIC_HANDHELDITEM_SELECT_PAGE")
	{
		auto page = new UIModelItemSelect_Page(this, UIModelItemType_ClassicHandheldItem);
		page->InitContents();
		return page;
	}
	else if (pageName == L"BONGOCAT_SELECT_PAGE")
	{
		auto page = new UIModelItemSelect_Page(this, UIModelItemType_BongoCat);
		page->InitContents();
		return page;
	}








	return nullptr;
}

void MainUiForm::_UpdateNavigateButton()
{
	if (nextVisitIndex >= 2)
	{
		pBtnLeft->SetEnabled(true);
	}
	else
	{
		pBtnLeft->SetEnabled(false);
	}
	if (nextVisitIndex < pageVisitList.size())
	{
		pBtnRight->SetEnabled(true);
	}
	else
	{
		pBtnRight->SetEnabled(false);
	}


}
