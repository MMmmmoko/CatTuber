#ifndef _CTBGamepadInput_h
#define _CTBGamepadInput_h


#include<SDL3/SDL.h>


//键盘输入以DirectInput中提供的有名称的按键为准，
//即取DirectInput和SDL按键定义的交集(交集含有几乎全部通用键，应该能覆盖mac和Linux)，
//名称按SDL的来（因为DirectInput不跨平台，所以那边的命名可能不是很通用）。


//怎么在设计上进行手柄分P?





class GamepadInput
{


public:
	bool Init();
	void Release();
	void UpdateAndPumpEvents();


	static const char* GetSDLButtonKeyName(SDL_GamepadButton button);
private:

	void ResetData();




	struct GamepadState
	{


	};














};

















#endif