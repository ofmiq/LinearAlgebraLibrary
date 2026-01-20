#define _GNU_SOURCE
#include <unistd.h>

#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "vec.h"

#define STRESS_SIZE 10000000
#define ALIGNMENT 64
#define EPSILON 1e-12

static double square(double x) { return x * x; }

static double bad_func(double x) {
  (void)x;
  return INFINITY;
}

static void assert_vec_contents(const vec_t* v, const double* expected,
                                size_t n) {
  const double* data = vec_data(v);
  TEST_ASSERT_NOT_NULL(data);
  
  for (size_t i = 0; i < n; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, expected[i], data[i]);
  }
}

void setUp(void) {}
void tearDown(void) {}

/* ============================================================ */
/*                     Lifecycle Management                     */
/* ============================================================ */

void test_vec_alloc_basic(void) {
  vec_t* v = vec_alloc(5);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_NOT_NULL(v->data);
  TEST_ASSERT_EQUAL_UINT64(5, vec_size(v));
  vec_free(v);
}

void test_vec_alloc_edge_cases(void) {
  vec_t* v = vec_alloc(0);
  TEST_ASSERT_NULL(v);

#ifdef VECTOR_MAX_ELEMENTS
  v = vec_alloc(VECTOR_MAX_ELEMENTS + 1);
  TEST_ASSERT_NULL(v);
#endif
}

void test_vec_from_array(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  TEST_ASSERT_NOT_NULL(v);
  assert_vec_contents(v, arr, 3);
  vec_free(v);
}

void test_vec_from_array_edge_cases(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  
  vec_t* v = vec_from_array(NULL, 3);
  TEST_ASSERT_NULL(v);
  
  v = vec_from_array(arr, 0);
  TEST_ASSERT_NULL(v);
}

void test_vec_free(void) {
  vec_t* v = vec_alloc(5);
  TEST_ASSERT_NOT_NULL(v);
  
  vec_free(v);
  vec_free(NULL);
}

void test_vec_freep(void) {
  vec_t* v = vec_alloc(5);
  TEST_ASSERT_NOT_NULL(v);
  
  vec_freep(&v);
  TEST_ASSERT_NULL(v);
  
  vec_freep(NULL);
  
  v = NULL;
  vec_freep(&v);
}

void test_vec_resize_new_expand(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* resized = vec_resize_new(v, 5);
  TEST_ASSERT_NOT_NULL(resized);
  TEST_ASSERT_EQUAL_UINT64(5, vec_size(resized));
  
  const double expected[5] = {1.0, 2.0, 3.0, 0.0, 0.0};
  assert_vec_contents(resized, expected, 5);
  
  vec_free(v);
  vec_free(resized);
}

void test_vec_resize_new_shrink(void) {
  double arr[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
  vec_t* v = vec_from_array(arr, 5);
  
  vec_t* resized = vec_resize_new(v, 2);
  TEST_ASSERT_NOT_NULL(resized);
  TEST_ASSERT_EQUAL_UINT64(2, vec_size(resized));
  
  const double expected[2] = {1.0, 2.0};
  assert_vec_contents(resized, expected, 2);
  
  vec_free(v);
  vec_free(resized);
}

void test_vec_resize_inplace(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  TEST_ASSERT_TRUE(vec_resize_inplace(v, 5));
  TEST_ASSERT_EQUAL_UINT64(5, vec_size(v));
  
  const double expected[5] = {1.0, 2.0, 3.0, 0.0, 0.0};
  assert_vec_contents(v, expected, 5);
  
  vec_free(v);
}

void test_vec_resize_edge_cases(void) {
  vec_t* v = vec_alloc(5);
  
  TEST_ASSERT_FALSE(vec_resize_inplace(v, 0));
  
  vec_t* resized = vec_resize_new(NULL, 10);
  TEST_ASSERT_NULL(resized);
  
  vec_free(v);
}

/* ============================================================ */
/*                  Data Access and Inspection                  */
/* ============================================================ */

void test_vec_set_get(void) {
  vec_t* v = vec_alloc(3);
  
  TEST_ASSERT_TRUE(vec_set(v, 0, 1.5));
  TEST_ASSERT_TRUE(vec_set(v, 1, 2.5));
  TEST_ASSERT_TRUE(vec_set(v, 2, 3.5));
  
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.5, vec_get(v, 0));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 2.5, vec_get(v, 1));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.5, vec_get(v, 2));
  
  vec_free(v);
}

