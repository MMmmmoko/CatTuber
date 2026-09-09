#include"Dui.h"
#include"SettingsPage.h"
#include"Input/InputParser.h"

static inline double MouseSpeedToSliderValue(double mouseSpeed)
{
	//滑块的范围为0~10000//对应鼠标速度0.1~10
	//速度采用指数映射
	//实际假定滑块范围为-1~+1，左端为0.1 右端为10，0点为1
	//以f(x)=a^x解a=10

	double midVal = SDL_log10(mouseSpeed);
	return (midVal + 1) * 5000;
}

static inline double SliderValueToMouseSpeed(double sliderValue)
{
	double midVal = (sliderValue / 5000)-1;
	return SDL_pow(10, midVal);
}


static inline double DeadZoneToSliderValue(double deadZone)
{
	//滑块的范围为0~10000//对应死区0~1
	//死区采用指数映射
	//实际假定滑块范围为-1~+1，左端为0 右端为1，0点为0.1
	//以f(x)=k*a^x+C解a=9 k=0.9/8  (0.1125)   C=-0.1/8 (-0.0125)
	//即f(x)=0.1125*9^x-0.0125

	double midVal = SDL_log((deadZone+0.0125)/0.1125)/SDL_log(9);
	return (midVal + 1) * 5000;
}

static inline double SliderValueToDeadZone(double sliderValue)
{
	double midVal = (sliderValue / 5000)-1;
	double resultVar = SDL_pow(9, midVal) * 0.1125 - 0.0125;
	//因为死区中有合法的整数0和1，在精度范围里进行抹除
	constexpr double ignore = 0.0001;
	if (resultVar < ignore)resultVar = 0;
	if (resultVar > 1 - ignore)resultVar = 1;
	return resultVar;
}


static inline std::string DeadZoneValuePrint(double value)
{
	char buf[20] = {};

	value=SDL_clamp(value,0,1);
		SDL_snprintf(buf, sizeof(buf), "%.5f", value * 100);
		//if (buf[3] == '.')
		//{
		//	buf[3] = '%';
		//	buf[4] = 0;
		//}
		//else
		//{
		//	buf[4] = '%';
		//	buf[5] = 0;
		//}

		{
		//从索引第4字节往前删0
		
			int endPos = 3;//结束后第x字节为0
			for (int i = 3; i > 0; i--)
			{
				if (buf[i] == '0')
				{
					buf[i] = 0;
					continue;
				}
				if (buf[i] == '.')
				{
					buf[i] = 0;
					endPos = i;
					break;
				}
				endPos = i + 1;
				break;
			}

			buf[endPos] = '%';
			buf[endPos+1] = 0;
		}


		return buf;
}


