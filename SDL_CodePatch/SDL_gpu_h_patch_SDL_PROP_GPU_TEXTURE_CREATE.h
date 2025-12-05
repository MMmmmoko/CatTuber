//由于一些原因，CatTuber必须改动SDL的部分代码
//为了方便更新维护，修改的代码不存放在源文件中。

//修改方式：将本文件内容include到SDL_gpu.h中

//使D3D12纹理可共享,CatTuber中用来与D3D11互操作（目前SDL透明窗口只能用D3D11渲染器）
#define SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_BOOL "SDL.gpu.texture.create.share.isshare"
#define SDL_PROP_GPU_TEXTURE_CREATE_D3D12_SHARE_HANDLE_POINTER "SDL.gpu.texture.create.share.handle"
   