void test_vec_set_get_edge_cases(void) {
  vec_t* v = vec_alloc(3);
  
  TEST_ASSERT_FALSE(vec_set(v, 3, 1.0));
  TEST_ASSERT_FALSE(vec_set(v, 100, 1.0));
  
  double val = vec_get(v, 3);
  TEST_ASSERT_TRUE(isnan(val));
  
  val = vec_get(v, 100);
  TEST_ASSERT_TRUE(isnan(val));
  
  TEST_ASSERT_FALSE(vec_set(NULL, 0, 1.0));
  
  val = vec_get(NULL, 0);
  TEST_ASSERT_TRUE(isnan(val));
  
  vec_free(v);
}

void test_vec_size(void) {
  vec_t* v = vec_alloc(7);
  TEST_ASSERT_EQUAL_UINT64(7, vec_size(v));
  vec_free(v);
  
  TEST_ASSERT_EQUAL_UINT64(0, vec_size(NULL));
}

void test_vec_data(void) {
  double arr[4] = {0.5, -1.5, 2.25, 3.75};
  vec_t* v = vec_from_array(arr, 4);
  
  const double* data = vec_data(v);
  TEST_ASSERT_NOT_NULL(data);
  
  for (size_t i = 0; i < 4; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, arr[i], data[i]);
  }
  
  vec_free(v);
  
  TEST_ASSERT_NULL(vec_data(NULL));
}

/* ============================================================ */
/*                   Basic Vector Arithmetic                    */
/* ============================================================ */

void test_vec_add_new(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 0.5};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_add_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {5.0, 1.0, 3.5};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_add_new_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  vec_t* result = vec_add_new(NULL, a);
  TEST_ASSERT_NULL(result);
  
  result = vec_add_new(a, NULL);
  TEST_ASSERT_NULL(result);
  
  vec_free(a);
}

void test_vec_add_inplace(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 0.5};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  TEST_ASSERT_TRUE(vec_add_inplace(a, b));
  
  const double expected[3] = {5.0, 1.0, 3.5};
  assert_vec_contents(a, expected, 3);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_add_size_mismatch(void) {
  vec_t* a = vec_alloc(3);
  vec_t* b = vec_alloc(2);
  
  vec_t* result = vec_add_new(a, b);
  TEST_ASSERT_NULL(result);
  
  TEST_ASSERT_FALSE(vec_add_inplace(a, b));
  
  vec_free(a);
  vec_free(b);
}

