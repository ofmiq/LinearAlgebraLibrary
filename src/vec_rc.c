#include "vec_rc.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "util.h"

/* ============================================================ */
/*                     Lifecycle Management                     */
/* ============================================================ */

util_error_t vec_alloc_rc(vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = (vec_t*)malloc(sizeof(vec_t));
  if (v == NULL) {
    return ERR_ALLOC;
  }

  v->n = n;

  size_t aligned_bytes = get_aligned_size(n);

  v->data = (double*)aligned_alloc(ALIGNMENT, aligned_bytes);
  if (v->data == NULL) {
    free(v);
    return ERR_ALLOC;
  }

  *out = v;
  return ERR_OK;
}

util_error_t vec_from_array_rc(const double* data, vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (data == NULL) {
    return ERR_NULL;
  }

  util_error_t rc = vec_alloc_rc(out, n);
  if (rc != ERR_OK) {
    return rc;
  }

  memcpy((*out)->data, data, n * sizeof(double));

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

util_error_t vec_resize_rc(vec_t** vp, size_t new_n) {
  if (vp == NULL || *vp == NULL) {
    return ERR_NULL;
  }

  if (new_n == 0 || new_n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = *vp;

  if (new_n == v->n) {
    return ERR_OK;
  }

  size_t new_aligned_bytes = get_aligned_size(new_n);

  double* new_data = (double*)aligned_alloc(ALIGNMENT, new_aligned_bytes);
  if (new_data == NULL) {
    return ERR_ALLOC;
  }

  size_t elements_to_copy = (v->n < new_n) ? v->n : new_n;
  memcpy(new_data, v->data, elements_to_copy * sizeof(double));

  if (new_n > v->n) {
    memset(new_data + v->n, 0, (new_n - v->n) * sizeof(double));
  }

  free(v->data);
  v->data = new_data;
  v->n = new_n;
  return ERR_OK;
}

/* ============================================================ */
/*                  Data Access and Inspection                  */
/* ============================================================ */

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

util_error_t vec_size_rc(const vec_t* restrict v, size_t* restrict out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }

  *out = v->n;
  return ERR_OK;
}

util_error_t vec_data_rc(const vec_t* restrict v, const double** restrict out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (v->data == NULL) {
    return ERR_NULL;
  }
  *out = v->data;

  return ERR_OK;
}

/* ============================================================ */
/*                   Basic Vector Arithmetic                    */
/* ============================================================ */

util_error_t vec_add_rc(const vec_t* restrict a, const vec_t* restrict b,
                        vec_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] + b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_add_inplace_rc(vec_t* restrict dest,
                                const vec_t* restrict src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != src->n) {
    return ERR_DIM;
  }

  const size_t n = dest->n;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  for (size_t i = 0; i < n; ++i) {
    dest_data[i] += src_data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] - b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_inplace_rc(vec_t* restrict dest,
                                     const vec_t* restrict src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != src->n) {
    return ERR_DIM;
  }

  const size_t n = dest->n;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  for (size_t i = 0; i < n; ++i) {
    dest_data[i] -= src_data[i];
  }

  return ERR_OK;
}

util_error_t vec_negate_rc(const vec_t* restrict v, vec_t* restrict out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (v->n != out->n) {
    return ERR_DIM;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = -v_data[i];
  }

  return ERR_OK;
}

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

util_error_t vec_scale_rc(const vec_t* restrict a, vec_t* restrict out,
                          double scalar) {
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != out->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t vec_scale_inplace_rc(vec_t* restrict v, double scalar) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  const size_t n = v->n;
  double* restrict v_data = v->data;

  for (size_t i = 0; i < n; ++i) {
    v_data[i] *= scalar;
  }

  return ERR_OK;
}

util_error_t vec_axpy_rc(double a, const vec_t* restrict x, vec_t* restrict y) {
  if (x == NULL || y == NULL) {
    return ERR_NULL;
  }
  if (x->data == NULL || y->data == NULL) {
    return ERR_NULL;
  }
  if (x->n != y->n) {
    return ERR_DIM;
  }

  const size_t n = x->n;
  const double* restrict x_data = x->data;
  double* restrict y_data = y->data;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    y_data[i] = a * x_data[i] + y_data[i];
    y_data[i + 1] = a * x_data[i + 1] + y_data[i + 1];
    y_data[i + 2] = a * x_data[i + 2] + y_data[i + 2];
    y_data[i + 3] = a * x_data[i + 3] + y_data[i + 3];
  }

  for (; i < n; ++i) {
    y_data[i] = a * x_data[i] + y_data[i];
  }

  return ERR_OK;
}

