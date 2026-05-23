#include"DuiCommon.h"

#include"MainUiForm.h"
#include"UIPageBase.h"
#include"SettingsPage.h"
#include"RemoteLinkPage.h"
#include"SceneSelectPage.h"
#include"UIModelItemSelect_Page.h"
#include"UserControl/UIScenePanel.h"
#include"AppContext.h"

MainUiForm::MainUiForm()
{
}

MainUiForm::~MainUiForm()
{
}

void MainUiForm::OnInitWindow()
{
	__super::OnInitWindow();


	pageContainer= dynamic_cast<ui::TabBox*>(FindControl(L"box_workSpaceContainer"));


	//基础控件区
	auto baseControl_btn = dynamic_cast<ui::ButtonBox*>( FindControl(L"baseControl_btn_settings"));
	baseControl_btn->AttachClick(ui::UiBind(&MainUiForm::OnBaseControlBtnClicked, this, std::placeholders::_1));
	baseControl_btn = dynamic_cast<ui::ButtonBox*>( FindControl(L"baseControl_btn_help"));
	baseControl_btn->AttachClick(ui::UiBind(&MainUiForm::OnBaseControlBtnClicked, this, std::placeholders::_1));
	baseControl_btn = dynamic_cast<ui::ButtonBox*>( FindControl(L"baseControl_btn_remoteLink"));
	baseControl_btn->AttachClick(ui::UiBind(&MainUiForm::OnBaseControlBtnClicked, this, std::placeholders::_1));



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
	GoToPage(PAGE_SCENESELECT);




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


			if (!GetPage(pageVisitList[nextVisitIndex - 1])->IsValid())
			{
				return OnNavigationBtnClicked(msg);
			}
			pageContainer->SelectItem(GetPage(pageVisitList[nextVisitIndex - 1]));
			GetPage(pageVisitList[nextVisitIndex - 1])->OnEnterThisPage(UIPageBase::PageEnter_NavigationLeft);
			//GetPage(pageVisitList[nextVisitIndex - 1])->SetVisible(true);

			//auto childCount = pageContainer->GetItemCount();
			//for (size_t i = 0; i < childCount; i++)
			//{
			//	auto curItem = pageContainer->GetItemAt(i);
			//	curItem->SetVisible(false);
			//	if (curItem->GetName() == pageVisitList[nextVisitIndex-1])
			//	{
			//		curItem->SetVisible(true);
			//	}
			//}
		}


		_UpdateNavigateButton();
	}
	else if (msg.GetSender() == pBtnRight)
	{
		//向右
		if (nextVisitIndex< pageVisitList.size())
		{
			if (!GetPage(pageVisitList[nextVisitIndex])->IsValid())
			{
				nextVisitIndex++;
				return OnNavigationBtnClicked(msg);
			}

			pageContainer->SelectItem(GetPage(pageVisitList[nextVisitIndex]));
			GetPage(pageVisitList[nextVisitIndex])->OnEnterThisPage(UIPageBase::PageEnter_NavigationRight);

			//auto childCount = pageContainer->GetItemCount();
			//for (size_t i = 0; i < childCount; i++)
			//{
			//	auto curItem = pageContainer->GetItemAt(i);
			//	curItem->SetVisible(false);
			//	if (curItem->GetName() == pageVisitList[nextVisitIndex])
			//	{
			//		curItem->SetVisible(true);
			//	}
			//}

			nextVisitIndex++;
		}


		_UpdateNavigateButton();
	}


	return true;
}

bool MainUiForm::OnBaseControlBtnClicked(const ui::EventArgs& msg)
{
	std::wstring senderName = msg.GetSender()->GetName();

	if (senderName==L"baseControl_btn_settings")
	{
		GoToPage(PAGE_SETTINGS);
	}

	else if (senderName==L"baseControl_btn_remoteLink")
	{
		GoToPage(PAGE_REMOTELINK);
	}
	else if (senderName == L"baseControl_btn_help")
	{
		//todo 完善语言系统，当前获取的语言是unspecified

		//打开软件的帮助页面
		//构建链接
		std::string appBasePath=AppContext::GetAppBasePath();
		//获取当前语言链接
		if (0 == strcmp("schinese", AppContext::GetAppLang()))
		{
			appBasePath += "Docs/schinese/index.html";
		}
		else
		{
			appBasePath += "Docs/english/index.html";
		}
		SDL_OpenURL(appBasePath.c_str());


	}



	return true;
}

