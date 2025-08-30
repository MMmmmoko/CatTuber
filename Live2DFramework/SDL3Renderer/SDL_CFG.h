#pragma once


//设置SDL的后端
//MacOS上为Metal Windows上为D3d12，linux上为Vulkan
#include<SDL3/SDL_platform_defines.h>



#ifdef SDL_PLATFORM_WINDOWS
#define SDL_SHADERFORMAT_AUTO SDL_GPU_SHADERFORMAT_DXIL
#endif // WINDOWS

#ifdef SDL_PLATFORM_MACOS

#define SDL_SHADERFORMAT_AUTO SDL_GPU_SHADERFORMAT_MSL



#endif // Mac

#ifdef SDL_PLATFORM_LINUX
#define SDL_SHADERFORMAT_AUTO SDL_GPU_SHADERFORMAT_SPIRV


#endif // Linux
