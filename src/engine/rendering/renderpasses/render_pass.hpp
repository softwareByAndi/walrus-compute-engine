#ifndef WALRUS_COMPUTE_ENGINE_RENDER_PASS_HPP
#define WALRUS_COMPUTE_ENGINE_RENDER_PASS_HPP

#include <vk_types.h>

#include <vector>

namespace walrus {

  class RenderPass {
  public:

    struct CreateInfo {
      std::vector<VkAttachmentDescription> attachmentDescriptions{};
      std::vector<VkAttachmentReference> attachmentReferences{};
      std::vector<VkSubpassDescription> subPasses{};
      VkRenderPassCreateInfo createInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};

      [[nodiscard]] bool isEmpty() const;

      void updateCreateInfo();
    };

    static void GetDefaultRenderPassCreateInfo(
      RenderPass::CreateInfo &createInfo,
      VkFormat &vkSwapchainFormat
    );

    RenderPass() = delete;

  private:

  };

} // walrus

#endif //WALRUS_COMPUTE_ENGINE_RENDER_PASS_HPP
