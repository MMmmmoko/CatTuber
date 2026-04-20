//
//#ifdef _WINDOWS
//#include <windows.h>
//#endif // _WINDOWS



#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include"AppContext.h"
#include"CatTuberApp.h"


//库
#ifdef _WINDOWS



//SDL_Mixer
//#pragma comment(lib,"../ThirdPart/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/lib/x64/SDL3_mixer.lib")
//#pragma comment(lib,"G:/Projects/CatTuber64/project/CatTuber64/x64/Debug/SDL3.lib")



//Windows
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Imm32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Opengl32.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "version.lib")
//网络
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


//Live2D
#ifdef _DEBUG
#pragma comment(lib,"../ThirdPart/CubismSdkForNative-5-r.4.1/Core/lib/windows/x86_64/143/Live2DCubismCore_MDd.lib")
#else
#pragma comment(lib,"../ThirdPart/CubismSdkForNative-5-r.4.1/Core/lib/windows/x86_64/143/Live2DCubismCore_MD.lib")
#endif // _DEBUG

//nimdui
#ifdef _DEBUG
#pragma comment(lib, "cximage_d.lib")
#pragma comment(lib, "duilib_d.lib")
#pragma comment(lib, "libpng_d.lib")
#pragma comment(lib, "libwebp_d.lib")
#pragma comment(lib, "zlib_d.lib")

#else
#pragma comment(lib, "cximage_s.lib")
#pragma comment(lib, "duilib_s.lib")
#pragma comment(lib, "libpng_s.lib")
#pragma comment(lib, "libwebp_s.lib")
#pragma comment(lib, "zlib_s.lib")
#endif // _DEBUG

#pragma comment(lib, "turbojpeg-static.lib")

//SKIA
//#ifdef _DEBUG
//#pragma comment(lib, "msvc.x64.MDd/skia.lib") 
//#pragma comment(lib, "msvc.x64.MDd/bentleyottmann.lib")
//#pragma comment(lib, "msvc.x64.MDd/expat.lib")
//#pragma comment(lib, "msvc.x64.MDd/jsonreader.lib")
//#pragma comment(lib, "msvc.x64.MDd/pathkit.lib")
//#pragma comment(lib, "msvc.x64.MDd/skcms.lib")
//#pragma comment(lib, "msvc.x64.MDd/skottie.lib")
//#pragma comment(lib, "msvc.x64.MDd/skresources.lib")
//#pragma comment(lib, "msvc.x64.MDd/skshaper.lib")
//#pragma comment(lib, "msvc.x64.MDd/svg.lib")
//#else
//#pragma comment(lib, "msvc.x64.MD/skia.lib") 
//#pragma comment(lib, "msvc.x64.MD/bentleyottmann.lib")
//#pragma comment(lib, "msvc.x64.MD/expat.lib")
//#pragma comment(lib, "msvc.x64.MD/jsonreader.lib")
//#pragma comment(lib, "msvc.x64.MD/pathkit.lib")
//#pragma comment(lib, "msvc.x64.MD/skcms.lib")
//#pragma comment(lib, "msvc.x64.MD/skottie.lib")
//#pragma comment(lib, "msvc.x64.MD/skresources.lib")
//#pragma comment(lib, "msvc.x64.MD/skshaper.lib")
//#pragma comment(lib, "msvc.x64.MD/svg.lib")
//#endif // _DEBUG
//SKIA
#ifdef _DEBUG
#pragma comment(lib, "llvm.x64.debug/skia.lib") 
#pragma comment(lib, "llvm.x64.debug/bentleyottmann.lib")
#pragma comment(lib, "llvm.x64.debug/expat.lib")
#pragma comment(lib, "llvm.x64.debug/jsonreader.lib")
#pragma comment(lib, "llvm.x64.debug/skcms.lib")
#pragma comment(lib, "llvm.x64.debug/skottie.lib")
#pragma comment(lib, "llvm.x64.debug/skresources.lib")
#pragma comment(lib, "llvm.x64.debug/skshaper.lib")
#pragma comment(lib, "llvm.x64.debug/svg.lib")
#pragma comment(lib, "llvm.x64.debug/sksg.lib")
#else
#pragma comment(lib, "llvm.x64.release/skia.lib") 
#pragma comment(lib, "llvm.x64.release/bentleyottmann.lib")
#pragma comment(lib, "llvm.x64.release/expat.lib")
#pragma comment(lib, "llvm.x64.release/jsonreader.lib")
#pragma comment(lib, "llvm.x64.release/skcms.lib")
#pragma comment(lib, "llvm.x64.release/skottie.lib")
#pragma comment(lib, "llvm.x64.release/skresources.lib")
#pragma comment(lib, "llvm.x64.release/skshaper.lib")
#pragma comment(lib, "llvm.x64.release/svg.lib")
#pragma comment(lib, "llvm.x64.release/sksg.lib")
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

