#ifndef ALCHEMIST_PIPELINE_H_
#define ALCHEMIST_PIPELINE_H_

#include <vulkan/vulkan.h>

#include "Alchemist_shader.h"

namespace tt {

struct VulkanPipeline {
  VkPipeline handle;
  VkPipelineLayout layout;

  VulkanShader shader;
};

VulkanPipeline vulkan_create_pipeline(struct VulkanContext *,
                                      struct VulkanRenderpass *,
                                      struct RenderState *);

}  // namespace tt

#endif  // ALCHEMIST_PIPELINE_H_
