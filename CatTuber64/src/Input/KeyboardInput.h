#ifndef _KeyboardInput_h
#define _KeyboardInput_h


#include<SDL3/SDL.h>


//键盘输入以DirectInput中提供的有名称的按键为准，
//即取DirectInput和SDL按键定义的交集(交集含有几乎全部通用键，应该能覆盖mac和Linux)，
//名称按SDL的来（因为DirectInput不跨平台，所以那边的命名可能不是很通用）。

class KeyboardInput
{


public:
	bool Init();
	void Release();
	void UpdateAndPumpEvents();



private:

	void ResetData();

};

















#endif