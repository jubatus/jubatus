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

  double entropy();
  double moment(const std::string &key, int n, double c);

  const std::string type; //messy

  bool save(std::ostream&);
  bool load(std::istream&);

private:
  friend class pfi::data::serialization::access;
  template <class Archive>
  void serialize(Archive &ar) {
    ar & window_size_ & window_;
  }

  size_t window_size_;

  struct stat_val {
    stat_val()
      : n_(0)
      , sum_(0)
      , max_(0)
      , min_(0) {
    }

    void add(double d) {
      n_ += 1;
      sum_ += d;
      sum2_ += d * d;

      if (n_ > 1)
        max_ = std::max(max_, d);
      else
        max_ = d;

      if (n_ > 1)
        min_ = std::min(min_, d);
      else
        min_ = d;
    }

    void rem(double d, const std::string &key, stat &st) {
      n_ -= 1;
      sum_ -= d;
      sum2_ -= d * d;
      if (max_ == d) {
        if (n_ > 0) {
          bool first = true;
          for (size_t i = 0; i < st.window_.size(); ++i) {
            if (st.window_[i].second.first != key) continue;
            double d = st.window_[i].second.second;
            if (first) {
              max_ = d;
              first = false;
            }
            else {
              max_ = std::max(max_, d);
            }
          }
        }
        else {
          max_ = 0;
        }
      }
      if (min_ == d) {
        if (n_ > 0) {
          bool first = true;
          for (size_t i = 0; i < st.window_.size(); ++i) {
            if (st.window_[i].second.first != key) continue;
            double d = st.window_[i].second.second;
            if (first) {
              min_ = d;
              first = false;
            }
            else {
              min_ = std::min(min_, d);
            }
          }
        }
        else {
          min_ = 0;
        }
      }
    }

    size_t n_;

    double sum_, sum2_;
    double max_;
    double min_;
  };

  std::deque<std::pair<uint64_t, std::pair<std::string, double> > > window_;
  pfi::data::unordered_map<std::string, stat_val> stats_;

  pfi::concurrent::rw_mutex m_;
};

}
} // namespace jubatus
