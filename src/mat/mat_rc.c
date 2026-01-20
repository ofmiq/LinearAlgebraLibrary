#include "mat_rc.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "mat_internal.h"
#include "vec/vec_internal.h"

/* ============================================================ */
/*                      Lifecycle Management                    */
/* ============================================================ */

util_error_t mat_alloc_rc(mat_t** restrict out, size_t rows, size_t cols) {
  MAT_REQUIRE_NON_NULL(out);

  if (rows == 0 || cols == 0) {
    return ERR_RANGE;
  }

  if (rows > MATRIX_MAX_ROWS || cols > MATRIX_MAX_COLUMNS) {
    return ERR_RANGE;
  }

  if (rows > SIZE_MAX / cols) {
    return ERR_RANGE;
  }

  size_t elements = rows * cols;

  if (elements > MATRIX_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  mat_t* m = (mat_t*)malloc(sizeof(mat_t));
  if (m == NULL) {
    return ERR_ALLOC;
  }

  m->rows = rows;
  m->cols = cols;

  size_t aligned_bytes = get_aligned_size(elements);

  m->data = (double*)aligned_alloc(ALIGNMENT, aligned_bytes);
  if (m->data == NULL) {
    free(m);
    return ERR_ALLOC;
  }

  *out = m;
  return ERR_OK;
}

util_error_t mat_from_array_rc(const double* restrict data,
                               mat_t** restrict out, size_t rows, size_t cols) {
  MAT_REQUIRE_NON_NULL_2(data, out);

  util_error_t rc = mat_alloc_rc(out, rows, cols);
  if (rc != ERR_OK) {
    return rc;
  }

  size_t elements = rows * cols;
  memcpy((*out)->data, data, elements * sizeof(double));

  return ERR_OK;
}

void mat_free_rc(mat_t* m) {
  if (!m) {
    return;
  }

  free(m->data);
  free(m);
}

void mat_freep_rc(mat_t** restrict mp) {
  if (mp == NULL || *mp == NULL) {
    return;
  }

  free((*mp)->data);
  free(*mp);
  *mp = NULL;
}

util_error_t mat_resize_rc(mat_t** restrict mp, size_t new_rows,
                           size_t new_cols) {
  MAT_REQUIRE_NON_NULL(mp);
  MAT_REQUIRE_VALID_MAT(*mp);

  if (new_rows == 0 || new_cols == 0) {
    return ERR_RANGE;
  }

  mat_t* m = *mp;

  if (new_rows == m->rows && new_cols == m->cols) {
    return ERR_OK;
  }

  if (new_rows > MATRIX_MAX_ROWS || new_cols > MATRIX_MAX_COLUMNS) {
    return ERR_RANGE;
  }

  if (new_rows > SIZE_MAX / new_cols) {
    return ERR_RANGE;
  }

  size_t new_elements = new_rows * new_cols;
  if (new_elements > MATRIX_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  size_t new_aligned_bytes = get_aligned_size(new_elements);
  double* new_data = (double*)aligned_alloc(ALIGNMENT, new_aligned_bytes);
  if (new_data == NULL) {
    return ERR_ALLOC;
  }

  memset(new_data, 0, new_elements * sizeof(double));

  size_t copy_rows = (m->rows < new_rows) ? m->rows : new_rows;
  size_t copy_cols = (m->cols < new_cols) ? m->cols : new_cols;
  size_t row_copy_size = copy_cols * sizeof(double);

  const double* restrict src_base = m->data;
  double* restrict dst_base = new_data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < copy_rows; ++i) {
    const double* src_row = src_base + (i * m->cols);
    double* dst_row = dst_base + (i * new_cols);
    memcpy(dst_row, src_row, row_copy_size);
  }

  free(m->data);
  m->data = new_data;
  m->rows = new_rows;
  m->cols = new_cols;

  return ERR_OK;
}

/* ============================================================ */
/*                Data Access and Inspection                    */
/* ============================================================ */

util_error_t mat_set_rc(mat_t* restrict m, size_t i, size_t j, double val) {
  MAT_REQUIRE_VALID_MAT(m);

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  MAT_AT(m, i, j) = val;
  return ERR_OK;
}

util_error_t mat_set_row_rc(mat_t* restrict m, size_t row,
                         const vec_t* restrict v) {
  MAT_REQUIRE_VALID_MAT(m);
  VEC_REQUIRE_VALID_VEC(v);

  if (row >= m->rows) {
    return ERR_RANGE;
  }

  if (v->n != m->cols) {
    return ERR_DIM;
  }

  double* dest_ptr = m->data + (row * m->cols);
  memcpy(dest_ptr, v->data, m->cols * sizeof(double));

  return ERR_OK;
}

util_error_t mat_set_column_rc(mat_t* restrict m, size_t col,
                            const vec_t* restrict v) {
  MAT_REQUIRE_VALID_MAT(m);
  VEC_REQUIRE_VALID_VEC(v);

  if (col >= m->cols) {
    return ERR_RANGE;
  }

  if (v->n != m->rows) {
    return ERR_DIM;
  }

  size_t rows = m->rows;
  size_t cols = m->cols;
  double* restrict m_data = m->data;
  const double* restrict v_data = v->data;

  double* current_ptr = m_data + col;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < rows; ++i) {
    *current_ptr = v_data[i];
    current_ptr += cols;
  }

  return ERR_OK;
}

