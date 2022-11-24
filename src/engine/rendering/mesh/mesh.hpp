#ifndef WALRUS_COMPUTE_ENGINE_MESH_HPP
#define WALRUS_COMPUTE_ENGINE_MESH_HPP

#include <vk_types.h>
#include <vector>
#include <glm/vec3.hpp>

namespace walrus {

  struct VertexInputDescription {
    std::vector<VkVertexInputBindingDescription> bindings{};
    std::vector<VkVertexInputAttributeDescription> attributes{};
    VkPipelineVertexInputStateCreateFlags flags = 0;
  };

  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription getVertexInputDescription();
  };

  struct Mesh {
    std::vector<Vertex> vertices;
    AllocatedBuffer vertexBuffer;
  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_MESH_HPP
