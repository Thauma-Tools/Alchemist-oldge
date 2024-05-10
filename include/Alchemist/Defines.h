#ifndef ALCHEMIST_DEFINES_H_
#define ALCHEMIST_DEFINES_H_

#if defined(ALCHEMIST_EXPORT_BUILD)
#if defined(ALCHEMIST_PLATFORM_WINDOWS)
#define ALCH_EXPORT __declspec(dllexport)
#elif defined(ALCHEMIST_PLATFORM_LINUX)
#define ALCH_EXPORT __attribute__((visibility("default")))
#endif
#else
#if defined(ALCHEMIST_PLATFORM_WINDOWS)
#define ALCH_EXPORT __declspec(dllimport)
#elif defined(ALCHEMIST_PLATFORM_LINUX)
#endif
#define ALCH_EXPORT
#endif

using s8 = signed char;
using s16 = short int;
using s32 = int;
using s64 = long int;

using u8 = unsigned char;
using u16 = unsigned short int;
using u32 = unsigned int;
using u64 = unsigned long int;

using f32 = float;
using f64 = double;

#endif  // ALCHEMIST_DEFINES_H_