util_error_t mat_get_rc(const mat_t* restrict m, size_t i, size_t j,
                        double* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  MAT_REQUIRE_NON_NULL(out);

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  *out = MAT_AT(m, i, j);
  return ERR_OK;
}

util_error_t mat_get_row_rc(const mat_t* restrict m, size_t row,
                         vec_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  VEC_REQUIRE_VALID_VEC(out);

  if (row >= m->rows) {
    return ERR_RANGE;
  }

  if (out->n != m->cols) {
    return ERR_DIM;
  }

  const double* src_ptr = m->data + (row * m->cols);
  memcpy(out->data, src_ptr, m->cols * sizeof(double));

  return ERR_OK;
}

util_error_t mat_get_column_rc(const mat_t* restrict m, size_t col,
                            vec_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  VEC_REQUIRE_VALID_VEC(out);

  if (col >= m->cols) {
    return ERR_RANGE;
  }

  if (out->n != m->rows) {
    return ERR_DIM;
  }

  const size_t rows = m->rows;
  const size_t stride = m->cols;
  const double* restrict m_data = m->data;
  double* restrict v_data = out->data;

  const double* current_src = m_data + col;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < rows; ++i) {
    v_data[i] = *current_src;
    current_src += stride;
  }

  return ERR_OK;
}

util_error_t mat_rows_rc(const mat_t* restrict m, size_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  MAT_REQUIRE_NON_NULL(out);

  *out = m->rows;

  return ERR_OK;
}

util_error_t mat_cols_rc(const mat_t* restrict m, size_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  MAT_REQUIRE_NON_NULL(out);

  *out = m->cols;

  return ERR_OK;
}

util_error_t mat_data_rc(const mat_t* restrict m, const double** restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  MAT_REQUIRE_NON_NULL(out);

  *out = m->data;
  return ERR_OK;
}

/* ============================================================ */
/*                    Initialization Helpers                    */
/* ============================================================ */

util_error_t mat_fill_rc(mat_t* restrict m, double val) {
  MAT_REQUIRE_VALID_MAT(m);

  if (!isfinite(val)) {
    return ERR_INVALID_ARG;
  }

  const size_t n = m->rows * m->cols;
  double* restrict m_data = m->data;
  
  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    m_data[i] = val;
  }

  return ERR_OK;
}

util_error_t mat_zeros_rc(mat_t* restrict m) {
  MAT_REQUIRE_VALID_MAT(m);

  memset(m->data, 0, m->rows * m->cols * sizeof(double));

  return ERR_OK;
}

