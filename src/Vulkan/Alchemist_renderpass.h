#ifndef ALCHEMIST_RENDERPASS_H_
#define ALCHEMIST_RENDERPASS_H_

#include <vulkan/vulkan.h>

namespace tt {

struct VulkanRenderpass {
  VkRenderPass handle;
};

VulkanRenderpass vulkan_create_renderpass(struct VulkanContext *,
                                          struct RenderState *);

}  // namespace tt

#endif  // ALCHEMIST_RENDERPASS_H_
