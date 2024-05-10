#include <Alchemist/Application.h>

#include "Alchemist_window.h"

namespace tt {

void Application::set_window_title(const char *title) {
  _window->title = title;
}

void Application::set_window_width(const s32 width) { _window->width = width; }

void Application::set_window_height(const s32 height) {
  _window->height = height;
}

}  // namespace tt