util_error_t vec_multiply_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || out->n != a->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_map_rc(const vec_t* restrict src, vec_t* restrict dest,
                        vec_map_func_t func) {
  if (src == NULL || dest == NULL || func == NULL) {
    return ERR_NULL;
  }
  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }
  if (src->n != dest->n) {
    return ERR_DIM;
  }

  const size_t n = src->n;
  const double* restrict src_data = src->data;
  double* restrict dest_data = dest->data;

  for (size_t i = 0; i < n; ++i) {
    double tmp = func(src_data[i]);
    if (!isfinite(tmp)) {
      return ERR_RANGE;
    }
    dest_data[i] = tmp;
  }

  return ERR_OK;
}

util_error_t vec_fill_rc(vec_t* restrict v, double val) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (!isfinite(val)) {
    return ERR_INVALID_ARG;
  }

  const size_t n = v->n;
  double* restrict v_data = v->data;

  for (size_t i = 0; i < n; ++i) {
    v_data[i] = val;
  }

  return ERR_OK;
}

/* ============================================================ */
/*           Vector Products and Geometric Properties           */
/* ============================================================ */

util_error_t vec_dot_rc(const vec_t* restrict a, const vec_t* restrict b,
                        double* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    sum0 += a_data[i] * b_data[i];
    sum1 += a_data[i + 1] * b_data[i + 1];
    sum2 += a_data[i + 2] * b_data[i + 2];
    sum3 += a_data[i + 3] * b_data[i + 3];
  }

  double total_sum = (sum0 + sum1) + (sum2 + sum3);

  for (; i < n; ++i) {
    total_sum += a_data[i] * b_data[i];
  }

  *out = total_sum;
  return ERR_OK;
}

util_error_t vec_cross_rc(const vec_t* restrict a, const vec_t* restrict b,
                          vec_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != 3 || b->n != 3 || out->n != 3) {
    return ERR_DIM;
  }

  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  double x = a_data[1] * b_data[2] - a_data[2] * b_data[1];
  double y = a_data[2] * b_data[0] - a_data[0] * b_data[2];
  double z = a_data[0] * b_data[1] - a_data[1] * b_data[0];

  out_data[0] = x;
  out_data[1] = y;
  out_data[2] = z;

  return ERR_OK;
}

util_error_t vec_cross_inplace_rc(vec_t* restrict dest,
                                  const vec_t* restrict src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }
  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }
  if (dest->n != 3 || src->n != 3) {
    return ERR_DIM;
  }

  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  double a0 = dest_data[0];
  double a1 = dest_data[1];
  double a2 = dest_data[2];
  double b0 = src_data[0];
  double b1 = src_data[1];
  double b2 = src_data[2];

  dest_data[0] = a1 * b2 - a2 * b1;
  dest_data[1] = a2 * b0 - a0 * b2;
  dest_data[2] = a0 * b1 - a1 * b0;

  return ERR_OK;
}

util_error_t vec_len_rc(const vec_t* restrict v, double* restrict out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    sum0 += v_data[i] * v_data[i];
    sum1 += v_data[i + 1] * v_data[i + 1];
    sum2 += v_data[i + 2] * v_data[i + 2];
    sum3 += v_data[i + 3] * v_data[i + 3];
  }

  double total_sum = (sum0 + sum1) + (sum2 + sum3);

  for (; i < n; ++i) {
    total_sum += v_data[i] * v_data[i];
  }

  *out = sqrt(total_sum);

  return ERR_OK;
}

util_error_t vec_normalize_inplace_rc(vec_t* restrict v) {
  if (v == NULL) {
    return ERR_NULL;
  }

  double len = 0.0;
  util_error_t rc = vec_len_rc(v, &len);
  if (rc != ERR_OK) {
    return rc;
  }

  if (len < VEC_EPSILON) {
    return ERR_DIV_ZERO;
  }

  double inv_len = 1.0 / len;

  return vec_scale_inplace_rc(v, inv_len);
}

util_error_t vec_angle_rc(const vec_t* restrict a, const vec_t* restrict b,
                          double* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  double dot = 0.0;
  util_error_t rc = vec_dot_rc(a, b, &dot);
  if (rc != ERR_OK) {
    return rc;
  }

  double len_a = 0.0;
  double len_b = 0.0;

  rc = vec_len_rc(a, &len_a);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_len_rc(b, &len_b);
  if (rc != ERR_OK) {
    return rc;
  }

  if (len_a < VEC_EPSILON || len_b < VEC_EPSILON) {
    return ERR_DIV_ZERO;
  }

  double cosine = dot / (len_a * len_b);

  if (cosine > 1.0) {
    cosine = 1.0;
  }
  if (cosine < -1.0) {
    cosine = -1.0;
  }

  *out = acos(cosine);
  return ERR_OK;
}

