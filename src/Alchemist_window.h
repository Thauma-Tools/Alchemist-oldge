#ifndef ALCHEMIST_WINDOW_H_
#define ALCHEMIST_WINDOW_H_

#include <Alchemist/Defines.h>
#include <GLFW/glfw3.h>

namespace tt {

struct Window {
  GLFWwindow *window;

  const char *title;
  s32 width, height;
};

}  // namespace tt

#endif  // ALCHEMIST_WINDOW_H_
