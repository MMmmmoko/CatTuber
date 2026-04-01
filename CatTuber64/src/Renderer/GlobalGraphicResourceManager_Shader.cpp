#include"GlobalGraphicResourceManager.h"
#include <Live2DFramework/SDL3Renderer/SDL_CFG.h>

    
#ifdef SDL_PLATFORM_WINDOWS
typedef  uint8_t BYTE;
#include"Shader/DXIL/BongoCatSpriteVS.h"
#include"Shader/DXIL/BongoCatSpritePS.h"
#include"Shader/DXIL/BongoCatHandVS.h"
#include"Shader/DXIL/BongoCatHandPS.h"
#include"Shader/DXIL/BongoCatHandTexVS.h"
#include"Shader/DXIL/BongoCatHandTexPS.h"





#endif // DEBUG












#define VS SDL_GPU_SHADERSTAGE_VERTEX
#define PS SDL_GPU_SHADERSTAGE_FRAGMENT
// ShaderName
// ShaderStage
// num_samplers,
// num_storage_textures,
// num_storage_buffers,
// num_uniform_buffers
#define CATTUBER_SHADER_LIST(F) \
F(BongoCatSpriteVS,VS,0,0,0,1)\
F(BongoCatSpritePS,PS,1,0,0,0)\
F(BongoCatHandVS,VS,0,0,0,1)\
F(BongoCatHandPS,PS,1,0,0,0)\
F(BongoCatHandTexVS,VS,0,0,0,1)\
F(BongoCatHandTexPS,PS,1,0,0,0)\



static std::unordered_map<std::string,SDL_GPUShaderCreateInfo> shaderCreateInfoMap;


SDL_GPUShader* GlobalGraphicResourceManager::GetShaderByName(const char* shaderName)
{
    auto it = shaderPool.find(shaderName);
    if (it == shaderPool.end() || it->second == nullptr)
    {
        //打表
        if (shaderCreateInfoMap.empty())
        {
            //SDL_GPUShaderCreateInfo shaderDesc = {};
#define SHADER_INFO_SETUP(ShaderName,Stage,numSamplers,numStorageTextures,numStorageBuffers,numUniformBuffers) \
{ \
auto& shaderDesc=shaderCreateInfoMap[#ShaderName];\
shaderDesc.code_size=sizeof(ShaderName);\
shaderDesc.code=ShaderName;\
shaderDesc.entrypoint=#ShaderName;\
shaderDesc.format = SDL_SHADERFORMAT_AUTO;\
 shaderDesc.stage=Stage;\
 shaderDesc.num_samplers = numSamplers;\
 shaderDesc.num_storage_textures = numStorageTextures;\
 shaderDesc.num_storage_buffers = numStorageBuffers;\
 shaderDesc.num_uniform_buffers = numUniformBuffers;\
}

        { CATTUBER_SHADER_LIST(SHADER_INFO_SETUP) }
        }

        shaderPool[shaderName]= SDL_CreateGPUShader(pDevice,&shaderCreateInfoMap[shaderName]);
    }


    return  shaderPool[shaderName];
}