util_error_t mat_identity_rc(mat_t* restrict m) {
  MAT_REQUIRE_VALID_MAT(m);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  memset(m->data, 0, m->rows * m->cols * sizeof(double));

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < m->rows; ++i) {
    m->data[i * m->cols + i] = 1.0;
  }

  return ERR_OK;
}

/* ============================================================ */
/*                   Basic Matrix Arithmetic                    */
/* ============================================================ */

util_error_t mat_add_rc(const mat_t* restrict a, const mat_t* restrict b,
                        mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_3(a, b, out);
  MAT_REQUIRE_VALID_MAT_3(a, b, out);

  util_error_t rc = mat_same_shape(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_same_shape(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] + b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_add_inplace_rc(mat_t* restrict dest,
                                const mat_t* restrict src) {
  MAT_REQUIRE_NON_NULL_2(dest, src);
  MAT_REQUIRE_VALID_MAT_2(dest, src);

  util_error_t rc = mat_same_shape(dest, src);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    dest_data[i] += src_data[i];
  }

  return ERR_OK;
}

util_error_t mat_subtract_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_3(a, b, out);
  MAT_REQUIRE_VALID_MAT_3(a, b, out);

  util_error_t rc = mat_same_shape(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_same_shape(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] - b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_subtract_inplace_rc(mat_t* restrict dest,
                                     const mat_t* restrict src) {
  MAT_REQUIRE_NON_NULL_2(dest, src);
  MAT_REQUIRE_VALID_MAT_2(dest, src);

  util_error_t rc = mat_same_shape(dest, src);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    dest_data[i] -= src_data[i];
  }

  return ERR_OK;
}

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

util_error_t mat_scale_rc(const mat_t* restrict a, mat_t* restrict out,
                          double scalar) {
  MAT_REQUIRE_NON_NULL_2(a, out);
  MAT_REQUIRE_VALID_MAT_2(a, out);

  util_error_t rc = mat_same_shape(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t mat_scale_inplace_rc(mat_t* restrict dest, double scalar) {
  MAT_REQUIRE_VALID_MAT(dest);

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    dest_data[i] *= scalar;
  }

  return ERR_OK;
}

util_error_t mat_hadamard_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_3(a, b, out);
  MAT_REQUIRE_VALID_MAT_3(a, b, out);

  util_error_t rc = mat_same_shape(a, b);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_same_shape(a, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_map_rc(const mat_t* restrict src, mat_t* restrict dest,
                        mat_map_func_t func) {
  MAT_REQUIRE_NON_NULL_3(src, dest, func);
  MAT_REQUIRE_VALID_MAT_2(src, dest);

  util_error_t rc = mat_same_shape(src, dest);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = src->rows * src->cols;
  const double* restrict s = src->data;
  double* restrict d = dest->data;

  for (size_t i = 0; i < n; ++i) {
    double tmp = func(s[i]);
    if (!isfinite(tmp)) {
      return ERR_RANGE;
    }
    d[i] = tmp;
  }

  return ERR_OK;
}

/* ============================================================ */
/*                        Matrix Products                       */
/* ============================================================ */

util_error_t mat_multiply_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_3(a, b, out);
  MAT_REQUIRE_VALID_MAT_3(a, b, out);

  if (a->cols != b->rows) {
    return ERR_DIM;
  }

  if (out->rows != a->rows || out->cols != b->cols) {
    return ERR_DIM;
  }

  mat_t* b_t = NULL;
  util_error_t rc = mat_alloc_rc(&b_t, b->cols, b->rows);
  if (rc != ERR_OK) {
    return rc;
  }

  mat_transpose_rc(b, b_t);

  const size_t a_rows = a->rows;
  const size_t a_cols = a->cols;
  const size_t out_cols = out->cols;

  const double* restrict a_data = a->data;
  const double* restrict bt_data = b_t->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for collapse(2) schedule(static)
  for (size_t i = 0; i < a_rows; ++i) {
    for (size_t j = 0; j < out_cols; ++j) {
      double sum = 0.0;
      const double* restrict row_a = &a_data[i * a_cols];
      const double* restrict row_bt = &bt_data[j * a_cols];
      #pragma omp simd reduction(+ : sum)
      for (size_t k = 0; k < a_cols; ++k) {
        sum += row_a[k] * row_bt[k];
      }
      out_data[i * out_cols + j] = sum;
    }
  }

  mat_free_rc(b_t);

  return ERR_OK;
}

util_error_t mat_vec_multiply_rc(const mat_t* restrict m,
                                 const vec_t* restrict v, vec_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(m);
  VEC_REQUIRE_VALID_VEC_2(v, out);
  MAT_REQUIRE_NON_NULL(out);

  if (m->cols != v->n) {
    return ERR_DIM;
  }

  if (out->n != m->rows) {
    return ERR_DIM;
  }

  const size_t rows = m->rows;
  const size_t cols = m->cols;

  const double* restrict m_data = m->data;
  const double* restrict v_data = v->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < rows; ++i) {
    double sum = 0.0;
    const double* restrict row_m = &m_data[i * cols];
    #pragma omp simd reduction(+ : sum)
    for (size_t j = 0; j < cols; ++j) {
      sum += row_m[j] * v_data[j];
    }
    out_data[i] = sum;
  }

  return ERR_OK;
}

/* ============================================================ */
/*                    Matrix transformations                    */
/* ============================================================ */

util_error_t mat_transpose_rc(const mat_t* restrict a, mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_2(a, out);
  MAT_REQUIRE_VALID_MAT_2(a, out);

  if (a->rows != out->cols || a->cols != out->rows) {
    return ERR_DIM;
  }

  const size_t rows = a->rows;
  const size_t cols = a->cols;

  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  #pragma omp parallel for collapse(2) schedule(static)
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      out_data[j * rows + i] = a_data[i * cols + j];
    }
  }

  return ERR_OK;
}

