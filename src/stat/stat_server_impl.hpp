#pragma once

#include "stat_server.hpp"
#include "stat.hpp"

namespace jubatus {
namespace server {

class stat_impl : public stat<stat_impl> {
public:
  stat_impl(int window_size, double timeout_sec)
    : stat(timeout_sec)
    , stat_(window_size) {
  }

  int push(const std::string &key, double val);

  double sum(const std::string &key);
  double stddev(const std::string &key);
  double max(const std::string &key);
  double min(const std::string &key);

  double entropy();
  double moment(const std::string &key, int n, double d);

  int save(const std::string &key);
  int load(const std::string &key);

private:
  jubatus::stat::stat stat_;
};

} // namespace server
} // namespace jubatus
