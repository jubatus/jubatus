/**
 * Jubatus In-Process Wrapper for Recommender
 */

#ifndef JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_H_
#define JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <stdint.h>

#include "../common.h"

#ifdef _WIN32
#  ifdef _EXPORTING_JUBATUS_DLL
#    define JUBATUS_PUBLIC_API __declspec(dllexport)
#  else
#    define JUBATUS_PUBLIC_API __declspec(dllimport)
#  endif
#else
#  define JUBATUS_PUBLIC_API
#endif

struct recommender_similar_result_elem {
  const char* id;
  float score;
};

struct recommender_similar_result {
  uint64_t size;
  struct recommender_similar_result_elem** results;
};

JUBATUS_PUBLIC_API
const JUBATUS_HANDLE recommender_create();

JUBATUS_PUBLIC_API
void recommender_dispose(const JUBATUS_HANDLE recommender);

JUBATUS_PUBLIC_API
void recommender_update_row(
    const JUBATUS_HANDLE recommender,
    const char* id,
    const struct jubatus_datum* d);

JUBATUS_PUBLIC_API
struct recommender_similar_result recommender_similar_row_from_datum(
    const JUBATUS_HANDLE recommender,
    const struct jubatus_datum* d,
    size_t size);

JUBATUS_PUBLIC_API
void recommender_similar_result_dispose(struct recommender_similar_result* result);

/* Note: caller is responsible to free the returned pointer */
JUBATUS_PUBLIC_API
void recommender_save(const JUBATUS_HANDLE recommender, char** data, size_t* length);

JUBATUS_PUBLIC_API
void recommender_load(const JUBATUS_HANDLE recommender, const char* data, const size_t length);

JUBATUS_PUBLIC_API
void recommender_clear(const JUBATUS_HANDLE recommender);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // JUBATUS_INPROCESS_RECOMMENDER_RECOMMENDER_H_
