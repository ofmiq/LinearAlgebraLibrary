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

#define ROWS 4000
#define COLS 4000
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
  printf("[Matrix × Matrix]   Time: %.4f s\n", multiply_time);

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
  printf("[Matrix × Vector]   Time: %.4f s\n", matvec_time);

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

  // Cleanup
  mat_free_rc(m1);
  mat_freep_rc(&m2);
  mat_free_rc(m3);
  mat_free_rc(m_arr);
  mat_free_rc(mT);
  vec_free_rc(v_tmp);
  vec_free_rc(vx);
  vec_free_rc(vy);
  free(raw_arr);

  printf("------------------------------------\n");
  printf("TOTAL EXECUTION TIME: %.4f s\n", get_wall_time() - total_start);
  printf("CHECKSUM (dummy): %f\n", dummy);
  printf("------------------------------------\n");

  return 0;
}