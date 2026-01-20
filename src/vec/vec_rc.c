#include "vec_rc.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "util.h"
#include "vec_internal.h"

/* ============================================================ */
/*                     Lifecycle Management                     */
/* ============================================================ */

util_error_t vec_alloc_rc(vec_t** out, size_t n) {
  VEC_REQUIRE_NON_NULL(out);

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
  VEC_REQUIRE_NON_NULL_2(data, out);

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
  VEC_REQUIRE_NON_NULL(vp);
  VEC_REQUIRE_VALID_VEC(*vp);

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
  VEC_REQUIRE_VALID_VEC(v);
  
  if (i >= v->n) {
    return ERR_RANGE;
  }

  v->data[i] = val;
  return ERR_OK;
}

util_error_t vec_get_rc(const vec_t* v, size_t i, double* out) {
  VEC_REQUIRE_VALID_VEC(v);
  VEC_REQUIRE_NON_NULL(out);
  
  if (i >= v->n) {
    return ERR_RANGE;
  }

  *out = v->data[i];
  return ERR_OK;
}

util_error_t vec_size_rc(const vec_t* restrict v, size_t* restrict out) {
  VEC_REQUIRE_NON_NULL_2(v, out);

  *out = v->n;
  return ERR_OK;
}

util_error_t vec_data_rc(const vec_t* restrict v, const double** restrict out) {
  VEC_REQUIRE_VALID_VEC(v);
  VEC_REQUIRE_NON_NULL(out);

  *out = v->data;

  return ERR_OK;
}

/* ============================================================ */
/*                   Basic Vector Arithmetic                    */
/* ============================================================ */

util_error_t vec_add_rc(const vec_t* restrict a, const vec_t* restrict b,
                        vec_t* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_3(a, b, out);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_same_size(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] + b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_add_inplace_rc(vec_t* restrict dest,
                                const vec_t* restrict src) {
  VEC_REQUIRE_NON_NULL_2(dest, src);
  VEC_REQUIRE_VALID_VEC_2(dest, src);

  util_error_t rc = vec_require_same_size(dest, src);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = dest->n;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    dest_data[i] += src_data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_3(a, b, out);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_same_size(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] - b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_inplace_rc(vec_t* restrict dest,
                                     const vec_t* restrict src) {
  VEC_REQUIRE_NON_NULL_2(dest, src);
  VEC_REQUIRE_VALID_VEC_2(dest, src);

  util_error_t rc = vec_require_same_size(dest, src);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = dest->n;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    dest_data[i] -= src_data[i];
  }

  return ERR_OK;
}

util_error_t vec_negate_rc(const vec_t* restrict v, vec_t* restrict out) {
  VEC_REQUIRE_NON_NULL_2(v, out);
  VEC_REQUIRE_VALID_VEC_2(v, out);

  util_error_t rc = vec_require_same_size(v, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = v->n;
  const double* restrict v_data = v->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
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
  VEC_REQUIRE_NON_NULL_2(a, out);
  VEC_REQUIRE_VALID_VEC_2(a, out);

  util_error_t rc = vec_require_same_size(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t vec_scale_inplace_rc(vec_t* restrict v, double scalar) {
  VEC_REQUIRE_VALID_VEC(v);

  const size_t n = v->n;
  double* restrict v_data = v->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    v_data[i] *= scalar;
  }

  return ERR_OK;
}

util_error_t vec_axpy_rc(double a, const vec_t* restrict x, vec_t* restrict y) {
  VEC_REQUIRE_NON_NULL_2(x, y);
  VEC_REQUIRE_VALID_VEC_2(x, y);

  util_error_t rc = vec_require_same_size(x, y);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = x->n;
  const double* restrict x_data = x->data;
  double* restrict y_data = y->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    y_data[i] = a * x_data[i] + y_data[i];
  }

  return ERR_OK;
}

util_error_t vec_multiply_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_3(a, b, out);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_same_size(out, a);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * b_data[i];
  }

  return ERR_OK;
}

util_error_t vec_map_rc(const vec_t* restrict src, vec_t* restrict dest,
                        vec_map_func_t func) {
  VEC_REQUIRE_NON_NULL_3(src, dest, func);
  VEC_REQUIRE_VALID_VEC_2(src, dest);

  util_error_t rc = vec_require_same_size(src, dest);
  if (rc != ERR_OK) {
    return rc;
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
  VEC_REQUIRE_VALID_VEC(v);

  if (!isfinite(val)) {
    return ERR_INVALID_ARG;
  }

  const size_t n = v->n;
  double* restrict v_data = v->data;

  #pragma omp parallel for schedule(static)
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
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_2(a, b);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum = 0.0;

  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    sum += a_data[i] * b_data[i];
  }

  *out = sum;
  
  return ERR_OK;
}