util_error_t vec_project_rc(const vec_t* restrict a, const vec_t* restrict b,
                            vec_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || out->n != b->n) {
    return ERR_DIM;
  }

  double dot_ab = 0.0;
  util_error_t rc = vec_dot_rc(a, b, &dot_ab);
  if (rc != ERR_OK) {
    return rc;
  }

  double dot_bb = 0.0;
  rc = vec_dot_rc(b, b, &dot_bb);
  if (rc != ERR_OK) {
    return rc;
  }

  if (dot_bb < VEC_EPSILON) {
    return ERR_DIV_ZERO;
  }

  double scale = dot_ab / dot_bb;

  const size_t n = b->n;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = scale * b_data[i];
  }

  return ERR_OK;
}

/* ============================================================ */
/*                    Comparison and Metrics                    */
/* ============================================================ */

util_error_t vec_is_equal_rc(const vec_t* restrict a, const vec_t* restrict b,
                             double epsilon, bool* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  for (size_t i = 0; i < n; ++i) {
    if (fabs(a_data[i] - b_data[i]) > epsilon) {
      *out = false;
      return ERR_OK;
    }
  }

  *out = true;
  return ERR_OK;
}

util_error_t vec_dist_rc(const vec_t* restrict a, const vec_t* restrict b,
                         double* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    double diff0 = b_data[i] - a_data[i];
    double diff1 = b_data[i + 1] - a_data[i + 1];
    double diff2 = b_data[i + 2] - a_data[i + 2];
    double diff3 = b_data[i + 3] - a_data[i + 3];

    sum0 += diff0 * diff0;
    sum1 += diff1 * diff1;
    sum2 += diff2 * diff2;
    sum3 += diff3 * diff3;
  }

  double total_sum = (sum0 + sum1) + (sum2 + sum3);

  for (; i < n; ++i) {
    double diff = b_data[i] - a_data[i];
    total_sum += diff * diff;
  }

  *out = sqrt(total_sum);

  return ERR_OK;
}

util_error_t vec_dist_sq_rc(const vec_t* restrict a, const vec_t* restrict b,
                            double* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  double diff0 = 0.0;
  double diff1 = 0.0;
  double diff2 = 0.0;
  double diff3 = 0.0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    diff0 = b_data[i] - a_data[i];
    diff1 = b_data[i + 1] - a_data[i + 1];
    diff2 = b_data[i + 2] - a_data[i + 2];
    diff3 = b_data[i + 3] - a_data[i + 3];

    sum0 += diff0 * diff0;
    sum1 += diff1 * diff1;
    sum2 += diff2 * diff2;
    sum3 += diff3 * diff3;
  }

  double total_sum = (sum0 + sum1) + (sum2 + sum3);

  for (; i < n; ++i) {
    double diff = b_data[i] - a_data[i];
    total_sum += diff * diff;
  }

  *out = total_sum;

  return ERR_OK;
}

/* ============================================================ */
/*              Utility and Statistical Functions               */
/* ============================================================ */

util_error_t vec_min_rc(const vec_t* restrict v, double* restrict out) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_DIM;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double min_val = v_data[0];
  for (size_t i = 1; i < n; ++i) {
    if (v_data[i] < min_val) {
      min_val = v_data[i];
    }
  }

  *out = min_val;

  return ERR_OK;
}

util_error_t vec_max_rc(const vec_t* restrict v, double* restrict out) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_DIM;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double max_val = v_data[0];
  for (size_t i = 1; i < n; ++i) {
    if (v_data[i] > max_val) {
      max_val = v_data[i];
    }
  }

  *out = max_val;

  return ERR_OK;
}

util_error_t vec_sum_rc(const vec_t* restrict v, double* restrict out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double sum0 = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;
  double sum3 = 0.0;

  size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    sum0 += v_data[i];
    sum1 += v_data[i + 1];
    sum2 += v_data[i + 2];
    sum3 += v_data[i + 3];
  }

  double total_sum = (sum0 + sum1) + (sum2 + sum3);
  for (; i < n; ++i) {
    total_sum += v_data[i];
  }

  *out = total_sum;
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

util_error_t vec_copy_rc(const vec_t* restrict src, vec_t* restrict dest) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL;
  }
  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }
  if (src->n != dest->n) {
    return ERR_DIM;
  }

  const size_t n = src->n;
  const double* restrict src_data = src->data;
  double* restrict dest_data = dest->data;

  memcpy(dest_data, src_data, n * sizeof(double));
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
