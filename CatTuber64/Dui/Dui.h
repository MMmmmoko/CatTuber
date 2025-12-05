#ifndef _Dui_h
#define _Dui_h


#include"DuiCommon.h"

//仅主线程
class Dui
{
public:
	static void Init();
	static void ShutDown();


	static void OpenMainUiWindow();




private:
	Dui& GetIns() { static Dui ref; return ref; };
	
	

};




#endif

//nimdui patch note
//nimdui修改记录，
//WindowBuilder.cpp
//ParseWindowAttributes尾部增加对xmlns的排除
//if(!strName.starts_with(L"xmlns"))