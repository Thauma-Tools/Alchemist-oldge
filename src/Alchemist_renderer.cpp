#include "Alchemist_renderer.h"

#include <Alchemist/Defines.h>

#include "Vulkan/Alchemist_vulkan.h"

namespace tt {

void renderer_configure(struct VulkanContext *context,
                        struct RenderState *state) {
  state->swapchain.frame_buffers.resize(state->swapchain.image_views.size());

  for (s32 i = 0; i < state->swapchain.image_views.size(); i++) {
    VkImageView attachments[] = {
        state->swapchain.image_views[i],
    };

    VkFramebufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = state->renderpass.handle,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = state->swapchain.extent.width,
        .height = state->swapchain.extent.height,
        .layers = 1,
    };

    vkCreateFramebuffer(context->devices.logical, &create_info, nullptr,
                        &state->swapchain.frame_buffers[i]);
  }

  VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = context->devices.graphics_queue_index,
  };

  vkCreateCommandPool(context->devices.logical, &pool_info, nullptr,
                      &state->command_pool);

  VkCommandBufferAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = state->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  vkAllocateCommandBuffers(context->devices.logical, &alloc_info,
                           &state->current_command_buffer);
}

void renderer_begin_frame(struct VulkanContext *context,
                          struct RenderState *state) {
  vkWaitForFences(context->devices.logical, 1,
                  &state->swapchain.frame_in_flight, VK_TRUE, UINT64_MAX);
  vkResetFences(context->devices.logical, 1, &state->swapchain.frame_in_flight);
  vkResetCommandBuffer(state->current_command_buffer, 0);

  vkAcquireNextImageKHR(context->devices.logical, state->swapchain.swapchain,
                        UINT64_MAX, state->swapchain.image_available,
                        VK_NULL_HANDLE, &state->current_frame);

  VkCommandBufferBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
  };

  vkBeginCommandBuffer(state->current_command_buffer, &begin_info);

  VkClearValue clear_color = {{{0.f, 0.f, 0.f, 1.f}}};

  VkRenderPassBeginInfo renderpass_begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = state->renderpass.handle,
      .framebuffer = state->swapchain.frame_buffers[state->current_frame],
      .renderArea =
          {
              .offset = {0, 0},
              .extent = state->swapchain.extent,
          },
      .clearValueCount = 1,
      .pClearValues = &clear_color,
  };

  vkCmdBeginRenderPass(state->current_command_buffer, &renderpass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(state->current_command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS, state->pipeline.handle);

  VkViewport viewport = {
      .x = 0.f,
      .y = 0.f,
      .width = (f32)state->swapchain.extent.width,
      .height = (f32)state->swapchain.extent.height,
      .minDepth = 0.f,
      .maxDepth = 1.f,
  };
  vkCmdSetViewport(state->current_command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {
      .offset = {0, 0},
      .extent = state->swapchain.extent,
  };
  vkCmdSetScissor(state->current_command_buffer, 0, 1, &scissor);
}

void renderer_end_frame(struct VulkanContext *context,
                        struct RenderState *state) {
  vkCmdEndRenderPass(state->current_command_buffer);
  vkEndCommandBuffer(state->current_command_buffer);

  VkSemaphore wait[] = {state->swapchain.image_available};
  VkSemaphore signal[] = {state->swapchain.render_finished};
  VkPipelineStageFlags stages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = wait,
      .pWaitDstStageMask = stages,
      .commandBufferCount = 1,
      .pCommandBuffers = &state->current_command_buffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = signal,
  };

  vkQueueSubmit(context->devices.graphics_queue, 1, &submit_info,
                state->swapchain.frame_in_flight);

  VkSwapchainKHR swapchains[] = {state->swapchain.swapchain};

  VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = signal,
      .swapchainCount = 1,
      .pSwapchains = swapchains,
      .pImageIndices = &state->current_frame,
      .pResults = nullptr,
  };

  vkQueuePresentKHR(context->devices.present_queue, &present_info);
}

}  // namespace tt
