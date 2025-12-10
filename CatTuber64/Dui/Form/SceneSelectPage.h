#ifndef _SceneSelectPage_h
#define _SceneSelectPage_h





//这是UI用于选择的已保存的场景，不是场景中的某个物件
class UISceneItem :public ui::ListBoxItemV
{

public:
	UISceneItem(ui::Window* pWindow);
	void InitSubControls(const std::string& name, const std::string& img, size_t dataIndex,bool selected);

	std::string GetSceneName();
	class SceneItemProvider* GetProvider();

private:
	//控件菜单
	bool OnRightClick(const ui::EventArgs& args);
	//左键选择
	//bool OnLeftClick(const ui::EventArgs& args);

	//菜单选项
	bool OnLoadClick(const ui::EventArgs& args);
	bool OnRenameClick(const ui::EventArgs& args);
	bool OnRemoveClick(const ui::EventArgs& args);


	



	ui::Control* imgCover=nullptr;
	ui::Label* labelSceneName=nullptr;
	size_t index=MAXSIZE_T;
	bool isSelected = false;
};







class SceneItemProvider :public ui::VirtualListBoxElement
{
	friend class UISceneItem;
public:
	SceneItemProvider();
	~SceneItemProvider() = default;

	void LoadSceneList();



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
		RESULT_STATUS_RENAME_WRITE_FILE_FAILED
	};
	bool LoadScene(size_t index);
	RESULT_STATUS RenameScene(size_t index,std::string name);
	bool RemoveItem(size_t index);

	struct SceneInfo
	{
		std::string name;
		std::string imgPath;
		std::string filePath;
		bool selected = false;
	};
	const SceneInfo& GetSceneInfo(size_t nElementIndex);




private:



	std::vector<SceneInfo> sceneList;



};



class SceneSelectPage :public ui::VBox
{

public:
	SceneSelectPage(ui::Window* pWindow) :VBox(pWindow) {};
	void InitContents();


};









#endif