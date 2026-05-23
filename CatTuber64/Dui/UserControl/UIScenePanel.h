#ifndef _UIScenePanel_h
#define _UIScenePanel_h

//UI主页左侧面板中的Scene显示区域

//场景面板从上到下分为3个部分
//第一部分为当前场景名
//第二部分为mainItem
//第三部分为item列表



//class UISceneContentListItem :public ui::ListBoxItem
class UISceneContentListItem :public ui::ControlDragableT<ui::ListBoxItem>
{


public:
	UISceneContentListItem(class UIScenePanel* pParent);
	void InitSubControls( const std::wstring& img1, const std::wstring& img2, const std::wstring& img3, size_t dataIndex);

	size_t GetItemIndex() { return index; };

	class UISceneItemListProvider* GetProvider();
	//void UpdateUI();
	virtual void PaintBkImage(ui::IRender* pRender)override;


private:
	//控件菜单
	bool OnRightClick(const ui::EventArgs& args);
	////左键选择,进入设置页面，刷新按钮
	bool OnLeftClick(const ui::EventArgs& args);

	////菜单选项
	//bool OnSelectItem(const ui::EventArgs& args);
	//bool OnDeselectItem(const ui::EventArgs& args);
	//bool OnFavoriteClick(const ui::EventArgs& args);
	//bool OnUnfavoriteClick(const ui::EventArgs& args);

	//控件完成顺序替换
	virtual void OnItemOrdersChanged(size_t nOldItemIndex, size_t nNewItemIndex)override;


	std::unique_ptr<ui::Image>img1;
	std::unique_ptr<ui::Image>img2;
	std::unique_ptr<ui::Image>img3;




	size_t index = MAXSIZE_T;
	UIScenePanel* pParent;

};













class UISceneItemListProvider :public ui::VirtualListBoxElement
{

	friend class UIMSceneListItem;
public:
	UISceneItemListProvider(class UIScenePanel* pParent):pParent(pParent){};
	~UISceneItemListProvider() = default;




	void LoadItemList(int windowIndex);



	virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;
	virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;
	virtual size_t GetElementCount() const override;
	virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;
	virtual bool IsElementSelected(size_t nElementIndex) const override;
	virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;
	virtual bool IsMultiSelect() const override { return false; }
	virtual void SetMultiSelect(bool bMultiSelect) override { /* do nothing */ }


	




	struct ItemInfo
	{
		//std::string imgPath;
		std::wstring imgPath1;
		std::wstring imgPath2;
		std::wstring imgPath3;
		bool selected = false;
		class ISceneItem* item = nullptr;
	};
	ItemInfo& GetItemInfo(size_t nElementIndex);


	//物品排序
	void ItemOrderChange(size_t nOldItemIndex, size_t nNewItemIndex);
	void ItemMoveUp(size_t nItemIndex);
	void ItemMoveDown(size_t nItemIndex);
	void ItemMoveTop(size_t nItemIndex);
	void ItemMoveBottom(size_t nItemIndex);
	void RemoveItem(size_t nItemIndex);


	//用于UI更新
	void SetListItemImg(size_t nItemIndex, const char* type,const char* packPath, const char* packPath2=nullptr, const char* packPath3=nullptr);


private:


	std::vector<ItemInfo> itemList;

	class UIScenePanel* pParent;
	class RenderWindowController* targetWindow = nullptr;


};















class UIScenePanel :public ui::VBox
{

	friend class UIModelItemProvider;
public:

	UIScenePanel(class MainUiForm* pParent);
	~UIScenePanel();

	void InitContents();


	void UpdatePanelButtonEnable(/*UISceneContentListItem* pChild*/);
	ui::VirtualVListBox* GetSceneItemListBox() { return containers[currentWindowIndex]; };
private:

	bool OnSceneTitleClicked(const ui::EventArgs& msg);
	bool OnMainItemSlotClicked(const ui::EventArgs& msg);
	bool OnSceneContentListControlButtonClicked(const ui::EventArgs& msg);
	bool OnSceneItemAddMenuClicked(const ui::EventArgs& msg);




	class MainUiForm* pParent;
	ui::VirtualVListBox* containers[4] = {};
	//UISceneContentListItem* panelFocusedButton;





	//class UISceneItemListProvider* provider=nullptr;
	 //provider=nullptr;
	 std::vector<std::unique_ptr<UISceneItemListProvider>> providers;
	 int currentWindowIndex = 0;



	//控件
	ui::Button* btnSceneTitle = nullptr;


	//经典面板涉及的控件
	ui::VBox* scenePanel_classic = nullptr;
	ui::Button* classicPanel_slotCharacter = nullptr;
	ui::Button* classicPanel_slotDesk = nullptr;
	ui::Button* classicPanel_slotHandheldItem = nullptr;

	//BongoCat面板涉及的控件
	ui::VBox* scenePanel_bongoCat = nullptr;
	ui::Button* bongoCatPanel_slot = nullptr;

	//场景内容列表控件
	ui::Button* scenePanel_btnItemAdd = nullptr;
	ui::Button* scenePanel_btnItemRemove = nullptr;
	ui::Button* scenePanel_btnItemMoveUp = nullptr;
	ui::Button* scenePanel_btnItemMoveDown = nullptr;






};

















#endif