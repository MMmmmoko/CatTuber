
// DuilibLearn.cpp : 定义应用程序的入口点。
#include <memory>
#include "UiThread.h"
#include"AppContext.h"
#include"AppSettings.h"
#include"CatTuberApp.h"

//自定义控件
//#include"UserControl/ImageCropperControl.h"

//#include"duilib"

void UIMiscThread::OnInit()
{
    //#if defined(DUILIB_BUILD_FOR_WIN)&&!defined (DUILIB_BUILD_FOR_SDL)
    //    ::OleInitialize(nullptr);
    //#endif
}

void UIMiscThread::OnCleanup()
{
    //#if defined(DUILIB_BUILD_FOR_WIN)&&!defined (DUILIB_BUILD_FOR_SDL)
    //    ::OleUninitialize();
    //#endif
}








void UIMainThread::OnInit()
{
    static bool init = false;
    if (init)return;
    init = true;

    // 启动杂事处理线程
    misc_thread_.reset(new UIMiscThread(ui::kThreadUser, L"Global Misc Thread"));
    misc_thread_->Start();

    // 获取资源路径，初始化全局参数
    // 默认皮肤使用 resources\\themes\\default
    // 默认语言使用 resources\\lang\\zh_CN
    // 如需修改请指定 Startup 最后两个参数
   // std::wstring theme_dir = QPath::GetAppPath();
    //ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), false);




    //创建回调
    auto createFunc = [](const std::wstring& controlName)->ui::Control*
        {

            //if (L"CoverCropper" == controlName)return new ImageCropperControl(null);

        };


    

	CatTuberApp::StartDuiManager();










}

void UIMainThread::OnCleanup()
{
    misc_thread_->Stop();
    misc_thread_.reset(nullptr);
    ui::GlobalManager::Instance().Shutdown();
}