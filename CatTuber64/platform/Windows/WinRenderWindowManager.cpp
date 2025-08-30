#include<SDL3/SDL.h>
#ifdef SDL_PLATFORM_WINDOWS

#include<Windows.h>

#include"RenderWindowManager.h"






void RenderWindowController::SetLock(bool b)
{

    //先尝试SDL_SetWindowShape是否可行，不可行再直接调用平台API





    //
    //for (auto& wc : controllers)
    //{
    //    // 1. 获取窗口属性集
    //    SDL_PropertiesID props = SDL_GetWindowProperties(wc->window);

    //    // 2. 从属性集中读取 HWND
    //    //    直接返回指针值，若平台不支持该属性则为 nullptr
    //    HWND hwnd = (HWND)SDL_GetPointerProperty(
    //        props,
    //        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
    //        nullptr       // 可选：传入 SDL_bool* 获取是否查询成功
    //    );
    //}
}




















#endif