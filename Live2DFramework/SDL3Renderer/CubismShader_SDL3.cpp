//#include<filesystem>

#include"CubismShader_SDL3.hpp"

#include"SDL_CFG.h"
#include"CubismRenderContext_SDL3.hpp"




//着色器头文件
typedef  uint8_t BYTE;
#include"PixelMasked.h"
#include"PixelMaskedInverted.h"
#include"PixelMaskedInvertedPremult.h"
#include"PixelMaskedPremult.h"
#include"PixelNormal.h"
#include"PixelNormalPremult.h"
#include"PixelSetupMask.h"
#include"VertMasked.h"
#include"VertNormal.h"
#include"VertSetupMask.h"



#define _MAKESTR(x) #x
#define MAKESTR(x) _MAKESTR(x)
#define LOAD_VSSHADER(shaderName) LoadShaderProgram(device, false, ShaderNames_ ## shaderName,g_Vert ## shaderName,sizeof(g_Vert ## shaderName) ,static_cast<const csmChar*>( MAKESTR(Vert ## shaderName)))
#define LOAD_PSSHADER(shaderName) LoadShaderProgram(device, true, ShaderNames_ ## shaderName,g_Pixel ## shaderName,sizeof(g_Pixel ## shaderName) ,static_cast<const csmChar*>(  MAKESTR(Pixel ## shaderName)))