void SettingsPage_input::InitContents(class SettingsPage* parent)
{
	//鼠标
	{

		op_inputmode_move = (ui::Option*)parent->FindSubControl(L"op_inputmode_move");
		op_inputmode_coordinate = (ui::Option*)parent->FindSubControl(L"op_inputmode_coordinate");

		box_inputmode_move = (ui::HBox*)parent->FindSubControl(L"box_inputmode_move");
		box_inputmode_coordinate = (ui::HBox*)parent->FindSubControl(L"box_inputmode_coordinate");



		slider_mouseSpeed = (ui::Slider*)parent->FindSubControl(L"slider_mouseSpeed");
		text_mouseSpeed = (ui::Label*)parent->FindSubControl(L"text_mouseSpeed");
		btn_mouseArea = (ui::Button*)parent->FindSubControl(L"btn_mouseArea");
		cb_mouseInvertX = (ui::CheckBox*)parent->FindSubControl(L"cb_mouseInvertX");



		double mouseSpeed = AppSettings::GetIns().GetMouseSpeed();
		slider_mouseSpeed->SetValue(MouseSpeedToSliderValue(mouseSpeed));


		char mouseSpeedStr[12] = {};
		SDL_snprintf(mouseSpeedStr, sizeof(mouseSpeedStr), "x%.2f", mouseSpeed);
		text_mouseSpeed->SetUTF8Text(mouseSpeedStr);


		int targetIndex = 0;
		SDL_DisplayID curdisplay= InputParser::StrToDisplay(AppSettings::GetIns().GetMouseInputArea(),&targetIndex);

		if (0 == curdisplay)
		{
			btn_mouseArea->SetText(L"");
			btn_mouseArea->SetTextId(L"STRID_SETTINGS_INPUT_MOUSE_AREA_ALLDISPLAYS");
		}
		else
		{

			btn_mouseArea->SetText(GETDUISTRING(L"STRID_SETTINGS_INPUT_MOUSE_AREA_DISPLAY") + std::to_wstring(targetIndex) + L":  "
				+ ui::StringConvert::UTF8ToWString(SDL_GetDisplayName(curdisplay)));
			btn_mouseArea->SetTextId(L"");
		}
		cb_mouseInvertX->Selected(AppSettings::GetIns().GetMouseInvertX());






		//挂接函数
		//挂接函数
		//挂接函数
		op_inputmode_move->AttachSelect(std::bind(&SettingsPage_input::OnMouseControlSelected, this, std::placeholders::_1));
		op_inputmode_coordinate->AttachSelect(std::bind(&SettingsPage_input::OnMouseControlSelected, this, std::placeholders::_1));
		
		slider_mouseSpeed->AttachValueChanged(std::bind(&SettingsPage_input::OnMouseSpeedValueChanged, this, std::placeholders::_1));
		btn_mouseArea->AttachClick(std::bind(&SettingsPage_input::OnMouseAreaClicked, this, std::placeholders::_1));
		cb_mouseInvertX->AttachSelect(std::bind(&SettingsPage_input::OnMouseControlSelected, this, std::placeholders::_1));
		cb_mouseInvertX->AttachUnSelect(std::bind(&SettingsPage_input::OnMouseControlSelected, this, std::placeholders::_1));





		if (AppSettings::GetIns().GetMouseRelativeMove())
			op_inputmode_move->Selected(true,true);
		else
			op_inputmode_coordinate->Selected(true, true);
	}

	//手柄
	{
		slider_deadzone_stick = (ui::Slider*)parent->FindSubControl(L"slider_deadzone_stick");
		slider_deadzone_trigger = (ui::Slider*)parent->FindSubControl(L"slider_deadzone_trigger");
		slider_deadzone_joystick = (ui::Slider*)parent->FindSubControl(L"slider_deadzone_joystick");
		text_deadzone_stick = (ui::Label*)parent->FindSubControl(L"text_deadzone_stick");
		text_deadzone_trigger = (ui::Label*)parent->FindSubControl(L"text_deadzone_trigger");
		text_deadzone_joystick = (ui::Label*)parent->FindSubControl(L"text_deadzone_joystick");


		double deadzone_stick = AppSettings::GetIns().GetDeadZoneGamepadStick();
		double deadzone_trigger = AppSettings::GetIns().GetDeadZoneGamepadTrigger();
		double deadzone_joystick = AppSettings::GetIns().GetDeadZoneJoystick();

		slider_deadzone_stick->SetValue(DeadZoneToSliderValue(deadzone_stick));
		slider_deadzone_trigger->SetValue(DeadZoneToSliderValue(deadzone_trigger));
		slider_deadzone_joystick->SetValue(DeadZoneToSliderValue(deadzone_joystick));
	

		text_deadzone_stick->SetUTF8Text(DeadZoneValuePrint(AppSettings::GetIns().GetDeadZoneGamepadStick()));
		text_deadzone_trigger->SetUTF8Text(DeadZoneValuePrint(AppSettings::GetIns().GetDeadZoneGamepadTrigger()));
		text_deadzone_joystick->SetUTF8Text(DeadZoneValuePrint(AppSettings::GetIns().GetDeadZoneJoystick()));

		slider_deadzone_stick->AttachValueChanged(std::bind(&SettingsPage_input::OnDeadZondeSliderValueChanged, this, std::placeholders::_1));
		slider_deadzone_trigger->AttachValueChanged(std::bind(&SettingsPage_input::OnDeadZondeSliderValueChanged, this, std::placeholders::_1));
		slider_deadzone_joystick->AttachValueChanged(std::bind(&SettingsPage_input::OnDeadZondeSliderValueChanged, this, std::placeholders::_1));

	}
}

bool SettingsPage_input::OnMouseControlSelected(const ui::EventArgs& msg)
{
	if (msg.GetSender() == op_inputmode_move)
	{
		AppSettings::GetIns().SetMouseRelativeMove(true);
		box_inputmode_move->SetVisible(true);
		box_inputmode_coordinate->SetVisible(false);
	}
	else if (msg.GetSender() == op_inputmode_coordinate)
	{
		AppSettings::GetIns().SetMouseRelativeMove(false);
		box_inputmode_move->SetVisible(false);
		box_inputmode_coordinate->SetVisible(true);
	}
	else if (msg.GetSender() == cb_mouseInvertX)
	{
		AppSettings::GetIns().SetMouseInvertX(cb_mouseInvertX->IsSelected());
	}
	return true;
}

bool SettingsPage_input::OnMouseSpeedValueChanged(const ui::EventArgs& msg)
{
	double realValue = SliderValueToMouseSpeed(slider_mouseSpeed->GetValue());
	//100
	AppSettings::GetIns().SetMouseSpeed(realValue);

	char mouseSpeedStr[12] = {};
	SDL_snprintf(mouseSpeedStr, sizeof(mouseSpeedStr), "x%.2f", realValue);
	text_mouseSpeed->SetUTF8Text(mouseSpeedStr);


	return true;
}

