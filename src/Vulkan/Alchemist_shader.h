#ifndef ALCHEMIST_SHADER_H_
#define ALCHEMIST_SHADER_H_

#include <vulkan/vulkan.h>

namespace tt {

struct VulkanShader {
  VkShaderModule vertex, fragment;
};

VulkanShader vulkan_create_shader(struct VulkanContext *, const char *,
                                  const char *);

}  // namespace tt

#endif  // ALCHEMIST_SHADER_H_
