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

	void GoToPage(const std::wstring& pageName);
	ui::Box* BuildPage(const std::wstring& pageName);
private:
	//更新按钮的enable属性
	void _UpdateNavigateButton();

	ui::Box* pageContainer=nullptr;
	ui::Box* secenMainItemPanel = nullptr;
	class UIScenePanel* scenePanel = nullptr;

	ui::Box* pageSettings = NULL;
	
	//导航按钮
	ui::Button* pBtnLeft = nullptr;
	ui::Button* pBtnRight = nullptr;

	size_t nextVisitIndex = 0;//指向end()
	std::vector<std::wstring> pageVisitList;//用于回撤
};
























#endif