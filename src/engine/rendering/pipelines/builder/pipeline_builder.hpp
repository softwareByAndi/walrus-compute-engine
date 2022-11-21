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

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    VkPipelineVertexInputStateCreateInfo vertexInputState = defaults::pipeline::vertexInputStateCreateInfo();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = defaults::pipeline::inputAssemblyStateCreateInfo();
    VkPipelineRasterizationStateCreateInfo rasterizerState = defaults::pipeline::rasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo multisampleState = defaults::pipeline::multisampleStateCreateInfo();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = defaults::pipeline::colorBlendAttachmentState();
    VkRect2D scissor{};
    VkViewport viewport{};
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_PIPELINE_BUILDER_HPP
