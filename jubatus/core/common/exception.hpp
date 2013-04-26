// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_EXCEPTION_HPP_
#define JUBATUS_CORE_COMMON_EXCEPTION_HPP_

#include <exception>
#include <stdexcept>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/demangle.h>
#include "exception_info.hpp"

namespace jubatus {
namespace exception {

typedef error_info<struct error_at_file_, std::string> error_at_file;
typedef error_info<struct error_at_func_, std::string> error_at_func;
typedef error_info<struct error_at_line_, int> error_at_line;
typedef error_info<struct error_errno_, int> error_errno;
inline std::string to_string(const error_errno& info) {
  std::string msg(strerror(info.value()));
  msg += " (" + pfi::lang::lexical_cast<std::string>(info.value()) + ")";
  return msg;
}

typedef error_info<struct error_file_name_, std::string> error_file_name;
typedef error_info<struct error_api_func_, std::string> error_api_func;
typedef error_info<struct error_message_, std::string> error_message;

typedef error_info<struct error_splitter_, void> error_splitter;

struct exception_thrower_binder_type {
};
// for exception_thrower

#if defined(__GNUC__)
#define JUBATUS_ERROR_FUNC \
  jubatus::exception::error_at_func(__PRETTY_FUNCTION__)
#else
#define JUBATUS_ERROR_FUNC \
  jubatus::exception::error_at_func(__func__)
#endif

#define JUBATUS_CURRENT_ERROR_INFO() \
  jubatus::exception::error_at_file(__FILE__) \
    << jubatus::exception::error_at_line(__LINE__) \
    << JUBATUS_ERROR_FUNC << jubatus::exception::error_splitter()
#define JUBATUS_EXCEPTION(e) e << \
  jubatus::exception::exception_thrower_binder_type() \
    << JUBATUS_CURRENT_ERROR_INFO()

class exception_thrower_base;
typedef pfi::lang::shared_ptr<exception_thrower_base> exception_thrower_ptr;

typedef std::vector<pfi::lang::shared_ptr<error_info_base> > error_info_list_t;

class jubatus_exception : public std::exception {
 public:
  jubatus_exception() throw () {
  }
  virtual ~jubatus_exception() throw () {
  }

  virtual exception_thrower_ptr thrower() const = 0;

  template<class Exception>
  friend const Exception& add_info(
      const Exception& e,
      pfi::lang::shared_ptr<error_info_base> info);

  std::string name() const throw () {
    // does not assume multithreading
    if (exception_class_name_.empty()) {
      exception_class_name_ = pfi::lang::demangle(typeid(*this).name());
    }

    return exception_class_name_;
  }

  virtual const char* what() const throw () {
    name();
    return exception_class_name_.c_str();
  }

  error_info_list_t error_info() const;
  std::string diagnostic_information(bool display_what = false) const;

 private:
  mutable std::string exception_class_name_;
  mutable error_info_list_t info_list_;
};

template<class Exception>
inline const Exception& add_info(
    const Exception& e,
    pfi::lang::shared_ptr<error_info_base> info) {
  e.info_list_.push_back(info);
  return e;
}

template<class Exception, class Tag, class V>
inline const Exception& operator <<(
    const Exception& e,
    const error_info<Tag, V>& info) {
  return add_info(
      e, pfi::lang::shared_ptr<error_info_base>(new error_info<Tag, V>(info)));
}

template<class Exception>
inline const Exception& operator <<(
    const Exception& e,
    pfi::lang::shared_ptr<error_info_base> info) {
  return add_info(e, info);
}

class exception_thrower_base {
 public:
  exception_thrower_base() {
  }
  virtual ~exception_thrower_base() {
  }

  virtual void throw_exception() const = 0;
};

template<class Exception>
class exception_thrower_impl : public exception_thrower_base {
 public:
  explicit exception_thrower_impl(const Exception& e)
      : exception_(e) {
  }

 private:
  // noncopylable
  exception_thrower_impl(const exception_thrower_impl&);
  exception_thrower_impl& operator=(const exception_thrower_impl&);

 public:
  void throw_exception() const {
    throw exception_;
  }

