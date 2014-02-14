/*
 * Jubatus In-Process Wrapper
 */

#include "recommender_wrapper.hpp"
#include "../datum.hpp"

extern "C" {

#include "recommender_wrapper_c.h"
#include "../common.h"

typedef jubatus::inprocess::recommender::recommender jubarecommender;

/****************************************************************
 * Helper Functions
 ****************************************************************/

static const struct recommender_similar_result pack_similar_result(
    const std::vector<std::pair<std::string, float> >& r) {
  struct recommender_similar_result r2 = {0};
  size_t count = r.size();

  r2.size = count;
  if (0 < count) {
    r2.results = (recommender_similar_result_elem**) malloc(
        sizeof(recommender_similar_result_elem *) * count);
    for (size_t i = 0; i < count; ++i) {
      r2.results[i] = (recommender_similar_result_elem *) malloc(
          sizeof(recommender_similar_result_elem));
      std::string id_src = r[i].first;
      char* id_copy = (char *) malloc(id_src.length() + 1);
      std::memcpy(id_copy, id_src.c_str(), id_src.length());
      id_copy[id_src.length()] = '\0';
      r2.results[i]->id = id_copy;
      r2.results[i]->score = r[i].second;
    }
  }
  return r2;
}

struct recommender_rows pack_rows(std::vector<std::string>& r) {
  struct recommender_rows r2 = {0};
  size_t count = r.size();

  r2.size = count;
  if (0 < count) {
    r2.rows = (char **) malloc(sizeof(char *) * count);
    for (size_t i = 0; i < count; ++i) {
      r2.rows[i] = (char *) malloc(r[i].length() + 1);
      std::memcpy(r2.rows[i], r[i].c_str(), r[i].length());
      r2.rows[i][r[i].length()] = '\0';
    }
  }
  return r2;
}

/****************************************************************
 * API Implementations
 ****************************************************************/

const JUBATUS_HANDLE recommender_create() {
  return new jubarecommender();
}

const JUBATUS_HANDLE recommender_create_with_converter(
    JUBATUS_CONVERTER converter) {
  return new jubarecommender(
      *static_cast<jubatus::core::fv_converter::converter_config*>(converter));
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
  return pack_similar_result(static_cast<jubarecommender*>(recommender)
      ->get_driver()->similar_row_from_datum(
          jubatus::inprocess::unpack_datum(*d), size));
}

void recommender_similar_result_dispose(
    struct recommender_similar_result* result) {
  for (size_t i = 0; i < result->size; ++i) {
    free((void *) (result->results[i]->id));
    free((void *) (result->results[i]));
  }
  free(result->results);
}

struct recommender_rows recommender_get_all_rows(
    const JUBATUS_HANDLE recommender) {
  return pack_rows(static_cast<jubarecommender*>(recommender)
      ->get_driver()->get_all_rows());
}

void recommender_rows_dispose(struct recommender_rows* rows) {
  for (size_t i = 0; i < rows->size; ++i) {
    void* ptr = (void *) (rows->rows[i]);
    free(ptr);
  }
  if (0 < rows->size) {
    free(rows->rows);
  }
}

void recommender_clear_row(
    const JUBATUS_HANDLE recommender,
    const char* id) {
  static_cast<jubarecommender*>(recommender)->get_driver()->clear_row(id);
}

void recommender_save(
    const JUBATUS_HANDLE recommender,
    char** data,
    size_t* length) {
  std::ostringstream oss;
  static_cast<jubarecommender*>(recommender)->save(oss);
  std::string str = oss.str();
  char* model = (char*) malloc(str.length());
  std::memcpy(model, str.c_str(), str.length());
  *data = model;
  *length = str.length();
}

void recommender_load(
    const JUBATUS_HANDLE recommender,
    const char* data,
    const size_t length) {
  std::istringstream iss(std::string(data, length));
  static_cast<jubarecommender*>(recommender)->load(iss);
}

void recommender_save_data_dispose(char** data) {
  if (*data != NULL) {
    free(*data);
  }
}

void recommender_clear(const JUBATUS_HANDLE recommender) {
  static_cast<jubarecommender*>(recommender)->get_driver()->clear();
}

}  // extern "C"
