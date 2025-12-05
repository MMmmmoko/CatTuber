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

	void GoToPage(const std::wstring& pageName);
	ui::Box* BuildPage(const std::wstring& pageName);
private:
	ui::Box* pageContainer;


	ui::Box* pageSettings = NULL;
	
	std::vector<std::wstring> pageVisitList;//用于回撤
};
























#endif