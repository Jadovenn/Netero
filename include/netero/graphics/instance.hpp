/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <netero/graphics/axis.hpp>

namespace netero::graphics {

    /**
     * @brief Actual instance of a model
     * An instance represent one model in the context
     * that is actually in the graphics pipeline.
     * Many instance of a model may exist.
     */
    class Instance {
        friend class Model;
    public:
        Axis    x; /**< Width axis. */
        Axis    y; /**< Height axis */
        Axis    z; /**< Depth axis. Thanks captain obvious.*/

    private:
        struct Data {
            glm::mat4   model;
        };

        Data    shared; /**< Shared struct with the instance buffer. */
        int     offset = 0; /**< Offset of this Instance in the instance buffer. */

        /**
         * @brief Compute a model matrix for this instance.
         * @return Instance::Data* - A pointer to the shared data of the instance.
         */
        Data* update() {
            if (!x.isAltered() && !y.isAltered() && !z.isAltered()) {
                return &this->shared;
            }
            this->shared.model = glm::mat4(1.f);
            this->shared.model = glm::translate(this->shared.model, glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
            this->shared.model = glm::rotate(this->shared.model, x.getRotation(), glm::vec3(1, 0, 0));
            this->shared.model = glm::rotate(this->shared.model, y.getRotation(), glm::vec3(0, 1, 0));
            this->shared.model = glm::rotate(this->shared.model, z.getRotation(), glm::vec3(0, 0, 1));
            this->shared.model = glm::scale(this->shared.model, glm::vec3(x.getScaling(), y.getScaling(), z.getScaling()));
            return &this->shared;
        }

        /**
         * Return the binding description required by vulkan.
         */
        static  VkVertexInputBindingDescription  getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription;

            bindingDescription.binding = 1;
            bindingDescription.stride = sizeof(glm::mat4);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            return bindingDescription;
        }

        /**
         * Return input attributes required by vulkan, to bind instance buffer to the
         * model pipeline.
         */
        static std::array<VkVertexInputAttributeDescription, 4>    getAttributeDescription() {
            std::array<VkVertexInputAttributeDescription, 4>    attributeDescriptions{};

            attributeDescriptions[0].binding = 1;
            attributeDescriptions[0].location = 2;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[0].offset = 0;
            attributeDescriptions[1].binding = 1;
            attributeDescriptions[1].location = 3;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = sizeof(glm::vec4);
            attributeDescriptions[2].binding = 1;
            attributeDescriptions[2].location = 4;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[2].offset = sizeof(glm::vec4) * 2;
            attributeDescriptions[3].binding = 1;
            attributeDescriptions[3].location = 5;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = sizeof(glm::vec4) * 3;
            return attributeDescriptions;
        }
    };
}

