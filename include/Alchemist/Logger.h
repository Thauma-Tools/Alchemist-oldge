#ifndef ALCHEMIST_LOGGER_H_
#define ALCHEMIST_LOGGER_H_

#include <spdlog/logger.h>

namespace tt::Logger {

spdlog::logger &get(const char * = "Alchemist");

}

#endif  // ALCHEMIST_LOGGER_H_
