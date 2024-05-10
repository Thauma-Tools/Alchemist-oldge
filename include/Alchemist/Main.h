#ifndef ALCHEMIST_MAIN_H_
#define ALCHEMIST_MAIN_H_

#if !defined(ALCHEMIST_EXPORT_BUILD)
extern int __alchemist_entry(int, char **);

#if defined(ALCHEMIST_PLATFORM_WINDOWS)
#else
int main(int argc, char **argv) { return __alchemist_entry(argc, argv); }
#endif

#define main __alchemist_main
#endif

#endif  // ALCHEMIST_MAIN_H_