util_error_t mat_reshape_rc(mat_t* restrict m, size_t new_rows,
                            size_t new_cols) {
  MAT_REQUIRE_VALID_MAT(m);

  size_t old_elements = m->rows * m->cols;
  size_t new_elements = new_rows * new_cols;

  if (old_elements != new_elements) {
    return ERR_DIM;
  }

  m->rows = new_rows;
  m->cols = new_cols;

  return ERR_OK;
}

/* ============================================================ */
/*                        Linear Algebra                        */
/* ============================================================ */

util_error_t mat_lu_decompose_rc(const mat_t* restrict src,
                                 mat_t* restrict dest, size_t* restrict piv,
                                 int* restrict sign) {
  MAT_REQUIRE_NON_NULL_3(src, dest, piv);
  MAT_REQUIRE_NON_NULL(sign);
  MAT_REQUIRE_VALID_MAT_2(src, dest);

  util_error_t rc = mat_same_shape(src, dest);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_require_square(src);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = src->rows;

  rc = mat_copy_rc(src, dest);
  if (rc != ERR_OK) {
    return rc;
  }

  *sign = 1;

  for (size_t i = 0; i < n; ++i) {
    piv[i] = i;
  }

  double* restrict data = dest->data;
  const size_t cols = dest->cols;
  const double epsilon = 1e-12;

  for (size_t k = 0; k < n; ++k) {
    size_t pivot_row = k;
    double max_value = fabs(data[k * cols + k]);

    for (size_t i = k + 1; i < n; ++i) {
      double current_value = fabs(data[i * cols + k]);
      if (current_value > max_value) {
        max_value = current_value;
        pivot_row = i;
      }
    }

    if (max_value < epsilon) {
      return ERR_SINGULAR;
    }

    if (pivot_row != k) {
      rc = mat_swap_rows_rc(dest, k, pivot_row);
      if (rc != ERR_OK) {
        return rc;
      }

      size_t temp_piv = piv[k];
      piv[k] = piv[pivot_row];
      piv[pivot_row] = temp_piv;

      *sign = -*sign;
    }

    const double pivot = data[k * cols + k];

    for (size_t i = k + 1; i < n; ++i) {
      double multiplier = data[i * cols + k] / pivot;
      data[i * cols + k] = multiplier;

      #pragma omp simd
      for (size_t j = k + 1; j < n; ++j) {
        data[i * cols + j] =
            data[i * cols + j] - multiplier * data[k * cols + j];
      }
    }
  }

  return ERR_OK;
}

