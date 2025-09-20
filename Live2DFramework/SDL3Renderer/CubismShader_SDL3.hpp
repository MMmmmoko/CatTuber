#pragma once
#include<SDL3/SDL.h>


#include "Rendering/CubismRenderer.hpp"
#include "CubismType_SDL3.hpp"
#include "CubismFramework.hpp"
#include "Type/csmVector.hpp"




namespace Live2D {
    namespace Cubism {
        namespace Framework {
            enum ShaderNames
            {
                // SetupMask
                ShaderNames_SetupMask,

                //Normal
                ShaderNames_Normal,
                ShaderNames_NormalMasked,
                ShaderNames_NormalMaskedInverted,
                ShaderNames_NormalPremultipliedAlpha,
                ShaderNames_NormalMaskedPremultipliedAlpha,
                ShaderNames_NormalMaskedInvertedPremultipliedAlpha,

                //Add
                ShaderNames_Add,
                ShaderNames_AddMasked,
                ShaderNames_AddMaskedInverted,
                ShaderNames_AddPremultipliedAlpha,
                ShaderNames_AddMaskedPremultipliedAlpha,
                ShaderNames_AddMaskedInvertedPremultipliedAlpha,

                //Mult
                ShaderNames_Mult,
                ShaderNames_MultMasked,
                ShaderNames_MultMaskedInverted,
                ShaderNames_MultPremultipliedAlpha,
                ShaderNames_MultMaskedPremultipliedAlpha,
                ShaderNames_MultMaskedInvertedPremultipliedAlpha,

                ShaderNames_Max,
            };
        }
    }
}

//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {

                //  前方宣言
                class CubismRenderer_SDL3;
                class CubismClippingContext;
                class CubismRenderContext_SDL3;


                //一个渲染管线中的顶点片元着色器，可能不需要这个类
                class CubismShaderSet
                {
                public:
                    CubismShaderSet()
                        : _vertexShader(NULL)
                        , _pixelShader(NULL)
                    {
                    }

                    SDL_GPUShader* _vertexShader;
                    SDL_GPUShader* _pixelShader;
                };





                class CubismShader_SDL3
                {
                    friend class CubismRenderer_SDL3;
                public:
                    CubismShader_SDL3();
                    virtual ~CubismShader_SDL3();

                    void ReleaseShaderProgram();

                    SDL_GPUShader* GetVertexShader(csmUint32 assign);
                    SDL_GPUShader* GetFragmentShader(csmUint32 assign);


                    void SetupShader(SDL_GPUDevice* device, CubismRenderContext_SDL3* renderContext);

                private:
                    //创建着色器
                    void GenerateShaders(SDL_GPUDevice* device);

                    //成功为true
                    Csm::csmBool LoadShaderProgram(SDL_GPUDevice* device, bool isPs, csmInt32 assign, const uint8_t* bytes, size_t size, const csmChar* entryPoint, csmInt32 samplerNum=0);


                    SDL_GPUDevice* _device = NULL;

                    csmVector<CubismShaderSet*> _shaderSets;

                    //保存着色器
                    csmVector<SDL_GPUShader*> _shaderSetsVS;
                    csmVector<SDL_GPUShader*> _shaderSetsPS;

                    //Inputlayout Like
                    SDL_GPUVertexBufferDescription _vertexBufferDesc;
                    SDL_GPUVertexAttribute  _vertexAttr[2];
                    SDL_GPUVertexInputState _vertexFormat;
                };








            }
        }
    }
}
//------------ LIVE2D NAMESPACE ------------
