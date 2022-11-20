#include "pipeline_builder.hpp"

#include "pretty_io.hpp"
#include <iostream>

namespace walrus {

  PipelineBuilder::PipelineBuilder() {
    init();
  }

  PipelineBuilder::PipelineBuilder(VkExtent2D &extent) {
    init();
    this->viewport.width = (float)extent.width;
    this->viewport.height = (float)extent.height;
    this->scissor.extent = extent;
  }

  void PipelineBuilder::init() {
    this->scissor.offset = { 0, 0 };
    this->viewport.x = 0.f;
    this->viewport.y = 0.f;
  }

  void PipelineBuilder::build(VkDevice vkDevice, VkRenderPass vkRenderPass, VkPipeline *outPipeline) {
    /// VIEWPORT
    // TODO: enable multiple viewports
    VkPipelineViewportStateCreateInfo viewportCreate = {};
    viewportCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCreate.pNext = nullptr;
    viewportCreate.viewportCount = 1;
    viewportCreate.pViewports = &this->viewport;
    viewportCreate.scissorCount = 1;
    viewportCreate.pScissors = &this->scissor;

    /// COLOR BLEND
    // TODO: enable true color blending logic
    VkPipelineColorBlendStateCreateInfo colorBlendCreate{}; /// NOTE: this has to match fragment shader outputs
    colorBlendCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreate.pNext = nullptr;
    colorBlendCreate.logicOpEnable = VK_FALSE;
    colorBlendCreate.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCreate.attachmentCount = 1;
    colorBlendCreate.pAttachments = &this->colorBlendAttachment;

    /// PIPELINE
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pNext = nullptr;
    info.stageCount = this->shaderStagesCreate.size();
    info.pStages = this->shaderStagesCreate.data();
    info.pVertexInputState = &this->vertexInputCreate;
    info.pInputAssemblyState = &this->inputAssemblyCreate;
    info.pRasterizationState = &this->rasterizerCreate;
    info.pMultisampleState = &this->multisampleCreate;
    info.pViewportState = &viewportCreate;
    info.pColorBlendState = &colorBlendCreate;
    info.renderPass = vkRenderPass;
    info.layout = this->pipelineLayout;
    info.subpass = 0;
    info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
      vkDevice,
      VK_NULL_HANDLE,
      1,
      &info,
      nullptr,
      outPipeline
    ) != VK_SUCCESS) {
      vkDestroyPipeline(vkDevice, *outPipeline, nullptr);
      std::cout << io::to_color_string(io::RED, "failed to create pipeline");
      outPipeline = VK_NULL_HANDLE;
    }
  }

} // walrus