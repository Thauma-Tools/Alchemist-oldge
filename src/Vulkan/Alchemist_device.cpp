#include "Alchemist_device.h"

#include <set>
#include <vector>

#include "../Alchemist_logger.h"
#include "Alchemist_vulkan.h"

namespace tt {

static const std::vector<const char *> _DeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

struct VulkanQueueIndices {
  u32 graphics, present;

  bool found;
};

static inline VulkanQueueIndices vulkan_find_queue_indices(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
  VulkanQueueIndices indices = {0};

  u32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                           queue_families.data());

  s32 i = 0;
  bool graphics_set = false;
  bool present_set = false;
  for (VkQueueFamilyProperties props : queue_families) {
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
    if (present_support) {
      indices.present = i;
      present_set = true;
    }

    if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = i;
      graphics_set = true;
    }

    if (graphics_set && present_set) {
      indices.found = true;
      break;
    }

    i++;
  }

  return indices;
}

static inline bool vulkan_check_extension_support(VkPhysicalDevice device) {
  u32 extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       extensions.data());

  std::set<std::string> required_extensions(_DeviceExtensions.begin(),
                                            _DeviceExtensions.end());

  for (auto extension : extensions) {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
}

static inline bool vulkan_device_suitable(VulkanContext *context,
                                          VkPhysicalDevice device) {
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;

  vkGetPhysicalDeviceProperties(device, &properties);
  vkGetPhysicalDeviceFeatures(device, &features);

  VulkanQueueIndices indices =
      vulkan_find_queue_indices(device, context->surface);
  bool extensions_supported = vulkan_check_extension_support(device);
  bool swapchain_good_enough = false;

  if (extensions_supported) {
    VulkanSwapchainDetails details =
        vulkan_query_swapchain_details(device, context);
    swapchain_good_enough =
        !details.formats.empty() && !details.present_modes.empty();
  }

  bool suitable =
      (indices.found) && extensions_supported && swapchain_good_enough;

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
    if (vulkan_device_suitable(context, device)) {
      return device;
    }
  }

  return VK_NULL_HANDLE;
}

static inline bool vulkan_create_logical_device(VulkanContext *context) {
  ALCH_DEBUG("Creating logical device");

  VkPhysicalDeviceFeatures device_features = {};

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
      .enabledExtensionCount = (u32)_DeviceExtensions.size(),
      .ppEnabledExtensionNames = _DeviceExtensions.data(),
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
  VulkanQueueIndices indices =
      vulkan_find_queue_indices(context->devices.physical, context->surface);
  context->devices.graphics_queue_index = indices.graphics;
  context->devices.present_queue_index = indices.present;
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