 private:
  Exception exception_;
};

template<class Exception>
class jubaexception : public jubatus_exception {
 public:
  jubaexception() {
  }
  virtual ~jubaexception() throw () {
  }

  exception_thrower_ptr thrower() const {
    if (thrower_) {
      return thrower_;
    } else {
      return exception_thrower_ptr(
          new exception_thrower_impl<Exception>(
              *(static_cast<const Exception*>(this))));
    }
  }

  // This is desireble in private
  void bind_thrower(exception_thrower_ptr thrower) const {
    thrower_ = thrower;
  }

 private:
  mutable exception_thrower_ptr thrower_;
};

template<class Exception>
inline const Exception& operator <<(
    const Exception& e,
    const exception_thrower_binder_type&) {
  e.bind_thrower(
      exception_thrower_ptr(new exception_thrower_impl<Exception>(e)));
  return e;
}

class unknown_exception : public jubaexception<unknown_exception> {
 public:
  explicit unknown_exception() {
    // TODO(kashihara): push unknown_exception
  }

  const char* what() const throw () {
    return "unknown exception";
  }
};

class runtime_error : public jubaexception<runtime_error> {
 public:
  explicit runtime_error(const std::string& what)
      : what_(what) {
  }

  ~runtime_error() throw () {
  }

  const char* what() const throw () {
    return what_.c_str();
  }
 private:
  std::string what_;
};

namespace detail {
template<class Exception>
exception_thrower_ptr current_std_exception(const Exception& e) {
  return exception_thrower_ptr(new exception_thrower_impl<Exception>(e));
}

}  // namespace detail

// Don't call without catch blocks
inline exception_thrower_ptr get_current_exception() {
  exception_thrower_ptr ptr;

  try {
    throw;
  } catch (const std::bad_alloc& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::bad_cast& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::bad_exception& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::bad_typeid& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::domain_error& e) {  // logic_error
    ptr = detail::current_std_exception(e);
  } catch (const std::invalid_argument& e) {  // logic_error
    ptr = detail::current_std_exception(e);
  } catch (const std::length_error& e) {  // logic_error
    ptr = detail::current_std_exception(e);
  } catch (const std::out_of_range& e) {  // logic_error
    ptr = detail::current_std_exception(e);
  } catch (const std::logic_error& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::range_error& e) {  // runtime_error
    ptr = detail::current_std_exception(e);
  } catch (const std::overflow_error& e) {  // runtime_error
    ptr = detail::current_std_exception(e);
  } catch (const std::underflow_error& e) {  // runtime_error
    ptr = detail::current_std_exception(e);
  } catch (const std::runtime_error& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const std::ios_base::failure& e) {  // exception
    ptr = detail::current_std_exception(e);
  } catch (const jubatus_exception& e) {
    ptr = e.thrower();
  } catch (const std::exception& e) {
    ptr = detail::current_std_exception(e);
  } catch (...) {
    ptr = unknown_exception().thrower();
  }

  return ptr;
}

}  // namespace exception

class config_exception : public exception::jubaexception<config_exception> {
};

class storage_not_set : public exception::jubaexception<storage_not_set> {
};

class config_not_set : public exception::jubaexception<config_not_set> {
  const char* what() const throw () {
    return "config_not_set";
  }
};

class unsupported_method : public exception::runtime_error {
 public:
  explicit unsupported_method(const std::string& n)
      : jubatus::exception::runtime_error(
          std::string("unsupported method (") + n + ")") {
  }
};

class bad_storage_type : public exception::runtime_error {
 public:
  explicit bad_storage_type(const std::string& n)
      : jubatus::exception::runtime_error(n) {
  }
};

class membership_error : public exception::runtime_error {
 public:
  explicit membership_error(const std::string& n)
      : jubatus::exception::runtime_error(n) {
  }
};

class not_found : public membership_error {
 public:
  explicit not_found(const std::string& n)
      : membership_error(n) {
  }
};

class argv_error : public exception::runtime_error {
 public:
  explicit argv_error(const std::string& n)
      : jubatus::exception::runtime_error(n) {
  }
};

}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_EXCEPTION_HPP_
