#ifndef _MainUiForm_h
#define _MainUiForm_h

//UI



class MainUiForm:public ui::WindowImplBase
{
public:
	MainUiForm();
	virtual ~MainUiForm();

	virtual std::wstring GetSkinFolder() override { return L"CatTuber_default"; };
	virtual std::wstring GetSkinFile() override { return L"MainUiForm.xml"; };

	virtual void OnInitWindow()override;



	bool OnLanguageBtnClicked(ui::EventArgs* msg);
	bool OnLanguageMenuBtnClicked(ui::EventArgs* msg);
	bool OnNavigationBtnClicked(const ui::EventArgs& msg);
	bool OnBaseControlBtnClicked(const ui::EventArgs& msg);


	enum PageEnum
	{
		PAGE_SCENESELECT,
		PAGE_SETTINGS,
		PAGE_CLASSIC_CHARACTER_SELECT,
		PAGE_CLASSIC_DESK_SELECT,
		PAGE_CLASSIC_HANDHELDITEM_SELECT,
		PAGE_BONGOCAT_SELECT,
		PAGE_DECORATIONITEM_SELECT,
		PAGE_MAXCOUNT
	};

	void GoToPage(PageEnum pageType,uintptr_t userData1=0, uintptr_t userData2=0);
	class UIPageBase* GetPage(PageEnum pageType);

	//void GoToPage(const std::wstring& pageName);
	//ui::Box* BuildPage(const std::wstring& pageName);

	class UIScenePanel* GetScenePanel() { return scenePanel; };
private:
	//更新按钮的enable属性
	void _UpdateNavigateButton();

	ui::TabBox* pageContainer=nullptr;
	ui::Box* secenMainItemPanel = nullptr;
	class UIScenePanel* scenePanel = nullptr;

	ui::Box* pageSettings = NULL;
	
	//导航按钮
	ui::Button* pBtnLeft = nullptr;
	ui::Button* pBtnRight = nullptr;

	size_t nextVisitIndex = 0;//指向end()
	//std::vector<std::wstring> pageVisitList;//用于回撤
	std::vector<PageEnum> pageVisitList;//用于回撤


	class UIPageBase* pageArray[PAGE_MAXCOUNT] = {0};

};
























#endif