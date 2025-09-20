#include<SDL3/SDL.h>
#ifdef SDL_PLATFORM_WINDOWS

#include<unordered_map>
#include<Windows.h>
#include <windowsx.h>
#include"RenderWindowManager.h"
#include"UserEvent.h"

static std::unordered_map<HWND, RenderWindowController*> hwnd2SDL_Window;



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

//CatTuber需要拦截处理WM_Sizing事件

typedef  LRESULT(CALLBACK* __WINDOWSPROC)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static __WINDOWSPROC SDL_WindowProc=NULL;



LRESULT CALLBACK RenderWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    //if (uMsg == WM_ENTERSIZEMOVE) {
    //    RenderWindowController* _this = hwnd2SDL_Window[hwnd];
    //    SetTimer(hwnd, (uintptr_t)_this, 8, NULL);
    //}
    //else if (uMsg == WM_EXITSIZEMOVE) {
    //    RenderWindowController* _this = hwnd2SDL_Window[hwnd];
    //    KillTimer(hwnd, (uintptr_t)_this);
    //}
    //else if (uMsg == WM_TIMER) {
    //    //InvalidateRect(hwnd, NULL, FALSE);
    //    SDL_Event event;
    //    if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_USER + UserEvent::TASK, SDL_EVENT_USER + UserEvent::TASK) > 0)
    //    {
    //        UserEvent::HandleUserEvent(&event.user);
    //    }
    //}

    //if (uMsg == WM_SIZE) {
    //    UINT width = LOWORD(lParam);
    //    UINT height = HIWORD(lParam);
    //    RenderWindowController* _this = hwnd2SDL_Window[hwnd];
    //    _this->_OnResize(width, height);
    //}
    if (uMsg == WM_SIZING) {
        do
        {
            RenderWindowController* _this = hwnd2SDL_Window[hwnd];
            int aspectRatio_width, aspectRatio_height;
            _this->GetAspectSize(&aspectRatio_width,&aspectRatio_height);
            RECT* prect = (RECT*)lParam;
            RECT crect = *prect;
            long _style = GetWindowStyle(hwnd);
            long _exstyle = GetWindowExStyle(hwnd);
            if (!AdjustWindowRectEx(&crect, _style, GetMenu(hwnd) != NULL, _exstyle))break;
            crect.left = prect->left + prect->left - crect.left;
            crect.right = prect->right + prect->right - crect.right;
            crect.top = prect->top + prect->top - crect.top;
            crect.bottom = prect->bottom + prect->bottom - crect.bottom;

            //根据比例计算最小宽高
            int minWidth, minHeight;
            if (aspectRatio_width > aspectRatio_height)
            {
                minHeight = 100;
                minWidth = minHeight * aspectRatio_width / aspectRatio_height;
            }
            else
            {
                minWidth = 100;
                minHeight= minWidth * aspectRatio_height/ aspectRatio_width ;
            }

            switch (wParam)
            {
            case WMSZ_BOTTOM:
            case WMSZ_TOP:
            {
                int height = crect.bottom - crect.top;
                if (height < minHeight)
                {
                    (wParam == WMSZ_BOTTOM) ? (crect.bottom= crect.top+ minHeight): (crect.top = crect.bottom - minHeight);
                    height = minHeight;
                }

                int width = height* aspectRatio_width/ aspectRatio_height;
                //水平中点
                int dW =static_cast<int>( (width - (crect.right - crect.left)) * 0.5);
                crect.left -= dW;
                crect.right += dW;
                break;
            }
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
            {
                int width= crect.right - crect.left;
                int temHeight= width* aspectRatio_height / aspectRatio_width;
                if (temHeight > crect.bottom - crect.top)
                {
                    //temHeight:应该达到的高度，客户区高度比应该达到的高度小，说明方向为←→
                    if (width < minWidth)
                    {
                        (wParam == WMSZ_BOTTOMLEFT) ? (crect.left = crect.right - minWidth) : (crect.right = crect.left + minWidth);
                        width = minWidth;
                    }
                    int height = width * aspectRatio_height / aspectRatio_width;
                    crect.bottom = crect.top + height;
                }
                else
                {
                    int height = crect.bottom - crect.top;
                    if (height < minHeight)
                    {
                         height = minHeight;
                    }
                    width= height * aspectRatio_width / aspectRatio_height;
                    (wParam == WMSZ_BOTTOMLEFT) ? (crect.left = crect.right - width) : (crect.right = crect.left + width);
                }
                
                break;
            }
            case WMSZ_LEFT:
            case WMSZ_RIGHT:
            {
                int width = crect.right - crect.left;
                if (width < minWidth)
                {
                    (wParam == WMSZ_LEFT) ? (crect.left = crect.right - minWidth) : (crect.right = crect.left + minWidth);
                    width = minWidth;
                }
                int height = width * aspectRatio_height / aspectRatio_width;
                int dH = static_cast<int>((height - (crect.bottom - crect.top)) * 0.5);
                crect.top -= dH;
                crect.bottom += dH;
                break;
            }
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
            {
                int width = crect.right - crect.left;
                int temHeight = width * aspectRatio_height / aspectRatio_width;
                if (temHeight > crect.bottom - crect.top)
                {
                    //temHeight:应该达到的高度，客户区高度比应该达到的高度小，说明方向为←→
                    if (width < minWidth)
                    {
                        (wParam == WMSZ_TOPLEFT) ? (crect.left = crect.right - minWidth) : (crect.right = crect.left + minWidth);
                        width = minWidth;
                    }
                    int height = width * aspectRatio_height / aspectRatio_width;
                    crect.top = crect.bottom - height;
                }
                else
                {
                    int height = crect.bottom - crect.top;
                    if (height < minHeight)
                    {
                        height = minHeight;
                    }
                    width = height * aspectRatio_width / aspectRatio_height;
                    (wParam == WMSZ_TOPLEFT) ? (crect.left = crect.right - width) : (crect.right = crect.left + width);
                }
                break;
            }
            default:
                break;
            }

            //_this->_OnResizing(crect.right-crect.left,crect.bottom-crect.top);
            _this->_OnResize(crect.right-crect.left,crect.bottom-crect.top);
            if(!AdjustWindowRectEx(&crect,_style,NULL,_exstyle))break;
            *prect = crect;

            //测试WM_PAINT能否处理
            InvalidateRect(hwnd, NULL, FALSE);


        } while (false);
        

        //return TRUE;


    }

    // 其他消息交给原窗口过程
    return CallWindowProc(SDL_WindowProc, hwnd, uMsg, wParam, lParam);
}

void RenderWindowController::_AfterCreateWindow()
{
    //获取HWND
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    HWND hwnd = (HWND)SDL_GetPointerProperty(
                props,
                SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                nullptr       // 可选：传入 SDL_bool* 获取是否查询成功
            );
    if (!hwnd)return;

    //因为SDL库内部可能会使用UserData，所以这里使用查表的方式从hwnd获取sdlwindow而不是写入GWLP_USERDATA
    hwnd2SDL_Window[hwnd] = this;
    
    SDL_WindowProc =(__WINDOWSPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    



    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)RenderWndProc);


    //禁用最大化按钮
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

}

extern "C" {
    extern SDL_DECLSPEC  bool SDLCALL SDL_D3D12_ResizeSwapchain(SDL_GPUCommandBuffer* commandBuffer, SDL_Window* window);
}
bool RenderWindowController::_ResizeSwapchain(SDL_GPUCommandBuffer* commandBuffer, SDL_Window* window)
{
     return  SDL_D3D12_ResizeSwapchain(commandBuffer,window);
}














#endif