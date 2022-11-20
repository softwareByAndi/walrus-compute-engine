#ifndef WALRUS_COMPUTE_ENGINE_PIPELINE_DEFAULTS_HPP
#define WALRUS_COMPUTE_ENGINE_PIPELINE_DEFAULTS_HPP

#include <vk_types.h>

namespace walrus::defaults::pipeline {

  VkPipelineShaderStageCreateInfo shaderStageCreateInfo(VkShaderStageFlagBits flags, VkShaderModule vkShaderModule);

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(VkPolygonMode polygonMode = VK_POLYGON_MODE_LINE);

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo();

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState();

  VkPipelineLayoutCreateInfo  layoutCreateInfo();

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_PIPELINE_DEFAULTS_HPP
