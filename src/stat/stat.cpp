#include "stat.hpp"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <pficommon/system/time_util.h>
#include <pficommon/data/serialization.h>

using namespace std;
using namespace pfi::system::time;

namespace jubatus { namespace stat {

namespace {
inline double sq(double d){ return d * d; }
} // namespace

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

  {
    clock_time ct = get_clock_time();
    window_.push_back(make_pair((uint64_t)ct, make_pair(key, val)));
    stats_[key].add(val);
  }
  while(window_.size() > window_size_) {
    string &key = window_.front().second.first;
    double val = window_.front().second.second;
    stat_val &st = stats_[key];
    st.rem(val, key, *this);
    window_.pop_front();
    if (st.n_ == 0)
      stats_.erase(key);
  }
}

double stat::sum(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  return stats_[key].sum_;
}

double stat::stddev(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  stat_val &st = stats_[key];
  return sqrt(moment(key, 2, st.sum_ / st.n_));
}

double stat::max(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  return stats_[key].max_;
}

double stat::min(const std::string &key)
{
  pfi::concurrent::scoped_rlock lk(m_);

  return stats_[key].min_;
}

double stat::entropy()
{
  pfi::concurrent::scoped_rlock lk(m_);

  size_t total = 0;
  for (pfi::data::unordered_map<std::string, stat_val>::iterator p = stats_.begin();
       p != stats_.end(); ++p) {
    total += p->second.n_;
  }
  double ret = 0;
  for (pfi::data::unordered_map<std::string, stat_val>::iterator p = stats_.begin();
       p != stats_.end(); ++p) {
    double pr = p->second.n_ / (double)total;
    ret += pr * log(pr);
  }
  return ret;
}

double stat::moment(const std::string &key, int n, double c)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (n < 0) return -1;

  stat_val &st = stats_[key];

  if (n == 0) return st.n_;

  if (n == 1) return st.sum_ - c * st.n_;

  if (n == 2)
    return sqrt(st.sum2_ - st.n_ * sq(c));

  // fallback
  double ret = 0;
  for (size_t i = 0; i < window_.size(); ++i) {
    if (window_[i].second.first != key) continue;
    ret += pow(window_[i].second.second - c, n);
  }
  return ret;
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
