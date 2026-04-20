#include"GamepadInput.h"



static const char* sdlButtonKeyNameList[SDL_GamepadButton::SDL_GAMEPAD_BUTTON_COUNT] = {
	"GamepadButton.SOUTH",
	"GamepadButton.EAST",
	"GamepadButton.WEST",
	"GamepadButton.NORTH",
	"GamepadButton.BACK",
	"GamepadButton.GUIDE",
	"GamepadButton.START",
	"GamepadButton.LS",
	"GamepadButton.RS",
	"GamepadButton.LB",
	"GamepadButton.RB",
	"GamepadButton.DPAD_UP",
	"GamepadButton.DPAD_DOWN",
	"GamepadButton.DPAD_LEFT",
	"GamepadButton.DPAD_RIGHT",
	"GamepadButton.MISC1",
	"GamepadButton.RIGHT_PADDLE1",
	"GamepadButton.LEFT_PADDLE1",
	"GamepadButton.RIGHT_PADDLE2",
	"GamepadButton.LEFT_PADDLE2",
	"GamepadButton.TOUCHPAD",
	"GamepadButton.MISC2",
	"GamepadButton.MISC3",
	"GamepadButton.MISC4",
	"GamepadButton.MISC5",
	"GamepadButton.MISC6",
};


const char* GamepadInput::GetSDLButtonKeyName(SDL_GamepadButton button)
{
	return sdlButtonKeyNameList[button];

}