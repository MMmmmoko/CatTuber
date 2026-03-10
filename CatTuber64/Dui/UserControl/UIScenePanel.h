#ifndef _UIScenePanel_h
#define _UIScenePanel_h

//UI主页左侧面板中的Scene显示区域

//场景面板从上到下分为3个部分
//第一部分为当前场景名
//第二部分为mainItem
//第三部分为item列表


class UIScenePanel :public ui::VBox
{

public:

	UIScenePanel(class MainUiForm* pParent);


	void InitContents();




private:

	bool OnSceneTitleClicked(const ui::EventArgs& msg);
	bool OnMainItemSlotClicked(const ui::EventArgs& msg);




	class MainUiForm* pParent;





	//控件
	ui::Button* btnSceneTitle = nullptr;


	//经典面板涉及的控件
	ui::VBox* scenePanel_classic = nullptr;
	ui::Button* classicPanel_slotCharacter = nullptr;
	ui::Button* classicPanel_slotTable = nullptr;
	ui::Button* classicPanel_slotHandheldItem = nullptr;

	//BongoCat面板涉及的控件
	ui::VBox* scenePanel_bongoCat = nullptr;
	ui::Button* bongoCatPanel_slot = nullptr;





};

















#endif