//void MainUiForm::GoToPage(const std::wstring& pageName)
void MainUiForm::GoToPage(PageEnum pageType, uintptr_t userData1, uintptr_t userData2)
{
	//先从pagecontainer里检索

	if (pageVisitList.empty() == false && pageType == pageVisitList[nextVisitIndex - 1])
	{
		//需要前往的页面正好是当前页
		//不需要套后续动作的窗口类型
		switch (pageType)
		{
		case MainUiForm::PAGE_SCENESELECT:
		case MainUiForm::PAGE_SETTINGS:
		case MainUiForm::PAGE_REMOTELINK:
		case MainUiForm::PAGE_CLASSIC_CHARACTER_SELECT:
		case MainUiForm::PAGE_CLASSIC_DESK_SELECT:
		case MainUiForm::PAGE_CLASSIC_HANDHELDITEM_SELECT:
		case MainUiForm::PAGE_BONGOCAT_SELECT:
		case MainUiForm::PAGE_DECORATIONITEM_SELECT:
			GetPage(pageType)->OnEnterThisPage(UIPageBase::PageEnter_New);
			return;
		default:
			assert(false);
			break;
		}


	};

	//if(nextVisitIndex - 1>=0)
	//	GetPage( pageVisitList[nextVisitIndex - 1])->SetVisible(false);

	pageVisitList.resize(nextVisitIndex);
	pageVisitList.push_back(pageType);
	nextVisitIndex++;
	GetPage(pageType)->InitContents(userData1, userData2);
	pageContainer->SelectItem(GetPage(pageType));
	GetPage(pageType)->OnEnterThisPage(UIPageBase::PageEnter_New);
	//GetPage(pageType)->SetVisible(true);


	_UpdateNavigateButton();

}

//ui::Box* MainUiForm::BuildPage(const std::wstring& pageName)
UIPageBase* MainUiForm::GetPage(PageEnum pageName)
{
	if (pageArray[pageName])
		return pageArray[pageName];


	switch (pageName)
	{
	case MainUiForm::PAGE_SCENESELECT:
		pageArray[pageName]= new SceneSelectPage(this);
		break;
	case MainUiForm::PAGE_SETTINGS:
		pageArray[pageName] = new SettingsPage(this);
		break;
	case MainUiForm::PAGE_REMOTELINK:
		pageArray[pageName] = new RemoteLinkPage(this);
		break;
	case MainUiForm::PAGE_CLASSIC_CHARACTER_SELECT:
		pageArray[pageName]  = new UIModelItemSelect_Page(this, UIModelItemType_ClassicCharacter);
		break;
	case MainUiForm::PAGE_CLASSIC_DESK_SELECT:
		pageArray[pageName] = new UIModelItemSelect_Page(this, UIModelItemType_ClassicDesk);
		break;
	case MainUiForm::PAGE_CLASSIC_HANDHELDITEM_SELECT:
		pageArray[pageName] = new UIModelItemSelect_Page(this, UIModelItemType_ClassicHandheldItem);
		break;
	case MainUiForm::PAGE_BONGOCAT_SELECT:
		pageArray[pageName] = new UIModelItemSelect_Page(this, UIModelItemType_BongoCat);
		break;
	case MainUiForm::PAGE_DECORATIONITEM_SELECT:
		pageArray[pageName] = new UIModelItemSelect_Page(this, UIModelItemType_DecorationItem);
		break;
	default:
		assert(false);
		break;
	}

	pageContainer->AddItem(pageArray[pageName]);
	return pageArray[pageName];
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
