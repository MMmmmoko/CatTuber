
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include<Live2DCubismCore.hpp>
#include<CubismFramework.hpp>
#include<../../Live2DFramework/SDL3Renderer/CubismRenderer_SDL3.hpp>

#include"Model/Live2DModelBase.h"

#include"AppSettings.h"
#include"AppContext.h"
#include"Input/InputManager.h"

#include "CatTuberApp.h"
#include"UserEvent.h"





Live2DModelBase* model;

bool CatTuberApp::Init(int argC, char* argV[])
{
	//TODO:命令行处理
	if (false)
	{
		return false;
	}


	//软件相关的初始化



	//先进行AppSettings的加载,因为有一些功能需要很早获取设置项
	//比如获取是否在管理员身份中运行的项，尽早以管理员身份重启软件
	AppSettings::GetIns().Load();

	//************************************
	//todo判断是否需要以管理员身份重启
	//
	//
	//**************************************




	//************************************
	//todo将SDL打印信息输出到文件
	//
	//
	//**************************************







	//版本打印
	//CatTuber
	SDL_Log("CatTuber Version: %d", SDL_VERSION);
	SDL_Log("Platform: %s", SDL_GetPlatform());

	//SDL
	SDL_VERSION;  // 获取编译时的版本号
	SDL_Log("SDL Compiled Version: %d", SDL_VERSION);
	SDL_Log("SDL Linked Version: %d", SDL_GetVersion());
	SDL_Log("SDL_image Compiled Version: %d", SDL_IMAGE_VERSION);
	SDL_Log("SDL_image Linked Version: %d", IMG_Version());
	//Live2D version
	{

		const Live2D::Cubism::Core::csmVersion version = Live2D::Cubism::Core::csmGetVersion();
		const uint32_t major = static_cast<uint32_t>((version & 0xFF000000) >> 24);
		const uint32_t minor = static_cast<uint32_t>((version & 0x00FF0000) >> 16);
		const uint32_t patch = static_cast<uint32_t>((version & 0x0000FFFF));
		const uint32_t versionNumber = version;
		//不确定核心版本和SDK版本是否完全一致，
		//没有获取SDK版本的比较方便的方法，手动写吧...
		SDL_Log("Live2D Cubism Core version: %02d.%02d.%04d (%d)", major, minor, patch, versionNumber);
		SDL_Log("Live2D SDK version: 5-r4.1");
	}





	//SDL初始化
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Can not init SDL: %s", SDL_GetError());
		return false;
	}
	UserEvent::Init();









	bool b_debug = false;
#ifdef _DEBUG
	b_debug = true;
#endif // _DEBUG



	SDL_GPUDevice* pdevice = NULL;
#if defined SDL_PLATFORM_WINDOWS
	pdevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, b_debug, "direct3d12");
#elif defined SDL_PLATFORM_MACOS
	pdevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, b_debug, "metal");
#elif defined SDL_PLATFORM_LINUX
	pdevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, b_debug, "vulkan");
#endif // 

	if (NULL == pdevice)
	{
		//设备无效抛出异常
		SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_GPU,"Can not create a GPU Device. %s",SDL_GetError());
		
		//THROW;
		return false;
	}
	AppContext::_ref()._gpudevice = pdevice;
















	//初始化Live2D
	{
		static Csm::CubismFramework::Option _cubismOption;
		_cubismOption.LogFunction = &AppContext::LogFunc;
		_cubismOption.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Warning;
#ifdef _DEBUG
		_cubismOption.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Debug;
#endif // _DEBUG
		//LIVE2D Api要的是32位的size，这里转换一下
		_cubismOption.LoadFileFunction = [](const std::string filePath, uint32_t* outSize) {
			size_t size = 0;
			auto result = AppContext::LoadFileFunc(filePath, &size);
			*outSize = static_cast<uint32_t>(size);
			return result;
			};
		_cubismOption.ReleaseBytesFunction = &AppContext::ReleaseBytesFunc;

		//Live2d的内存申请器
		static class Live2DAllocator :public Csm::ICubismAllocator
		{
			virtual void* Allocate(const Csm::csmSizeType size)override { return AppContext::AllocFunc(size); }
			virtual void Deallocate(void* memory)override { AppContext::DeallocFunc(memory); }
			virtual void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment)override
			{
				return AppContext::AllocFunc_Aligned(size, alignment);
			}
			virtual void DeallocateAligned(void* alignedMemory)override { AppContext::DeallocFunc_Aligned(alignedMemory); }
		}allocator;

		if (!Csm::CubismFramework::StartUp(&allocator, &_cubismOption))
		{
			SDL_LogError(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, "Can Not StartUp Live2D Framework.");
			return false;
		}

		Csm::CubismFramework::Initialize();

		//加载模型前初始化渲染器
		Live2D::Cubism::Framework::Rendering::CubismRenderer_SDL3::InitializeConstantSettings(1, AppContext::GetGraphicDevice());
		//创建Live2d用的SDL3 render context
		Csm::Rendering::CubismRenderContext_SDL3* context = Csm::Rendering::CubismRenderContext_SDL3::CreateSDLGpuRenderContext(AppContext::GetGraphicDevice());
		AppContext::_ref()._l2dRenderContext = context;

	}
	



	//创建窗口
	 //RenderWindowManager::GetIns().CreateRenderWindow("CatTuber1", 1920, 1080, 100, 100);
	//载入上次运行时的设置
	RenderWindowManager::GetIns().LoadScene("#LastExit");








	//创建渲染线程
	renderThread.Start();

	return true;
}

