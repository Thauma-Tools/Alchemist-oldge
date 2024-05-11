#include "Alchemist_pipeline.h"

#include <Alchemist/Defines.h>

#include <vector>

#include "../Alchemist_renderer.h"
#include "Alchemist_renderpass.h"
#include "Alchemist_vulkan.h"

namespace tt {

VulkanPipeline vulkan_create_pipeline(VulkanContext *context,
                                      VulkanRenderpass *render_pass,
                                      RenderState *render_state) {
  VulkanPipeline pipeline = {0};

  pipeline.shader = vulkan_create_shader(context, "assets/default_vert.spv",
                                         "assets/default_frag.spv");

  VkPipelineShaderStageCreateInfo vertex_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = pipeline.shader.vertex,
      .pName = "main",
  };
  VkPipelineShaderStageCreateInfo fragment_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = pipeline.shader.fragment,
      .pName = "main",
  };
  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_info,
                                                     fragment_info};

  std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = (u32)dynamic_states.size(),
      .pDynamicStates = dynamic_states.data(),
  };

  VkPipelineVertexInputStateCreateInfo vertex_input = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr,
  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport = {
      .x = 0.f,
      .y = 0.f,
      .width = (f32)render_state->swapchain.extent.width,
      .height = (f32)render_state->swapchain.extent.height,
      .minDepth = 0.f,
      .maxDepth = 1.f,
  };

  VkRect2D scissor = {
      .offset = {.x = 0, .y = 0},
      .extent = render_state->swapchain.extent,
  };

  VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.f,
  };

  VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
  };

  VkPipelineColorBlendAttachmentState color_attachment = {
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo blend_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &color_attachment,
  };

  VkPipelineLayoutCreateInfo pipeline_layout = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };

  vkCreatePipelineLayout(context->devices.logical, &pipeline_layout, nullptr,
                         &pipeline.layout);

  VkGraphicsPipelineCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = shader_stages,
      .pVertexInputState = &vertex_input,
      .pInputAssemblyState = &input_assembly,
      .pViewportState = &viewport_state,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &blend_state,
      .pDynamicState = &dynamic_state,
      .layout = pipeline.layout,
      .renderPass = render_pass->handle,
      .subpass = 0,
  };

  vkCreateGraphicsPipelines(context->devices.logical, VK_NULL_HANDLE, 1,
                            &create_info, nullptr, &pipeline.handle);

  return pipeline;
}

}  // namespace tt
