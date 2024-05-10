#ifndef ALCHEMIST_DEVICE_H_
#define ALCHEMIST_DEVICE_H_

#include <Alchemist/Defines.h>
#include <vulkan/vulkan.h>

namespace tt {

struct VulkanDevice {
  VkPhysicalDevice physical;
  VkDevice logical;

  u32 graphics_queue_index;
  u32 present_queue_index;

  VkQueue graphics_queue;
  VkQueue present_queue;
};

bool vulkan_create_devices(struct VulkanContext *);

}  // namespace tt

#endif  // ALCHEMIST_DEVICE_H_
