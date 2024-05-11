#ifndef ALCHEMIST_MAIN_H_
#define ALCHEMIST_MAIN_H_

#if !defined(ALCHEMIST_EXPORT_BUILD)
#include <SDL_main.h>

extern int __alchemist_entry(int, char **);
int __alchemist_main(int, char **);

int main(int argc, char **argv) {
  int result = __alchemist_main(argc, argv);
  if (result != 0) return result;

  return __alchemist_entry(argc, argv);
}
#endif

#undef main
#define main __alchemist_main

#endif  // ALCHEMIST_MAIN_H_
