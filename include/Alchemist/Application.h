#ifndef ALCHEMIST_APPLICATION_H_
#define ALCHEMIST_APPLICATION_H_

#include "Defines.h"

namespace tt {

class ALCH_EXPORT Application {
 public:
  Application();
  ~Application() = delete;

  void set_window_title(const char *);
  void set_window_width(const s32);
  void set_window_height(const s32);

  void *operator new(u64);

 private:
  struct Window *_window;
};

}  // namespace tt

#endif  // ALCHEMIST_APPLICATION_H_
