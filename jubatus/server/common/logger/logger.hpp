#ifndef JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
#define JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_

#include <sstream>
#include <log4cxx/level.h>

#include <jubatus/util/lang/noncopyable.h>

#define LOG(level) LOG_##level(level)

#define FATAL ::log4cxx::Level::FATAL_INT
#define ERROR ::log4cxx::Level::ERROR_INT
#define WARN  ::log4cxx::Level::WARN_INT
#define INFO  ::log4cxx::Level::INFO_INT
#define DEBUG ::log4cxx::Level::DEBUG_INT
#define TRACE ::log4cxx::Level::TRACE_INT

// Internal macros
#define STREAM_LOGGER(level) \
    ::jubatus::server::common::logger::stream_logger(level, __FILE__, __LINE__)

#ifdef NDEBUG
#define DEBUG_ONLY true ? (void) 0 : ::logger::voidify() & // compile away
#else
#define DEBUG_ONLY
#endif

#define LOG_FATAL(level) STREAM_LOGGER(level)
#define LOG_ERROR(level) STREAM_LOGGER(level)
#define LOG_WARN(level)  STREAM_LOGGER(level)
#define LOG_INFO(level)  STREAM_LOGGER(level)
#define LOG_DEBUG(level) DEBUG_ONLY STREAM_LOGGER(level)
#define LOG_TRACE(level) DEBUG_ONLY STREAM_LOGGER(level)

namespace jubatus {
namespace server {
namespace common {
namespace logger {

class stream_logger : jubatus::util::lang::noncopyable {
 public:
  stream_logger(const int level, const char* file, const int line);
  ~stream_logger();

  template <typename T>
  inline stream_logger& operator<<(const T& msg) {
    buf_ << msg;
    return *this;
  }

 private:
  const int level_;
  const char* file_;
  const int line_;
  std::ostringstream buf_;

};

class voidify {
 public:
  voidify() {}
  void operator&(const stream_logger&) {}
};

/**
 * Initializes the logging library. (standard output)
 */
void init();

/**
 * Initializes the logging library with given config file.
 */
void init(const std::string&);

}  // namespace logger
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
