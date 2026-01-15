#include <config.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "benchmark_utils.h"
#include "mat_rc.h"
#include "vec_rc.h"

#define ROWS 3000
#define COLS 3000
#define ITER 1

static inline double get_wall_time() {
#ifdef _OPENMP
  return omp_get_wtime();  // wall-clock time OpenMP
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
#endif
}

int main() {
#ifdef _OPENMP
  printf("--- MATRIX HARDCORE PERFORMANCE BENCHMARK (%dx%d) ---\n", ROWS, COLS);
  printf("OpenMP enabled: %d threads\n", omp_get_max_threads());
#else
  printf("--- MATRIX HARDCORE PERFORMANCE BENCHMARK (%dx%d) ---\n", ROWS, COLS);
  printf("OpenMP disabled\n");
#endif

  double total_start = get_wall_time();
  double s;
  double dummy = 0;

  // 1. Allocation and Initialization
  s = get_wall_time();
  mat_t *m1 = NULL, *m2 = NULL, *m3 = NULL;
  mat_alloc_rc(&m1, ROWS, COLS);
  mat_alloc_rc(&m2, ROWS, COLS);
  mat_alloc_rc(&m3, ROWS, COLS);

  double* raw_arr = malloc(ROWS * COLS * sizeof(double));
  if (!raw_arr) return 1;
  for (size_t i = 0; i < ROWS * COLS; i++) raw_arr[i] = (double)i;

  for (int i = 0; i < ITER; i++) {
    mat_fill_rc(m1, 1.5 + i);
    mat_zeros_rc(m2);
    mat_identity_rc(m1);
    mat_t* temp = NULL;
    mat_alloc_rc(&temp, 10, 10);
    mat_free_rc(temp);
  }
  mat_t* m_arr = NULL;
  mat_from_array_rc(raw_arr, &m_arr, ROWS, COLS);

  const double* data_ptr;
  mat_data_rc(m_arr, &data_ptr);
  dummy += data_ptr[0];
  printf("[Init/Alloc/Fill]   Time: %.4f s\n", get_wall_time() - s);

  // 2. Getters, Setters and Metadata
  s = get_wall_time();
  double val;
  size_t r_count, c_count;
  vec_t* v_tmp = NULL;
  vec_alloc_rc(&v_tmp, COLS);

  for (int i = 0; i < ITER * 100; i++) {
    size_t ri = i % ROWS;
    size_t ci = i % COLS;
    mat_set_rc(m1, ri, ci, (double)i);
    mat_get_rc(m1, ri, ci, &val);
    dummy += val;

    mat_rows_rc(m1, &r_count);
    mat_cols_rc(m1, &c_count);
    dummy += (double)(r_count + c_count);
  }
  for (int i = 0; i < ITER; i++) {
    mat_get_row(m1, i % ROWS, v_tmp);
    mat_set_column(m2, i % COLS, v_tmp);
    mat_get_column(m1, i % COLS, v_tmp);
    mat_set_row(m2, i % ROWS, v_tmp);
    dummy += v_tmp->data[0];
  }
  printf("[Get/Set/Meta/Vec]  Time: %.4f s\n", get_wall_time() - s);

  // 3. Basic Arithmetic
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_add_inplace_rc(m1, m2);
    mat_subtract_inplace_rc(m1, m2);
    mat_add_rc(m1, m2, m3);
    mat_subtract_rc(m1, m2, m3);
    dummy += m3->data[0];
  }
  printf("[Arith. Basic]      Time: %.4f s\n", get_wall_time() - s);

  // 4. Scalar and Element-wise
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_scale_inplace_rc(m1, 1.0001);
    mat_scale_rc(m2, m3, 0.9999);
    mat_hadamard_rc(m1, m2, m3);
    mat_map_rc(m1, m3, sqrt);
    dummy += m3->data[0];
  }
  printf("[Scalar/Elem/Map]   Time: %.4f s\n", get_wall_time() - s);

  // 5. Matrix-Matrix Multiplication
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_multiply_rc(m1, m2, m3);
    dummy += m3->data[0];
  }
  double multiply_time = get_wall_time() - s;
  printf("[Matrix x Matrix]   Time: %.4f s\n", multiply_time);

  // 6. Matrix-Vector Multiplication
  vec_t *vx = NULL, *vy = NULL;
  vec_alloc_rc(&vx, COLS);
  vec_alloc_rc(&vy, ROWS);
  vec_fill_rc(vx, 1.0);

  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_vec_multiply_rc(m1, vx, vy);
    dummy += vy->data[0];
  }
  double matvec_time = get_wall_time() - s;
  printf("[Matrix x Vector]   Time: %.4f s\n", matvec_time);

  // 7. Transformations and Reshape
  s = get_wall_time();
  mat_t* mT = NULL;
  mat_alloc_rc(&mT, COLS, ROWS);
  for (int i = 0; i < ITER; i++) {
    mat_transpose_rc(m1, mT);
    mat_reshape_rc(m1, ROWS / 2, COLS * 2);
    mat_reshape_rc(m1, ROWS, COLS);
    dummy += mT->data[0];
  }
  printf("[Transform/Reshape] Time: %.4f s\n", get_wall_time() - s);

  // 8. Properties and Utility
  s = get_wall_time();
  bool is_sq, is_eq;
  double total_sum;
  for (int i = 0; i < ITER; i++) {
    mat_is_square_rc(m1, &is_sq);
    mat_is_equal_rc(m1, m2, 1e-6, &is_eq);
    mat_sum_rc(m1, &total_sum);
    mat_swap_rc(m1, m2);
    mat_copy_rc(m1, m3);
    dummy += (double)is_sq + (double)is_eq + total_sum;
  }
  printf("[Properties/Util]   Time: %.4f s\n", get_wall_time() - s);

  // 9. Resize Test
  s = get_wall_time();
  for (int i = 0; i < 10; i++) {
    mat_resize_rc(&m3, ROWS + 10, COLS + 10);
    mat_resize_rc(&m3, ROWS, COLS);
  }
  printf("[Resize In-place]   Time: %.4f s\n", get_wall_time() - s);

  // 10. Row Operations 
  s = get_wall_time();
  for (int i = 0; i < ITER * 100; i++) {
    size_t r1 = i % ROWS;
    size_t r2 = (i + 1) % ROWS;
    mat_swap_rows_rc(m1, r1, r2);
  }
  printf("[Row Swaps]         Time: %.4f s\n", get_wall_time() - s);

  // 11. Linear Algebra (FULL 4000x4000) 
  printf("\n--- Linear Algebra Hardcore Tests (%dx%d) ---\n", ROWS, COLS);
  
  // Create a well-conditioned square matrix
  mat_identity_rc(m1);
  for (size_t i = 0; i < ROWS; i++) {
    for (size_t j = 0; j < COLS; j++) {
      if (i != j) {
        mat_set_rc(m1, i, j, 0.001 * (i + j) / ROWS);
      } else {
        mat_set_rc(m1, i, j, 10.0 + 0.01 * i);
      }
    }
  }

  mat_t *m_inv = NULL;
  mat_alloc_rc(&m_inv, ROWS, COLS);

  // 11a. Determinant
  s = get_wall_time();
  double det;
  for (int i = 0; i < ITER; i++) {
    mat_det_rc(m1, &det);
    dummy += det;
  }
  printf("[Determinant]       Time: %.4f s (det=%.2e)\n", 
         get_wall_time() - s, det);

  // 11b. Determinant In-place
  mat_copy_rc(m1, m2);
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_copy_rc(m1, m2);
    mat_det_inplace_rc(m2, &det);
    dummy += det;
  }
  printf("[Determinant Inpl]  Time: %.4f s\n", get_wall_time() - s);

  // 11c. Trace
  s = get_wall_time();
  double trace;
  for (int i = 0; i < ITER * 100; i++) {
    mat_trace_rc(m1, &trace);
    dummy += trace;
  }
  printf("[Trace]             Time: %.4f s (trace=%.2f)\n", 
         get_wall_time() - s, trace);

  // 11d. Inverse
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_inverse_rc(m1, m_inv);
    dummy += m_inv->data[0];
  }
  printf("[Inverse]           Time: %.4f s\n", get_wall_time() - s);

  // 11e. In-place Inverse
  mat_copy_rc(m1, m2);
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_copy_rc(m1, m2);
    mat_inverse_inplace_rc(m2);
    dummy += m2->data[0];
  }
  printf("[Inverse In-place]  Time: %.4f s\n", get_wall_time() - s);

  // 11f. Linear System Solve (Ax = b)
  vec_t *b_vec = NULL, *x_vec = NULL;
  vec_alloc_rc(&b_vec, ROWS);
  vec_alloc_rc(&x_vec, ROWS);
  vec_fill_rc(b_vec, 1.0);

  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_solve_rc(m1, b_vec, x_vec);
    dummy += x_vec->data[0];
  }
  printf("[Linear Solve]      Time: %.4f s\n", get_wall_time() - s);

  // 11g. LU Decomposition
  mat_t *lu = NULL;
  mat_alloc_rc(&lu, ROWS, COLS);
  size_t* piv = malloc(ROWS * sizeof(size_t));
  int sign;

  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_lu_decompose_rc(m1, lu, piv, &sign);
    dummy += lu->data[0];
  }
  printf("[LU Decompose]      Time: %.4f s\n", get_wall_time() - s);

  // 11h. LU Decomposition In-place
  s = get_wall_time();
  for (int i = 0; i < ITER; i++) {
    mat_copy_rc(m1, m2);
    mat_lu_decompose_inplace_rc(m2, piv, &sign);
    dummy += m2->data[0];
  }
  printf("[LU Decompose Inpl] Time: %.4f s\n", get_wall_time() - s);

  // ========== Cleanup ==========
  mat_free_rc(m1);
  mat_freep_rc(&m2);
  mat_free_rc(m3);
  mat_free_rc(m_arr);
  mat_free_rc(mT);
  mat_free_rc(m_inv);
  mat_free_rc(lu);
  vec_free_rc(v_tmp);
  vec_free_rc(vx);
  vec_free_rc(vy);
  vec_free_rc(b_vec);
  vec_free_rc(x_vec);
  free(raw_arr);
  free(piv);

  printf("\n====================================\n");
  printf("TOTAL EXECUTION TIME: %.4f s\n", get_wall_time() - total_start);
  printf("CHECKSUM (dummy): %f\n", dummy);
  printf("====================================\n");

  return 0;
}