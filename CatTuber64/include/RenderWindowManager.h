#ifndef _RenderWindowManager_H
#define _RenderWindowManager_H

#include <SDL3/SDL.h>
#include <vector>
#include <memory>


#include"Item/Scene.h"



class RenderWindowController
{
    friend class RenderWindowManager;
public:
    RenderWindowController(const char* title, int w, int h,int x=SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED);
    RenderWindowController() :RenderWindowController("CatTuber", 400, 300) {};
    ~RenderWindowController();


    bool _CreateWindow();

    // 初始化窗口和渲染器//重设尺寸
    bool ResetGraphic(int W,int H);


    // 处理针对本窗口的事件
    void HandleEvent(const SDL_Event& event);


    void Update(uint64_t deltaTicksNS);
    // 渲染本窗口内容
    void Render(/*SDL_GPUCommandBuffer* cmd*/);
    //Present
    void Present();

    // 清理资源
    void Shutdown();

    // 获取窗口ID用于事件路由
    Uint32 _GetWindowID() const { return windowID; }

    
    //只能在主线程中调用
    Json::Value Save();
    //不可调用多次
    void Load(const Json::Value& json);

    //窗口设置项目
    void SetTitle(const char* title);    //这个函数只能在主线程中被调用
    void SetTransparent(bool t);
    void SetTop(bool b);
    void SetLock(bool b);
    void SetClearColor(SDL_Color color);

    void SetWindowSize(int W,int H);

    void GetRenderSize(int* pw, int* ph) ;
    void GetAspectSize(int* aw, int* ah) ;
    void SetAspectSize(int aw, int ah) ;

    //GET
    SDL_Window* GetSDLWindow() { return window; };


    //仅供内部使用
    // 窗口尺寸变化回调,参数是渲染区域而不是整个窗口大小
    void _OnResize(int newW, int newH);
private:
    //各平台在创建窗口后的特殊处理
    //Windows使用此函数监听原WM_SIZING信息
    void _AfterCreateWindow();
    bool _ResizeSwapchain(SDL_GPUCommandBuffer* commandBuffer, SDL_Window* window);
    const char* title;

    //窗口尺寸变化变化
    int targetX;
    int targetY;
    int targetW;
    int targetH;

    int renderW;
    int renderH;

    int aspectRatioW;
    int aspectRatioH;

    //可设置项
    static SDL_FColor clearColor;
    bool isTransparent=false;
    bool deviceClaimed = false;

    SDL_Window* window = nullptr;
    //SDL_Renderer* renderer = nullptr;
    Uint32            windowID = 0;

    Scene scene;
    SDL_GPUTexture* depthStencil = nullptr;
    SDL_Renderer* renderer = NULL;
    SDL_GPUTexture* offscreenTex = NULL;
    SDL_GPUTransferBuffer* offscreenTexTb = NULL;
    SDL_Texture* offscreenTex_2D = NULL;

    SDL_GPUCommandBuffer* cmdCurframe = NULL;



};












class RenderWindowManager {
    friend class CatTuberApp;
    friend class RenderThread;
public:
    RenderWindowManager();
    ~RenderWindowManager();


    static RenderWindowManager& GetIns() {static RenderWindowManager ref; return ref; }




    // 创建一个新窗口并注册控制器
    bool CreateRenderWindow(const char* title,
        int w, int h,
        int x= SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED);

    //// 主循环：事件处理 + 渲染
    //void Run();

    // 询问是否仍在运行
    bool IsRunning() const { return running; }

    // 立即终止运行
    void Quit() { running = false; }

    // 事件处理：拉取 SDL_Event 并路由
    void HandleEvent(const SDL_Event& event);

    // 渲染所有窗口
    void UpdateAll(uint64_t deltaTicksNS);
    void RenderAll();
    void PresentAll();

    // 清理所有窗口控制器
    void ShutdownAll();




    //设置项
    //均在主线程中调用..
    void SetWindowTop(bool b);
    void SetWindowTransparent(bool b);
    void SetWindowLock(bool b);
    void SetWindowVisible(bool b);
    void SetWindowBackgroundColor(SDL_Color backgroundColor);
    void SetFrameLimit(int frameLimit);

    //将所有窗口的位置大小 以及其中的内容保存下来
    bool SaveScene(const char* sceneName,bool isQuitSave=false);
    bool LoadScene(const char* sceneName, bool isQuitSave = false);
    
private:
    bool _BuildFromJson(const Json::Value& json);

    int _frameLimit=60;//永远大于0，UI中显示
    //uint64_t _frameTickNs = ;//一帧需要的纳秒数
    SDL_FColor _clearColor = {};//透明度值永远为0
    std::vector<std::unique_ptr<RenderWindowController>> controllers;
    bool running = true;
    bool canStartFrame = true;
};


















#endif // !RenderWindowManager_H
