#define _GNU_SOURCE
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "unity.h"
#include "vec_rc.h"

#define STRESS_SIZE 10000000
#define ALIGNMENT 64
#define EPSILON 1e-12

static vec_t* make_vec_from_array(const double* arr, size_t n) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_from_array_rc(arr, &v, n));
  return v;
}

static double square(double x) { return x * x; }

static double bad_func(double x) {
  (void)x;
  return INFINITY;
}

static void assert_vec_contents(const vec_t* v, const double* expected,
                                size_t n) {
  const double* data = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_data_rc(v, &data));

  for (size_t i = 0; i < n; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, expected[i], data[i]);
  }
}

void setUp(void) {}
void tearDown(void) {}

void test_vec_alloc_rc_basic(void) {
  vec_t* v = NULL;

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 5));
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_NOT_NULL(v->data);

  size_t n = 0;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(5, n);

  vec_free_rc(v);
}

void test_vec_alloc_rc_edge_cases(void) {
  vec_t* v = NULL;

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_alloc_rc(&v, 0));
  TEST_ASSERT_NULL(v);

  #ifdef VECTOR_MAX_ELEMENTS
  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_alloc_rc(&v, VECTOR_MAX_ELEMENTS + 1));
  TEST_ASSERT_NULL(v);
  #endif

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_alloc_rc(NULL, 5));
}

void test_vec_from_array_rc(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = NULL;

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_from_array_rc(arr, &v, 3));
  TEST_ASSERT_NOT_NULL(v);

  assert_vec_contents(v, arr, 3);

  vec_free_rc(v);
}

void test_vec_from_array_rc_edge_cases(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = NULL;

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_from_array_rc(NULL, &v, 3));

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_from_array_rc(arr, NULL, 3));

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_from_array_rc(arr, &v, 0));
}

void test_vec_free_rc(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 5));

  vec_free_rc(v);

  vec_free_rc(NULL);
}

void test_vec_freep_rc(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 5));
  TEST_ASSERT_NOT_NULL(v);

  vec_freep_rc(&v);
  TEST_ASSERT_NULL(v);

  vec_freep_rc(NULL);

  v = NULL;
  vec_freep_rc(&v);
}

void test_vec_resize_rc_expand(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_resize_rc(&v, 5));

  size_t n;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(5, n);

  const double expected[5] = {1.0, 2.0, 3.0, 0.0, 0.0};
  assert_vec_contents(v, expected, 5);

  vec_free_rc(v);
}

void test_vec_resize_rc_shrink(void) {
  double arr[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
  vec_t* v = make_vec_from_array(arr, 5);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_resize_rc(&v, 2));

  size_t n;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(2, n);

  const double expected[2] = {1.0, 2.0};
  assert_vec_contents(v, expected, 2);

  vec_free_rc(v);
}

void test_vec_resize_rc_same_size(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_resize_rc(&v, 3));

  size_t n;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(3, n);

  assert_vec_contents(v, arr, 3);

  vec_free_rc(v);
}

void test_vec_resize_rc_edge_cases(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 5));

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_resize_rc(&v, 0));

  #ifdef VECTOR_MAX_ELEMENTS
  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_resize_rc(&v, VECTOR_MAX_ELEMENTS + 1));
  #endif

  vec_free_rc(v);

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_resize_rc(NULL, 10));
}

void test_vec_set_get_rc(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_set_rc(v, 0, 1.5));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_set_rc(v, 1, 2.5));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_set_rc(v, 2, 3.5));

  double val;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(v, 0, &val));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.5, val);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(v, 1, &val));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 2.5, val);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(v, 2, &val));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.5, val);

  vec_free_rc(v);
}

void test_vec_set_get_rc_edge_cases(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 3));
  double val;

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_set_rc(v, 3, 1.0));
  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_set_rc(v, 100, 1.0));

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_get_rc(v, 3, &val));
  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_get_rc(v, 100, &val));

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_set_rc(NULL, 0, 1.0));
  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_get_rc(NULL, 0, &val));
  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_get_rc(v, 0, NULL));

  vec_free_rc(v);
}

void test_vec_size_rc(void) {
  vec_t* v = NULL;
  size_t n;

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 7));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(7, n);

  vec_free_rc(v);

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_size_rc(NULL, &n));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 3));
  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_size_rc(v, NULL));
  vec_free_rc(v);
}

