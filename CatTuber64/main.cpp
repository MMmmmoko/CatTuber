//
//#ifdef _WINDOWS
//#include <windows.h>
//#endif // _WINDOWS



#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>

#include"CatTuberApp.h"


//库
#ifdef _WINDOWS
//Windows
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
//Live2D
#ifdef _DEBUG
#pragma comment(lib,"../ThirdPart/CubismSdkForNative-5-r.4.1/Core/lib/windows/x86_64/143/Live2DCubismCore_MDd.lib")
#else
#pragma comment(lib,"../ThirdPart/CubismSdkForNative-5-r.4.1/Core/lib/windows/x86_64/143/Live2DCubismCore_MD.lib")
#endif // _DEBUG
#endif


int main(int argc, char** argv)
{

	try {


		CatTuberApp& app = CatTuberApp::GetInstance();
		if (app.Init(argc, argv))
		{
			app.Run();
		}
		std::string aaa;
		throw(aaa);
		return 0;
	}
	catch (const std::exception& e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what(), NULL);
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", e.what());
	}
	catch (const std::string& e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.c_str(), NULL);
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", e.c_str());
	}
	catch (const char* e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e, NULL);
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", e);
	}
	catch (...)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unknown non-standard library exceptions", NULL);
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", "Unknown non-standard library exceptions");
	}
	return -1;
}

//
//
//#ifdef _WINDOWS
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//
//
//	try {
//		int argc;
//		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
//
//
//
//		CatTuberApp& app=CatTuberApp::GetInstance();
//		if (app.Init(argc, argv)) 
//		{
//			app.Run();
//		}
//		std::string aaa;
//		throw(aaa);
//		return 0;
//	}
//	catch (const std::exception& e)
//	{
//		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error",e.what(),NULL);
//		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR,"[Exception] %s", e.what());
//	}
//	catch (const std::string& e)
//	{
//		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.c_str(), NULL);
//		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", e.c_str());
//	}
//	catch (const char* e)
//	{
//		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e, NULL);
//		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", e);
//	}
//	catch (...)
//	{
//		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unknown non-standard library exceptions", NULL);
//		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "[Exception] %s", "Unknown non-standard library exceptions");
//	}
//	return -1;
//}
//#endif // _WINDOWS

