#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/cast.h>
#include <msgpack.hpp>

#include "server.hpp"
#include "../common/exception.hpp"
#include "regression/regression.hpp"
#include "fv_converter/datum.hpp"
#include "fv_converter/datum_to_fv_converter.hpp"
#include "fv_converter/converter_config.hpp"
#include "classifier/classifier_factory.hpp"
#include "classifier/classifier_base.hpp"
#include "storage/local_storage_mixture.hpp"
#include "diffv.hpp"
#include "server_util.hpp"

SET_PROGNAME("classifier");

namespace jubatus {
namespace classifier {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

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

int save(const model*, const std::string& id){
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
    return 0;
  }catch(const std::exception& e){
    throw e;
  }
  return -1;
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
  diffv d;
  return d;
}
int mix(const model*, const diffv& lhs, const diffv& rhs, diffv& acc){
  return -1;//FIXME
}
int put_diff(model*, const diffv&){  //FIXME
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

  serv.register_analysis<std::string, int>(
      "save", &jubatus::classifier::save);
  serv.register_update<std::string>(
      "load", &jubatus::classifier::load);

  // FIXME: we need bind 'get_status' API here.

#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::function<jubatus::diffv(const jubatus::classifier::model*)> g(&jubatus::classifier::get_diff);
  pfi::lang::function<int(const jubatus::classifier::model*,
                          const jubatus::diffv&,
                          const jubatus::diffv&,
                          jubatus::diffv&)>  m(&jubatus::classifier::mix);
  pfi::lang::function<int(jubatus::classifier::model*, const jubatus::diffv&)> p(&jubatus::classifier::put_diff);
  serv.start_mixer(g, m, p);

#endif

  return serv.serv(a.port, a.timeout);
}
