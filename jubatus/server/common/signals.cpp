// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <cerrno>
#include <csignal>

#include <glog/logging.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/thread.h>

#include "jubatus/core/common/assert.hpp"
#include "jubatus/core/common/exception.hpp"

namespace jubatus {
namespace server {
namespace common {
namespace util {

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

void setup_sigset_for_sigterm(sigset_t* ss) {
  clear_sigset(ss);
  add_signal(ss, SIGTERM);
  add_signal(ss, SIGINT);
}

bool blocking_sigterm = false;
bool handling_sigterm = false;
pfi::lang::function<void()> action_on_term;
pfi::concurrent::mutex mutex_on_term;

void handle_sigterm() {
  // internal function; do not call this function outside of set_exit_on_term
  try {
    sigset_t ss;
    setup_sigset_for_sigterm(&ss);

    int signo;
    if (sigwait(&ss, &signo) != 0) {
      throw JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("failed to call sigwait")
        << core::common::exception::error_api_func("sigwait"));
    }

    switch (signo) {
      case SIGINT:
        LOG(INFO) << "caught SIGINT";
        break;
      case SIGTERM:
        LOG(INFO) << "caught SIGTERM";
        break;
      default:
        // unintended signal; raise error (assertion may be better?)
        throw JUBATUS_EXCEPTION(
          core::common::exception::runtime_error(
              "unknown signal caught by sigwait (possibily logic error)"));
    }

    {
      pfi::concurrent::scoped_lock lk(mutex_on_term);

      if (action_on_term) {
        action_on_term();
        pfi::lang::function<void()>().swap(action_on_term);
      } else {
        kill(getpid(), signo);  // no signal handler; resend signal
                                // (the signal will be blocked and pending)
      }

      handling_sigterm = false;
    }

    return;  // signal handling is successfully done.
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
  } catch (const std::exception& e) {
    LOG(FATAL) << e.what();
  }
  JUBATUS_ASSERT_UNREACHABLE();
}

void prepare_sigterm_handling() {
  if (handling_sigterm) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error(
          "start_sigterm_handling() is already called."));
  }

  sigset_t ss;
  setup_sigset_for_sigterm(&ss);
  block_signals(&ss);

  blocking_sigterm = true;
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
  prepare_sigterm_handling();
  ignore_sigpipe();
}

void set_action_on_term(pfi::lang::function<void()> action) {
  pfi::concurrent::scoped_lock lk(mutex_on_term);

  if (!blocking_sigterm) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error(
        "prepare_signal_handling must be called before set_action_on_term."));
  }

  if (!handling_sigterm) {
    pfi::concurrent::thread(&handle_sigterm).start();
    handling_sigterm = true;
  }
  action_on_term.swap(action);
}

}  // namespace util
}  // namespace common
}  // namespace server
}  // namespace jubatus

