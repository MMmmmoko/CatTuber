
#include <SDL3/SDL.h>
#include"RenderThread.h"
#include"RenderWindowManager.h"
#include"Input/InputManager.h"
#include"Util/SDL_LockGuard.h"
void RenderThread::Start()
{
	//这个函数会在主线程调用

	if (renderThread) {
		// 已启动
		return;
	}
	stopFlag = false;

	//互斥锁
	if (!taskQueueMutex)
	{
		taskQueueMutex = SDL_CreateMutex();
		if (!taskQueueMutex)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create mutex!%s",SDL_GetError());
			throw(std::runtime_error(std::string("Couldn't create mutex!")+ SDL_GetError()));
			return;
		}
	}

	renderThread =   SDL_CreateThread( [](void* data)->int {

		RenderThread* pThis = (RenderThread*)data;
		try {
			pThis->InitGraphics();

			pThis->ThreadLoop();
			pThis->ShutdownGraphics();
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

		pThis->ShutdownGraphics();
		return 1;

		},"LogicAndRenderThread",this);
	
	if (!renderThread)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create thread!%s", SDL_GetError());
		throw(std::runtime_error(std::string("Couldn't create thread!")+ SDL_GetError()));
		return;
	}


}

void RenderThread::Stop()
{
	
	PostTask([](void* userdata, uint64_t userdata2) {
		((RenderThread*)userdata)->stopFlag = true;
		},this);

	
	//如果不在渲染线程,等待
	if (SDL_GetCurrentThreadID() != SDL_GetThreadID(renderThread))
	{
		int status;
		SDL_WaitThread(renderThread, &status);
	}
}

void RenderThread::PostTask(void(*task)(void* userdata, uint64_t userdata2), void* userdata, uint64_t userdata2)
{
	//如果已经在渲染线程
	if (SDL_GetCurrentThreadID() == SDL_GetThreadID(renderThread))
	{
		task(userdata, userdata2);
	}
	{
		SDL_LockGuard guard(taskQueueMutex);
		taskQueueBack.push({ task,userdata,userdata2 });
	}
}

void RenderThread::SendTask(void(*task)(void* userdata, uint64_t userdata2), void* userdata, uint64_t userdata2)
{
	struct TemStruct
	{
		void(*task)(void* userdata, uint64_t userdata2)=NULL;
		void* userdata = NULL;
		uint64_t userdata2 = 0;
		bool done = false;
	}patchData;
	patchData.task = task;
	patchData.userdata = userdata;
	PostTask([](void* userdata, uint64_t userdata2) {
		TemStruct* pTemStruct = (TemStruct*)userdata;
		pTemStruct->task(pTemStruct->userdata, userdata2);
		pTemStruct->done = true;
		}, &patchData);

	while (!patchData.done)
	{
		SDL_DelayNS(1000);
	}
}

void RenderThread::ThreadLoop()
{
	//SDL_WaitAndAcquireGPUSwapchainTexture();
	auto& wm = RenderWindowManager::GetIns();
	auto& im = InputManager::GetIns();

	uint64_t startTick = SDL_GetTicksNS();
	uint64_t savedTick = startTick;
	while (!stopFlag)
	{
		//处理Task事件
		{
			SDL_LockGuard guard(taskQueueMutex);
			std::swap(taskQueueFront, taskQueueBack);
		}
		while (!taskQueueFront.empty())
		{
			auto task = std::move(taskQueueFront.front());
			taskQueueFront.pop();
			task.task(task.userData, task.userdata2);
		}

		if (!wm.canStartFrame)
			continue;

		//计算时间差
		uint64_t currentTick = SDL_GetTicksNS();
		uint64_t deltaTick = currentTick - savedTick;
		savedTick = currentTick;

		

		im.PumpDeviceOrNetworkInputEvents();
		wm.UpdateAll(deltaTick);
		wm.RenderAll();
		wm.PresentAll();

		//计算睡眠
		uint64_t _CurFrameTick = SDL_GetTicksNS() - currentTick;
		//到下一帧的ns tick值
		uint64_t frameDuration= 1'000'000'000 / wm._frameLimit;
		int64_t sleepTime = frameDuration - _CurFrameTick;
		//仅在需要进行sleep的时候睡眠,如果不能睡眠一定时间那就不睡
		if (sleepTime > 1)
			SDL_DelayNS(sleepTime);
	}












}

void RenderThread::InitGraphics()
{
	//TODO
}

void RenderThread::ShutdownGraphics()
{
	//TODO
}




