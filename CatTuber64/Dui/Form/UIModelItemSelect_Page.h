#ifndef _Classic_CharacterSelect_Page_h
#define _Classic_CharacterSelect_Page_h



//这是UI用于选择的已保存的场景，不是场景中的某个物件


enum UIModelItemType
{
	UIModelItemType_Empty,
	UIModelItemType_ClassicCharacter,
	UIModelItemType_ClassicTable,
	UIModelItemType_ClassicHandheldItem,

	UIModelItemType_BongoCat,//BongoCatMver



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
	bool OnSelectItem(const ui::EventArgs& args);
	bool OnDeselectItem(const ui::EventArgs& args);
	bool OnFavoriteClick(const ui::EventArgs& args);
	bool OnUnfavoriteClick(const ui::EventArgs& args);


	



	ui::Control* imgCover=nullptr;
	ui::Control* imgFavoriteIcon =nullptr;
	ui::Label* labelSceneName=nullptr;
	size_t index=MAXSIZE_T;
	bool isSelected = false;
	bool isFavorite = false;
	bool isEmptyItem = false;//是否是一个用于卸下物体的空物体
};







class UIModelItemProvider :public ui::VirtualListBoxElement
{
	friend class UIModelItem;
public:
	UIModelItemProvider(UIModelItemType itemType);
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
	bool SetSelect(size_t index);
	bool SetFavorite(size_t index,bool bFavorite=true);
	
	


	//这里传入的是Scene文件夹里的图片路径，即调用前已经在指定位置生成好了新的封面
	void OnCoverSetted(size_t index,const char* imageFileInSceneFolder);



	struct ItemInfo
	{
		std::string name;
		std::string uploader;
		std::string imgPath;//能UI识别的imgpath
		std::string description;
		std::string filePath;
		bool selected = false;
		bool favorite = false;
		bool emptyItem = false;
	};
	ItemInfo& GetItemInfo(size_t nElementIndex);




private:
	UIModelItemType itemType;






	std::vector<ItemInfo> itemList;



};



class UIModelItemSelect_Page :public ui::VBox
{

public:

	UIModelItemSelect_Page(ui::Window* pWindow, UIModelItemType itemType);
	void InitContents();

private:
	bool OnBtnClicked(const ui::EventArgs& args);



	UIModelItemType itemType;
	class UIModelItemProvider* provider=nullptr;
};









#endif