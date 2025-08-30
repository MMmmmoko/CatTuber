#ifndef _RenderThread_h
#define _RenderThread_h

#include<functional>
#include<queue>



//因为SDL提供了一些基于平台的额外功能，所以使用SDL的线程


class RenderThread
{

public:
    static RenderThread& GetIns() { static RenderThread ref; return ref; }
    void Start();     // 启动线程并初始化渲染上下文
    void Stop();      // 请求退出并回收资源
    void PostTask(void(* task)(void* userdata, uint64_t userdata2), void* userdata=NULL,uint64_t userdata2=0); // 投递任务到渲染线程
    void SendTask(void(*task)(void* userdata, uint64_t userdata2), void* userdata = NULL, uint64_t userdata2 = 0);//等待执行完成。不可在渲染线程调用
    //void LockThread();//锁定渲染线程以执行其他工作
    
private:
    


    void ThreadLoop(); // 实际渲染线程逻辑
    void InitGraphics(); // 初始化 Vulkan/OpenGL/DirectX中需要在渲染线程执行的部分
    void ShutdownGraphics(); // 清理资源

    struct TaskCallback
    {
        void(*task)(void* userdata, uint64_t userdata2);
        void* userData;
        uint64_t userdata2;
    };
    std::queue<TaskCallback> taskQueueFront;//用于处理callback
    std::queue<TaskCallback> taskQueueBack;//用于pushcallback



    SDL_Thread* renderThread=NULL;
    SDL_Mutex* taskQueueMutex = NULL;
    bool stopFlag = false;

};

#endif