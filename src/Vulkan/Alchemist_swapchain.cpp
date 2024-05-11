#include "Alchemist_swapchain.h"

#include <SDL.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <limits>
#include <vector>

#include "../Alchemist_logger.h"
#include "../Alchemist_window.h"
#include "Alchemist_device.h"
#include "Alchemist_vulkan.h"

namespace tt {

VulkanSwapchainDetails vulkan_query_swapchain_details(VkPhysicalDevice device,
                                                      VulkanContext *context) {
  VulkanSwapchainDetails details = {0};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, context->surface,
                                            &details.capabilities);

  u32 format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, context->surface, &format_count,
                                       nullptr);
  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, context->surface,
                                         &format_count, details.formats.data());
  }

  u32 present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, context->surface,
                                            &present_mode_count, nullptr);
  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, context->surface,
                                              &present_mode_count,
                                              details.present_modes.data());
  }

  return details;
}

static inline VkSurfaceFormatKHR vulkan_preferred_format(
    const std::vector<VkSurfaceFormatKHR> &formats) {
  for (const auto &format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return format;
  }

  return formats[0];
}

static inline VkPresentModeKHR vulkan_preferred_present(
    const std::vector<VkPresentModeKHR> &present_modes) {
  for (const auto &present_mode : present_modes)
    if (present_mode == VK_PRESENT_MODE_FIFO_KHR) return present_mode;

  return VK_PRESENT_MODE_FIFO_KHR;
}

static inline VkExtent2D vulkan_choose_extent(
    Window *window, const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
    return capabilities.currentExtent;
  else {
    s32 width, height;
    SDL_GetWindowSizeInPixels(window->window, &width, &height);

    VkExtent2D actual = {
        (u32)width,
        (u32)height,
    };

    actual.width = std::clamp(actual.width, capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    actual.height =
        std::clamp(actual.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actual;
  }

}  // namespace tt

VulkanSwapchain vulkan_create_swapchain(Window *window,
                                        VulkanContext *context) {
  VulkanSwapchain swapchain;

  VulkanSwapchainDetails details =
      vulkan_query_swapchain_details(context->devices.physical, context);

  VkSurfaceFormatKHR format = vulkan_preferred_format(details.formats);
  VkPresentModeKHR present_mode =
      vulkan_preferred_present(details.present_modes);
  VkExtent2D extent = vulkan_choose_extent(window, details.capabilities);

  u32 image_count = details.capabilities.minImageCount + 1;
  if (details.capabilities.maxImageCount > 0 &&
      image_count > details.capabilities.maxImageCount)
    image_count = details.capabilities.maxImageCount;

  u32 indices[] = {context->devices.graphics_queue_index,
                   context->devices.present_queue_index};

  VkSwapchainCreateInfoKHR create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = context->surface,
      .minImageCount = image_count,
      .imageFormat = format.format,
      .imageColorSpace = format.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = indices[0] != indices[1] ? VK_SHARING_MODE_CONCURRENT
                                                   : VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = indices[0] != indices[1] ? 2u : 0,
      .pQueueFamilyIndices = indices[0] != indices[1] ? indices : nullptr,
      .preTransform = details.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = present_mode,
      .clipped = VK_TRUE,
  };

  vkCreateSwapchainKHR(context->devices.logical, &create_info, nullptr,
                       &swapchain.swapchain);
  ALCH_DEBUG("Swapchain: {}", (void *)swapchain.swapchain);

  vkGetSwapchainImagesKHR(context->devices.logical, swapchain.swapchain,
                          &image_count, nullptr);
  swapchain.images.resize(image_count);
  vkGetSwapchainImagesKHR(context->devices.logical, swapchain.swapchain,
                          &image_count, swapchain.images.data());

  swapchain.format = format.format;
  swapchain.extent = extent;

  swapchain.image_views.resize(image_count);
  for (s32 i = 0; i < image_count; i++) {
    VkImageViewCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = swapchain.images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchain.format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    vkCreateImageView(context->devices.logical, &create_info, nullptr,
                      &swapchain.image_views[i]);
  }

  VkSemaphoreCreateInfo semaphore_info = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  vkCreateSemaphore(context->devices.logical, &semaphore_info, nullptr,
                    &swapchain.image_available);
  vkCreateSemaphore(context->devices.logical, &semaphore_info, nullptr,
                    &swapchain.render_finished);

  VkFenceCreateInfo fence_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  vkCreateFence(context->devices.logical, &fence_info, nullptr,
                &swapchain.frame_in_flight);

  return swapchain;
}

}  // namespace tt
