#include "vec.h"

#include <math.h>
#include <stdio.h>

#include "vec_rc.h"

vec_t* vec_alloc(size_t n) {
  vec_t* v = NULL;

  util_error_t rc = vec_alloc_rc(&v, n);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_alloc(%zu) failed: %s\n", n, util_error_str(rc));
    return NULL;
  }

  return v;
}

vec_t* vec_from_array(const double* src, size_t n) {
  vec_t* v = NULL;

  util_error_t rc = vec_from_array_rc(src, &v, n);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_from_array(...,%zu) failed: %s\n", n,
            util_error_str(rc));
    return NULL;
  }

  return v;
}

void vec_free(vec_t* v) {
  vec_free_rc(v);
}

void vec_freep(vec_t** vp) {
  vec_freep_rc(vp);
}

bool vec_set(vec_t* v, size_t i, double val) {
  util_error_t rc = vec_set_rc(v, i, val);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_set failed: %s\n", util_error_str(rc));
    return false;
  }

  return true;
}

double vec_get(const vec_t* v, size_t i) {
  double out = NAN;

  util_error_t rc = vec_get_rc(v, i, &out);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_get failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return out;
}

vec_t* vec_add_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_add_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_add_rc(a, b, result);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_add_new failed: %s\n", util_error_str(rc));
    vec_free(result);
    return NULL;
  }

  return result;
}

vec_t* vec_subtract_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_substract_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_subtract_rc(a, b, result);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_substract_new failed: %s\n", util_error_str(rc));
    vec_free(result);
    return NULL;
  }

  return result;
}

vec_t* vec_scale_new(const vec_t* a, double scalar) {
  if (a == NULL) {
    fprintf(stderr, "vec_scale_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_scale_rc(a, result, scalar);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_scale_new failed: %s\n", util_error_str(rc));
    vec_free(result);
    return NULL;
  }

  return result;
}

double vec_dot(const vec_t* a, const vec_t* b) {
  double out = NAN;
  util_error_t rc = vec_dot_rc(a, b, &out);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_dot failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return out;
}

void vec_print(const vec_t* v) {
  util_error_t rc = vec_print_rc(v);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_print failed: %s\n", util_error_str(rc));
  }
}