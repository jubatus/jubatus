#pragma once

#include <deque>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include <cstdlib>
#include <stdint.h>

namespace jubatus {
namespace stat {

class stat_error : public std::exception {
public:
  stat_error(const std::string &msg)
    : msg_(msg) {}
  ~stat_error() throw() {}

  const char *what() throw() {
    return msg_.c_str();
  }

private:
  std::string msg_;
};

class stat {
public:
  stat(size_t window_size);
  ~stat();

  void push(const std::string &key, double val);

  double sum(const std::string &key);
  double stddev(const std::string &key);
  double max(const std::string &key);
  double min(const std::string &key);
  double entropy(const std::string &key);
  double moment(const std::string &key);

  bool save(std::ostream&);
  bool load(std::istream&);
  std::string type()const;

private:
  friend class pfi::data::serialization::access;
  template <class Archive>
  void serialize(Archive &ar) {
    ar & window_size_ & window_;
  }

  size_t window_size_;

  typedef std::deque<std::pair<uint64_t, double> > queue_type;
  pfi::data::unordered_map<std::string, queue_type> window_;

  pfi::concurrent::rw_mutex m_;
};

}
} // namespace jubatus