void test_vec_data_rc(void) {
  double arr[4] = {0.5, -1.5, 2.25, 3.75};
  vec_t* v = make_vec_from_array(arr, 4);

  const double* data = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_data_rc(v, &data));
  TEST_ASSERT_NOT_NULL(data);

  for (size_t i = 0; i < 4; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, arr[i], data[i]);
  }

  vec_free_rc(v);

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_data_rc(NULL, &data));

  v = make_vec_from_array(arr, 4);
  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_data_rc(v, NULL));
  vec_free_rc(v);
}

void test_vec_add_rc(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 0.5};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_add_rc(a, b, out));

  const double expected[3] = {5.0, 1.0, 3.5};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_add_inplace_rc(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 0.5};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_add_inplace_rc(a, b));

  const double expected[3] = {5.0, 1.0, 3.5};
  assert_vec_contents(a, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_add_rc_size_mismatch(void) {
  vec_t* a = make_vec_from_array((double[]){1.0, 2.0, 3.0}, 3);
  vec_t* b = make_vec_from_array((double[]){1.0, 2.0}, 2);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_add_rc(a, b, out));

  vec_free_rc(out);
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 2));

  vec_t* c = make_vec_from_array((double[]){1.0, 2.0, 3.0}, 3);
  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_add_rc(a, c, out));

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(c);
  vec_free_rc(out);
}

void test_vec_subtract_rc(void) {
  double a_arr[3] = {5.0, 4.0, 3.0};
  double b_arr[3] = {1.0, 2.0, 3.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_subtract_rc(a, b, out));

  const double expected[3] = {4.0, 2.0, 0.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_subtract_inplace_rc(void) {
  double a_arr[3] = {5.0, 4.0, 3.0};
  double b_arr[3] = {1.0, 2.0, 3.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_subtract_inplace_rc(a, b));

  const double expected[3] = {4.0, 2.0, 0.0};
  assert_vec_contents(a, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_negate_rc(void) {
  double arr[3] = {1.0, -2.0, 0.0};
  vec_t* v = make_vec_from_array(arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_negate_rc(v, out));

  const double expected[3] = {-1.0, 2.0, 0.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(v);
  vec_free_rc(out);
}

void test_vec_scale_rc(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_scale_rc(v, out, 2.0));

  const double expected[3] = {2.0, 4.0, 6.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(v);
  vec_free_rc(out);
}

void test_vec_scale_inplace_rc(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_scale_inplace_rc(v, -0.5));

  const double expected[3] = {-0.5, -1.0, -1.5};
  assert_vec_contents(v, expected, 3);

  vec_free_rc(v);
}

void test_vec_axpy_rc(void) {
  double x_arr[3] = {2.0, 3.0, 4.0};
  double y_arr[3] = {1.0, 1.0, 1.0};
  vec_t* x = make_vec_from_array(x_arr, 3);
  vec_t* y = make_vec_from_array(y_arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_axpy_rc(2.0, x, y));

  const double expected[3] = {5.0, 7.0, 9.0};
  assert_vec_contents(y, expected, 3);

  vec_free_rc(x);
  vec_free_rc(y);
}

void test_vec_multiply_rc(void) {
  double a_arr[3] = {1.5, 2.0, 3.0};
  double b_arr[3] = {2.0, -1.0, 0.5};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_multiply_rc(a, b, out));

  const double expected[3] = {3.0, -2.0, 1.5};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_map_rc(void) {
  double arr[3] = {1.5, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_map_rc(v, out, square));

  const double expected[3] = {2.25, 4.0, 9.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(v);
  vec_free_rc(out);
}

void test_vec_map_rc_non_finite(void) {
  double arr[3] = {1.0, 2.0, 3.0};
  vec_t* v = make_vec_from_array(arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_RANGE, vec_map_rc(v, out, bad_func));

  vec_free_rc(v);
  vec_free_rc(out);
}

void test_vec_fill_rc(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 4));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(v, 7.5));

  const double expected[4] = {7.5, 7.5, 7.5, 7.5};
  assert_vec_contents(v, expected, 4);

  vec_free_rc(v);
}

void test_vec_fill_rc_non_finite(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 3));

  TEST_ASSERT_EQUAL_INT(ERR_INVALID_ARG, vec_fill_rc(v, NAN));

  TEST_ASSERT_EQUAL_INT(ERR_INVALID_ARG, vec_fill_rc(v, INFINITY));
  TEST_ASSERT_EQUAL_INT(ERR_INVALID_ARG, vec_fill_rc(v, -INFINITY));

  vec_free_rc(v);
}

