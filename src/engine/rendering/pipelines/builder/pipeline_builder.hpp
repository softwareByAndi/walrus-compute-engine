#ifndef WALRUS_COMPUTE_ENGINE_PIPELINE_BUILDER_HPP
#define WALRUS_COMPUTE_ENGINE_PIPELINE_BUILDER_HPP

#include "../defaults/pipeline_defaults.hpp"

#include <vk_types.h>
#include <vector>

namespace walrus {

  struct PipelineBuilder {
    PipelineBuilder();

    explicit PipelineBuilder(VkExtent2D &extent);

    void init();

    ~PipelineBuilder() = default;

    void build(VkDevice vkDevice, VkRenderPass vkRenderPass, VkPipeline *outPipeline);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreate{};
    VkPipelineVertexInputStateCreateInfo vertexInputCreate = defaults::pipeline::vertexInputStateCreateInfo();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreate = defaults::pipeline::inputAssemblyStateCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizerCreate = defaults::pipeline::rasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampleCreate = defaults::pipeline::multisampleStateCreateInfo();
    VkPipelineColorBlendAttachmentState colorBlendAttachment = defaults::pipeline::colorBlendAttachmentState();
    VkRect2D scissor{};
    VkViewport viewport{};
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_PIPELINE_BUILDER_HPP
