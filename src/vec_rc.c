#include "vec_rc.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

util_error_t vec_alloc_rc(vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = malloc(sizeof *v);
  if (v == NULL) {
    return ERR_ALLOC;
  }

  v->n = 0;
  v->data = NULL;

  if (n > SIZE_MAX / sizeof *v->data) {
    free(v);
    return ERR_RANGE;
  }

  v->data = malloc(sizeof *v->data * n);
  if (v->data == NULL) {
    free(v);
    return ERR_ALLOC;
  }

  v->n = n;
  *out = v;

  return ERR_OK;
}

void vec_free_rc(vec_t* v) {
  if (!v) {
    return;
  }

  free(v->data);
  free(v);
}

void vec_freep_rc(vec_t** vp) {
  if (vp == NULL || *vp == NULL) {
    return;
  }

  free((*vp)->data);
  free(*vp);
  *vp = NULL;
}

util_error_t vec_from_array_rc(const double* data, vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (data == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = NULL;
  util_error_t err = vec_alloc_rc(&v, n);
  if (err != ERR_OK) {
    return err;
  }

  memcpy(v->data, data, sizeof(double) * n);
  *out = v;
  return ERR_OK;
}

util_error_t vec_set_rc(vec_t* v, size_t i, double val) {
  if (v == NULL || v->data == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  v->data[i] = val;
  return ERR_OK;
}

util_error_t vec_get_rc(const vec_t* v, size_t i, double* out) {
  if (v == NULL || v->data == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  *out = v->data[i];
  return ERR_OK;
}

util_error_t vec_add_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] + b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_add_inplace_rc(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != src->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < dest->n; ++i) {
    dest->data[i] += src->data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] - b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_inplace_rc(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != src->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < dest->n; ++i) {
    dest->data[i] -= src->data[i];
  }

  return ERR_OK;
}

util_error_t vec_scale_rc(const vec_t* a, vec_t* out, double scalar) {
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t vec_scale_inplace_rc(vec_t* v, double scalar) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  for (size_t i = 0; i < v->n; ++i) {
    v->data[i] *= scalar;
  }

  return ERR_OK;
}

util_error_t vec_dot_rc(const vec_t* a, const vec_t* b, double* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  double result = 0.0;
  for (size_t i = 0; i < a->n; ++i) {
    result += a->data[i] * b->data[i];
  }

  *out = result;
  return ERR_OK;
}

util_error_t vec_cross_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != 3 || b->n != 3 || out->n != 3) {
    return ERR_DIM;
  }

  double x = a->data[1] * b->data[2] - a->data[2] * b->data[1];
  double y = a->data[2] * b->data[0] - a->data[0] * b->data[2];
  double z = a->data[0] * b->data[1] - a->data[1] * b->data[0];

  out->data[0] = x;
  out->data[1] = y;
  out->data[2] = z;

  return ERR_OK;
}

util_error_t vec_cross_inplace_rc(vec_t* dest, const vec_t* src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != 3 || src->n != 3) {
    return ERR_DIM;
  }

  double a0 = dest->data[0];
  double a1 = dest->data[1];
  double a2 = dest->data[2];
  double b0 = src->data[0];
  double b1 = src->data[1];
  double b2 = src->data[2];

  dest->data[0] = a1 * b2 - a2 * b1;
  dest->data[1] = a2 * b0 - a0 * b2;
  dest->data[2] = a0 * b1 - a1 * b0;

  return ERR_OK;
}

util_error_t vec_len_rc(const vec_t* v, double* out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  double sum = 0.0;
  
  for (size_t i = 0; i < v->n; ++i) {
    double x = v->data[i];
    sum += x * x;
  }
  
  *out = sqrt(sum);
  
  return ERR_OK;
}

util_error_t vec_copy_rc(const vec_t* src, vec_t* dest) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL;
  }
  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }
  if (src->n != dest->n) {
    return ERR_DIM;
  }

  memcpy(dest->data, src->data, src->n * sizeof(double));
  return ERR_OK;
}

bool vec_is_equal_rc(const vec_t* a, const vec_t* b, double epsilon) {
  if (a == NULL || b == NULL) {
    return false;
  }
  if (a->data == NULL || b->data == NULL) {
    return false;
  }
  if (a->n != b->n) {
    return false;
  }

  for (size_t i = 0; i < a->n; ++i) {
    if (fabs(a->data[i] - b->data[i]) > epsilon) {
      return false;
    }
  }

  return true;
}