util_error_t mat_lu_decompose_inplace_rc(mat_t* restrict a,
                                         size_t* restrict piv,
                                         int* restrict sign) {
  MAT_REQUIRE_NON_NULL_3(a, piv, sign);
  MAT_REQUIRE_VALID_MAT(a);

  util_error_t rc = mat_require_square(a);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = a->rows;

  *sign = 1;

  for (size_t i = 0; i < n; ++i) {
    piv[i] = i;
  }

  double* restrict data = a->data;
  const size_t cols = a->cols;
  const double epsilon = 1e-12;

  for (size_t k = 0; k < n; ++k) {
    size_t pivot_row = k;
    double max_value = fabs(data[k * cols + k]);

    for (size_t i = k + 1; i < n; ++i) {
      double current_value = fabs(data[i * cols + k]);
      if (current_value > max_value) {
        max_value = current_value;
        pivot_row = i;
      }
    }

    if (max_value < epsilon) {
      return ERR_SINGULAR;
    }

    if (pivot_row != k) {
      rc = mat_swap_rows_rc(a, k, pivot_row);

      if (rc != ERR_OK) {
        return rc;
      }

      size_t temp_piv = piv[k];
      piv[k] = piv[pivot_row];
      piv[pivot_row] = temp_piv;

      *sign = -*sign;
    }

    const double pivot = data[k * cols + k];

    for (size_t i = k + 1; i < n; ++i) {
      double multiplier = data[i * cols + k] / pivot;
      data[i * cols + k] = multiplier;

      for (size_t j = k + 1; j < n; ++j) {
        data[i * cols + j] =
            data[i * cols + j] - multiplier * data[k * cols + j];
      }
    }
  }

  return ERR_OK;
}

util_error_t mat_det_rc(const mat_t* restrict m, double* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);
  MAT_REQUIRE_VALID_MAT(m);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = m->rows;

  mat_t* lu = NULL;
  rc = mat_alloc_rc(&lu, n, n);
  if (rc != ERR_OK) {
    return rc;
  }

  size_t* piv = (size_t*)malloc(n * sizeof(size_t));
  if (piv == NULL) {
    mat_free_rc(lu);
    return ERR_ALLOC;
  }

  int sign = 1;

  rc = mat_lu_decompose_rc(m, lu, piv, &sign);
  if (rc != ERR_OK) {
    free(piv);
    mat_free_rc(lu);
    if (rc == ERR_SINGULAR) {
      *out = 0.0;
      return ERR_OK;
    }
    return rc;
  }

  double det = (double)sign;
  const double* restrict data = lu->data;
  const size_t cols = lu->cols;

  for (size_t i = 0; i < n; ++i) {
    det *= data[i * cols + i];
  }

  free(piv);
  mat_free_rc(lu);

  *out = det;
  return ERR_OK;
}

util_error_t mat_det_inplace_rc(mat_t* restrict m, double* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);
  MAT_REQUIRE_VALID_MAT(m);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = m->rows;

  size_t* piv = (size_t*)malloc(n * sizeof(size_t));
  if (piv == NULL) {
    return ERR_ALLOC;
  }

  int sign = 1;

  rc = mat_lu_decompose_inplace_rc(m, piv, &sign);
  if (rc != ERR_OK) {
    free(piv);
    if (rc == ERR_SINGULAR) {
      *out = 0.0;
      return ERR_OK;
    }
    return rc;
  }

  double det = (double)sign;
  const double* restrict data = m->data;
  const size_t cols = m->cols;

  for (size_t i = 0; i < n; ++i) {
    det *= data[i * cols + i];
  }

  free(piv);

  *out = det;

  return ERR_OK;
}

