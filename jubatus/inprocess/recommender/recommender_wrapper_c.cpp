/*
 * Jubatus In-Process Wrapper
 */

#include "recommender_wrapper.hpp"
#include "../datum.hpp"

extern "C" {

#include "recommender_wrapper_c.h"
#include "../common.h"

typedef jubatus::inprocess::recommender::recommender jubarecommender;

const struct recommender_similar_result pack_similar_result(
    const std::vector<std::pair<std::string, float> >& r) {
  struct recommender_similar_result r2;
  int count = r.size();

  r2.size = count;
  r2.results = (recommender_similar_result_elem**) malloc(sizeof(recommender_similar_result_elem *) * count);
  for (int i = 0; i < count; ++i) {
	r2.results[i] = (recommender_similar_result_elem *) malloc(sizeof(recommender_similar_result_elem));
    std::string id_src = r[i].first;
    char* id_copy = (char *) malloc(id_src.length() + 1);
    std::memcpy(id_copy, id_src.c_str(), id_src.length());
    id_copy[id_src.length()] = '\0';
    r2.results[i]->id = id_copy;
    r2.results[i]->score = r[i].second;
  }
  return r2;
}

const JUBATUS_HANDLE recommender_create() {
  return new jubarecommender();
}

void recommender_dispose(const JUBATUS_HANDLE recommender) {
  delete static_cast<jubarecommender*>(recommender);
}

void recommender_update_row(
    const JUBATUS_HANDLE recommender,
    const char* id,
    const struct jubatus_datum* d) {
  static_cast<jubarecommender*>(recommender)->get_driver()->update_row(
    id, jubatus::inprocess::unpack_datum(*d));
}

struct recommender_similar_result recommender_similar_row_from_datum(
    const JUBATUS_HANDLE recommender,
    const struct jubatus_datum* d,
    size_t size) {
  return pack_similar_result(
      static_cast<jubarecommender*>(recommender)->get_driver()->similar_row_from_datum(
          jubatus::inprocess::unpack_datum(*d), size));
}

void recommender_similar_result_dispose(struct recommender_similar_result* result) {
    for (uint64_t i = 0; i < result->size; ++i) {
		free((void *) (result->results[i]->id));
		free((void *) (result->results[i]));
    }
    free(result->results);
}

void recommender_save(const JUBATUS_HANDLE recommender, char** data, size_t* length) {
    std::ostringstream oss;
    static_cast<jubarecommender*>(recommender)->save(oss);
    std::string str = oss.str();
    char* model = (char*) malloc(str.length());
    std::memcpy(model, str.c_str(), str.length());
    *data = model;
    *length = str.length();
}

void recommender_load(const JUBATUS_HANDLE recommender, const char* data, const size_t length) {
    std::istringstream iss(std::string(data, length));
    static_cast<jubarecommender*>(recommender)->load(iss);
}

void recommender_clear(const JUBATUS_HANDLE recommender) {
    static_cast<jubarecommender*>(recommender)->get_driver()->clear();
}

}  // extern "C"
