#ifndef ALCHEMIST_WINDOW_H_
#define ALCHEMIST_WINDOW_H_

#include <Alchemist/Defines.h>
#include <SDL_video.h>

namespace tt {

struct Window {
  SDL_Window *window;

  const char *title;
  s32 width, height;
};

}  // namespace tt

#endif  // ALCHEMIST_WINDOW_H_
