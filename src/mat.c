#include "mat.h"

#include <math.h>
#include <stddef.h>

#include "mat_rc.h"
#include "vec.h"

/* ============================================================ */
/*                     Lifecycle Management                     */
/* ============================================================ */

mat_t* mat_alloc(size_t rows, size_t cols) {
  mat_t* m = NULL;

  util_error_t rc = mat_alloc_rc(&m, rows, cols);
  if (rc != ERR_OK) {
    return NULL;
  }

  return m;
}

mat_t* mat_from_array(const double* data, size_t rows, size_t cols) {
  mat_t* m = NULL;

  util_error_t rc = mat_from_array_rc(data, &m, rows, cols);
  if (rc != ERR_OK) {
    return NULL;
  }

  return m;
}

void mat_free(mat_t* m) { mat_free_rc(m); }

void mat_freep(mat_t** mp) { mat_freep_rc(mp); }

mat_t* mat_resize_new(const mat_t* m, size_t new_rows, size_t new_cols) {
  if (m == NULL) {
    return NULL;
  }

  mat_t* duplicate = mat_duplicate(m);
  if (duplicate == NULL) {
    return NULL;
  }

  util_error_t rc = mat_resize_rc(&duplicate, new_rows, new_cols);
  if (rc != ERR_OK) {
    mat_free(duplicate);
    return NULL;
  }

  return duplicate;
}

