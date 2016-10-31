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

#include "logger.hpp"

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include <log4cxx/logger.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/xml/domconfigurator.h>

#include <jubatus/util/lang/cast.h>

#if defined(__FreeBSD__)
#include <sys/thr.h>
#endif

#include <string>

#define LOGGER_NAME "jubatus"

namespace {

inline int gettid() {
#ifdef __APPLE__
  return ::syscall(SYS_thread_selfid);
#elif defined(__FreeBSD__)
  long lwpid;  // NOLINT
  thr_self(&lwpid);
  return static_cast<int>(lwpid);
#else
  return ::syscall(SYS_gettid);
#endif
}

}  // namespace

using jubatus::util::lang::lexical_cast;

namespace jubatus {
namespace server {
namespace common {
namespace logger {

namespace {

inline const char* const_basename(const char* path) {
  const char* base = ::strrchr(path, '/');
  return base ? (base + 1) : path;
}

}  // namespace

stream_logger::stream_logger(
    const log4cxx::LevelPtr& level,
    const char* file,
    int line,
    bool abort)
    : level_(level),
      file_(file),
      line_(line),
      abort_(abort),
      thread_id_(gettid()) {}

stream_logger::~stream_logger() {
  log4cxx::MDC::put("tid", lexical_cast<std::string>(thread_id_));
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(LOGGER_NAME);
  if (logger->isEnabledFor(level_)) {
    logger->forcedLog(
        level_,
        buf_.str(),
        log4cxx::spi::LocationInfo(const_basename(file_), "", line_));
  }
  if (abort_) {
    abort();
  }
}

void setup_parameters(const char* progname, const char* host, int port) {
  ::setenv("JUBATUS_PID", lexical_cast<std::string>(::getpid()).c_str(), 1);
  ::setenv("JUBATUS_PROCESS", progname, 1);
  ::setenv("JUBATUS_HOST", host, 1);
  ::setenv("JUBATUS_PORT", lexical_cast<std::string>(port).c_str(), 1);
}

void configure() {
  log4cxx::LayoutPtr layout(
      new log4cxx::PatternLayout("%d %X{tid} %-5p [%F:%L] %m%n"));
  log4cxx::AppenderPtr appender(new log4cxx::ConsoleAppender(layout));
  log4cxx::BasicConfigurator::configure(appender);
  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(LOGGER_NAME);
  logger->setLevel(log4cxx::Level::getInfo());
}

void configure(const std::string& config_file) {
  // Exception will not be thrown even if there is an error in config file.
  log4cxx::xml::DOMConfigurator::configure(config_file);
}

bool is_configured() {
  log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();
  return rootLogger->getAllAppenders().size() != 0;
}

}  // namespace logger
}  // namespace common
}  // namespace server
}  // namespace jubatus
