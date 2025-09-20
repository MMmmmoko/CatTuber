
// DuilibLearn.cpp : 定义应用程序的入口点。
#include <memory>
#include "UiThread.h"
#include"AppContext.h"
#include"AppSettings.h"


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
    misc_thread_.reset(new UIMiscThread(ui::kThreadMisc, L"Global Misc Thread"));
    misc_thread_->Start();

    // 获取资源路径，初始化全局参数
    // 默认皮肤使用 resources\\themes\\default
    // 默认语言使用 resources\\lang\\zh_CN
    // 如需修改请指定 Startup 最后两个参数
   // std::wstring theme_dir = QPath::GetAppPath();
    //ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), false);




#ifdef _DEBUG

    //统一使用APPCONTEXT（由SDL提供的文件路径）
    //ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    ui::FilePath resourcePath = ui::FilePath(AppContext::GetAppBasePath());
    resourcePath += L"Dui/";


    //使用本地文件的资源管理方式
    ui::LocalFilesResParam resParam(resourcePath);
    //auto curLang=AppSettings::GetIns().GetMiscLanguage()+".ini";


    resParam.languageFileName = L"schinese.ini";
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));



#else
    //windows 发布release版本时将zip放入程序资源中


    //原库没有正确处理dll模块句柄
    //先hook以暂时修复这个问题

    //auto pTargetFunc=  (&ui::ZipManager::OpenResZip);
    //void** ppTargetFunc = (void**)&pTargetFunc;
    //void* pFunc = *ppTargetFunc;
    //CatHook::AutoHook(pFunc,(void*)Hook_ZipManager_OpenResZip,(void**) & orig_ZipManager_OpenResZip);







    //使用exe资源文件中的zip压缩包
    ui::ResZipFileResParam resParam;
    resParam.resourcePath = _T("CSPMOD_res\\");
    //我们是dll不能空着，空着表示exe的资源，我要读的是dll资源
    resParam.hResModule = (HMODULE)CatHook::thisModule;
    resParam.resourceName = MAKEINTRESOURCE(IDR_THEME);
    resParam.resourceType = _T("Theme");
    resParam.zipPassword = _T("");
    ui::GlobalManager::Instance().Startup(resParam);
    //我超这个ui库有bug，对加载dll中资源没有进行测试.
#endif // _DEBUG










    //创建窗口
    //BasicForm* window = new BasicForm();
    //window->CreateWnd(nullptr,ui::WindowCreateParam(L"CSP Helper", true));
    ////正常应用主窗口被关闭时会退出，但我们是寄生在CSP主线程的，不能让CSP退出
    ////window->PostQuitMsgWhenClosed(true);
    //window->ShowWindow(ui::kSW_SHOW_NORMAL);




}

void UIMainThread::OnCleanup()
{
    misc_thread_->Stop();
    misc_thread_.reset(nullptr);
    ui::GlobalManager::Instance().Shutdown();
}