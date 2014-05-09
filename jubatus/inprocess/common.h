/*
 * Jubatus In-Process Wrapper
 */

#ifndef JUBATUS_INPROCESS_COMMON_H_
#define JUBATUS_INPROCESS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <stdint.h>

typedef void* JUBATUS_HANDLE;

struct jubatus_pair_sv {
  const char* key;
  const char* value;
};

struct jubatus_pair_nv {
  const char* key;
  float value;
};

struct jubatus_datum {
  uint64_t string_values_size;
  struct jubatus_pair_sv* string_values;

  uint64_t num_values_size;
  struct jubatus_pair_nv* num_values;
};

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // JUBATUS_INPROCESS_COMMON_H_
