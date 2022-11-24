#include "mesh.hpp"

namespace walrus {

  VertexInputDescription walrus::Vertex::getVertexInputDescription() {
    VertexInputDescription description {};

    VkVertexInputBindingDescription binding = {};
    binding.binding = 0;
    binding.stride = sizeof(walrus::Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    description.bindings.push_back(binding);

    VkVertexInputAttributeDescription positionAttribute{};
    positionAttribute.binding = 0;
    positionAttribute.location = 0;
    positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttribute.offset = offsetof(walrus::Vertex, position);

    VkVertexInputAttributeDescription normalAttribute{};
    normalAttribute.binding = 0;
    normalAttribute.location = 1;
    normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttribute.offset = offsetof(walrus::Vertex, normal);

    VkVertexInputAttributeDescription colorAttribute{};
    colorAttribute.binding = 0;
    colorAttribute.location = 2;
    colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttribute.offset = offsetof(walrus::Vertex, color);

    description.attributes.push_back(positionAttribute);
    description.attributes.push_back(normalAttribute);
    description.attributes.push_back(colorAttribute);
    return description;
  }

} // walrus