#include "Alchemist_device.h"

#include <set>
#include <vector>

#include "../Alchemist_logger.h"
#include "Alchemist_vulkan.h"

namespace tt {

static inline bool vulkan_device_suitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;

  vkGetPhysicalDeviceProperties(device, &properties);
  vkGetPhysicalDeviceFeatures(device, &features);

  bool suitable =
      properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
      features.geometryShader;
  if (suitable) {
    ALCH_DEBUG("Picked GPU\n\t- {}", properties.deviceName);
    return true;
  }

  return false;
}

static inline VkPhysicalDevice vulkan_pick_physical_device(
    VulkanContext *context) {
  ALCH_DEBUG("Picking GPU");

  u32 device_count = 0;
  vkEnumeratePhysicalDevices(context->instance, &device_count, nullptr);
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(context->instance, &device_count, devices.data());

  for (auto device : devices) {
    if (vulkan_device_suitable(device)) {
      return device;
    }
  }

  return VK_NULL_HANDLE;
}

static inline void vulkan_find_queue_indices(VulkanContext *context) {
  u32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(context->devices.physical,
                                           &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      context->devices.physical, &queue_family_count, queue_families.data());

  u32 i = 0;
  for (VkQueueFamilyProperties props : queue_families) {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(context->devices.physical, i,
                                         context->surface, &present_support);
    if (present_support) context->devices.present_queue_index = i;

    if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      context->devices.graphics_queue_index = i;
    }

    if (context->devices.graphics_queue_index > 0 &&
        context->devices.present_queue_index > 0)
      break;

    i++;
  }
}

static inline bool vulkan_create_logical_device(VulkanContext *context) {
  ALCH_DEBUG("Creating logical device");

  VkPhysicalDeviceFeatures device_features = {

  };

  float priority = 1.f;
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<u32> unique_queue_families = {context->devices.graphics_queue_index,
                                         context->devices.present_queue_index};

  for (u32 queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    queue_create_infos.push_back(queue_create_info);
  }

  VkDeviceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = (u32)queue_create_infos.size(),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledExtensionCount = 0,
      .pEnabledFeatures = &device_features,
  };

  VkResult result = vkCreateDevice(context->devices.physical, &create_info,
                                   nullptr, &context->devices.logical);
  if (result != VK_SUCCESS) return false;

  ALCH_DEBUG("Created logical device");

  return true;
}

bool vulkan_create_devices(VulkanContext *context) {
  context->devices.physical = vulkan_pick_physical_device(context);
  vulkan_find_queue_indices(context);
  vulkan_create_logical_device(context);

  vkGetDeviceQueue(context->devices.logical,
                   context->devices.graphics_queue_index, 0,
                   &context->devices.graphics_queue);
  vkGetDeviceQueue(context->devices.logical,
                   context->devices.present_queue_index, 0,
                   &context->devices.present_queue);

  return true;
}

}  // namespace tt
