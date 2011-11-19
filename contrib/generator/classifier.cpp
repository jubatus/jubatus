#include <vector>
#include <string>

// server
class classifier {
public:

  //@broadcast
  int set_config(config_data c){
    try{
      shared_ptr<datum_to_fv_converter> converter = shared_ptr<datum_to_fv_converter>(new datum_to_fv_converter());
      jubatus::initialize_converter(config.converter, *converter);
      
      config_ = config;
      converter_ = converter;
      
      classifier_.reset(classifier_factory::create_classifier(config.method, storage_.get()));
      return result<int>::ok(0);
    }catch(const exception& e){
      return result<int>::fail(e.what());
    }
  };
  
  //@random
  config_data get_config() const {
    return result<config_data>::ok(config_);
  };
  
  //@broadcast
  void save(string id) const {
    std::string ofile;
    bool ok = false;
    build_local_path_(ofile, type, id);
    
    ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
    if(!ofs){
      return result<int>::fail(ofile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    }
    try{
      ok = storage_->save(ofs);
      ofs.close();
      LOG(INFO) << "saved to " << ofile;
      return result<int>::ok(0);
    }catch(const std::exception& e){
      return result<int>::fail(e.what());
    }
  };
  
  //@broadcast
  void load(string id){
    std::string ifile;
    bool ok = false;
    build_local_path_(ifile, type, id);
    
    ifstream ifs(ifile.c_str(), std::ios::binary);
    if(!ifs){
      return result<int>::fail(ifile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
    }
    
    try{
      shared_ptr<storage::storage_base,pfi::concurrent::threading_model::multi_thread> s(storage::storage_factory::create_storage(storage_->type));
      
      if(!s) return result<int>::fail("cannot allocate memory for storage");
      
      ok = s->load(ifs);
      ifs.close();
      
      if(ok){
	storage_ = s;
	classifier_.reset(classifier_factory::create_classifier(config_.method, storage_.get()));
	return result<int>::ok(0);
      }
      return result<int>::fail("failed loading");
    }catch(const std::exception& e){
      return result<int>::fail(e.what());
    }
    
  };
  
  //@broadcast 
  void clear(){};
  
  //@random
  void train(vector<pair<string, datum> > train_data){
    if (!classifier_){
      LOG(ERROR) << __func__ << ": config is not set";
      return result<int>::fail("config_not_set");
    }
    
    int count = 0;
    for (size_t i = 0; i < data.size(); ++i) {
      sfv_t v;
      try{
	converter_->convert(data[i].second, v);
	classifier_->train(v, data[i].first);
	count++; //FIXME: try..catch clause and return error
	
#ifdef HAVE_ZOOKEEPER_H
	if(!!mixer_) mixer_->updated();
#endif
      }catch(const std::exception& e){
	LOG(WARNING) << e.what();
	continue;
      }
      
    }
    return result<int>::ok(count);
    
  };
  
  //@random
  vector< vector< pair< string, double > > > classify(vector<datum> data) const {

    std::vector<estimate_results> ret;
    scoped_lock lk(rlock(m_));
    if (!classifier_){
      LOG(ERROR) << __func__ << ": config is not set";
      return result<std::vector<estimate_results> >::fail("config_not_set");
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
      sfv_t v;
      try{
	converter_->convert(data[i], v);
	classify_result scores;
	classifier_->classify_with_scores(v, scores);
#ifdef HAVE_ZOOKEEPER_H
	if(!!mixer_) mixer_->accessed();
#endif
	estimate_results r;
	for (vector<classify_result_elem>::const_iterator p = scores.begin();
	     p != scores.end(); ++p){
	  if( isfinite(p->score) ){
	    r.push_back(estimate_result(p->label, p->score));
	  }else{
	    LOG(WARNING) << p->label << ":" << p->score;
	  }
	}
	ret.push_back(r);
      }catch(const std::exception& e){
	LOG(WARNING) << e.what();
	continue;
      }
    }
    return result<std::vector<estimate_results> >::ok(ret);
  };
  // 
  // 
  // haefasdfa ;;  // comment test
private:
  int a_;
  pfi::lang::shared_ptr<datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<classifier_base>       classifier_;
  pfi::lang::shared_ptr<storage::storage_base> storage_;

  vector<int> hogehoge;
  vector<map<int, string> > hogehoge;
};


class recommender {
public:
  int a(){};

};
