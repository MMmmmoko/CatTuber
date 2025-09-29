#ifndef AppContext_h
#define AppContext_h


#include <SDL3/SDL.h>
#include<iostream>


//device等变量、 软件版本号、  软件config

///
/// ********************
/// VERSION
/// 
/// 
///

#define CATTUBER_VER_MAJOR 0
#define CATTUBER_VER_MINOR 8
#define CATTUBER_VER_PATCH 2


#define CATTUBER_VER_BETA 10

//保证先展开再转字符串
#define ____TOSTRING(x) ____TOSTRING____(x)
#define ____TOSTRING____(x) #x

#ifdef CATTUBER_VER_BETA
//#define CATTUBER_VER ((CATTUBER_VER_MAJOR<<28)+(CATTUBER_VER_MINOR<<20)+(CATTUBER_VER_PATCH<<16)+CATTUBER_VER_BETA)
//参考sdl
#define CATTUBER_VER_NUM(MAJOR,MINOR,PATCH,BETA) (MAJOR*100'000'000 +MINOR*1000'000+PATCH*1000+BETA)
#define CATTUBER_VER (CATTUBER_VER_NUM(CATTUBER_VER_MAJOR,CATTUBER_VER_MINOR,CATTUBER_VER_PATCH,CATTUBER_VER_BETA))


#define CATTUBER_VER_STR \
    ____TOSTRING(CATTUBER_VER_MAJOR) "." \
    ____TOSTRING(CATTUBER_VER_MINOR) "." \
    ____TOSTRING(CATTUBER_VER_PATCH) "-beta-"\
    ____TOSTRING(CATTUBER_VER_BETA)
#else

#define CATTUBER_VER_NUM(MAJOR,MINOR,PATCH) (MAJOR*100'000'000 +MINOR*1000'000+PATCH*1000)
#define CATTUBER_VER (CATTUBER_VER_NUM(CATTUBER_VER_MAJOR,CATTUBER_VER_MINOR,CATTUBER_VER_PATCH))



#define CATTUBER_VER_STR \
    ____TOSTRING(CATTUBER_VER_MAJOR) "." \
    ____TOSTRING(CATTUBER_VER_MINOR) "." \
    ____TOSTRING(CATTUBER_VER_PATCH)
#endif


//APPMETADATA


#define CATTUBER_ORGNAME "CatSoft"
#define CATTUBER_APPNAME "CatTuber"


//文件
#define CATTUBER_SCENE_LAST_SAVED_FILE_NAME "_LastSaved.scene"


//模型资源相关
//模型描述文件名
#define CATTUBER_MODELRESOURCE_INFO_FILENAME "info.json"
#define CATTUBER_MODELRESOURCE_DESC_FILENAME "desc.json"
#define CATTUBER_MODEL_BUTTON_PARAM_HEAD "BUTTON_"
#define CATTUBER_MODEL_AXIS_PARAM_HEAD "AXIS_"
#define CATTUBER_MODEL_FOLDERDIR "Model"
#define CATTUBER_MODEL_IDLEGROUPNAME "Idle"



///
///*************
/// 





//屏蔽警告
#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning(push))
#define DISABLE_WARNING_POP            __pragma(warning(pop))
#define DISABLE_WARNING_MSVC(warn)     __pragma(warning(disable : warn))
#define DISABLE_WARNING_GCC(warn)
#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X)                   _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING_GCC(warn)      DO_PRAGMA(GCC diagnostic ignored warn)
#define DISABLE_WARNING_MSVC(warn)
#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_MSVC(warn)
#define DISABLE_WARNING_GCC(warn)
#endif

DISABLE_WARNING_PUSH
//DISABLE_WARNING_MSVC(26452)
DISABLE_WARNING_GCC("-Wshift-count-overflow")
DISABLE_WARNING_GCC("-Wshift-negative-value")


//声明live2d所用的renderContext
namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {
                class CubismRenderContext_SDL3;
            }
        }
    }
}


class AppContext
{
    friend class CatTuberApp;
public:
    ~AppContext();

    static SDL_GPUDevice* GetGraphicDevice() { return _ref()._gpudevice; };
    static SDL_GPUTextureFormat GetSwapchainFormat() { return  _ref()._swapchanFormat; };
    static Live2D::Cubism::Framework::Rendering::CubismRenderContext_SDL3* GetLive2DRenderContext() { return _ref()._l2dRenderContext; };

	static const char* GetAppVersionStr() { return CATTUBER_VER_STR; };
	static uint32_t GetAppVersion() { return CATTUBER_VER; };






    //统一的日志、文件接口（方便结合其他库使用） sadsa
public:
    //获取一个可写的路径,结尾有"/"符号
    static const char* GetPrefPath();//可以读写的路径
    static const char* GetAppBasePath();//windows：exe所在文件夹路径 （结尾有"/"符号）
    static const char* GetWorkShopPath();//windows：exe所在文件夹路径 （结尾有"/"符号）
    static std::string ResolvePathToAbsolute(const std::string& pathToResolve);//解析路径，将路径中的[AppBasePath]等字符串解析成绝对路径
    static std::string ResolvePathToRelative(const std::string& pathToResolve);//解析路径，将路径中的[AppBasePath]等字符串解析成相对路径




    static void LogFunc(const char* log);
    static unsigned char* LoadFileFunc(const std::string filePath, size_t* outSize);
    static void ReleaseBytesFunc(unsigned char* byteData);

    static void* AllocFunc(size_t size);
    static void DeallocFunc(void* memory);
    static void* AllocFunc_Aligned(size_t size,uint32_t alignment);
    static void DeallocFunc_Aligned(void* alignedMemory);
private:
    static AppContext& _ref() { static AppContext ref; return ref; };

    
    char* _prefPath = NULL;



    SDL_GPUDevice* _gpudevice=NULL;
    SDL_GPUTextureFormat _swapchanFormat= SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
    Live2D::Cubism::Framework::Rendering::CubismRenderContext_SDL3* _l2dRenderContext = NULL;
     //


};

DISABLE_WARNING_POP

//#undef ____TOSTRING

#endif