void test_vec_subtract_new(void) {
  double a_arr[3] = {5.0, 4.0, 3.0};
  double b_arr[3] = {1.0, 2.0, 3.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_subtract_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {4.0, 2.0, 0.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_subtract_inplace(void) {
  double a_arr[3] = {5.0, 4.0, 3.0};
  double b_arr[3] = {1.0, 2.0, 3.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  TEST_ASSERT_TRUE(vec_subtract_inplace(a, b));
  
  const double expected[3] = {4.0, 2.0, 0.0};
  assert_vec_contents(a, expected, 3);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_negate_new(void) {
  double arr[3] = {1.0, -2.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* result = vec_negate_new(v);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {-1.0, 2.0, 0.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(v);
  vec_free(result);
}

void test_vec_negate_edge_cases(void) {
  vec_t* result = vec_negate_new(NULL);
  TEST_ASSERT_NULL(result);
}

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

void test_vec_scale_new(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* result = vec_scale_new(v, 2.0);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {2.0, 4.0, 6.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(v);
  vec_free(result);
}

void test_vec_scale_inplace(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  TEST_ASSERT_TRUE(vec_scale_inplace(v, -0.5));
  
  const double expected[3] = {-0.5, -1.0, -1.5};
  assert_vec_contents(v, expected, 3);
  
  vec_free(v);
}

void test_vec_scale_edge_cases(void) {
  vec_t* result = vec_scale_new(NULL, 2.0);
  TEST_ASSERT_NULL(result);
  
  TEST_ASSERT_FALSE(vec_scale_inplace(NULL, 2.0));
}

void test_vec_axpy_new(void) {
  double x_arr[3] = {2.0, 3.0, 4.0};
  double y_arr[3] = {1.0, 1.0, 1.0};
  vec_t* x = vec_from_array(x_arr, 3);
  vec_t* y = vec_from_array(y_arr, 3);
  
  vec_t* result = vec_axpy_new(2.0, x, y);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {5.0, 7.0, 9.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(x);
  vec_free(y);
  vec_free(result);
}

void test_vec_multiply_new(void) {
  double a_arr[3] = {1.5, 2.0, 3.0};
  double b_arr[3] = {2.0, -1.0, 0.5};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_multiply_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {3.0, -2.0, 1.5};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_multiply_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  vec_t* result = vec_multiply_new(NULL, a);
  TEST_ASSERT_NULL(result);
  
  result = vec_multiply_new(a, NULL);
  TEST_ASSERT_NULL(result);
  
  vec_free(a);
}

void test_vec_map_new(void) {
  double arr[3] = {1.5, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* result = vec_map_new(v, square);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {2.25, 4.0, 9.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(v);
  vec_free(result);
}

void test_vec_map_edge_cases(void) {
  vec_t* v = vec_alloc(3);
  
  vec_t* result = vec_map_new(NULL, square);
  TEST_ASSERT_NULL(result);
  
  result = vec_map_new(v, NULL);
  TEST_ASSERT_NULL(result);
  
  result = vec_map_new(v, bad_func);
  TEST_ASSERT_NULL(result);
  
  vec_free(v);
}

void test_vec_zeros(void) {
  vec_t* v = vec_zeros(4);
  TEST_ASSERT_NOT_NULL(v);
  
  const double expected[4] = {0.0, 0.0, 0.0, 0.0};
  assert_vec_contents(v, expected, 4);
  
  vec_free(v);
}

void test_vec_ones(void) {
  vec_t* v = vec_ones(4);
  TEST_ASSERT_NOT_NULL(v);
  
  const double expected[4] = {1.0, 1.0, 1.0, 1.0};
  assert_vec_contents(v, expected, 4);
  
  vec_free(v);
}

void test_vec_fill(void) {
  vec_t* v = vec_alloc(4);
  
  TEST_ASSERT_TRUE(vec_fill(v, 7.5));
  
  const double expected[4] = {7.5, 7.5, 7.5, 7.5};
  assert_vec_contents(v, expected, 4);
  
  vec_free(v);
}

void test_vec_fill_edge_cases(void) {
  vec_t* v = vec_alloc(3);
  
  TEST_ASSERT_FALSE(vec_fill(v, NAN));
  TEST_ASSERT_FALSE(vec_fill(v, INFINITY));
  TEST_ASSERT_FALSE(vec_fill(v, -INFINITY));
  TEST_ASSERT_FALSE(vec_fill(NULL, 1.0));
  
  vec_free(v);
}

/* ============================================================ */
/*           Vector Products and Geometric Properties           */
/* ============================================================ */

void test_vec_dot(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 2.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double dot = vec_dot(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 8.0, dot);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_dot_orthogonal(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double dot = vec_dot(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, dot);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_dot_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  double dot = vec_dot(NULL, a);
  TEST_ASSERT_TRUE(isnan(dot));
  
  dot = vec_dot(a, NULL);
  TEST_ASSERT_TRUE(isnan(dot));
  
  vec_free(a);
}

void test_vec_cross_new(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_cross_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {0.0, 0.0, 1.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_cross_general(void) {
  double a_arr[3] = {2.0, 3.0, 4.0};
  double b_arr[3] = {5.0, 6.0, 7.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_cross_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {-3.0, 6.0, -3.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_cross_inplace(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  TEST_ASSERT_TRUE(vec_cross_inplace(a, b));
  
  const double expected[3] = {0.0, 0.0, 1.0};
  assert_vec_contents(a, expected, 3);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_cross_edge_cases(void) {
  vec_t* a = vec_alloc(2);
  vec_t* b = vec_alloc(2);
  
  vec_t* result = vec_cross_new(a, b);
  TEST_ASSERT_NULL(result);
  
  vec_free(a);
  vec_free(b);
  
  result = vec_cross_new(NULL, NULL);
  TEST_ASSERT_NULL(result);
}

void test_vec_len(void) {
  double arr[3] = {3.0, 4.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  double len = vec_len(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, len);
  
  vec_free(v);
}

void test_vec_len_unit_vector(void) {
  double arr[3] = {1.0, 0.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  double len = vec_len(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);
  
  vec_free(v);
}

void test_vec_len_edge_cases(void) {
  double len = vec_len(NULL);
  TEST_ASSERT_TRUE(isnan(len));
}

void test_vec_normalized_new(void) {
  double arr[3] = {3.0, 4.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* normalized = vec_normalized_new(v);
  TEST_ASSERT_NOT_NULL(normalized);
  
  double len = vec_len(normalized);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);
  
  const double expected[3] = {0.6, 0.8, 0.0};
  assert_vec_contents(normalized, expected, 3);
  
  vec_free(v);
  vec_free(normalized);
}

void test_vec_normalize(void) {
  double arr[3] = {3.0, 4.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  TEST_ASSERT_TRUE(vec_normalize(v));
  
  double len = vec_len(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);
  
  const double expected[3] = {0.6, 0.8, 0.0};
  assert_vec_contents(v, expected, 3);
  
  vec_free(v);
}

void test_vec_normalize_edge_cases(void) {
  double arr[3] = {0.0, 0.0, 0.0};
  vec_t* v = vec_from_array(arr, 3);
  
  TEST_ASSERT_FALSE(vec_normalize(v));
  
  vec_t* normalized = vec_normalized_new(v);
  TEST_ASSERT_NULL(normalized);
  
  vec_free(v);
  
  TEST_ASSERT_FALSE(vec_normalize(NULL));
}

void test_vec_angle(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double angle = vec_angle(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, M_PI_2, angle);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_angle_parallel(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {2.0, 0.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double angle = vec_angle(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, angle);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_angle_opposite(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {-1.0, 0.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double angle = vec_angle(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, M_PI, angle);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_angle_edge_cases(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 0.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double angle = vec_angle(a, b);
  TEST_ASSERT_TRUE(isnan(angle));
  
  vec_free(a);
  vec_free(b);
  
  angle = vec_angle(NULL, NULL);
  TEST_ASSERT_TRUE(isnan(angle));
}

void test_vec_project_new(void) {
  double a_arr[3] = {3.0, 4.0, 0.0};
  double b_arr[3] = {1.0, 0.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_project_new(a, b);
  TEST_ASSERT_NOT_NULL(result);
  
  const double expected[3] = {3.0, 0.0, 0.0};
  assert_vec_contents(result, expected, 3);
  
  vec_free(a);
  vec_free(b);
  vec_free(result);
}

void test_vec_project_edge_cases(void) {
  double a_arr[3] = {3.0, 4.0, 0.0};
  double b_arr[3] = {0.0, 0.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_t* result = vec_project_new(a, b);
  TEST_ASSERT_NULL(result);
  
  vec_free(a);
  vec_free(b);
  
  result = vec_project_new(NULL, NULL);
  TEST_ASSERT_NULL(result);
}

/* ============================================================ */
/*                    Comparison and Metrics                    */
/* ============================================================ */

void test_vec_is_equal_true(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {1.0 + 1e-10, 2.0 - 1e-10, 3.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  TEST_ASSERT_TRUE(vec_is_equal(a, b, 1e-8));
  
  vec_free(a);
  vec_free(b);
}

void test_vec_is_equal_false(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {1.0 + 1e-5, 2.0, 3.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  TEST_ASSERT_FALSE(vec_is_equal(a, b, 1e-8));
  
  vec_free(a);
  vec_free(b);
}

void test_vec_is_equal_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  TEST_ASSERT_FALSE(vec_is_equal(NULL, a, 1e-8));
  TEST_ASSERT_FALSE(vec_is_equal(a, NULL, 1e-8));
  
  vec_free(a);
}

void test_vec_dist(void) {
  double a_arr[3] = {0.0, 0.0, 0.0};
  double b_arr[3] = {3.0, 4.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double dist = vec_dist(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, dist);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_dist_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  double dist = vec_dist(NULL, a);
  TEST_ASSERT_TRUE(isnan(dist));
  
  dist = vec_dist(a, NULL);
  TEST_ASSERT_TRUE(isnan(dist));
  
  vec_free(a);
}

void test_vec_dist_sq(void) {
  double a_arr[3] = {0.0, 0.0, 0.0};
  double b_arr[3] = {3.0, 4.0, 0.0};
  vec_t* a = vec_from_array(a_arr, 3);
  vec_t* b = vec_from_array(b_arr, 3);
  
  double dist_sq = vec_dist_sq(a, b);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 25.0, dist_sq);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_dist_sq_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  double dist = vec_dist_sq(NULL, a);
  TEST_ASSERT_TRUE(isnan(dist));
  
  dist = vec_dist_sq(a, NULL);
  TEST_ASSERT_TRUE(isnan(dist));
  
  vec_free(a);
}

/* ============================================================ */
/*              Utility and Statistical Functions               */
/* ============================================================ */

void test_vec_min(void) {
  double arr[5] = {5.0, -2.0, 7.0, 1.0, -3.0};
  vec_t* v = vec_from_array(arr, 5);
  
  double min_val = vec_min(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -3.0, min_val);
  
  vec_free(v);
}

void test_vec_min_edge_cases(void) {
  double min_val = vec_min(NULL);
  TEST_ASSERT_TRUE(isnan(min_val));
}

void test_vec_max(void) {
  double arr[5] = {5.0, -2.0, 7.0, 1.0, -3.0};
  vec_t* v = vec_from_array(arr, 5);
  
  double max_val = vec_max(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 7.0, max_val);
  
  vec_free(v);
}

void test_vec_max_edge_cases(void) {
  double max_val = vec_max(NULL);
  TEST_ASSERT_TRUE(isnan(max_val));
}

void test_vec_sum(void) {
  double arr[4] = {1.0, 2.0, 3.0, 4.0};
  vec_t* v = vec_from_array(arr, 4);
  
  double sum = vec_sum(v);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 10.0, sum);
  
  vec_free(v);
}

void test_vec_sum_edge_cases(void) {
  double sum = vec_sum(NULL);
  TEST_ASSERT_TRUE(isnan(sum));
}

void test_vec_swap(void) {
  double a_arr[2] = {1.0, 2.0};
  double b_arr[3] = {3.0, 4.0, 5.0};
  vec_t* a = vec_from_array(a_arr, 2);
  vec_t* b = vec_from_array(b_arr, 3);
  
  vec_swap(a, b);
  
  TEST_ASSERT_EQUAL_UINT64(3, vec_size(a));
  TEST_ASSERT_EQUAL_UINT64(2, vec_size(b));
  
  assert_vec_contents(a, b_arr, 3);
  assert_vec_contents(b, a_arr, 2);
  
  vec_free(a);
  vec_free(b);
}

void test_vec_swap_edge_cases(void) {
  vec_t* a = vec_alloc(3);
  
  vec_swap(NULL, a);
  vec_swap(a, NULL);
  vec_swap(NULL, NULL);
  
  vec_free(a);
}

void test_vec_duplicate(void) {
  double arr[3] = {9.0, 8.0, 7.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* duplicate = vec_duplicate(v);
  TEST_ASSERT_NOT_NULL(duplicate);
  
  assert_vec_contents(duplicate, arr, 3);
  
  TEST_ASSERT_NOT_EQUAL(v->data, duplicate->data);
  
  vec_free(v);
  vec_free(duplicate);
}

void test_vec_duplicate_edge_cases(void) {
  vec_t* duplicate = vec_duplicate(NULL);
  TEST_ASSERT_NULL(duplicate);
}

void test_vec_print(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_print(v);
  vec_print(NULL);
  
  vec_free(v);
}

/* ============================================================ */
/*                        Stress Tests                          */
/* ============================================================ */

void test_large_alloc_and_alignment(void) {
  vec_t* v = vec_alloc(STRESS_SIZE);
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_NOT_NULL(v->data);
  
  TEST_ASSERT_EQUAL_INT(0, ((uintptr_t)v->data) % ALIGNMENT);
  
  TEST_ASSERT_TRUE(vec_fill(v, 1.234));
  
  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val = vec_get(v, i);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.234, val);
  }
  
  vec_free(v);
}

void test_large_vector_operations(void) {
  vec_t* a = vec_alloc(STRESS_SIZE);
  vec_t* b = vec_alloc(STRESS_SIZE);
  
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_NOT_NULL(b);
  
  TEST_ASSERT_TRUE(vec_fill(a, 1.0));
  TEST_ASSERT_TRUE(vec_fill(b, 2.0));
  
  double start = omp_get_wtime();
  vec_t* sum = vec_add_new(a, b);
  double end = omp_get_wtime();
  
  TEST_ASSERT_NOT_NULL(sum);
  printf("vec_add_new for %d elements: %.6f s\n", STRESS_SIZE, end - start);
  
  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val = vec_get(sum, i);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, val);
  }
  
  start = omp_get_wtime();
  double dot = vec_dot(a, b);
  end = omp_get_wtime();
  
  printf("vec_dot for %d elements: %.6f s\n", STRESS_SIZE, end - start);
  TEST_ASSERT_DOUBLE_WITHIN(1e-6, (double)STRESS_SIZE * 2.0, dot);
  
  vec_free(a);
  vec_free(b);
  vec_free(sum);
}

void test_resize_large_vector(void) {
  vec_t* v = vec_alloc(10);
  TEST_ASSERT_NOT_NULL(v);
  
  TEST_ASSERT_TRUE(vec_resize_inplace(v, STRESS_SIZE));
  TEST_ASSERT_TRUE(vec_fill(v, 42.0));
  
  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val = vec_get(v, i);
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 42.0, val);
  }
  
  TEST_ASSERT_TRUE(vec_resize_inplace(v, 5));
  TEST_ASSERT_EQUAL_UINT64(5, vec_size(v));
  
  vec_free(v);
}

void test_openmp_parallel_correctness(void) {
  const size_t test_size = 100000;
  vec_t* a = vec_alloc(test_size);
  vec_t* b = vec_alloc(test_size);
  
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_NOT_NULL(b);
  
  TEST_ASSERT_TRUE(vec_fill(a, 1.0));
  TEST_ASSERT_TRUE(vec_fill(b, 2.0));
  
  TEST_ASSERT_TRUE(vec_add_inplace(a, b));
  
  const double* data = vec_data(a);
  TEST_ASSERT_NOT_NULL(data);
  
  for (size_t i = 0; i < test_size; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, data[i]);
  }
  
  vec_free(a);
  vec_free(b);
}

void test_chained_operations(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = vec_from_array(arr, 3);
  
  vec_t* scaled = vec_scale_new(v, 2.0);
  TEST_ASSERT_NOT_NULL(scaled);
  
  vec_t* added = vec_add_new(scaled, v);
  TEST_ASSERT_NOT_NULL(added);
  
  vec_t* normalized = vec_normalized_new(added);
  TEST_ASSERT_NOT_NULL(normalized);
  
  double len = vec_len(normalized);
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);
  
  vec_free(v);
  vec_free(scaled);
  vec_free(added);
  vec_free(normalized);
}

void test_memory_intensive_operations(void) {
  const size_t num_vectors = 100;
  vec_t* vectors[num_vectors];
  
  for (size_t i = 0; i < num_vectors; ++i) {
    vectors[i] = vec_alloc(1000);
    TEST_ASSERT_NOT_NULL(vectors[i]);
    TEST_ASSERT_TRUE(vec_fill(vectors[i], (double)i));
  }
  
  for (size_t i = 0; i < num_vectors - 1; ++i) {
    vec_t* sum = vec_add_new(vectors[i], vectors[i + 1]);
    TEST_ASSERT_NOT_NULL(sum);
    vec_free(sum);
  }
  
  for (size_t i = 0; i < num_vectors; ++i) {
    vec_free(vectors[i]);
  }
}

/* ============================================================ */
/*                          Main                                */
/* ============================================================ */

int main(void) {
  UNITY_BEGIN();
  
  // Lifecycle Management
  RUN_TEST(test_vec_alloc_basic);
  RUN_TEST(test_vec_alloc_edge_cases);
  RUN_TEST(test_vec_from_array);
  RUN_TEST(test_vec_from_array_edge_cases);
  RUN_TEST(test_vec_free);
  RUN_TEST(test_vec_freep);
  RUN_TEST(test_vec_resize_new_expand);
  RUN_TEST(test_vec_resize_new_shrink);
  RUN_TEST(test_vec_resize_inplace);
  RUN_TEST(test_vec_resize_edge_cases);
  
  // Data Access and Inspection
  RUN_TEST(test_vec_set_get);
  RUN_TEST(test_vec_set_get_edge_cases);
  RUN_TEST(test_vec_size);
  RUN_TEST(test_vec_data);
  
  // Basic Vector Arithmetic
  RUN_TEST(test_vec_add_new);
  RUN_TEST(test_vec_add_new_edge_cases);
  RUN_TEST(test_vec_add_inplace);
  RUN_TEST(test_vec_add_size_mismatch);
  RUN_TEST(test_vec_subtract_new);
  RUN_TEST(test_vec_subtract_inplace);
  RUN_TEST(test_vec_negate_new);
  RUN_TEST(test_vec_negate_edge_cases);
  
  // Scalar and Element-wise Operations
  RUN_TEST(test_vec_scale_new);
  RUN_TEST(test_vec_scale_inplace);
  RUN_TEST(test_vec_scale_edge_cases);
  RUN_TEST(test_vec_axpy_new);
  RUN_TEST(test_vec_multiply_new);
  RUN_TEST(test_vec_multiply_edge_cases);
  RUN_TEST(test_vec_map_new);
  RUN_TEST(test_vec_map_edge_cases);
  RUN_TEST(test_vec_zeros);
  RUN_TEST(test_vec_ones);
  RUN_TEST(test_vec_fill);
  RUN_TEST(test_vec_fill_edge_cases);
  
  // Vector Products and Geometric Properties
  RUN_TEST(test_vec_dot);
  RUN_TEST(test_vec_dot_orthogonal);
  RUN_TEST(test_vec_dot_edge_cases);
  RUN_TEST(test_vec_cross_new);
  RUN_TEST(test_vec_cross_general);
  RUN_TEST(test_vec_cross_inplace);
  RUN_TEST(test_vec_cross_edge_cases);
  RUN_TEST(test_vec_len);
  RUN_TEST(test_vec_len_unit_vector);
  RUN_TEST(test_vec_len_edge_cases);
  RUN_TEST(test_vec_normalized_new);
  RUN_TEST(test_vec_normalize);
  RUN_TEST(test_vec_normalize_edge_cases);
  RUN_TEST(test_vec_angle);
  RUN_TEST(test_vec_angle_parallel);
  RUN_TEST(test_vec_angle_opposite);
  RUN_TEST(test_vec_angle_edge_cases);
  RUN_TEST(test_vec_project_new);
  RUN_TEST(test_vec_project_edge_cases);
  
  // Comparison and Metrics
  RUN_TEST(test_vec_is_equal_true);
  RUN_TEST(test_vec_is_equal_false);
  RUN_TEST(test_vec_is_equal_edge_cases);
  RUN_TEST(test_vec_dist);
  RUN_TEST(test_vec_dist_edge_cases);
  RUN_TEST(test_vec_dist_sq);
  RUN_TEST(test_vec_dist_sq_edge_cases);
  
  // Utility and Statistical Functions
  RUN_TEST(test_vec_min);
  RUN_TEST(test_vec_min_edge_cases);
  RUN_TEST(test_vec_max);
  RUN_TEST(test_vec_max_edge_cases);
  RUN_TEST(test_vec_sum);
  RUN_TEST(test_vec_sum_edge_cases);
  RUN_TEST(test_vec_swap);
  RUN_TEST(test_vec_swap_edge_cases);
  RUN_TEST(test_vec_duplicate);
  RUN_TEST(test_vec_duplicate_edge_cases);
  RUN_TEST(test_vec_print);
  
  // Stress Tests
  RUN_TEST(test_large_alloc_and_alignment);
  RUN_TEST(test_large_vector_operations);
  RUN_TEST(test_resize_large_vector);
  RUN_TEST(test_openmp_parallel_correctness);
  RUN_TEST(test_chained_operations);
  RUN_TEST(test_memory_intensive_operations);
  
  return UNITY_END();
}