util_error_t mat_inverse_rc(const mat_t* restrict m, mat_t* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);
  MAT_REQUIRE_VALID_MAT_2(m, out);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_same_shape(m, out);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = m->rows;

  mat_t* lu = NULL;
  rc = mat_alloc_rc(&lu, n, n);
  if (rc != ERR_OK) {
    return rc;
  }

  size_t* piv = (size_t*)malloc(n * sizeof(size_t));
  if (piv == NULL) {
    mat_free_rc(lu);
    return rc;
  }

  int sign = 1;

  rc = mat_lu_decompose_rc(m, lu, piv, &sign);
  if (rc != ERR_OK) {
    free(piv);
    mat_free_rc(lu);
    return rc;
  }

  double* restrict lu_data = lu->data;
  double* restrict out_data = out->data;
  const size_t lu_cols = lu->cols;
  
  #pragma omp parallel for schedule(static)
  for (size_t j = 0; j < n; ++j) {
    for (size_t i = 0; i < n; ++i) {
      out_data[j * n + i] = (piv[i] == j) ? 1.0 : 0.0;
    }

    for (size_t i = 0; i < n; ++i) {
      double sum = out_data[j * n + i];
      for (size_t k = 0; k < i; ++k) {
        sum -= lu_data[i * lu_cols + k] * out_data[j * n + k];
      }
      out_data[j * n + i] = sum;
    }

    for (size_t i = n; i-- > 0;) {
      double sum = out_data[j * n + i];
      for (size_t k = i + 1; k < n; ++k) {
        sum -= lu_data[i * lu_cols + k] * out_data[j * n + k];
      }
      out_data[j * n + i] = sum / lu_data[i * lu_cols + i];
    }
  }

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      double tmp = out_data[i * n + j];
      out_data[i * n + j] = out_data[j * n + i];
      out_data[j * n + i] = tmp;
    }
  }

  free(piv);
  mat_free_rc(lu);

  return ERR_OK;
}

util_error_t mat_inverse_inplace_rc(mat_t* restrict m) {
  MAT_REQUIRE_VALID_MAT(m);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = m->rows;

  mat_t* temp = NULL;
  rc = mat_alloc_rc(&temp, n, n);
  if (rc != ERR_OK) {
    return rc;
  }

  rc = mat_copy_rc(m, temp);
  if (rc != ERR_OK) {
    mat_free_rc(temp);
    return rc;
  }

  rc = mat_inverse_rc(temp, m);
  if (rc != ERR_OK) {
    mat_free_rc(temp);
    return rc;
  }

  mat_free_rc(temp);

  return ERR_OK;
}

util_error_t mat_solve_rc(const mat_t* restrict a, const vec_t* restrict b,
                          vec_t* restrict out) {
  MAT_REQUIRE_VALID_MAT(a);
  VEC_REQUIRE_VALID_VEC_2(b, out);
  MAT_REQUIRE_NON_NULL_3(a, b, out);

  util_error_t rc = mat_require_square(a);
  if (rc != ERR_OK) {
    return rc;
  }

  if (a->rows != b->n) {
    return ERR_DIM;
  }

  if (out->n != b->n) {
    return ERR_DIM;
  }

  const size_t n = a->rows;

  mat_t* lu = NULL;
  rc = mat_alloc_rc(&lu, n, n);
  if (rc != ERR_OK) {
    return rc;
  }

  size_t* piv = (size_t*)malloc(n * sizeof(size_t));
  if (piv == NULL) {
    mat_free_rc(lu);
    return ERR_ALLOC;
  }

  int sign = 1;

  rc = mat_lu_decompose_rc(a, lu, piv, &sign);
  if (rc != ERR_OK) {
    free(piv);
    mat_free_rc(lu);
    return rc;
  }

  const double* restrict lu_data = lu->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;
  const size_t cols = lu->cols;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = b_data[piv[i]];
  }

  for (size_t i = 0; i < n; ++i) {
    for (size_t k = 0; k < i; ++k) {
      out_data[i] -= lu_data[i * cols + k] * out_data[k];
    }
  }

  for (size_t i = n; i-- > 0;) {
    for (size_t k = i + 1; k < n; ++k) {
      out_data[i] -= lu_data[i * cols + k] * out_data[k];
    }
    out_data[i] /= lu_data[i * cols + i];
  }

  free(piv);
  mat_free_rc(lu);

  return ERR_OK;
}

