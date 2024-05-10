#include <Alchemist/Application.h>
#include <Alchemist/Defines.h>
#include <Alchemist/Logger.h>

#include <iostream>

#include "Alchemist_logger.h"
#include "Alchemist_window.h"
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

void *Application::operator new(u64 size) {
  if (_Application == nullptr)
    _Application = (Application *)::operator new(size);

  return _Application;
}

}  // namespace tt

extern int __alchemist_main(int, char **);

static inline s32 __alchemist_initialize_window() {
  ALCH_DEBUG("Initializing GLFW");
  if (!glfwInit()) return -1;
  ALCH_DEBUG("Initialized GLFW");

  ALCH_DEBUG("Creating Window");
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  tt::_Window.window = glfwCreateWindow(tt::_Window.width, tt::_Window.height,
                                        tt::_Window.title, nullptr, nullptr);
  if (tt::_Window.window == nullptr) return -2;
  ALCH_DEBUG("Created Window");

  return 0;
}

static inline void __alchemist_run() {
  while (!glfwWindowShouldClose(tt::_Window.window)) {
    glfwPollEvents();
  }
}

static inline void __alchemist_shutdown() {
  tt::vulkan_destroy_context(&tt::_Vulkan);

  glfwDestroyWindow(tt::_Window.window);
  glfwTerminate();
}

int ALCH_EXPORT __alchemist_entry(int argc, char **argv) {
  s32 result = __alchemist_main(argc, argv);
  if (result != 0) return result;

  std::cout << "Hello from Alchemist!" << std::endl;

  result = __alchemist_initialize_window();
  if (result != 0) return result;

  result = tt::vulkan_initialize_context(&tt::_Window, &tt::_Vulkan);
  if (result != 0) return result;

  __alchemist_run();

  ALCH_DEBUG("Shutting down Alchemist");
  __alchemist_shutdown();

  return 0;
}
