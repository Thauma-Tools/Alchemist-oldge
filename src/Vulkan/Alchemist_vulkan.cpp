#include "Alchemist_vulkan.h"

#include <SDL_vulkan.h>

#include <vector>

#include "../Alchemist_logger.h"
#include "../Alchemist_window.h"

namespace tt {

static const char *VALIDATION_LAYER = "VK_LAYER_KHRONOS_validation";

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                      VkDebugUtilsMessageTypeFlagsEXT type,
                      const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                      void *user_data) {
  ALCH_ERROR("{}", callback_data->pMessage);

  return VK_FALSE;
}

static inline bool vulkan_create_instance(Window *window,
                                          VulkanContext *context) {
  ALCH_DEBUG("Creating VkInstance");

  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = window->title,
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "Alchemist",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_2,
  };

  u32 extension_count = 0;
  const char *const *sdl_extensions =
      SDL_Vulkan_GetInstanceExtensions(&extension_count);

  std::vector<const char *> extensions(sdl_extensions,
                                       sdl_extensions + extension_count);

  bool validation_found = false;
#if defined(ALCHEMIST_DEBUG)
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  ALCH_DEBUG("Enabling validation");
  u32 layer_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  VkLayerProperties *layer_properties =
      (VkLayerProperties *)alloca(sizeof(VkLayerProperties) * layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

  for (s32 i = 0; i < layer_count; i++) {
    if (strcmp(VALIDATION_LAYER, layer_properties[i].layerName) == 0) {
      validation_found = true;
      ALCH_DEBUG("Enabled validation");
      break;
    }
  }
#endif

  VkInstanceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = validation_found ? 1u : 0,
      .ppEnabledLayerNames = &VALIDATION_LAYER,
      .enabledExtensionCount = (u32)extensions.size(),
      .ppEnabledExtensionNames = extensions.data(),
  };

  s32 success = vkCreateInstance(&create_info, nullptr, &context->instance);
  if (success != VK_SUCCESS) {
    ALCH_DEBUG("Failed to create VkInstance");
    return false;
  }
  ALCH_DEBUG("Created VkInstance - {}", (void *)context->instance);

  return true;
}

static inline void vulkan_create_surface(Window *window,
                                         VulkanContext *context) {
  ALCH_DEBUG("Creating surface");
  SDL_Vulkan_CreateSurface(window->window, context->instance, nullptr,
                           &context->surface);
  ALCH_DEBUG("Created surface");
}

static inline void vulkan_setup_messenger(VulkanContext *context) {
  ALCH_DEBUG("Creating debug messenger");
  VkDebugUtilsMessengerCreateInfoEXT create_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vulkan_debug_callback,
      .pUserData = nullptr,
  };

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      context->instance, "vkCreateDebugUtilsMessengerEXT");
  VkResult result =
      func(context->instance, &create_info, nullptr, &context->debug_messenger);
  if (result != VK_SUCCESS) {
    ALCH_ERROR("Failed to create debug messenger");
    return;
  }

  ALCH_DEBUG("Created debug messenger - {}", (void *)context->debug_messenger);
}

s32 vulkan_initialize_context(Window *window, VulkanContext *context) {
  if (!vulkan_create_instance(window, context)) return -10;
#if defined(ALCHEMIST_DEBUG)
  vulkan_setup_messenger(context);
#endif
  vulkan_create_surface(window, context);
  if (!vulkan_create_devices(context)) return -20;

  return 0;
}

static inline void vulkan_destroy_debug_messenger(VulkanContext *context) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      context->instance, "vkDestroyDebugUtilsMessengerEXT");
  func(context->instance, context->debug_messenger, nullptr);
}

void vulkan_destroy_context(VulkanContext *context) {
  vkDestroyDevice(context->devices.logical, nullptr);
  vkDestroySurfaceKHR(context->instance, context->surface, nullptr);
#if defined(ALCHEMIST_DEBUG)
  vulkan_destroy_debug_messenger(context);
#endif
  vkDestroyInstance(context->instance, nullptr);
}

}  // namespace tt