void CatTuberApp::Run()
{
	//主线程，处理窗口事件

	auto& wm = RenderWindowManager::GetIns();

	SDL_Event event;
	bool loopRunning = true;
	while(loopRunning&&SDL_WaitEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) {
			loopRunning = false;
			Quit();
		}
		if (event.type >= SDL_EVENT_USER && event.type < SDL_EVENT_LAST)
		{
			UserEvent::HandleUserEvent(&event.user);
		}

		wm.HandleEvent(event);
	}

	//不管怎么样都需要通知渲染线程结束





	//软件结束运行，执行一些结束时的动作
	ShutDown();
}

//RenderLoop在渲染线程类中
/*
void CatTuberApp::RenderLoop()
{
	//逻辑+渲染线程
	
	try {
		auto& wm = RenderWindowManager::GetIns();

		uint64_t startTick = SDL_GetTicksNS();
		uint64_t savedTick = startTick;
		while (wm.IsRunning())
		{
			uint64_t currentTick = SDL_GetTicksNS();
			//处理逻辑事件

			uint64_t deltaTick = currentTick - savedTick;
			savedTick = currentTick;
			//float secondDuring = deltaTick / 1000000000.F;
			float secondDuring = deltaTick * 0.000000001F;
			wm.UpdateAll(secondDuring);
			wm.RenderAll();


			//计算睡眠
			uint64_t _CurFrameTick = SDL_GetTicksNS() - currentTick;
			//到下一帧的毫秒值
			float sleepTime = ((1.f / wm._frameLimit) - _CurFrameTick * 0.000000001F) * 1000.F;
			//仅在需要进行sleep的时候睡眠,如果不能睡眠一定时间那就不睡
			if (sleepTime > 1.f)
				SDL_Delay(sleepTime);
		}
		return;
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




}
*/


void CatTuberApp::Quit()
{
	//收到退出命令的时候进行文件保存
	//应该只有主线程可以调用到这个函数
	//Quit是个命令，先处理命令（如退出前保存状态），下方的ShutDown是最终关闭程序时执行的一些动作



	//对是否能退出进行判断，并弹出相应的对话框
	{
	
	}

	AppSettings::GetIns().Save();
	RenderWindowManager::GetIns().SaveScene("#LastExist",true);
	//先停止渲染线程
	renderThread.Stop();

	RenderWindowManager::GetIns().Quit();
}


void CatTuberApp::ShutDown()
{
	//需要等待渲染线程结束？
	RenderWindowManager::GetIns().ShutdownAll();
	InputManager::GetIns().ShutDown();


	//不需要对Live2D相关资源进行清理(Live2D的demo中没有进行清理)
	//如果一些资源与线程强绑定的话（在哪个线程创建就在哪个线程清除）
	//那么可能还需要分线程进行清理
	if (AppContext::_ref()._l2dRenderContext)
	{
		Csm::Rendering::CubismRenderContext_SDL3::ReleaseSDLGpuRenderContext(AppContext::_ref()._l2dRenderContext);
		AppContext::_ref()._l2dRenderContext = NULL;
	}




	//最后清理Gpu
	if (AppContext::_ref()._gpudevice)
	{
		SDL_DestroyGPUDevice(AppContext::_ref()._gpudevice);
		AppContext::_ref()._gpudevice = NULL;
	}


}
