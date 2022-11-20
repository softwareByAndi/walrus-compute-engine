#include "pipeline_defaults.hpp"

namespace walrus::defaults::pipeline {

  VkPipelineShaderStageCreateInfo
  shaderStageCreateInfo(VkShaderStageFlagBits stageFlags, VkShaderModule vkShaderModule) {
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.stage = stageFlags;
    info.module = vkShaderModule;
    info.pName = "main"; // shader entry point
    return info;
  }


  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo() {
    VkPipelineVertexInputStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    // no vertex bindings or attributes
    info.vertexBindingDescriptionCount = 0;
    info.vertexAttributeDescriptionCount = 0;
    return info;
  }


  VkPipelineInputAssemblyStateCreateInfo
  inputAssemblyStateCreateInfo(VkPrimitiveTopology topology /* = VK_PRIMITIVE_TOPOLOGY_POINT_LIST */) {
    VkPipelineInputAssemblyStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.topology = topology;
    // TODO: research primitive restart
    info.primitiveRestartEnable = VK_FALSE;
    return info;
  }


  VkPipelineRasterizationStateCreateInfo
  rasterizationStateCreateInfo(VkPolygonMode polygonMode /* = VK_POLYGON_MODE_LINE */) {
    VkPipelineRasterizationStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.depthClampEnable = VK_FALSE;
    /*
     * if rasterizerDiscardEnable is enabled: discards all primitives before the rasterization stage.
     *    i.e. triangles, etc... never get drawn to the screen
     * enable this if you only want the side effects / calculation of the vertex processing stage
     *    i.e. writing to a buffer for later use, etc...
     */
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = polygonMode;
    info.lineWidth = 1.f;
    info.cullMode = VK_CULL_MODE_NONE; // no backface cull
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // no depth bias
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0.f;
    info.depthBiasClamp = 0.f;
    info.depthBiasSlopeFactor = 0.f;
    return info;
  }


  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo() {
    VkPipelineMultisampleStateCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.sampleShadingEnable = VK_FALSE;
    // multisampling defaulted to no multisampling (1 sample per pixel)
    // TODO: enable multisampling
    info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.minSampleShading = 1.f;
    info.pSampleMask = nullptr;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;
    return info;
  }


  VkPipelineColorBlendAttachmentState colorBlendAttachmentState() {
    // TODO: enable multi-attachment blending
    VkPipelineColorBlendAttachmentState state{};
    state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
      VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
    state.blendEnable = VK_FALSE;
    return state;
  }


  VkPipelineLayoutCreateInfo  layoutCreateInfo(){
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    // empty defaults
    // TODO: add layouts and push constants
    info.flags = 0;
    info.setLayoutCount = 0;
    info.pSetLayouts = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges = nullptr;
    return info;
  }




} // walrus