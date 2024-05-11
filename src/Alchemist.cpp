#include <Alchemist/Application.h>
#include <Alchemist/Defines.h>
#include <Alchemist/Logger.h>
#include <SDL.h>

#include <iostream>

#include "Alchemist_logger.h"
#include "Alchemist_renderer.h"
#include "Alchemist_window.h"
#include "Vulkan/Alchemist_pipeline.h"
#include "Vulkan/Alchemist_renderpass.h"
#include "Vulkan/Alchemist_vulkan.h"

namespace tt {

static tt::Application *_Application = {0};
static tt::Window _Window = {0};
static tt::VulkanContext _Vulkan = {0};

Application::Application() : _window(&_Window) {
  _window->title = "Alchemist Application";
  _window->width = 1280;
  _window->height = 720;
}

void *Application::operator new(size_t size) {
  if (_Application == nullptr)
    _Application = (Application *)::operator new(size);

  return _Application;
}

}  // namespace tt

extern int __alchemist_main(int, char **);

static inline s32 __alchemist_initialize_window() {
  ALCH_DEBUG("Initializing GLFW");
  if (SDL_Init(SDL_INIT_VIDEO) != 0) return -1;
  ALCH_DEBUG("Initialized GLFW");

  ALCH_DEBUG("Creating Window");
  tt::_Window.window = SDL_CreateWindow(tt::_Window.title, tt::_Window.width,
                                        tt::_Window.height, SDL_WINDOW_VULKAN);
  if (tt::_Window.window == nullptr) return -2;
  ALCH_DEBUG("Created Window");

  return 0;
}

static inline void __alchemist_run() {
  bool running = true;

  tt::RenderState render_state;
  render_state.swapchain =
      tt::vulkan_create_swapchain(&tt::_Window, &tt::_Vulkan);

  tt::VulkanRenderpass renderpass =
      tt::vulkan_create_renderpass(&tt::_Vulkan, &render_state);
  tt::VulkanPipeline pipeline =
      tt::vulkan_create_pipeline(&tt::_Vulkan, &renderpass, &render_state);
  render_state.renderpass = renderpass;
  render_state.pipeline = pipeline;

  tt::renderer_configure(&tt::_Vulkan, &render_state);

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT:
          running = false;
          break;
        case SDL_EVENT_KEY_UP:
          if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
          break;
        default:
          break;
      }
    }

    tt::renderer_begin_frame(&tt::_Vulkan, &render_state);
    vkCmdDraw(render_state.current_command_buffer, 3, 1, 0, 0);
    tt::renderer_end_frame(&tt::_Vulkan, &render_state);
  }
}

static inline void __alchemist_shutdown() {
  tt::vulkan_destroy_context(&tt::_Vulkan);

  SDL_DestroyWindow(tt::_Window.window);
  SDL_Quit();
}

int ALCH_EXPORT __alchemist_entry(int argc, char **argv) {
  std::cout << "Hello from Alchemist!" << std::endl;

  int result = __alchemist_initialize_window();
  if (result != 0) return result;

  result = tt::vulkan_initialize_context(&tt::_Window, &tt::_Vulkan);
  if (result != 0) return result;

  __alchemist_run();

  ALCH_DEBUG("Shutting down Alchemist");
  __alchemist_shutdown();

  return 0;
}
