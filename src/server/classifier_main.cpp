#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/cast.h>
#include <msgpack.hpp>

#include "server.hpp"
#include "../common/exception.hpp"
#include "fv_converter/datum_to_fv_converter.hpp"
#include "classifier/classifier_factory.hpp"
#include "classifier/classifier_base.hpp"
#include "storage/local_storage_mixture.hpp"
#include "server_util.hpp"
#include "diffv.hpp"

#include "classifier_types.hpp"

#include <cmath>

SET_PROGNAME("classifier");

namespace jubatus {
namespace classifier {

struct model {
  datum_to_fv_converter conv;
  pfi::lang::shared_ptr<classifier_base> classifier;
  pfi::lang::shared_ptr<storage::storage_base> storage;
  config_data conf;
};

void train(model* m, const std::pair<std::string, datum>& labeled) {
  sfv_t fv;
  m->conv.convert(labeled.second, fv);
  m->classifier->train(fv, labeled.first);
}

std::string classify(const model* m, const datum& dat) {
  sfv_t fv;
  const_cast<datum_to_fv_converter&>(m->conv).convert(dat, fv);
  return m->classifier->classify(fv);
}

void set_config(model* m, const config_data& conf) {
  m->conf = conf;
  initialize_converter(conf.converter, m->conv);
  m->storage.reset(new storage::local_storage_mixture());
  classifier_base* r = classifier_factory::create_classifier(conf.method,
                                                             m->storage.get());
  m->classifier.reset(r);
}

config_data get_config(const model* m, int){
  return m->conf;
}

void save(const model*, const std::string& id){
  std::string ofile = "/tmp/";
  // FIXME: user server_argv#tmpdir
  //build_local_path_(ofile, type, id);

  ofile += PROGNAME + "_" + id + ".jc";
  std::ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
  if(!ofs){
    throw std::runtime_error(ofile + ": cannot open (" +
                             pfi::lang::lexical_cast<std::string>(errno) + ")" );
  }
  try{
    // ok = m->storage->save(ofs); // FIXME: this operation is constant
    ofs.close();
    LOG(INFO) << "saved to " << ofile;
  }catch(const std::exception& e){
    throw e;
  }
}
void load(model* m, const std::string& id){
  std::string ifile;
  bool ok = false;
  // FIXME: user server_argv#tmpdir
  //  build_local_path_(ifile, type, id);

  ifile += PROGNAME + "_" + id + ".jc";
  std::ifstream ifs(ifile.c_str(), std::ios::binary);
  if(!ifs){
    throw std::runtime_error(ifile + ": cannot open (" +
                             pfi::lang::lexical_cast<std::string>(errno) + ")" );
  }

  try{
    pfi::lang::shared_ptr<storage::storage_base>
      s(new storage::local_storage_mixture());
    if(!s){
      throw std::runtime_error("cannot allocate memory for storage");
    }

    ok = s->load(ifs);
    ifs.close();
    
    if(!ok){
      throw std::runtime_error("failed on load()");
    }
    LOG(INFO) << "loaded from " << ifile;
    m->storage = s;
    m->classifier.reset(classifier_factory::create_classifier(m->conf.method,
                                                              s.get()));
  }catch(const std::exception& e){
    throw e;
  }
}

diffv get_diff(const model*){  //FIXME
  diffv ret;
  // FIXME: how do we count the number of 'train's.
  //  ret.count = mixer_->get_count();
  //  m->storage->get_diff(ret.v);
  return ret;
}

storage::val3_t mix_val3(const storage::val3_t& lhs, const storage::val3_t& rhs) {
  return storage::val3_t(lhs.v1 + rhs.v1,
                         lhs.v2, //FIXME!!! XXX!!! //min(lhs.v2, rhs.v2),
                         lhs.v3 + rhs.v3);
}
storage::feature_val3_t mix_feature(const storage::feature_val3_t& lhs
                                    , const storage::feature_val3_t& rhs) {
  storage::val3_t def(0, 1, 0);
  storage::feature_val3_t ret(lhs);
  storage::detail::binop(ret, rhs, mix_val3, def);
  return ret;
}
void mix_parameter(diffv& lhs, const diffv& rhs) {
  storage::features3_t r(rhs.v);
  for (size_t i = 0; i < r.size(); ++i) {
    storage::feature_val3_t& f = r[i].second;
    for (size_t j = 0; j < f.size(); ++j) {
      f[j].second.v1 *= rhs.count;
    }
  }
  storage::detail::binop(lhs.v, r, mix_feature);
  lhs.count += rhs.count;
}

  // FIXME: this important code in old classifier is not implemented.
  // we NEED to think more
  //
  // // average <= sum / servers_.size()
  // float div = 1.f / static_cast<float>(sum.count);
  // for (size_t i = 0; i < sum.v.size(); ++i) {
  //   feature_val3_t& f = sum.v[i].second;
  //   for (size_t j = 0; j < f.size(); ++j) {
  //     f[j].second.v1 *= div;
  //   }
  // }
int mix(const model*, const diffv& lhs, const diffv& rhs, diffv& acc){
  mix_parameter(acc, lhs);
  mix_parameter(acc, rhs);
  return 0;//FIXME
}
int put_diff(model* m, const diffv& diff){
  reinterpret_cast<storage::local_storage_mixture*>(m->storage.get())->
    set_average_and_clear_diff(diff.v);
  return -1;
}


} //classifier
} //jubatus

using namespace std;

int main(int argc, char* argv[]) {
  const jubatus::server_argv a(argc, argv);
  jubatus::server<jubatus::classifier::model, jubatus::diffv> serv(a);

  serv.register_update<pair<std::string, jubatus::datum> >(
      "train", &jubatus::classifier::train);
  serv.register_analysis<jubatus::datum, std::string>(
      "classify", &jubatus::classifier::classify);
  
  serv.register_update<jubatus::classifier::config_data>(
      "set_config", &jubatus::classifier::set_config);
  serv.register_analysis<int, jubatus::classifier::config_data>(
      "get_config", &jubatus::classifier::get_config);

  serv.register_update<std::string>(
      "save", &jubatus::classifier::save);
  serv.register_update<std::string>(
      "load", &jubatus::classifier::load);

  // FIXME: we need bind 'get_status' and get_storage API here.

#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::function<jubatus::diffv(const jubatus::classifier::model*)> g(&jubatus::classifier::get_diff);
  pfi::lang::function<int(const jubatus::classifier::model*,
                          const jubatus::diffv&,
                          const jubatus::diffv&,
                          jubatus::diffv&)>  m(&jubatus::classifier::mix);
  pfi::lang::function<int(jubatus::classifier::model*, const jubatus::diffv&)> p(&jubatus::classifier::put_diff);
  serv.set_mixer(g, m, p);

#endif

  return serv.start();
}
