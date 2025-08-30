#ifndef CatTuberApp_h
#define CatTuberApp_h




#ifdef _WINDOWS




#endif // _WINDOWS


#include<String>
#include"RenderThread.h"
#include"RenderWindowManager.h"



class CatTuberApp
{

public:
	static CatTuberApp& GetInstance()
	{
		static CatTuberApp app;
		return app;
	};//获取唯一实例


private:
	CatTuberApp() :renderThread(RenderThread::GetIns()){};
	~CatTuberApp() {};
public:
	bool Init(int argC, char* argV[]);
	void Run();
	//RenderLoop在渲染线程类中
	//void RenderLoop();
	//
	void Quit();
	 
private:
	void ShutDown();


	RenderThread& renderThread;
};










#endif