void test_vec_dot_rc(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {4.0, -1.0, 2.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double dot;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_dot_rc(a, b, &dot));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 8.0, dot);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_dot_rc_orthogonal(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double dot;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_dot_rc(a, b, &dot));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, dot);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_cross_rc(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_cross_rc(a, b, out));

  const double expected[3] = {0.0, 0.0, 1.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_cross_rc_general(void) {
  double a_arr[3] = {2.0, 3.0, 4.0};
  double b_arr[3] = {5.0, 6.0, 7.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_cross_rc(a, b, out));

  const double expected[3] = {-3.0, 6.0, -3.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_cross_inplace_rc(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_cross_inplace_rc(a, b));

  const double expected[3] = {0.0, 0.0, 1.0};
  assert_vec_contents(a, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_cross_rc_wrong_size(void) {
  vec_t* a = make_vec_from_array((double[]){1.0, 2.0}, 2);
  vec_t* b = make_vec_from_array((double[]){3.0, 4.0}, 2);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 2));

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_cross_rc(a, b, out));

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_len_rc(void) {
  double arr[3] = {3.0, 4.0, 0.0};
  vec_t* v = make_vec_from_array(arr, 3);

  double len;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_len_rc(v, &len));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, len);

  vec_free_rc(v);
}

void test_vec_len_rc_unit_vector(void) {
  double arr[3] = {1.0, 0.0, 0.0};
  vec_t* v = make_vec_from_array(arr, 3);

  double len;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_len_rc(v, &len));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);

  vec_free_rc(v);
}

void test_vec_normalize_inplace_rc(void) {
  double arr[3] = {3.0, 4.0, 0.0};
  vec_t* v = make_vec_from_array(arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_normalize_inplace_rc(v));

  double len;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_len_rc(v, &len));
  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.0, len);

  const double expected[3] = {0.6, 0.8, 0.0};
  assert_vec_contents(v, expected, 3);

  vec_free_rc(v);
}

void test_vec_normalize_inplace_rc_zero_vector(void) {
  double arr[3] = {0.0, 0.0, 0.0};
  vec_t* v = make_vec_from_array(arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_DIV_ZERO, vec_normalize_inplace_rc(v));

  vec_free_rc(v);
}

void test_vec_angle_rc(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 1.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double angle;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_angle_rc(a, b, &angle));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, M_PI_2, angle);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_angle_rc_parallel(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {2.0, 0.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double angle;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_angle_rc(a, b, &angle));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 0.0, angle);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_angle_rc_opposite(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {-1.0, 0.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double angle;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_angle_rc(a, b, &angle));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, M_PI, angle);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_angle_rc_zero_vector(void) {
  double a_arr[3] = {1.0, 0.0, 0.0};
  double b_arr[3] = {0.0, 0.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double angle;
  TEST_ASSERT_EQUAL_INT(ERR_DIV_ZERO, vec_angle_rc(a, b, &angle));

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_project_rc(void) {
  double a_arr[3] = {3.0, 4.0, 0.0};
  double b_arr[3] = {1.0, 0.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_project_rc(a, b, out));

  const double expected[3] = {3.0, 0.0, 0.0};
  assert_vec_contents(out, expected, 3);

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_project_rc_zero_vector(void) {
  double a_arr[3] = {3.0, 4.0, 0.0};
  double b_arr[3] = {0.0, 0.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);
  vec_t* out = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, 3));

  TEST_ASSERT_EQUAL_INT(ERR_DIV_ZERO, vec_project_rc(a, b, out));

  vec_free_rc(a);
  vec_free_rc(b);
  vec_free_rc(out);
}

void test_vec_is_equal_rc_true(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {1.0 + 1e-10, 2.0 - 1e-10, 3.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  bool equal;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_is_equal_rc(a, b, 1e-8, &equal));
  TEST_ASSERT_TRUE(equal);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_is_equal_rc_false(void) {
  double a_arr[3] = {1.0, 2.0, 3.0};
  double b_arr[3] = {1.0 + 1e-5, 2.0, 3.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  bool equal;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_is_equal_rc(a, b, 1e-8, &equal));
  TEST_ASSERT_FALSE(equal);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_dist_rc(void) {
  double a_arr[3] = {0.0, 0.0, 0.0};
  double b_arr[3] = {3.0, 4.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double dist;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_dist_rc(a, b, &dist));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 5.0, dist);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_dist_sq_rc(void) {
  double a_arr[3] = {0.0, 0.0, 0.0};
  double b_arr[3] = {3.0, 4.0, 0.0};
  vec_t* a = make_vec_from_array(a_arr, 3);
  vec_t* b = make_vec_from_array(b_arr, 3);

  double dist_sq;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_dist_sq_rc(a, b, &dist_sq));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 25.0, dist_sq);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_min_rc(void) {
  double arr[5] = {5.0, -2.0, 7.0, 1.0, -3.0};
  vec_t* v = make_vec_from_array(arr, 5);

  double min_val;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_min_rc(v, &min_val));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, -3.0, min_val);

  vec_free_rc(v);
}

