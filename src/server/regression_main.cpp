#include <pficommon/lang/shared_ptr.h>
#include <msgpack.hpp>

#include "server.hpp"
#include "regression/regression.hpp"
#include "fv_converter/datum.hpp"
#include "fv_converter/datum_to_fv_converter.hpp"
#include "fv_converter/converter_config.hpp"
#include "../common/zk.hpp"
#include "regression/regression_factory.hpp"
#include "storage/local_storage_mixture.hpp"

#include <pficommon/text/json.h>

namespace jubatus {
namespace regression {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

struct model {
  datum_to_fv_converter conv;
  pfi::lang::shared_ptr<regression_base> regression;
  pfi::lang::shared_ptr<storage::storage_base> storage;
};

void train(model* m, const std::pair<float, datum>& labeled) {
  sfv_t fv;
  m->conv.convert(labeled.second, fv);
  m->regression->train(fv, labeled.first);
}

float estimate(const model* m, const datum& dat) {
  sfv_t fv;
  const_cast<datum_to_fv_converter&>(m->conv).convert(dat, fv);
  return m->regression->estimate(fv);
}

void set_config(model* m, const config_data& conf) {
  initialize_converter(conf.converter, m->conv);
  m->storage.reset(new storage::local_storage_mixture());
  regression_base* r = regression_factory().create_regression(conf.method,
                                                              m->storage.get());
  m->regression.reset(r);
}

}
}

using namespace std;
using namespace pfi::lang;
using namespace pfi::concurrent::threading_model;

int main(int argc, char* argv[]) {
  int timeout = 10;
#ifdef HAVE_ZOOKEEPER_H
  std::string zkcluster = "localhost:2181";
  shared_ptr<jubatus::zk, multi_thread> z(new jubatus::zk(zkcluster, timeout, "log"));

  jubatus::server<jubatus::regression::model> serv(z, "test");
#endif

  jubatus::server<jubatus::regression::model> serv;
  serv.register_update<pair<float, jubatus::datum> >(
      "train", &jubatus::regression::train);
  serv.register_analysis<jubatus::datum, float>(
      "estimate", &jubatus::regression::estimate);
  
  serv.register_update<jubatus::regression::config_data>(
      "set_config", &jubatus::regression::set_config);

  serv.serv(9199, 10.);
}
