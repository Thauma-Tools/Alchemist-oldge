#include <Alchemist/Logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <unordered_map>

namespace tt {

static std::unordered_map<const char *, std::unique_ptr<spdlog::logger>>
    _Loggers;

static inline void _create(const char *name) {
  auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console->set_pattern("[%^%l%$][%n] %v");

  _Loggers[name] =
      std::make_unique<spdlog::logger>(name, spdlog::sinks_init_list{console});
  _Loggers[name]->set_level(spdlog::level::debug);
}

spdlog::logger &Logger::get(const char *name) {
  if (_Loggers[name] == nullptr) _create(name);

  return *_Loggers[name];
}

}  // namespace tt
