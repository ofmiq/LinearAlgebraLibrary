#include "mat_rc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

util_error_t mat_alloc_rc(mat_t** out, size_t rows, size_t cols) {
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

util_error_t mat_from_array_rc(const double* data, mat_t** out, size_t rows,
                               size_t cols) {
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

void mat_freep_rc(mat_t** mp) {
  if (mp == NULL || *mp == NULL) {
    return;
  }

  free((*mp)->data);
  free(*mp);
  *mp = NULL;
}

util_error_t mat_set_rc(mat_t* m, size_t i, size_t j, double val) {
  if (m == NULL || m->data == NULL) {
    return ERR_NULL;
  }

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  MAT_AT(m, i, j) = val;
  return ERR_OK;
}

util_error_t mat_get_rc(const mat_t* m, size_t i, size_t j, double* out) {
  if (m == NULL || m->data == NULL || out == NULL) {
    return ERR_NULL;
  }

  if (i >= m->rows || j >= m->cols) {
    return ERR_RANGE;
  }

  *out = MAT_AT(m, i, j);
  return ERR_OK;
}