bool SettingsPage_input::OnMouseAreaClicked(const ui::EventArgs& msg)
{
	//右键弹出菜单

	ui::Menu* menu = new ui::Menu(btn_mouseArea->GetWindow(), btn_mouseArea);
	menu->SetSkinFolder(L"CatTuber_default");
	DString xml(L"UIEmptyMenu.xml");

	




	ui::UiPoint curPoint(msg.ptMouse.x, msg.ptMouse.y + 4);
	btn_mouseArea->GetWindow()->ClientToScreen(curPoint);
	menu->ShowMenu(xml, curPoint);

	//class UILIB_API MenuListBox : public VListBox
	ui::VListBox* menuListBox=(ui::VListBox*)menu->FindControl(L"menuContainer");

	int displayCount = 0;
	SDL_DisplayID* displays= SDL_GetDisplays(&displayCount);



	ui::UiSize sizeMax(9999, 9999);
	int32_t maxW = 0;

	for (int i = 0; i < displayCount+1; i++)
	{
		ui::MenuItem* ItemMenu_Display = new ui::MenuItem(menu);
		//ItemMenu_Display->ResetLayout(new ui::HLayout());
		//ItemMenu_Display->SetClass(L"sceneItemMenuElement");
		ItemMenu_Display->ApplyAttributeList(LR"(height="26" width="auto" padding="20,0,20,0" margin="1,1,1,1" hot_color="itemHoverColor"  fade_hot="false")");

		//ui::Label* menuText= new ui::Label(menu);
		//menuText->SetClass(L"helperMenuText");
		//ItemMenu_Display->AddItem(menuText);
		if (i < displayCount)
		{
			ItemMenu_Display->SetName(L"[" + std::to_wstring(i) + L"][" + ui::StringConvert::UTF8ToWString(SDL_GetDisplayName(displays[i]))
				+ L"]");
			ItemMenu_Display->SetText(GETDUISTRING(L"STRID_SETTINGS_INPUT_MOUSE_AREA_DISPLAY") + std::to_wstring(i) + L":  "
				+ ui::StringConvert::UTF8ToWString(SDL_GetDisplayName(displays[i])));
		}
		else
		{
			ItemMenu_Display->SetName(L"[AllDisplays]");
			ItemMenu_Display->SetTextId(L"STRID_SETTINGS_INPUT_MOUSE_AREA_ALLDISPLAYS");
		}

		int32_t curW = ItemMenu_Display->EstimateSize(sizeMax).cx.GetInt32();
		if (curW > maxW)maxW = curW;

		ItemMenu_Display->AttachSelect(std::bind(&SettingsPage_input::OnMouseAreaMenuItemClicked, this, std::placeholders::_1));

		menuListBox->AddItem(ItemMenu_Display);
	}
	SDL_free(displays);

	for (int i = 0; i < displayCount + 1; i++)
	{
		menuListBox->GetItemAt(i)->SetFixedWidth(ui::UiFixedInt(maxW), true, false);

	}



	return true;
}

bool SettingsPage_input::OnMouseAreaMenuItemClicked(const ui::EventArgs& msg)
{
	AppSettings::GetIns().SetMouseInputArea(msg.GetSender()->GetUTF8Name());


	btn_mouseArea->SetText(((ui::MenuItem*)(msg.GetSender()))->GetText());
	btn_mouseArea->SetTextId(((ui::MenuItem*)(msg.GetSender()))->GetTextId());
	{
		//int targetIndex = 0;
		//SDL_DisplayID curdisplay = InputParser::StrToDisplay(AppSettings::GetIns().GetMouseInputArea(), &targetIndex);
		//if (0 == curdisplay)
		//{
		//	btn_mouseArea->SetText(L"");
		//	btn_mouseArea->SetTextId(L"STRID_SETTINGS_INPUT_MOUSE_AREA_ALLDISPLAYS");
		//}
		//else
		//{
		//	btn_mouseArea->SetText(GETDUISTRING(L"STRID_SETTINGS_INPUT_MOUSE_AREA_DISPLAY") + std::to_wstring(targetIndex) + L":  "
		//		+ ui::StringConvert::UTF8ToWString(SDL_GetDisplayName(curdisplay)));
		//	btn_mouseArea->SetTextId(L"");
		//}
	}
	return true;
}

bool SettingsPage_input::OnDeadZondeSliderValueChanged(const ui::EventArgs& msg)
{
	//SDL_assert(false);


	if (msg.GetSender() == slider_deadzone_stick)
	{

		double value = SliderValueToDeadZone(slider_deadzone_stick->GetValue());
		AppSettings::GetIns().SetDeadZoneGamepadStick(value);

		text_deadzone_stick->SetUTF8Text(DeadZoneValuePrint(value));
	}
	else if (msg.GetSender() == slider_deadzone_trigger)
	{
		double value = SliderValueToDeadZone(slider_deadzone_trigger->GetValue());
		AppSettings::GetIns().SetDeadZoneGamepadTrigger(value);
		text_deadzone_trigger->SetUTF8Text(DeadZoneValuePrint(value));
	}
	else if (msg.GetSender() == slider_deadzone_joystick)
	{
		double value = SliderValueToDeadZone(slider_deadzone_joystick->GetValue());
		AppSettings::GetIns().SetDeadZoneJoystick(value);
		text_deadzone_joystick->SetUTF8Text(DeadZoneValuePrint(value));
	}

	return true;
}
