#include "logger.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/xml/domconfigurator.h>

#include <jubatus/util/lang/cast.h>

#define LOGGER_NAME "jubatus"

namespace jubatus {
namespace server {
namespace common {
namespace logger {

#define DEFINE_LOG_LEVEL(level, likeliness) \
    void log##level( \
        const std::string& msg, \
        const char* file, \
        const int line) { \
      log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(LOGGER_NAME); \
      if (likeliness(logger->is##level##Enabled())) { \
        logger->forcedLog( \
            log4cxx::Level::get##level(), \
            msg, \
            log4cxx::spi::LocationInfo(file, "", line)); \
      } \
    }

#define LOGGER_LIKELY
#define LOGGER_UNLIKELY LOG4CXX_UNLIKELY

DEFINE_LOG_LEVEL(Fatal, LOGGER_LIKELY)
DEFINE_LOG_LEVEL(Error, LOGGER_LIKELY)
DEFINE_LOG_LEVEL(Warn,  LOGGER_LIKELY)
DEFINE_LOG_LEVEL(Info,  LOGGER_LIKELY)
DEFINE_LOG_LEVEL(Debug, LOGGER_UNLIKELY)
DEFINE_LOG_LEVEL(Trace, LOGGER_UNLIKELY)

namespace {

void setup_environment() {
  std::string pid = jubatus::util::lang::lexical_cast<std::string>(::getpid());
  ::setenv("JUBATUS_PID", pid.c_str(), 1); // overwrite environment variable
}

}  // namespace

stream_logger::stream_logger(
    const int level,
    const char* file,
    const int line)
    : level_(level), file_(file), line_(line) {}

stream_logger::~stream_logger() {
  switch(level_) {
  case FATAL:
    logFatal(buf_.str(), file_, line_);
    break;
  case ERROR:
    logError(buf_.str(), file_, line_);
    break;
  case WARN:
    logWarn(buf_.str(), file_, line_);
    break;
  case INFO:
    logInfo(buf_.str(), file_, line_);
    break;
  case DEBUG:
    logDebug(buf_.str(), file_, line_);
    break;
  case TRACE:
    logTrace(buf_.str(), file_, line_);
    break;
  }
}

void init() {
  setup_environment();
  log4cxx::BasicConfigurator::configure();
}

void init(const std::string& config_file) {
  setup_environment();

  // Exception will not be thrown even if there is an error in config file.
  log4cxx::xml::DOMConfigurator::configure(config_file);
}

}  // namespace logger
}  // namespace common
}  // namespace server
}  // namespace jubatus
