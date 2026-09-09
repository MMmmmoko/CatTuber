#ifndef _SettingsPage_input_h
#define _SettingsPage_input_h



//为了保持整洁  不在一个文件中塞太多代码，这里将设置页的每个模块拆分成不同文件

class SettingsPage_input
{

public:
	void InitContents(class SettingsPage* parent);




private:
	bool OnMouseControlSelected(const ui::EventArgs& msg);
	bool OnMouseSpeedValueChanged(const ui::EventArgs& msg);
	bool OnMouseAreaClicked(const ui::EventArgs& msg);
	bool OnMouseAreaMenuItemClicked(const ui::EventArgs& msg);

	bool OnDeadZondeSliderValueChanged(const ui::EventArgs& msg);




	SettingsPage* parent=nullptr;

	

	ui::Option* op_inputmode_move;
	ui::Option* op_inputmode_coordinate;
	
	ui::Box* box_inputmode_move;
	ui::Box* box_inputmode_coordinate;


	ui::Slider* slider_mouseSpeed;
	ui::Label* text_mouseSpeed;
	ui::Button* btn_mouseArea;
	ui::CheckBox* cb_mouseInvertX;


	ui::Slider* slider_deadzone_stick;
	ui::Slider* slider_deadzone_trigger;
	ui::Slider* slider_deadzone_joystick;
	ui::Label* text_deadzone_stick;
	ui::Label* text_deadzone_trigger;
	ui::Label* text_deadzone_joystick;

};
















#endif