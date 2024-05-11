#ifndef ALCHEMIST_VULKAN_H_
#define ALCHEMIST_VULKAN_H_

#include <Alchemist/Defines.h>
#include <vulkan/vulkan.h>

#include "Alchemist_device.h"
#include "Alchemist_swapchain.h"

namespace tt {

struct VulkanContext {
  VkInstance instance;

  VkSurfaceKHR surface;

  VulkanDevice devices;
  VulkanSwapchain swapchain;

  VkDebugUtilsMessengerEXT debug_messenger;
};

s32 vulkan_initialize_context(struct Window *, VulkanContext *);
void vulkan_destroy_context(VulkanContext *);

}  // namespace tt

#endif
