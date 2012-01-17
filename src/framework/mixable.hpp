#pragma once

#include <string>
#include <pficommon/lang/function.h>
#include <iostream>
#include <msgpack.hpp>
#include "../common/exception.hpp"

using pfi::lang::function;

namespace jubatus{
namespace server{

class mixable0 {
public:
  mixable0(){};
  virtual ~mixable0(){};
  virtual std::string get_diff()const = 0;
  virtual void put_diff(const std::string&) = 0;
  virtual void reduce(const std::string&, std::string&) const = 0;
  virtual void save(std::ostream & ofs) = 0;
  virtual void load(std::istream & ifs) = 0;
  virtual void clear() = 0;
};

template <typename Model, typename Diff>
class mixable : public mixable0 {
public:
  mixable(){};
  virtual ~mixable(){};

  virtual void clear() = 0;
  void set_model(pfi::lang::shared_ptr<Model> m){
    model_ = m;
  }

  std::string get_diff()const{
    msgpack::sbuffer sbuf;
    if(model_){
      std::string buf;
      pack_(get_diff_fun_(model_.get()), buf);
      return buf;
    }else{
      throw config_not_set();
    }
  };
  void put_diff(const std::string& d){
    if(model_){
      Diff diff;
      unpack_(d, diff);
      put_diff_fun_(model_.get(), diff);
    }else{
      throw config_not_set();
    }
  }
  void reduce(const std::string& lhs, std::string& acc) const {
    Diff l, a; //<- string
    unpack_(lhs, l);
    unpack_(acc, a);
    reduce_fun_(model_.get(), l, a);
    pack_(a, acc);
  }
  void save(std::ostream & os){
    model_->save(os);
  }
  void load(std::istream & is){
    model_->load(is);
  }

  void set_mixer(function<Diff(const Model*)> get_diff_fun, //get_diff
                 function<int(const Model*, const Diff&, Diff&)> reduce_fun, //mix
                 function<int(Model*, const Diff&)> put_diff_fun //put_diff
                 ) {
    get_diff_fun_ = get_diff_fun;
    reduce_fun_ = reduce_fun;
    put_diff_fun_ = put_diff_fun;
  };
  pfi::lang::shared_ptr<Model> get_model()const{return model_;};

private:
  void unpack_(const std::string& buf, Diff& d) const {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, buf.c_str(), buf.size());
    msg.get().convert(&d);
  }
  void pack_(const Diff& d, std::string& buf) const {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, d);
    buf = std::string(sbuf.data(), sbuf.size());
  }

  function<Diff(const Model*)> get_diff_fun_;
  function<int(const Model*, const Diff&, Diff&)> reduce_fun_;
  function<int(Model*, const Diff&)> put_diff_fun_;

  pfi::lang::shared_ptr<Model> model_;

};

} //server
} //jubatus
