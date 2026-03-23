#ifndef _UISceneMainItemSlot_Classic_h
#define _UISceneMainItemSlot_Classic_h




//UI主界面左侧主物件插槽



class UISceneMainItemSlot_Classic :public ui::VBox
{
public:
	UISceneMainItemSlot_Classic(ui::Window* pWindow) :VBox(pWindow) {};
	void InitControls(class MainUiForm* pParent, class ClassicItem* pClassicItem);





private:

	bool OnSlotOptionClicked(const ui::EventArgs& pEventArgs);





	class ClassicItem* pClassicItem = nullptr;
	class MainUiForm* mainForm = nullptr;
	ui::Option* slot_Character;
	ui::Option* slot_Desk;
	ui::Option* slot_HandheldItem;
};


















#endif