util_error_t vec_cross_rc(const vec_t* restrict a, const vec_t* restrict b,
                          vec_t* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_3(a, b, out);

  util_error_t rc = vec_require_size(a, 3);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_size(b, 3);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_size(out, 3);
  if (rc != ERR_OK) {
    return rc;
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
  VEC_REQUIRE_NON_NULL_2(dest, src);
  VEC_REQUIRE_VALID_VEC_2(dest, src);

  util_error_t rc = vec_require_size(dest, 3);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_size(src, 3);
  if (rc != ERR_OK) {
    return rc;
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
  VEC_REQUIRE_NON_NULL_2(v, out);
  VEC_REQUIRE_VALID_VEC(v);

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double sum = 0.0;

  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    sum += v_data[i] * v_data[i];
  }

  *out = sqrt(sum);

  return ERR_OK;
}

util_error_t vec_normalize_inplace_rc(vec_t* restrict v) {
  VEC_REQUIRE_VALID_VEC(v);

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
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_2(a, b);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  double dot = 0.0;
  rc = vec_dot_rc(a, b, &dot);
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
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_3(a, b, out);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = vec_require_same_size(out, b);
  if (rc != ERR_OK) {
    return rc;
  }

  double dot_ab = 0.0;
  rc = vec_dot_rc(a, b, &dot_ab);
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

  #pragma omp parallel for schedule(static)
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
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_2(a, b);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  int diff = 0;

  #pragma omp parallel for reduction(|:diff) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    if (fabs(a_data[i] - b_data[i]) > epsilon) {
      diff = 1;
    }
  }

  *out = (diff == 0);
  
  return ERR_OK;
}

util_error_t vec_dist_rc(const vec_t* restrict a, const vec_t* restrict b,
                         double* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_2(a, b);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum = 0.0;

  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    double d = b_data[i] - a_data[i];
    sum += d * d;
  }

  *out = sqrt(sum);

  return ERR_OK;
}

util_error_t vec_dist_sq_rc(const vec_t* restrict a, const vec_t* restrict b,
                            double* restrict out) {
  VEC_REQUIRE_NON_NULL_3(a, b, out);
  VEC_REQUIRE_VALID_VEC_2(a, b);

  util_error_t rc = vec_require_same_size(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->n;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  double sum = 0.0;

  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    double d = b_data[i] - a_data[i];
    sum += d * d;
  }

  *out = sum;

  return ERR_OK;
}

/* ============================================================ */
/*              Utility and Statistical Functions               */
/* ============================================================ */

util_error_t vec_min_rc(const vec_t* restrict v, double* restrict out) {
  VEC_REQUIRE_NON_NULL_2(v, out);
  VEC_REQUIRE_VALID_VEC(v);

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
  VEC_REQUIRE_NON_NULL_2(v, out);
  VEC_REQUIRE_VALID_VEC(v);

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
  VEC_REQUIRE_NON_NULL_2(v, out);
  VEC_REQUIRE_VALID_VEC(v);

  const size_t n = v->n;
  const double* restrict v_data = v->data;

  double sum = 0.0;

  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    sum += v_data[i];
  }

  *out = sum;

  return ERR_OK;
}

util_error_t vec_swap_rc(vec_t* a, vec_t* b) {
  VEC_REQUIRE_NON_NULL_2(a, b);

  size_t temp_n = a->n;
  a->n = b->n;
  b->n = temp_n;

  double* temp_data = a->data;
  a->data = b->data;
  b->data = temp_data;

  return ERR_OK;
}

util_error_t vec_copy_rc(const vec_t* restrict src, vec_t* restrict dest) {
  VEC_REQUIRE_NON_NULL_2(src, dest);
  VEC_REQUIRE_VALID_VEC_2(src, dest);

  util_error_t rc = vec_require_same_size(src, dest);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = src->n;
  const double* restrict src_data = src->data;
  double* restrict dest_data = dest->data;

  memcpy(dest_data, src_data, n * sizeof(double));
  return ERR_OK;
}

util_error_t vec_print_rc(const vec_t* v) {
  VEC_REQUIRE_VALID_VEC(v);

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
