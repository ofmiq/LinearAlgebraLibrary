#include "vec.h"

#include <math.h>
#include <stdio.h>

#include "config.h"
#include "vec_rc.h"

vec_t* vec_alloc(size_t n) {
  vec_t* v = NULL;

  util_error_t rc = vec_alloc_rc(&v, n);

  if (rc != ERR_OK) {
    return NULL;
  }

  return v;
}

vec_t* vec_from_array(const double* src, size_t n) {
  vec_t* v = NULL;

  util_error_t rc = vec_from_array_rc(src, &v, n);

  if (rc != ERR_OK) {
    return NULL;
  }

  return v;
}

void vec_free(vec_t* v) { vec_free_rc(v); }

void vec_freep(vec_t** vp) { vec_freep_rc(vp); }

bool vec_set(vec_t* v, size_t i, double val) {
  util_error_t rc = vec_set_rc(v, i, val);

  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

double vec_get(const vec_t* v, size_t i) {
  double out = NAN;

  util_error_t rc = vec_get_rc(v, i, &out);

  if (rc != ERR_OK) {
    return NAN;
  }

  return out;
}

vec_t* vec_add_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_add_rc(a, b, result);

  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

bool vec_add_inplace(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return false;
  }

  util_error_t rc = vec_add_inplace_rc(dest, src);

  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

vec_t* vec_subtract_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_subtract_rc(a, b, result);

  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

bool vec_subtract_inplace(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return false;
  }

  util_error_t rc = vec_subtract_inplace_rc(dest, src);

  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

vec_t* vec_scale_new(const vec_t* a, double scalar) {
  if (a == NULL) {
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_scale_rc(a, result, scalar);

  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

void vec_scale_inplace(vec_t* v, double scalar) {
  if (v == NULL) {
    return;
  }

  util_error_t rc = vec_scale_inplace_rc(v, scalar);

  if (rc != ERR_OK) {
    return;
  }
}

double vec_dot(const vec_t* a, const vec_t* b) {
  double out = NAN;
  util_error_t rc = vec_dot_rc(a, b, &out);

  if (rc != ERR_OK) {
    return NAN;
  }

  return out;
}

vec_t* vec_cross_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  if (a->n != 3 || b->n != 3) {
    return NULL;
  }

  vec_t* result = vec_alloc(3);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_cross_rc(a, b, result);

  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

bool vec_cross_inplace(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return false;
  }

  util_error_t rc = vec_cross_inplace_rc(dest, src);

  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

double vec_len(const vec_t* v) {
  double out = NAN;
  util_error_t rc = vec_len_rc(v, &out);

  if (rc != ERR_OK) {
    return NAN;
  }

  return out;
}

vec_t* vec_duplicate(const vec_t* v) {
  if (v == NULL) {
    return NULL;
  }

  vec_t* duplicate = vec_alloc(v->n);
  if (duplicate == NULL) {
    return NULL;
  }

  util_error_t rc = vec_copy_rc(v, duplicate);

  if (rc != ERR_OK) {
    vec_free(duplicate);
    return NULL;
  }

  return duplicate;
}

bool vec_is_equal(const vec_t* a, const vec_t* b, double epsilon) {
  if (a == NULL || b == NULL) {
    return false;
  }

  bool result = vec_is_equal_rc(a, b, epsilon);

  return result;
}

vec_t* vec_normalized_new(const vec_t* v) {
  if (v == NULL) {
    return NULL;
  }

  vec_t* normalized = vec_duplicate(v);
  if (normalized == NULL) {
    return NULL;
  }

  util_error_t rc = vec_normalize_rc(normalized);

  if (rc != ERR_OK) {
    vec_free(normalized);
    return NULL;
  }

  return normalized;
}

bool vec_normalize(vec_t* v) {
  if (v == NULL) {
    return false;
  }

  util_error_t rc = vec_normalize_rc(v);

  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

double vec_dist(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NAN;
  }

  double dist = 0.0;

  util_error_t rc = vec_dist_rc(a, b, &dist);

  if (rc != ERR_OK) {
    return NAN;
  }

  return dist;
}

double vec_dist_sq(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NAN;
  }

  double dist = 0.0;

  util_error_t rc = vec_dist_sq_rc(a, b, &dist);

  if (rc != ERR_OK) {
    return NAN;
  }

  return dist;
}

vec_t* vec_multiply_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_multiply_rc(a, b, result);
  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

vec_t* vec_zeros(size_t n) {
  if (n == 0) {
    return NULL;
  }

  vec_t* v = vec_alloc(n);
  if (v == NULL) {
    return NULL;
  }

  util_error_t rc = vec_fill_rc(v, 0.0);
  if (rc != ERR_OK) {
    vec_free(v);
    return NULL;
  }

  return v;
}

vec_t* vec_ones(size_t n) {
  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return NULL;
  }

  vec_t* v = vec_alloc(n);
  if (v == NULL) {
    return NULL;
  }

  util_error_t rc = vec_fill_rc(v, 1.0);

  if (rc != ERR_OK) {
    vec_free(v);
    return NULL;
  }

  return v;
}

void vec_fill(vec_t* v, double val) {
  if (v == NULL) {
    return;
  }

  util_error_t rc = vec_fill_rc(v, val);
  if (rc != ERR_OK) {
    return;
  }
}

double vec_min(const vec_t* v) {
  if (v == NULL) {
    return NAN;
  }

  double min_val = 0.0;

  util_error_t rc = vec_min_rc(v, &min_val);

  if (rc != ERR_OK) {
    return NAN;
  }

  return min_val;
}

double vec_max(const vec_t* v) {
  if (v == NULL) {
    return NAN;
  }

  double max_val = 0.0;

  util_error_t rc = vec_max_rc(v, &max_val);

  if (rc != ERR_OK) {
    return NAN;
  }

  return max_val;
}

vec_t* vec_map_new(const vec_t* v, vec_map_func_t func) {
  if (v == NULL || func == NULL) {
    return NULL;
  }

  vec_t* result = vec_alloc(v->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_map_rc(v, result, func);
  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

size_t vec_size(const vec_t* vec) {
  if (vec == NULL) {
    return 0;
  }

  size_t size = 0;
  util_error_t rc = vec_size_rc(vec, &size);

  if (rc != ERR_OK) {
    return 0;
  }

  return size;
}

const double* vec_data(const vec_t* v) {
  if (v == NULL) {
    return NULL;
  }

  const double* data = NULL;
  util_error_t rc = vec_data_rc(v, &data);
  if (rc != ERR_OK) {
    return NULL;
  }

  return data;
}

vec_t* vec_resize_new(const vec_t* v, size_t new_n) {
  if (v == NULL) {
    return NULL;
  }

  vec_t* duplicate = vec_duplicate(v);
  if (duplicate == NULL) {
    return NULL;
  }

  util_error_t rc = vec_resize_rc(&duplicate, new_n);
  if (rc != ERR_OK) {
    vec_free(duplicate);
    return NULL;
  }

  return duplicate;
}

bool vec_resize_inplace(vec_t* v, size_t new_n) {
  if (v == NULL) {
    return false;
  }

  util_error_t rc = vec_resize_rc(&v, new_n);
  if (rc != ERR_OK) {
    return false;
  }

  return true;
}

vec_t* vec_axpy_new(double a, const vec_t* x, const vec_t* y) {
  if (x == NULL || y == NULL) {
    return NULL;
  }

  vec_t* result = vec_duplicate(y);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_axpy_rc(a, x, result);
  if (rc != ERR_OK) {
    vec_free(result);
    return NULL;
  }

  return result;
}

void vec_swap(vec_t* a, vec_t* b) {
  if (a == NULL || b == NULL) {
    return;
  }

  util_error_t rc = vec_swap_rc(a, b);
  if (rc != ERR_OK) {
    return;
  }
}

void vec_print(const vec_t* v) {
  util_error_t rc = vec_print_rc(v);

  if (rc != ERR_OK) {
  }
}