#include "mat_rc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

/* internal helper: validate same shape */
static inline int mat_same_shape(const mat_t* restrict a,
                                 const mat_t* restrict b) {
  return a->rows == b->rows && a->cols == b->cols;
}

/* ============================================================ */
/*                      Lifecycle Management                    */
/* ============================================================ */

util_error_t mat_alloc_rc(mat_t** restrict out, size_t rows, size_t cols) {
  if (out == NULL) {
    return ERR_NULL;
  }

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
  if (out == NULL) {
    return ERR_NULL;
  }

  if (data == NULL) {
    return ERR_NULL;
  }

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
  if (mp == NULL || *mp == NULL) {
    return ERR_NULL;
  }

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
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  MAT_AT(m, i, j) = val;
  return ERR_OK;
}

util_error_t mat_set_row(mat_t* restrict m, size_t row,
                         const vec_t* restrict v) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (v == NULL || v->data == NULL) {
    return ERR_NULL;
  }

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

util_error_t mat_set_column(mat_t* restrict m, size_t col,
                            const vec_t* restrict v) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (v == NULL || v->data == NULL) {
    return ERR_NULL;
  }

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

  for (size_t i = 0; i < rows; ++i) {
    *current_ptr = v_data[i];
    current_ptr += cols;
  }

  return ERR_OK;
}

util_error_t mat_get_rc(const mat_t* restrict m, size_t i, size_t j,
                        double* restrict out) {
  if (m == NULL || m->data == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  *out = MAT_AT(m, i, j);
  return ERR_OK;
}

util_error_t mat_get_row(const mat_t* restrict m, size_t row,
                         vec_t* restrict out) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (out == NULL || out->data == NULL) {
    return ERR_NULL;
  }

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

util_error_t mat_get_column(const mat_t* restrict m, size_t col,
                            vec_t* restrict out) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (out == NULL || out->data == NULL) {
    return ERR_NULL;
  }

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

  for (size_t i = 0; i < rows; ++i) {
    v_data[i] = *current_src;
    current_src += stride;
  }

  return ERR_OK;
}

util_error_t mat_rows_rc(const mat_t* restrict m, size_t* restrict out) {
  if (m == NULL || m->data == NULL || out == NULL) {
    return ERR_NULL;
  }

  *out = m->rows;

  return ERR_OK;
}

util_error_t mat_cols_rc(const mat_t* restrict m, size_t* restrict out) {
  if (m == NULL || m->data == NULL || out == NULL) {
    return ERR_NULL;
  }

  *out = m->cols;

  return ERR_OK;
}

util_error_t mat_data_rc(const mat_t* restrict m, const double** restrict out) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (out == NULL) {
    return ERR_NULL;
  }

  *out = m->data;
  return ERR_OK;
}

/* ============================================================ */
/*                   Basic Matrix Arithmetic                    */
/* ============================================================ */

util_error_t mat_add_rc(const mat_t* restrict a, const mat_t* restrict b,
                        mat_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(a, b) || !mat_same_shape(a, out)) {
    return ERR_DIM;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] + b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_add_inplace_rc(mat_t* restrict dest,
                                const mat_t* restrict src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }

  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(dest, src)) {
    return ERR_DIM;
  }

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

  for (size_t i = 0; i < n; ++i) {
    dest_data[i] += src_data[i];
  }

  return ERR_OK;
}

util_error_t mat_subtract_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(a, b) || !mat_same_shape(a, out)) {
    return ERR_DIM;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] - b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_subtract_inplace_rc(mat_t* restrict dest,
                                     const mat_t* restrict src) {
  if (dest == NULL || src == NULL) {
    return ERR_NULL;
  }

  if (dest->data == NULL || src->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(dest, src)) {
    return ERR_DIM;
  }

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;
  const double* restrict src_data = src->data;

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
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(a, out)) {
    return ERR_DIM;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t mat_scale_inplace_rc(mat_t* restrict dest, double scalar) {
  if (dest == NULL) {
    return ERR_NULL;
  }

  if (dest->data == NULL) {
    return ERR_NULL;
  }

  const size_t n = dest->rows * dest->cols;
  double* restrict dest_data = dest->data;

  for (size_t i = 0; i < n; ++i) {
    dest_data[i] *= scalar;
  }

  return ERR_OK;
}

util_error_t mat_hadamard_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(a, b) || !mat_same_shape(a, out)) {
    return ERR_DIM;
  }

  const size_t n = a->rows * a->cols;
  const double* restrict a_data = a->data;
  const double* restrict b_data = b->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < n; ++i) {
    out_data[i] = a_data[i] * b_data[i];
  }

  return ERR_OK;
}

util_error_t mat_map_rc(const mat_t* restrict src, mat_t* restrict dest,
                        mat_map_func_t func) {
  if (src == NULL || dest == NULL || func == NULL) {
    return ERR_NULL;
  }

  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }

  if (!mat_same_shape(src, dest)) {
    return ERR_DIM;
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
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

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

  for (size_t i = 0; i < a_rows; ++i) {
    for (size_t j = 0; j < out_cols; ++j) {
      double sum = 0.0;
      const double* restrict row_a = &a_data[i * a_cols];
      const double* restrict row_bt = &bt_data[j * a_cols];
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
  if (m == NULL || v == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (m->data == NULL || v->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

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

  for (size_t i = 0; i < rows; ++i) {
    double sum = 0.0;
    const double* restrict row_m = &m_data[i * cols];
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
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }

  if (a->rows != out->cols || a->cols != out->rows) {
    return ERR_DIM;
  }

  const size_t rows = a->rows;
  const size_t cols = a->cols;

  const double* restrict a_data = a->data;
  double* restrict out_data = out->data;

  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      out_data[j * rows + i] = a_data[i * cols + j];
    }
  }

  return ERR_OK;
}

util_error_t mat_reshape_rc(mat_t* restrict m, size_t new_rows,
                            size_t new_cols) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  size_t old_elements = m->rows * m->cols;
  size_t new_elements = new_rows * new_cols;

  if (old_elements != new_elements) {
    return ERR_DIM;
  }

  m->rows = new_rows;
  m->cols = new_cols;

  return ERR_OK;
}
