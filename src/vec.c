#include "vec.h"

#include <math.h>
#include <stdio.h>

#include "config.h"
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

void vec_free(vec_t* v) { vec_free_rc(v); }

void vec_freep(vec_t** vp) { vec_freep_rc(vp); }

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
    fprintf(stderr, "vec_subtract_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_subtract_rc(a, b, result);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_subtract_new failed: %s\n", util_error_str(rc));
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

vec_t* vec_cross_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_cross_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_cross_rc(a, b, result);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_cross_new failed: %s\n", util_error_str(rc));
    vec_free(result);
    return NULL;
  }

  return result;
}

double vec_len(const vec_t* v) {
  double out = NAN;
  util_error_t rc = vec_len_rc(v, &out);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_len failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return out;
}

vec_t* vec_duplicate(const vec_t* v) {
  if (v == NULL) {
    fprintf(stderr, "vec_duplicate failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* duplicate = vec_alloc(v->n);
  if (duplicate == NULL) {
    return NULL;
  }

  util_error_t rc = vec_copy_rc(v, duplicate);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_duplicate failed: %s\n", util_error_str(rc));
    vec_free(duplicate);
    return NULL;
  }

  return duplicate;
}

bool vec_is_equal(const vec_t* a, const vec_t* b, double epsilon) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_is_equal failed: %s\n",
            util_error_str(ERR_NULL));
    return false;
  }

  bool result = vec_is_equal_rc(a, b, epsilon);

  return result;
}

vec_t* vec_normalized_new(const vec_t* v) {
  if (v == NULL) {
    fprintf(stderr, "vec_normalized_new failed: %s\n",
            util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* normalized = vec_duplicate(v);
  if (normalized == NULL) {
    return NULL;
  }

  util_error_t rc = vec_normalize_rc(normalized);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_normalized_new failed: %s\n", util_error_str(rc));
    vec_free(normalized);
    return NULL;
  }

  return normalized;
}

double vec_dist(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_dist failed: %s\n", util_error_str(ERR_INVALID_ARG));
    return NAN;
  }

  double dist = 0.0;

  util_error_t rc = vec_dist_rc(a, b, &dist);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_dist failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return dist;
}

double vec_dist_sq(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_dist failed: %s\n", util_error_str(ERR_INVALID_ARG));
    return NAN;
  }

  double dist = 0.0;

  util_error_t rc = vec_dist_sq_rc(a, b, &dist);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_dist failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return dist;
}

vec_t* vec_multiply_new(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "vec_multiply_new failed: %s\n", util_error_str(ERR_NULL));
    return NULL;
  }

  vec_t* result = vec_alloc(a->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_multiply_rc(a, b, result);
  if (rc != ERR_OK) {
    fprintf(stderr, "vec_multiply_new failed: %s\n", util_error_str(rc));
    return NULL;
  }

  return result;
}

vec_t* vec_zeros(size_t n) {
  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return NULL;
  }

  vec_t* v = (vec_t*)malloc(sizeof(vec_t));
  if (v == NULL) {
    return NULL;
  }

  v->data = (double*)calloc(n, sizeof(double));
  if (v->data == NULL) {
    free(v);
    return NULL;
  }

  v->n = n;

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
    fprintf(stderr, "vec_ones failed: %s\n", util_error_str(rc));
    vec_free(v);
    return NULL;
  }

  return v;
}

double vec_min(const vec_t* v) {
  if (v == NULL) {
    fprintf(stderr, "vec_min failed: %s\n", util_error_str(ERR_NULL));
    return NAN;
  }

  double min_val = 0.0;

  util_error_t rc = vec_min_rc(v, &min_val);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_min failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return min_val;
}

double vec_max(const vec_t* v) {
  if (v == NULL) {
    fprintf(stderr, "vec_max failed: %s\n", util_error_str(ERR_NULL));
    return NAN;
  }

  double max_val = 0.0;

  util_error_t rc = vec_max_rc(v, &max_val);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_min failed: %s\n", util_error_str(rc));
    return NAN;
  }

  return max_val;
}

vec_t* vec_map_new(const vec_t* v, vec_map_func_t func) {
  if (v == NULL || func == NULL) {
    return ERR_NULL;
  }

  vec_t* result = vec_alloc(v->n);
  if (result == NULL) {
    return NULL;
  }

  util_error_t rc = vec_map_rc(v, result, func);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_map_new failed: %s\n", util_error_str(rc));
    vec_free(result);
    return NULL;
  }

  return result;
}

void vec_print(const vec_t* v) {
  util_error_t rc = vec_print_rc(v);

  if (rc != ERR_OK) {
    fprintf(stderr, "vec_print failed: %s\n", util_error_str(rc));
  }
}