#ifndef _Classic_CharacterSelect_Page_h
#define _Classic_CharacterSelect_Page_h



//这是UI用于选择的已保存的场景，不是场景中的某个物件
#include"UIPageBase.h"

enum UIModelItemType
{
	UIModelItemType_Empty,
	UIModelItemType_ClassicCharacter,
	UIModelItemType_ClassicDesk,
	UIModelItemType_ClassicHandheldItem,


	UIModelItemType_BongoCat,//BongoCatMver
	UIModelItemType_DecorationItem,



	UIModelItemType_Count
};



class UIModelItem :public ui::ListBoxItemV
{

public:
	UIModelItem(ui::Window* pWindow);
	//isEmptyItem 这个item是否是一个用于卸下物体的空物体
	void InitSubControls(const std::string& name, const std::string& img, size_t dataIndex, UIModelItemType itemType,bool selected,bool isFavorate,bool isEmptyItem=false);

	size_t GetItemIndex() { return index; };

	class UIModelItemProvider* GetProvider();
	void UpdateUI();
private:
	//控件菜单
	bool OnRightClick(const ui::EventArgs& args);
	//左键选择
	//bool OnLeftClick(const ui::EventArgs& args);

	//菜单选项
	bool OnMenuSelectItem(const ui::EventArgs& args);
	bool OnMenuDeselectItem(const ui::EventArgs& args);
	bool OnFavoriteClick(const ui::EventArgs& args);
	bool OnUnfavoriteClick(const ui::EventArgs& args);


	



	ui::Control* imgCover=nullptr;
	ui::Control* imgFavoriteIcon =nullptr;
	ui::Label* labelItenName=nullptr;
	size_t index=MAXSIZE_T;
	UIModelItemType itemType;
	bool isSelected = false;
	bool isFavorite = false;
	bool isEmptyItem = false;//是否是一个用于卸下物体的空物体
};







class UIModelItemProvider :public ui::VirtualListBoxElement
{
	friend class UIModelItem;
	friend class UIModelItemSelect_Page;
public:
	UIModelItemProvider(UIModelItemType itemType,class UIModelItemSelect_Page* parentPage);
	~UIModelItemProvider() = default;




	void LoadItemList();



	virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;
	virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;
	virtual size_t GetElementCount() const override;
	virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;
	virtual bool IsElementSelected(size_t nElementIndex) const override;
	virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;
	virtual bool IsMultiSelect() const override { return false; }
	virtual void SetMultiSelect(bool bMultiSelect) override { /* do nothing */ }

	

	enum RESULT_STATUS
	{
		RESULT_STATUS_DONE,
		RESULT_STATUS_INDEX_OUT_OF_RANGE,

		RESULT_STATUS_RENAME_EMPTY_NAME,
		RESULT_STATUS_RENAME_DUPLICATE_SCENE_NAME,
		RESULT_STATUS_RENAME_WRITE_FILE_FAILED,

		RESULT_STATUS_CREATE_EMPTY_NAME,
		RESULT_STATUS_CREATE_DUPLICATE_SCENE_NAME,
		RESULT_STATUS_CREATE_WRITE_FILE_FAILED
	};


	//0表示deselect
	bool OnSetSelect(size_t index);
	void OnSetDeselect(size_t index);
	bool SetFavorite(size_t index,bool bFavorite=true);
	
	




	struct ItemInfo
	{
		std::string name;
		std::string uploader;
		std::string imgPath;//能UI识别的imgpath
		std::string description;
		std::string filePath;
		SDL_Time itemOrderTime;//本地文件为创建时间，workshop文件为订阅时间
		SDL_Time addFavoriteTime;
		bool selected = false;
		bool favorite = false;
		bool emptyItem = false;
	};
	ItemInfo& GetItemInfo(size_t nElementIndex);




private:
	void Resort();


	UIModelItemType itemType;
	bool enableEmptyItem = false;





	std::vector<ItemInfo> itemList;
	std::vector<uint16_t> itemView;
	//std::vector<std::shared_ptr<ui::ImageInfo>> imageCache;

	std::string curSelectItemPath;

	UIModelItemSelect_Page* _parentPage = nullptr;
};



class UIModelItemSelect_Page :public UIPageBase
{
	friend class UIModelItemProvider;
public:

	UIModelItemSelect_Page(ui::Window* pWindow, UIModelItemType itemType);
	~UIModelItemSelect_Page();
	virtual void InitContents(uintptr_t userdata1, uintptr_t userdata2)override;
	virtual MainUiForm::PageEnum GetPageType()override;

	//virtual void OnSetVisible(bool bChanged)override;
	

private:
	struct DecorationPageStates
	{
		//ui::Control* targetItemUIListItem;
		class DecorationItem* targetItem;
	};

	union PageStates
	{
		DecorationPageStates decorationPageStates;
	}pageStates;

	//bool OnBtnClicked(const ui::EventArgs& args);
	
	//virtual void OnSetVisible(bool bChanged)override;
	virtual void OnEnterThisPage(PageEnterFlag enterFlag) override;

	UIModelItemType itemType;
	class UIModelItemProvider* provider=nullptr;


};









#endif