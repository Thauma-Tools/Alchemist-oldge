#ifndef ALCHEMIST_RENDERER_H_
#define ALCHEMIST_RENDERER_H_

#include <Alchemist/Defines.h>

#include "Vulkan/Alchemist_pipeline.h"
#include "Vulkan/Alchemist_renderpass.h"
#include "Vulkan/Alchemist_swapchain.h"

namespace tt {

struct RenderState {
  VulkanSwapchain swapchain;
  VulkanRenderpass renderpass;
  VulkanPipeline pipeline;

  u32 current_frame = 0;

  VkCommandPool command_pool;
  VkCommandBuffer current_command_buffer;
};

void renderer_configure(struct VulkanContext *, struct RenderState *);

void renderer_begin_frame(struct VulkanContext *, struct RenderState *);
void renderer_end_frame(struct VulkanContext *, struct RenderState *);

}  // namespace tt

#endif  // ALCHEMIST_RENDERER_H_