util_error_t mat_trace_rc(const mat_t* restrict m, double* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);
  MAT_REQUIRE_VALID_MAT(m);

  util_error_t rc = mat_require_square(m);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = m->rows;
  const double* restrict m_data = m->data;
  const size_t m_cols = m->cols;
  double trace = 0.0;

  #pragma omp parallel for reduction(+ : trace) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    trace += m_data[i * m_cols + i];
  }

  *out = trace;

  return ERR_OK;
}

/* ============================================================ */
/*              Properties, Comparison and Utility              */
/* ============================================================ */

util_error_t mat_is_square_rc(const mat_t* restrict m, bool* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);

  *out = (m->rows == m->cols);

  return ERR_OK;
}

util_error_t mat_is_equal_rc(const mat_t* restrict a, const mat_t* restrict b,
                             double epsilon, bool* restrict out) {
  MAT_REQUIRE_NON_NULL_3(a, b, out);
  MAT_REQUIRE_VALID_MAT_2(a, b);

  if (!mat_same_shape(a, b)) {
    *out = false;
    return ERR_OK;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;

  bool equal = true;
  #pragma omp parallel for schedule(static)
  for (size_t i = 0; i < n; ++i) {
    if (fabs(a_data[i] - b_data[i]) > epsilon) {
      #pragma omp atomic write
      equal = false;
      }
  }
  
  *out = equal;

  return ERR_OK;
}

util_error_t mat_sum_rc(const mat_t* restrict m, double* restrict out) {
  MAT_REQUIRE_NON_NULL_2(m, out);
  MAT_REQUIRE_VALID_MAT(m);

  const size_t n = m->rows * m->cols;
  const double* restrict m_data = m->data;
  double total_sum = 0.0;

  #pragma omp parallel for reduction(+ : total_sum) schedule(static)
  for (size_t i = 0; i < n; ++i) {
    total_sum += m_data[i];
  }

  *out = total_sum;
  return ERR_OK;
}

util_error_t mat_swap_rc(mat_t* restrict a, mat_t* restrict b) {
  MAT_REQUIRE_NON_NULL_2(a, b);

  if (a == b) {
    return ERR_OK;
  }

  size_t temp_rows = a->rows;
  a->rows = b->rows;
  b->rows = temp_rows;

  size_t temp_cols = a->cols;
  a->cols = b->cols;
  b->cols = temp_cols;

  double* temp_data = a->data;
  a->data = b->data;
  b->data = temp_data;

  return ERR_OK;
}

util_error_t mat_swap_rows_rc(mat_t* restrict m, size_t row_a, size_t row_b) {
  MAT_REQUIRE_VALID_MAT(m);

  if (row_a >= m->rows || row_b >= m->rows) {
    return ERR_RANGE;
  }

  if (row_a == row_b) {
    return ERR_OK;
  }

  const size_t cols = m->cols;
  double* restrict data = m->data;
  double* restrict r1 = data + (row_a * cols);
  double* restrict r2 = data + (row_b * cols);

  for (size_t j = 0; j < cols; ++j) {
    double temp = r1[j];
    r1[j] = r2[j];
    r2[j] = temp;
  }

  return ERR_OK;
}

util_error_t mat_copy_rc(const mat_t* restrict src, mat_t* restrict dest) {
  MAT_REQUIRE_NON_NULL_2(src, dest);
  MAT_REQUIRE_VALID_MAT_2(src, dest);

  util_error_t rc = mat_same_shape(src, dest);
  if (rc != ERR_OK) {
    return rc;
  }

  const size_t n = src->rows * src->cols;
  const double* restrict src_data = src->data;
  double* restrict dest_data = dest->data;

  memcpy(dest_data, src_data, n * sizeof(double));

  return ERR_OK;
}
