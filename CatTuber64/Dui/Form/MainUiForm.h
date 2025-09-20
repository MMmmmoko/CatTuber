#ifndef _MainUiForm_h
#define _MainUiForm_h

//UIÖ÷´°Ìå



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

};
























#endif