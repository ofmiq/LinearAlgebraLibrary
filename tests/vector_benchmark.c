#include <config.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "benchmark_utils.h"
#include "vec.h"
#include "vec_rc.h"

#define N 1000000  // size of vector
#define ITER 50    // num of iterations

int main() {
  printf("--- TOTAL PERFORMANCE BENCHMARK ---\n");
  clock_t total_start = clock();
  clock_t s;
  double dummy = 0;

  // 1. allocation and initialization
  s = clock();
  vec_t* v1 = vec_alloc(N);
  vec_t* v2 = vec_alloc(N);
  vec_t* v3 = vec_alloc(N);
  double* arr = malloc(N * sizeof(double));
  for (size_t i = 0; i < N; i++) arr[i] = (double)i;

  for (int i = 0; i < ITER; i++) {
    vec_fill(v1, 10.0 + (i % 2));
    vec_fill_rc(v2, 20.0 - (i % 2));
    vec_t* z = vec_zeros(N);
    vec_t* o = vec_ones(N);
    dummy += (z->data[0] + o->data[0]);
    vec_free(z);
    vec_free(o);
  }
  vec_t* v_arr = vec_from_array(arr, N);
  dummy += v_arr->data[N - 1];
  printf("[Init/Alloc/Fill]  Time: %.4f s\n", get_t(s));

  // 2. getters and setters
  s = clock();
  double val;
  const double* data_ptr;
  size_t sz;
  for (int i = 0; i < ITER * 1000; i++) {
    vec_set(v1, 0, (double)i);
    vec_set_rc(v2, 0, (double)i + 1);
    val = vec_get(v1, 0);
    vec_get_rc(v2, 0, &val);
    dummy += val;
    sz = vec_size(v1);
    vec_size_rc(v2, &sz);
    dummy += (double)sz;
    data_ptr = vec_data(v1);
    vec_data_rc(v2, &data_ptr);
    dummy += data_ptr[0];
  }
  printf("[Get/Set/Meta]     Time: %.4f s\n", get_t(s));

  // 3. in-place arithmetic
  s = clock();
  for (int i = 0; i < ITER; i++) {
    vec_add_inplace(v1, v2);
    vec_add_inplace_rc(v1, v2);
    vec_subtract_inplace(v1, v2);
    vec_subtract_inplace_rc(v1, v2);
    vec_scale_inplace(v1, 1.00001);
    vec_scale_inplace_rc(v2, 0.99999);
    vec_axpy_rc(0.1, v1, v2);
    dummy += v1->data[0] + v2->data[0];
  }
  printf("[Arith. In-place]  Time: %.4f s\n", get_t(s));

  // 4. new arithmetic
  s = clock();
  for (int i = 0; i < ITER; i++) {
    vec_t* a = vec_add_new(v1, v2);
    vec_t* b = vec_subtract_new(v1, v2);
    vec_t* c = vec_scale_new(v1, 1.1);
    vec_t* d = vec_multiply_new(v1, v2);
    vec_t* e = vec_negate_new(v1);
    vec_t* f = vec_axpy_new(1.1, v1, v2);
    dummy += a->data[0] + f->data[0];
    vec_free(a);
    vec_free(b);
    vec_free(c);
    vec_free(d);
    vec_free(e);
    vec_free(f);
  }
  printf("[Arith. New]       Time: %.4f s\n", get_t(s));

  // 5. geometry and statistics
  s = clock();
  double res_d;
  for (int i = 0; i < ITER; i++) {
    dummy += vec_dot(v1, v2);
    dummy += vec_len(v1);
    vec_dist_rc(v1, v2, &res_d);
    dummy += res_d;
    dummy += vec_dist_sq(v1, v2);
    dummy += vec_sum(v1);
    dummy += vec_min(v1);
    dummy += vec_max(v1);
    dummy += vec_angle(v1, v2);
  }
  printf("[Geom/Stats]       Time: %.4f s\n", get_t(s));

  // 6. transformations
  s = clock();
  for (int i = 0; i < ITER; i++) {
    vec_normalize(v1);
    vec_t* n = vec_normalized_new(v2);
    vec_t* m = vec_map_new(v1, sqrt);
    vec_t* p = vec_project_new(v1, v2);
    vec_t* dup = vec_duplicate(v1);
    dummy += n->data[0] + m->data[0] + p->data[0] + dup->data[0];
    vec_free(n);
    vec_free(m);
    vec_free(p);
    vec_free(dup);
  }
  printf("[Transformations]  Time: %.4f s\n", get_t(s));

  // 7. logic and special operations
  s = clock();
  bool eq;
  for (int i = 0; i < ITER; i++) {
    eq = vec_is_equal(v1, v2, 1e-6);
    dummy += (double)eq;
    vec_swap(v1, v2);
    vec_t* r = vec_resize_new(v1, N + 10);
    vec_resize_inplace(v2, N + 10);
    dummy += r->data[0];
    vec_free(r);
  }
  vec_t* c1 = vec_alloc(3);
  vec_t* c2 = vec_alloc(3);
  vec_fill(c1, 1.0);
  vec_fill(c2, 2.0);
  for (int i = 0; i < ITER * 10000; i++) {
    vec_cross_inplace(c1, c2);
    vec_t* cr = vec_cross_new(c1, c2);
    dummy += cr->data[0];
    vec_free(cr);
  }
  printf("[Logic/Resize/Cr]  Time: %.4f s\n", get_t(s));

  // 8. freeing up memory
  vec_free(v1);
  vec_freep(&v2);
  vec_free(v3);
  vec_free(v_arr);
  free(arr);
  vec_free(c1);
  vec_free(c2);

  printf("------------------------------------\n");
  printf("TOTAL EXECUTION TIME: %.4f s\n", get_t(total_start));
  printf("CHECKSUM (to prevent optimization): %f\n", dummy);
  printf("------------------------------------\n");
  printf("\nTotal Benchmark Finished.\n");
  return 0;
}