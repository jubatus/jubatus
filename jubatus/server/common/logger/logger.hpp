#ifndef JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
#define JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_

#include <sstream>
#include <log4cxx/level.h>

#include <jubatus/util/lang/noncopyable.h>

#define LOG(level) LOG_##level(level)

#define FATAL ::log4cxx::Level::getFatal()
#define ERROR ::log4cxx::Level::getError()
#define WARN  ::log4cxx::Level::getWarn()
#define INFO  ::log4cxx::Level::getInfo()
#define DEBUG ::log4cxx::Level::getDebug()
#define TRACE ::log4cxx::Level::getTrace()

// Internal macros
#define STREAM_LOGGER(level) \
    ::jubatus::server::common::logger::stream_logger(level, __FILE__, __LINE__)

#ifdef NDEBUG
#define DEBUG_ONLY true ? (void) 0 : \
                          ::jubatus::server::common::logger::voidify() &
#else
#define DEBUG_ONLY
#endif

#define LOG_FATAL(level) STREAM_LOGGER(level)
#define LOG_ERROR(level) STREAM_LOGGER(level)
#define LOG_WARN(level)  STREAM_LOGGER(level)
#define LOG_INFO(level)  STREAM_LOGGER(level)
#define LOG_DEBUG(level) DEBUG_ONLY STREAM_LOGGER(level)
#define LOG_TRACE(level) DEBUG_ONLY STREAM_LOGGER(level)

// Deprecated (for glog transition)
#define WARNING WARN
#define LOG_WARNING LOG_WARN
#define DLOG(level) LOG(DEBUG)

namespace jubatus {
namespace server {
namespace common {
namespace logger {

class stream_logger : jubatus::util::lang::noncopyable {
 public:
  stream_logger(
      const log4cxx::LevelPtr level,
      const char* file,
      const int line);
  ~stream_logger();

  template <typename T>
  inline stream_logger& operator<<(const T& msg) {
    buf_ << msg;
    return *this;
  }

 private:
  const log4cxx::LevelPtr level_;
  const char* file_;
  const int line_;
  const int thread_id_;
  std::ostringstream buf_;

};

class voidify {
 public:
  voidify() {}
  void operator&(const stream_logger&) {}
};

/**
 * Bind parameters for the logging library.
 * Must be called before `configure` to take effect.
 */
void setup_parameters(const char* progname, const char* host, const int port);

/**
 * Configures the logging library. (standard output)
 */
void configure();

/**
 * Configures the logging library with given config file.
 */
void configure(const std::string&);

}  // namespace logger
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
