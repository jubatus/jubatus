#include "stat_server_impl.hpp"

using namespace std;

namespace jubatus {
namespace server {

int stat_impl::push(const string &key, double val)
{
  stat_.push(key, val);
  return 0;
}

double stat_impl::sum(const string &key)
{
  return stat_.sum(key);
}

double stat_impl::stddev(const string &key)
{
  return stat_.stddev(key);
}

double stat_impl::max(const string &key)
{
  return stat_.max(key);
}

double stat_impl::min(const string &key)
{
  return stat_.min(key);
}

double stat_impl::entropy()
{
  return stat_.entropy();
}

double stat_impl::moment(const std::string &key, int n, double d)
{
  return stat_.moment(key, n, d);
}

int stat_impl::save(const string &key)
{
  // stat_.save(key);
  return 0;
}

int stat_impl::load(const string &key)
{
  // stat_.load(key);
  return 0;
}

} // namespace server
} // namespace jubatus
