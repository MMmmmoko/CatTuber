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
    RenderWindowController(const char* title, int x, int y, int w, int h);
    RenderWindowController() :RenderWindowController("CatTuber", 100, 100, 400, 300) {};
    ~RenderWindowController();

    // 初始化窗口和渲染器
    bool Init();

    // 处理针对本窗口的事件
    void HandleEvent(const SDL_Event& event);


    void Update(uint64_t deltaTicksNS);
    // 渲染本窗口内容
    void Render(/*SDL_GPUCommandBuffer* cmd*/);

    // 清理资源
    void Shutdown();

    // 获取窗口ID用于事件路由
    Uint32 GetWindowID() const { return windowID; }

    
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

    void GetRenderSize(int* pw, int* ph) ;
private:
    const char* title;
    //int x, y, w, h;

    //窗口尺寸变化变化
    int targetX;
    int targetY;
    int targetW;
    int targetH;

    int renderW;
    int renderH;


    //可设置项
    static SDL_FColor clearColor;
    bool isTransparent=false;


    SDL_Window* window = nullptr;
    //SDL_Renderer* renderer = nullptr;
    Uint32            windowID = 0;

    Scene scene;
    SDL_GPUTexture* depthStencil = nullptr;
    SDL_Renderer* renderer = NULL;
    SDL_GPUTexture* offscreenTex = NULL;
    SDL_GPUTransferBuffer* offscreenTexTb = NULL;
    SDL_Texture* offscreenTex_2D = NULL;




    // 窗口尺寸变化回调,参数是渲染区域而不是整个窗口大小
    void OnResize(int newW, int newH);

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
        int x, int y,
        int w, int h);

    //// 主循环：事件处理 + 渲染
    //void Run();

    // 询问是否仍在运行
    bool IsRunning() const { return running; }

    // 立即终止运行
    void Quit() { running = false; }

    // 事件处理：拉取 SDL_Event 并路由
    void HandleEvent(const SDL_Event& event);

    // 渲染所有窗口
    void RenderAll();
    void UpdateAll(uint64_t deltaTicksNS);

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
};


















#endif // !RenderWindowManager_H
