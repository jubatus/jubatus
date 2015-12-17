// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "signals.hpp"

#include <unistd.h>
#include <string>
#include <cerrno>
#include <csignal>

#include "jubatus/util/concurrent/lock.h"
#include "jubatus/util/concurrent/mutex.h"
#include "jubatus/util/concurrent/thread.h"

#include "jubatus/core/common/assert.hpp"
#include "jubatus/core/common/exception.hpp"

#include "logger/logger.hpp"

using jubatus::util::lang::lexical_cast;

namespace jubatus {
namespace server {
namespace common {

namespace {

// helper functions to handle sigset

void clear_sigset(sigset_t* ss) {
  if (sigemptyset(ss) != 0) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("failed to clear sigset")
      << core::common::exception::error_api_func("sigemptyset"));
  }
}

void add_signal(sigset_t* ss, int signum) {
  if (sigaddset(ss, signum) != 0) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("failed to add singal to sigset")
      << core::common::exception::error_api_func("sigaddset"));
  }
}

void block_signals(const sigset_t* ss) {
  if (pthread_sigmask(SIG_BLOCK, ss, NULL) != 0) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("failed to mask signals")
      << core::common::exception::error_api_func("pthread_sigmask")
      << core::common::exception::error_errno(errno));
  }
}

void setup_sigset(sigset_t* ss) {
  clear_sigset(ss);
  add_signal(ss, SIGTERM);
  add_signal(ss, SIGINT);
  add_signal(ss, SIGHUP);
}

bool handling_signals = false;
jubatus::util::lang::function<void()> action_on_term;
jubatus::util::lang::function<void()> action_on_hup;
jubatus::util::concurrent::mutex mutex_on_signal;

void default_action_on_term() {
  exit(1);
}

void handle_signal(jubatus::util::lang::function<void()>& action) {
  jubatus::util::lang::function<void()> f;
  {
    jubatus::util::concurrent::scoped_lock lk(mutex_on_signal);
    f.swap(action);
  }

  // Execute the action without the mutex lock so that the action
  // can re-register the action via set_action_on_hup etc.
  if (f) {
    f();
  }
}

void handle_signals() {
  try {
    sigset_t ss;
    setup_sigset(&ss);

    while (1) {
      int signo;
      if (sigwait(&ss, &signo) != 0) {
        throw JUBATUS_EXCEPTION(
          core::common::exception::runtime_error("failed to call sigwait")
          << core::common::exception::error_api_func("sigwait"));
      }

      switch (signo) {
        case SIGTERM:
          LOG(INFO) << "caught SIGTERM";
          handle_signal(action_on_term);
          return;
        case SIGINT:  // behave the same way as SIGTERM
          LOG(INFO) << "caught SIGINT";
          handle_signal(action_on_term);
          return;
        case SIGHUP:
          LOG(INFO) << "caught SIGHUP";
          handle_signal(action_on_hup);
          break;
        default:
          // unintended signal; raise error (assertion may be better?)
          throw JUBATUS_EXCEPTION(
            core::common::exception::runtime_error(
                "unknown signal (" + lexical_cast<std::string>(signo) +
                ") caught by sigwait (possibly logic error)"));
      }
    }
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << "exception in sigwait thread: "
               << e.diagnostic_information(true);
  } catch (const std::exception& e) {
    LOG(FATAL) << "error in sigwait thread: " << e.what();
  }
  JUBATUS_ASSERT_UNREACHABLE();
}

void ignore_sigpipe() {
  // portable code for socket write(2) MSG_NOSIGNAL
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    throw JUBATUS_EXCEPTION(
        jubatus::core::common::exception::runtime_error("can't ignore SIGPIPE")
        << jubatus::core::common::exception::error_api_func("signal")
        << jubatus::core::common::exception::error_errno(errno));
  }
}

}  // anonymous namespace

void prepare_signal_handling() {
  sigset_t ss;
  setup_sigset(&ss);
  block_signals(&ss);

  ignore_sigpipe();

  if (!handling_signals) {
    set_action_on_term(&default_action_on_term);
    jubatus::util::concurrent::thread(&handle_signals).start();
    handling_signals = true;
  }
}

/**
 * Bind one-shot action to SIGTERM and SIGINT signal.
 */
void set_action_on_term(jubatus::util::lang::function<void()> action) {
  jubatus::util::concurrent::scoped_lock lk(mutex_on_signal);
  action_on_term.swap(action);
}

/**
 * Bind one-shot action to SIGHUP signal.
 */
void set_action_on_hup(jubatus::util::lang::function<void()> action) {
  jubatus::util::concurrent::scoped_lock lk(mutex_on_signal);
  action_on_hup.swap(action);
}

}  // namespace common
}  // namespace server
}  // namespace jubatus