util_error_t vec_normalize_rc(vec_t* v) {
  if (v == NULL) {
    return ERR_NULL;
  }

  double len = 0.0;
  util_error_t rc = vec_len_rc(v, &len);
  if (rc != ERR_OK) {
    return rc;
  }

  if (len < VEC_EPSILON) {
    return ERR_RANGE;
  }

  double inv_len = 1.0 / len;

  return vec_scale_inplace_rc(v, inv_len);
}

util_error_t vec_dist_rc(const vec_t* a, const vec_t* b, double* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  double sum = 0.0;

  for (size_t i = 0; i < a->n; ++i) {
    double diff = b->data[i] - a->data[i];
    sum += diff * diff;
  }

  *out = sqrt(sum);

  return ERR_OK;
}

util_error_t vec_dist_sq_rc(const vec_t* a, const vec_t* b, double* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  double sum = 0.0;

  for (size_t i = 0; i < a->n; ++i) {
    double diff = b->data[i] - a->data[i];
    sum += diff * diff;
  }

  *out = sum;

  return ERR_OK;
}

util_error_t vec_multiply_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || out->n != a->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] * b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_fill_rc(vec_t* v, double val) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (!isfinite(val)) {
    return ERR_INVALID_ARG;
  }

  for (size_t i = 0; i < v->n; ++i) {
    v->data[i] = val;
  }

  return ERR_OK;
}

util_error_t vec_min_rc(const vec_t* v, double* out) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_DIM;
  }

  double min_val = v->data[0];
  for (size_t i = 1; i < v->n; ++i) {
    if (v->data[i] < min_val) {
      min_val = v->data[i];
    }
  }

  *out = min_val;

  return ERR_OK;
}

util_error_t vec_max_rc(const vec_t* v, double* out) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_DIM;
  }

  double max_val = v->data[0];
  for (size_t i = 1; i < v->n; ++i) {
    if (v->data[i] > max_val) {
      max_val = v->data[i];
    }
  }

  *out = max_val;

  return ERR_OK;
}

util_error_t vec_map_rc(const vec_t* src, vec_t* dest, vec_map_func_t func) {
  if (src == NULL || dest == NULL || func == NULL) {
    return ERR_NULL;
  }
  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }
  if (src->n != dest->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < src->n; ++i) {
    double tmp = func(src->data[i]);
    if (!isfinite(tmp)) {
      return ERR_RANGE;
    }
    dest->data[i] = tmp;
  }

  return ERR_OK;
}

util_error_t vec_size_rc(const vec_t* v, size_t* out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }

  *out = v->n;
  return ERR_OK;
}

util_error_t vec_data_rc(const vec_t* v, const double** out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (v->data == NULL) {
    return ERR_NULL;
  }
  *out = v->data;

  return ERR_OK;
}

util_error_t vec_resize_rc(vec_t** vp, size_t new_n) {
  if (vp == NULL || *vp == NULL) {
    return ERR_NULL;
  }
  vec_t* v = *vp;

  if (new_n == v->n) {
    return ERR_OK;
  }

  if (new_n == 0 || new_n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  if (new_n > SIZE_MAX / sizeof *v->data) {
    return ERR_RANGE;
  }

  double* new_data = realloc(v->data, new_n * sizeof *v->data);
  if (new_data == NULL) {
    return ERR_ALLOC;
  }

  if (new_n > v->n) {
    memset(new_data + v->n, 0, (new_n - v->n) * sizeof *new_data);
  }

  v->data = new_data;
  v->n = new_n;
  return ERR_OK;
}

util_error_t vec_axpy_rc(double a, const vec_t* x, vec_t* y) {
  if (x == NULL || y == NULL) {
    return ERR_NULL;
  }
  if (x->data == NULL || y->data == NULL) {
    return ERR_NULL;
  }
  if (x->n != y->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < x->n; ++i) {
    y->data[i] = a * x->data[i] + y->data[i];
  }

  return ERR_OK;
}

util_error_t vec_swap_rc(vec_t* a, vec_t* b) {
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  size_t temp_n = a->n;
  a->n = b->n;
  b->n = temp_n;

  double* temp_data = a->data;
  a->data = b->data;
  b->data = temp_data;

  return ERR_OK;
}

util_error_t vec_print_rc(const vec_t* v) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_OK;
  }

  printf("(");
  for (size_t i = 0; i < v->n; ++i) {
    printf("%g", v->data[i]);
    if (i < v->n - 1) printf(", ");
  }
  printf(")\n");

  return ERR_OK;
}
