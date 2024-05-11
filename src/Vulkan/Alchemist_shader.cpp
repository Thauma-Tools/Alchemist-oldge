#include "Alchemist_shader.h"

#include <Alchemist/Defines.h>
#include <SDL_iostream.h>

#include "Alchemist_vulkan.h"

namespace tt {

static inline u8 *vulkan_read_spv(const char *path, u64 *size) {
  u8 *data = nullptr;

  SDL_IOStream *handle = SDL_IOFromFile(path, "rb");
  if (handle) {
    *size = SDL_GetIOSize(handle);
    data = new u8[*size];
    SDL_ReadIO(handle, data, *size);
    SDL_CloseIO(handle);
  }

  return data;
}

VulkanShader vulkan_create_shader(VulkanContext *context,
                                  const char *vertex_path,
                                  const char *fragment_path) {
  VulkanShader shader;

  u64 code_size;
  u8 *vertex_code = vulkan_read_spv(vertex_path, &code_size);
  VkShaderModuleCreateInfo vertex_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code_size,
      .pCode = reinterpret_cast<u32 *>(vertex_code),
  };
  vkCreateShaderModule(context->devices.logical, &vertex_create_info, nullptr,
                       &shader.vertex);
  delete[] vertex_code;

  u8 *fragment_code = vulkan_read_spv(fragment_path, &code_size);
  VkShaderModuleCreateInfo fragment_create_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = code_size,
      .pCode = reinterpret_cast<u32 *>(fragment_code),
  };
  vkCreateShaderModule(context->devices.logical, &fragment_create_info, nullptr,
                       &shader.fragment);

  return shader;
}

}  // namespace tt
