#ifndef ALCHEMIST_LOGGER_INTERNAL_H_
#define ALCHEMIST_LOGGER_INTERNAL_H_

#include <Alchemist/Logger.h>

#define ALCH_DEBUG(...) tt::Logger::get().debug(__VA_ARGS__)
#define ALCH_ERROR(...) tt::Logger::get().error(__VA_ARGS__)
#define ALCH_INFO(...) tt::Logger::get().info(__VA_ARGS__)
#define ALCH_WARN(...) tt::Logger::get().warn(__VA_ARGS__)

#endif  // ALCHEMIST_LOGGER_INTERNAL_H_