bool mat_resize_inplace(mat_t* m, size_t new_rows, size_t new_cols) {
  util_error_t rc = mat_resize_rc(&m, new_rows, new_cols);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

/* ============================================================ */
/*                  Data Access and Inspection                  */
/* ============================================================ */

bool mat_set(mat_t* m, size_t i, size_t j, double val) {
  util_error_t rc = mat_set_rc(m, i, j, val);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

bool mat_set_row(mat_t* m, size_t row, const vec_t* v) {
  util_error_t rc = mat_set_row(m, row, v);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

bool mat_set_column(mat_t* m, size_t col, const vec_t* v) {
  util_error_t rc = mat_set_column(m, col, v);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

double mat_get(const mat_t* m, size_t i, size_t j) {
  double out = NAN;
  
  util_error_t rc = mat_get_rc(m, i, j, &out);
  if (rc != ERR_OK) {
    return NAN;
  }
  
  return out;
}

vec_t* mat_get_row(const mat_t* m, size_t row) {
  size_t cols = 0;
  
  if (mat_cols_rc(m, &cols) != ERR_OK) {
    return NULL;
  }

  vec_t* v = vec_alloc(cols);
  if (v == NULL) {
    return NULL;
  }

  util_error_t rc = mat_get_row_rc(m, row, v);
  if (rc != ERR_OK) {
    vec_free(v);
    return NULL;
  }

  return v;
}

vec_t* mat_get_column(const mat_t* m, size_t col) {
  size_t rows = 0;
  if (mat_rows_rc(m, &rows) != ERR_OK) {
    return NULL;
  }

  vec_t* v = vec_alloc(rows);
  if (v == NULL) {
    return NULL;
  }

  util_error_t rc = mat_get_column_rc(m, col, v);
  if (rc != ERR_OK) {
    vec_free(v);
    return NULL;
  }

  return v;
}

size_t mat_rows(const mat_t* m) {
  size_t rows = 0;
  
  util_error_t rc = mat_rows_rc(m, &rows);
  if (rc != ERR_OK) {
    return 0;
  }
  
  return rows;
}

size_t mat_cols(const mat_t* m) {
  size_t cols = 0;
  
  util_error_t rc = mat_cols_rc(m, &cols);
  if (rc != ERR_OK) {
    return 0;
  }
  
  return cols;
}

const double* mat_data(const mat_t* m) {
  const double* data = NULL;
  
  util_error_t rc = mat_data_rc(m, &data);
  if (rc != ERR_OK) {
    return NULL;
  }
  
  return data;
}

/* ============================================================ */
/*                    Initialization Helpers                    */
/* ============================================================ */

bool mat_fill(mat_t* m, double val) {
  util_error_t rc = mat_fill_rc(m, val);
  
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

mat_t* mat_zeros(size_t rows, size_t cols) {
  mat_t* m = mat_alloc(rows, cols);
  if (m == NULL) {
    return NULL;
  }

  util_error_t rc = mat_zeros_rc(m);
  if (rc != ERR_OK) {
    mat_free(m);
    return NULL;
  }
  
  return m;
}

mat_t* mat_identity(size_t n) {
  mat_t* m = mat_alloc(n, n);
  if (m == NULL) {
    return NULL;
  }

  util_error_t rc = mat_identity_rc(m);
  if (rc != ERR_OK) {
    mat_free(m);
    return NULL;
  }
  
  return m;
}

/* ============================================================ */
/*                   Basic Matrix Arithmetic                    */
/* ============================================================ */

mat_t* mat_add_new(const mat_t* a, const mat_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_add_rc(a, b, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

bool mat_add_inplace(mat_t* dest, const mat_t* src) {
  util_error_t rc = mat_add_inplace_rc(dest, src);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

mat_t* mat_subtract_new(const mat_t* a, const mat_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_subtract_rc(a, b, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

bool mat_subtract_inplace(mat_t* dest, const mat_t* src) {
  util_error_t rc = mat_subtract_inplace_rc(dest, src);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

mat_t* mat_scale_new(const mat_t* a, double scalar) {
  if (a == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_scale_rc(a, out, scalar);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

bool mat_scale_inplace(mat_t* dest, double scalar) {
  util_error_t rc = mat_scale_inplace_rc(dest, scalar);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

mat_t* mat_hadamard_new(const mat_t* a, const mat_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_hadamard_rc(a, b, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

mat_t* mat_map_new(const mat_t* src, mat_map_func_t func) {
  if (src == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(src, &rows) != ERR_OK || mat_cols_rc(src, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_map_rc(src, out, func);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

/* ============================================================ */
/*                        Matrix Products                       */
/* ============================================================ */

mat_t* mat_multiply_new(const mat_t* a, const mat_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  size_t rows_a;
  size_t cols_a; 
  size_t rows_b; 
  size_t cols_b;

  if (mat_rows_rc(a, &rows_a) != ERR_OK || mat_cols_rc(b, &cols_b) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows_a, cols_b);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_multiply_rc(a, b, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

vec_t* mat_vec_multiply_new(const mat_t* m, const vec_t* v) {
  if (m == NULL || v == NULL) {
    return NULL;
  }

  size_t rows_m;

  if (mat_rows_rc(m, &rows_m) != ERR_OK) {
    return NULL;
  }

  vec_t* out = vec_alloc(rows_m);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_vec_multiply_rc(m, v, out);
  if (rc != ERR_OK) {
    vec_free(out);
    return NULL;
  }

  return out;
}

/* ============================================================ */
/*                    Matrix Transformations                    */
/* ============================================================ */

mat_t* mat_transpose_new(const mat_t* a) {
  if (a == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(cols, rows);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_transpose_rc(a, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

bool mat_reshape(mat_t* m, size_t new_rows, size_t new_cols) {
  util_error_t rc = mat_reshape_rc(m, new_rows, new_cols);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

/* ============================================================ */
/*                        Linear Algebra                        */
/* ============================================================ */

mat_t* mat_lu_decompose_new(const mat_t* src, size_t* piv, int* sign) {
  if (src == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(src, &rows) != ERR_OK || mat_cols_rc(src, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* dest = mat_alloc(rows, cols);
  if (dest == NULL) {
    return NULL;
  }

  util_error_t rc = mat_lu_decompose_rc(src, dest, piv, sign);
  if (rc != ERR_OK) {
    mat_free(dest);
    return NULL;
  }

  return dest;
}

bool mat_lu_decompose_inplace(mat_t* a, size_t* piv, int* sign) {
  util_error_t rc = mat_lu_decompose_inplace_rc(a, piv, sign);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

double mat_det(const mat_t* m) {
  double out = NAN;
  
  util_error_t rc = mat_det_rc(m, &out);
  if (rc != ERR_OK) {
    return NAN;
  }
  
  return out;
}

mat_t* mat_inverse_new(const mat_t* m) {
  if (m == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(m, &rows) != ERR_OK || mat_cols_rc(m, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* out = mat_alloc(rows, cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_inverse_rc(m, out);
  if (rc != ERR_OK) {
    mat_free(out);
    return NULL;
  }

  return out;
}

bool mat_inverse_inplace(mat_t* m) {
  util_error_t rc = mat_inverse_inplace_rc(m);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

vec_t* mat_solve_new(const mat_t* a, const vec_t* b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;

  if (mat_rows_rc(a, &rows) != ERR_OK || mat_cols_rc(a, &cols) != ERR_OK) {
    return NULL;
  }

  vec_t* out = vec_alloc(cols);
  if (out == NULL) {
    return NULL;
  }

  util_error_t rc = mat_solve_rc(a, b, out);
  if (rc != ERR_OK) {
    vec_free(out);
    return NULL;
  }

  return out;
}

double mat_trace(const mat_t* m) {
  double out = NAN;
  
  util_error_t rc = mat_trace_rc(m, &out);
  if (rc != ERR_OK) {
    return NAN;
  }
  
  return out;
}

/* ============================================================ */
/*              Properties, Comparison and Utility              */
/* ============================================================ */

bool mat_is_square(const mat_t* m) {
  bool out = false;
  
  util_error_t rc = mat_is_square_rc(m, &out);
  if (rc != ERR_OK) {
    return false;
  }
  
  return out;
}

bool mat_is_equal(const mat_t* a, const mat_t* b, double epsilon) {
  bool out = false;
  
  util_error_t rc = mat_is_equal_rc(a, b, epsilon, &out);
  if (rc != ERR_OK) {
    return false;
  }
  
  return out;
}

double mat_sum(const mat_t* m) {
  double out = NAN;
  
  util_error_t rc = mat_sum_rc(m, &out);
  if (rc != ERR_OK) {
    return NAN;
  }
  
  return out;
}

void mat_swap(mat_t* a, mat_t* b) { mat_swap_rc(a, b); }

bool mat_swap_rows(mat_t* m, size_t row_a, size_t row_b) {
  util_error_t rc = mat_swap_rows_rc(m, row_a, row_b);
  if (rc != ERR_OK) {
    return false;
  }
  
  return true;
}

mat_t* mat_duplicate(const mat_t* src) {
  if (src == NULL) {
    return NULL;
  }

  size_t rows;
  size_t cols;
  
  if (mat_rows_rc(src, &rows) != ERR_OK || mat_cols_rc(src, &cols) != ERR_OK) {
    return NULL;
  }

  mat_t* dest = mat_alloc(rows, cols);
  if (dest == NULL) {
    return NULL;
  }

  util_error_t rc = mat_copy_rc(src, dest);
  if (rc != ERR_OK) {
    mat_free(dest);
    return NULL;
  }

  return dest;
}