void test_vec_max_rc(void) {
  double arr[5] = {5.0, -2.0, 7.0, 1.0, -3.0};
  vec_t* v = make_vec_from_array(arr, 5);

  double max_val;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_max_rc(v, &max_val));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 7.0, max_val);

  vec_free_rc(v);
}

void test_vec_sum_rc(void) {
  double arr[4] = {1.0, 2.0, 3.0, 4.0};
  vec_t* v = make_vec_from_array(arr, 4);

  double sum;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_sum_rc(v, &sum));

  TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 10.0, sum);

  vec_free_rc(v);
}

void test_vec_swap_rc(void) {
  double a_arr[2] = {1.0, 2.0};
  double b_arr[3] = {3.0, 4.0, 5.0};
  vec_t* a = make_vec_from_array(a_arr, 2);
  vec_t* b = make_vec_from_array(b_arr, 3);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_swap_rc(a, b));

  size_t a_size, b_size;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(a, &a_size));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(b, &b_size));
  TEST_ASSERT_EQUAL_UINT64(3, a_size);
  TEST_ASSERT_EQUAL_UINT64(2, b_size);

  assert_vec_contents(a, b_arr, 3);
  assert_vec_contents(b, a_arr, 2);

  vec_free_rc(a);
  vec_free_rc(b);
}

void test_vec_copy_rc(void) {
  double src_arr[3] = {9.0, 8.0, 7.0};
  vec_t* src = make_vec_from_array(src_arr, 3);
  vec_t* dest = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&dest, 3));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_copy_rc(src, dest));

  assert_vec_contents(dest, src_arr, 3);

  vec_free_rc(src);
  vec_free_rc(dest);
}

void test_vec_copy_rc_size_mismatch(void) {
  vec_t* src = make_vec_from_array((double[]){1.0, 2.0, 3.0}, 3);
  vec_t* dest = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&dest, 2));

  TEST_ASSERT_NOT_EQUAL(ERR_OK, vec_copy_rc(src, dest));

  vec_free_rc(src);
  vec_free_rc(dest);
}

void test_large_alloc_and_alignment(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, STRESS_SIZE));
  TEST_ASSERT_NOT_NULL(v);
  TEST_ASSERT_NOT_NULL(v->data);

  TEST_ASSERT_EQUAL_INT(0, ((uintptr_t)v->data) % ALIGNMENT);

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(v, 1.234));

  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val;
    TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(v, i, &val));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 1.234, val);
  }

  vec_free_rc(v);
}

void test_large_vector_operations(void) {
  vec_t* a = NULL;
  vec_t* b = NULL;
  vec_t* out = NULL;

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&a, STRESS_SIZE));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&b, STRESS_SIZE));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&out, STRESS_SIZE));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(a, 1.0));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(b, 2.0));

  double start = omp_get_wtime();
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_add_rc(a, b, out));
  double end = omp_get_wtime();

  printf("vec_add_rc for %d elements: %.6f s\n", STRESS_SIZE, end - start);

  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val;
    TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(out, i, &val));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, val);
  }

  double dot;
  start = omp_get_wtime();
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_dot_rc(a, b, &dot));
  end = omp_get_wtime();

  printf("vec_dot_rc for %d elements: %.6f s\n", STRESS_SIZE, end - start);
  TEST_ASSERT_DOUBLE_WITHIN(1e-6, (double)STRESS_SIZE * 2.0, dot);

  vec_free_rc(out);
  vec_free_rc(a);
  vec_free_rc(b);
}

void test_resize_large_vector(void) {
  vec_t* v = NULL;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&v, 10));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_resize_rc(&v, STRESS_SIZE));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(v, 42.0));

  for (size_t i = 0; i < STRESS_SIZE; i += STRESS_SIZE / 10) {
    double val;
    TEST_ASSERT_EQUAL_INT(ERR_OK, vec_get_rc(v, i, &val));
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 42.0, val);
  }

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_resize_rc(&v, 5));

  size_t n;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_size_rc(v, &n));
  TEST_ASSERT_EQUAL_UINT64(5, n);

  vec_free_rc(v);
}

