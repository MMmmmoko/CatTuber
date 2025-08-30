#pragma once


#include <SDL3/SDL.h>
#include<unordered_map>

//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
    namespace Cubism {
        namespace Framework {
            namespace Rendering {


                //按需检索，如无则创建



                class CubismPipelineManager_SDL3
                {



                public:
                    SDL_GPUGraphicsPipeline* GetPipeline(SDL_GPUDevice* device, SDL_GPUGraphicsPipelineCreateInfo* pipelineInfo);

                    



                private:

                    std::unordered_map<SDL_GPUGraphicsPipelineCreateInfo, SDL_GPUGraphicsPipeline*> pipelineMap;
                };



























            }
        }
    }
}