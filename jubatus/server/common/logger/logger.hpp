// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Networks and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
#define JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_

#include <log4cxx/level.h>
#include <jubatus/util/lang/noncopyable.h>

#include <string>
#include <sstream>

#define LOG(level) LOG_##level(level)

#define FATAL ::log4cxx::Level::getFatal()
#define ERROR ::log4cxx::Level::getError()
#define WARN  ::log4cxx::Level::getWarn()
#define INFO  ::log4cxx::Level::getInfo()
#define DEBUG ::log4cxx::Level::getDebug()
#define TRACE ::log4cxx::Level::getTrace()

// Internal macros
#define STREAM_LOGGER(level, abort) \
    ::jubatus::server::common::logger::stream_logger( \
        level, __FILE__, __LINE__, abort)

#ifdef NDEBUG
#define DEBUG_ONLY true ? (void) 0 : \
                          ::jubatus::server::common::logger::voidify() &
#else
#define DEBUG_ONLY
#endif

#define LOG_FATAL(level) STREAM_LOGGER(level, true)
#define LOG_ERROR(level) STREAM_LOGGER(level, false)
#define LOG_WARN(level)  STREAM_LOGGER(level, false)
#define LOG_INFO(level)  STREAM_LOGGER(level, false)
#define LOG_DEBUG(level) DEBUG_ONLY STREAM_LOGGER(level, false)
#define LOG_TRACE(level) DEBUG_ONLY STREAM_LOGGER(level, false)

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
      const log4cxx::LevelPtr& level,
      const char* file,
      int line,
      bool abort);
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
  const bool abort_;
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
void setup_parameters(const char* progname, const char* host, int port);

/**
 * Configures the logging library. (standard output)
 */
void configure();

/**
 * Configures the logging library with given config file.
 * Note: even if the configuration file contains an error,
 * exception will not be thrown and just ignored; in that
 * case, if you are reconfiguring the logger, the original
 * logger configuration remains active.
 */
void configure(const std::string&);

/**
 * Returns if the logging library is configured with at least 1 logger.
 *
 * @return true when at least one logger is configured, false if not.
 */
bool is_configured();

}  // namespace logger
}  // namespace common
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_COMMON_LOGGER_LOGGER_HPP_