void test_openmp_parallel_correctness(void) {
  const size_t test_size = 100000;
  vec_t* a = NULL;
  vec_t* b = NULL;

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&a, test_size));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_alloc_rc(&b, test_size));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(a, 1.0));
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_fill_rc(b, 2.0));

  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_add_inplace_rc(a, b));

  const double* data;
  TEST_ASSERT_EQUAL_INT(ERR_OK, vec_data_rc(a, &data));
  for (size_t i = 0; i < test_size; ++i) {
    TEST_ASSERT_DOUBLE_WITHIN(EPSILON, 3.0, data[i]);
  }

  vec_free_rc(a);
  vec_free_rc(b);
}

int main(void) {
  UNITY_BEGIN();

  // Lifecycle Management
  RUN_TEST(test_vec_alloc_rc_basic);
  RUN_TEST(test_vec_alloc_rc_edge_cases);
  RUN_TEST(test_vec_from_array_rc);
  RUN_TEST(test_vec_from_array_rc_edge_cases);
  RUN_TEST(test_vec_free_rc);
  RUN_TEST(test_vec_freep_rc);
  RUN_TEST(test_vec_resize_rc_expand);
  RUN_TEST(test_vec_resize_rc_shrink);
  RUN_TEST(test_vec_resize_rc_same_size);
  RUN_TEST(test_vec_resize_rc_edge_cases);

  // Data Access
  RUN_TEST(test_vec_set_get_rc);
  RUN_TEST(test_vec_set_get_rc_edge_cases);
  RUN_TEST(test_vec_size_rc);
  RUN_TEST(test_vec_data_rc);

  // Basic Arithmetic
  RUN_TEST(test_vec_add_rc);
  RUN_TEST(test_vec_add_inplace_rc);
  RUN_TEST(test_vec_add_rc_size_mismatch);
  RUN_TEST(test_vec_subtract_rc);
  RUN_TEST(test_vec_subtract_inplace_rc);
  RUN_TEST(test_vec_negate_rc);

  // Scalar Operations
  RUN_TEST(test_vec_scale_rc);
  RUN_TEST(test_vec_scale_inplace_rc);
  RUN_TEST(test_vec_axpy_rc);
  RUN_TEST(test_vec_multiply_rc);
  RUN_TEST(test_vec_map_rc);
  RUN_TEST(test_vec_map_rc_non_finite);
  RUN_TEST(test_vec_fill_rc);
  RUN_TEST(test_vec_fill_rc_non_finite);

  // Vector Products
  RUN_TEST(test_vec_dot_rc);
  RUN_TEST(test_vec_dot_rc_orthogonal);
  RUN_TEST(test_vec_cross_rc);
  RUN_TEST(test_vec_cross_rc_general);
  RUN_TEST(test_vec_cross_inplace_rc);
  RUN_TEST(test_vec_cross_rc_wrong_size);
  RUN_TEST(test_vec_len_rc);
  RUN_TEST(test_vec_len_rc_unit_vector);
  RUN_TEST(test_vec_normalize_inplace_rc);
  RUN_TEST(test_vec_normalize_inplace_rc_zero_vector);
  RUN_TEST(test_vec_angle_rc);
  RUN_TEST(test_vec_angle_rc_parallel);
  RUN_TEST(test_vec_angle_rc_opposite);
  RUN_TEST(test_vec_angle_rc_zero_vector);
  RUN_TEST(test_vec_project_rc);
  RUN_TEST(test_vec_project_rc_zero_vector);

  // Comparison
  RUN_TEST(test_vec_is_equal_rc_true);
  RUN_TEST(test_vec_is_equal_rc_false);
  RUN_TEST(test_vec_dist_rc);
  RUN_TEST(test_vec_dist_sq_rc);

  // Utility
  RUN_TEST(test_vec_min_rc);
  RUN_TEST(test_vec_max_rc);
  RUN_TEST(test_vec_sum_rc);
  RUN_TEST(test_vec_swap_rc);
  RUN_TEST(test_vec_copy_rc);
  RUN_TEST(test_vec_copy_rc_size_mismatch);

  // Stress Tests
  RUN_TEST(test_large_alloc_and_alignment);
  RUN_TEST(test_large_vector_operations);
  RUN_TEST(test_resize_large_vector);
  RUN_TEST(test_openmp_parallel_correctness);

  return UNITY_END();
}