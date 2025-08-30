#pragma once
#include<glm/glm.hpp>


//------------ LIVE2D NAMESPACE ------------
namespace Live2D {
    namespace Cubism {
        namespace Framework {

            /**
             * @brief   D3D11»ù±¾í”µã
             */

            struct CubismVertexSDL3
            {
                float x, y;     // Position
                float u, v;     // UVs
            };

            /**
             * @brief   ¥·¥§©`¥À©`¥³¥ó¥¹¥¿¥ó¥È¥Ð¥Ã¥Õ¥¡
             */
            struct CubismConstantBufferSDL3
            {
                glm::mat4x4 projectMatrix;
                glm::mat4x4 clipMatrix;
                glm::vec4 baseColor;
                glm::vec4 multiplyColor;
                glm::vec4 screenColor;
                glm::vec4 channelFlag;
            };

        }
    }
}
