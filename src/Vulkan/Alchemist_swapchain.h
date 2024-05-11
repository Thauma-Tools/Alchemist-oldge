#ifndef ALCHEMIST_SWAPCHAIN_H_
#define ALCHEMIST_SWAPCHAIN_H_

#include <vulkan/vulkan.h>

#include <vector>

namespace tt {

struct VulkanSwapchain {
  VkSwapchainKHR swapchain;

  VkFormat format;
  VkExtent2D extent;

  std::vector<VkImage> images;
  std::vector<VkImageView> image_views;

  std::vector<VkFramebuffer> frame_buffers;

  VkSemaphore image_available, render_finished;
  VkFence frame_in_flight;
};

struct VulkanSwapchainDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

VulkanSwapchain vulkan_create_swapchain(struct Window *,
                                        struct VulkanContext *);
VulkanSwapchainDetails vulkan_query_swapchain_details(VkPhysicalDevice,
                                                      struct VulkanContext *);

}  // namespace tt

#endif  // ALCHEMIST_SWAPCHAIN_H_
