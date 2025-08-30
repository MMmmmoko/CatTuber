#ifndef _MouseInput_h
#define _MouseInput_h


#include<SDL3/SDL.h>
#include"Input/InputCommon.h"

//鼠标输入
//仅在处理输入的线程使用
class MouseInput
{


public:
	bool Init();
	void Release();
	void UpdateAndPumpEvents();
	void SetDisplayRect(SDL_Rect* _mouseArea) { if(_mouseArea)mouseArea = *_mouseArea; };
	void SetMoveSpeed(float _speed) { mouseSpeed=_speed; };
	void SetInverX(bool b) { isInvertX =b; };
private:
	//数据列表
	ButtonProxy mouseButtonList[5] = {};
	ButtonProxy mouseWheelUp ;//滚轮朝远离身体的地方
	ButtonProxy mouseWheelDown ; //滚轮朝靠近身体的地方
	AxisProxy mouseCoordinateX ;
	AxisProxy mouseCoordinateY ;
	AxisProxy mouseMoveX;
	AxisProxy mouseMoveY;



	bool isInvertX = false;
	float mouseSpeed=1.f;
	float relatePosX=0.f;
	float relatePosY=0.f;
	int primaryScreenW=1920;
	int primaryScreenH=1080;
	SDL_Rect mouseArea = {};
};











#endif