//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {

                void CubismShader_SDL3::ReleaseShaderProgram()
                {
                    //_shaderSrc.Clear();
                    if (!_device)return;

                    for (csmInt32 i = 0; i < ShaderNames_Max; i++)
                    {
                        if (_shaderSetsVS[i])
                        {
                            SDL_ReleaseGPUShader(_device,_shaderSetsVS[i]);
                            _shaderSetsVS[i] = NULL;
                        }

                        if (_shaderSetsPS[i])
                        {
                              SDL_ReleaseGPUShader(_device, _shaderSetsPS[i]);
                            _shaderSetsPS[i] = NULL;
                        }
                    }
                    _device = NULL;
                }
                

                CubismShader_SDL3::CubismShader_SDL3()
                {
                    // 器作成
                    for (csmInt32 i = 0; i < ShaderNames_Max; i++)
                    {
                        _shaderSetsVS.PushBack(NULL);
                        _shaderSetsPS.PushBack(NULL);
                    }
                }

                CubismShader_SDL3::~CubismShader_SDL3()
                {
                    ReleaseShaderProgram();

                    // 器の削除
                    _shaderSetsVS.Clear();
                    _shaderSetsPS.Clear();
                }

                void CubismShader_SDL3::GenerateShaders(SDL_GPUDevice* device)
                {
                    //防止反复生成
                    if (_device)
                        return;

                    //如果生成则先清理
                    ReleaseShaderProgram();
                    _device = device;



                    csmBool isSuccess = false;


                    //神秘防goto写法
                    do
                    {
                        //加载着色器源码
                        
                        //通过SDL加载文件
                        //std::string u8path=SDL_GetBasePath();
                        //std::string shaderPath = u8path + "Shader/Live2d.Shader";
                        //
                        //SDL_IOStream* shaderCodeStream= SDL_IOFromFile(shaderPath.c_str(),"r");
                        //if (!shaderCodeStream)
                        //{
                        //    CubismLogError("Open Shader File Failed: Shader/Live2d.Shader.");
                        //    break;
                        //}

                        //uint64_t size = SDL_GetIOSize(shaderCodeStream);
                        //void* buffer = SDL_malloc(size);
                        //SDL_ReadIO(shaderCodeStream, buffer, size);

                        ////复制到缓存中
                        //_shaderSrc.Resize(size);
                        //memcpy(_shaderSrc.GetPtr(), buffer,size);

                        //SDL_free(buffer);
                        //SDL_CloseIO(shaderCodeStream); // 关闭文件流




                        //加载着色器
                        //mask

                       ;

                        if (!LoadShaderProgram(device, false, ShaderNames_SetupMask,g_VertSetupMask,sizeof(g_VertSetupMask), static_cast<const csmChar*>("VertSetupMask")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_SetupMask, g_PixelSetupMask,sizeof(g_PixelSetupMask), static_cast<const csmChar*>("PixelSetupMask")))
                        {
                            break;
                        }
                        
                        //顶点
                        if (!LoadShaderProgram(device, false, ShaderNames_Normal, g_VertNormal, sizeof(g_VertNormal), static_cast<const csmChar*>("VertNormal")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, false, ShaderNames_NormalMasked, g_VertMasked,sizeof(g_VertMasked), static_cast<const csmChar*>("VertMasked")))
                        {
                            break;
                        }

                        //像素
                        if (!LoadShaderProgram(device, true, ShaderNames_Normal,g_PixelNormal,sizeof(g_PixelNormal), static_cast<const csmChar*>("PixelNormal")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_NormalMasked,g_PixelMasked,sizeof(g_PixelMasked), static_cast<const csmChar*>("PixelMasked")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_NormalMaskedInverted,g_PixelMaskedInverted,sizeof(g_PixelMaskedInverted), static_cast<csmChar*>("PixelMaskedInverted")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_NormalPremultipliedAlpha,g_PixelNormalPremult,sizeof(g_PixelNormalPremult), static_cast<csmChar*>("PixelNormalPremult")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_NormalMaskedPremultipliedAlpha,g_PixelMaskedPremult,sizeof(g_PixelMaskedPremult), static_cast<const csmChar*>("PixelMaskedPremult")))
                        {
                            break;
                        }
                        if (!LoadShaderProgram(device, true, ShaderNames_NormalMaskedInvertedPremultipliedAlpha,g_PixelMaskedInvertedPremult,sizeof(g_PixelMaskedInvertedPremult), static_cast<csmChar*>("PixelMaskedInvertedPremult")))
                        {
                            break;
                        }

                        // 成功
                        isSuccess = true;
                    } while (0);


                    if (!isSuccess)
                    {
                        CubismLogError("Fail Compile shader");
                        CSM_ASSERT(0);
                        return;
                    }
                   
                    //顶点布局
                    _vertexFormat = {};
                    _vertexBufferDesc = {};

                    _vertexFormat.vertex_buffer_descriptions = &_vertexBufferDesc;
                    _vertexFormat.num_vertex_buffers = 1;
                    _vertexFormat.vertex_attributes = _vertexAttr;
                    _vertexFormat.num_vertex_attributes = 2;

                    _vertexBufferDesc.slot = 0;
                    _vertexBufferDesc.pitch = sizeof(CubismVertexSDL3);
                    _vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
                    _vertexBufferDesc.instance_step_rate = 0;

                    _vertexAttr[0].location = 0;
                    _vertexAttr[0].buffer_slot = 0;
                    _vertexAttr[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
                    _vertexAttr[0].offset = 0;

                    _vertexAttr[1].location = 1;
                    _vertexAttr[1].buffer_slot = 0;
                    _vertexAttr[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
                    _vertexAttr[1].offset = sizeof(float) * 2;// XYUV;






                }



                Csm::csmBool CubismShader_SDL3::LoadShaderProgram(SDL_GPUDevice* device, bool isPs, csmInt32 assign, const uint8_t* bytes, size_t size, const csmChar* entryPoint)
                {
                    if (!device) return false;


                    SDL_GPUShaderCreateInfo shaderDesc = {};
                    shaderDesc.code_size = size;
                    shaderDesc.code = (uint8_t*)bytes;
                    shaderDesc.entrypoint = entryPoint;
                    shaderDesc.format = SDL_SHADERFORMAT_AUTO;
                    shaderDesc.stage = isPs ? SDL_GPU_SHADERSTAGE_FRAGMENT : SDL_GPU_SHADERSTAGE_VERTEX;
                    shaderDesc.num_samplers = isPs ? 2:0;
                    shaderDesc.num_storage_textures = 0;
                    shaderDesc.num_storage_buffers = 0;
                    shaderDesc.num_uniform_buffers = 1;


                    SDL_GPUShader* pCurShader=SDL_CreateGPUShader(device,&shaderDesc);

                    if (!pCurShader)
                    {
                        CubismLogWarning("Fail Load Shader : %s", entryPoint == NULL ? "" : entryPoint);
                        return false;
                    }

                    if (isPs)
                    {
                        _shaderSetsPS[assign] = pCurShader;
                    }
                    else
                    {
                        _shaderSetsVS[assign] = pCurShader;
                    }

                    return true;
                }


                SDL_GPUShader* CubismShader_SDL3::GetVertexShader(csmUint32 assign)
                {
                    if (assign < ShaderNames_Max)
                    {
                        return _shaderSetsVS[assign];
                    }

                    return NULL;
                }

                SDL_GPUShader* CubismShader_SDL3::GetFragmentShader(csmUint32 assign)
                {
                    if (assign < ShaderNames_Max)
                    {
                        return _shaderSetsPS[assign];
                    }

                    return NULL;
                }


                void CubismShader_SDL3::SetupShader(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext)
                {
                    GenerateShaders(device);
                    if (!renderContext /*|| !_vertexFormat*/)
                    {
                        return;
                    }
                    renderContext->SetInputLayout(&_vertexFormat);
                }






            }
        }
    }
}