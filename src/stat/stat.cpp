#include "stat.hpp"

#include <cassert>
#include <algorithm>
#include <pficommon/system/time_util.h>
#include <pficommon/data/serialization.h>

using namespace std;
using namespace pfi::system::time;

namespace jubatus { namespace stat {

stat::stat(size_t window_size)
  : window_size_(window_size)
{
}

stat::~stat()
{
}

void stat::push(const std::string &key, double val)
{
  pfi::concurrent::scoped_wlock lk(m_);

  queue_type &q = window_[key];
  clock_time ct = get_clock_time();
  q.push_back(make_pair((uint64_t)ct, val));
  while (q.size() > window_size_) q.pop_front();
}

double stat::sum(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  queue_type &q = window_[key];

  double ret = 0;
  for (size_t i = 0; i < q.size(); ++i)
    ret += q[i].second;
  return ret;
}

double stat::stddev(const std::string &key)
{
  assert(!"stddev is underconstructions");
}

double stat::max(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  queue_type &q = window_[key];
  if (q.empty()) {
    throw stat_error("max: " + key + " is empty");
  }

  double ret = q[0].second;
  for (size_t i = 1; i < q.size(); ++i)
    ret = std::max(ret, q[i].second);
  return ret;
}

double stat::min(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  queue_type &q = window_[key];
  if (q.empty()) {
    throw stat_error("min: " + key + " is empty");
  }

  double ret = q[0].second;
  for (size_t i = 1; i < q.size(); ++i)
    ret = std::min(ret, q[i].second);
  return ret;
}

double stat::entropy(const std::string &key)
{
  assert(!"entropy is underconstructions");
}

double stat::moment(const std::string &key)
{
  assert(!"moment is underconstructions");
}

bool stat::save(std::ostream& os){
  pfi::data::serialization::binary_oarchive oa(os);
  oa << *this;
  return true;
}
bool stat::load(std::istream& is){
  pfi::data::serialization::binary_iarchive ia(is);
  ia >> *this;
  return true;
}
std::string stat::type()const{ return "stat"; };

}
} // namespace jubatus
