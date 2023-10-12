#include "render_pass.hpp"

#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <cassert>

namespace walrus {

  bool RenderPass::CreateInfo::isEmpty() const {
    return attachmentDescriptions.empty() && attachmentReferences.empty() && subPasses.empty();
  }

  void RenderPass::CreateInfo::updateCreateInfo() {
    createInfo.attachmentCount = attachmentDescriptions.size();
    createInfo.pAttachments = attachmentDescriptions.data();
    createInfo.subpassCount = subPasses.size();
    createInfo.pSubpasses = subPasses.data();
  }

  void RenderPass::GetDefaultRenderPassCreateInfo(
    RenderPass::CreateInfo &createInfo,
    VkFormat &vkSwapchainImageFormat
  ) {
    assert(createInfo.isEmpty() && "You shouldn't call this function on a non-empty struct");

    size_t index_desc = createInfo.attachmentDescriptions.size();
    createInfo.attachmentDescriptions.push_back(VkAttachmentDescription{});
    createInfo.attachmentReferences.push_back(VkAttachmentReference{});
    createInfo.subPasses.push_back(VkSubpassDescription{});
    auto colorAttachment = &createInfo.attachmentDescriptions.back();
    auto attachmentRef = &createInfo.attachmentReferences.back();
    auto subpass = &createInfo.subPasses.back();

    colorAttachment->format = vkSwapchainImageFormat;
    colorAttachment->samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
    colorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear when attachment is loaded
    colorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store attachment after renderpass ends
    // don't care about stencil
    colorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // don't know or care about starting layout
    colorAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // after the renderpass ends, the image hs to be on a layout ready for display
    colorAttachment->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentRef->attachment = index_desc; // indexes into _renderpass.pAttachments
    attachmentRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // TODO: research sub pass
    // 1 sub pass is minimum
    subpass->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass->colorAttachmentCount = createInfo.attachmentReferences.size();
    subpass->pColorAttachments = createInfo.attachmentReferences.data();

    // TODO: refactor for depth test
    createInfo.updateCreateInfo();
  }


} // walrus