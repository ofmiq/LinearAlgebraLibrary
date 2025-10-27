#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "vec.h"

static void expect_rc(util_error_t rc, util_error_t want, const char* msg) {
  if (rc != want) {
    fprintf(stderr, "FAIL: %s — expected %d (%s), got %d (%s)\n", msg,
            (int)want, util_error_str(want), (int)rc, util_error_str(rc));
    exit(EXIT_FAILURE);
  }
}

static void expect_double_eq(double a, double b, double eps, const char* msg) {
  if (fabs(a - b) > eps) {
    fprintf(stderr, "FAIL: %s — expected %.12g, got %.12g\n", msg, b, a);
    exit(EXIT_FAILURE);
  }
}

static void test_alloc_and_free(void) {
  printf("TEST: alloc and free\n");

  vec_t* v = NULL;
  util_error_t rc;

  rc = vec_alloc(&v, 3);
  expect_rc(rc, ERR_OK, "vec_alloc(3) should succeed");
  assert(v != NULL && v->data != NULL && v->n == 3);

  rc = vec_free(v);
  expect_rc(rc, ERR_OK, "vec_free(valid) should return ERR_OK");

  rc = vec_free(NULL);
  expect_rc(rc, ERR_OK, "vec_free(NULL) should return ERR_OK");

  v = NULL;
  rc = vec_alloc(&v, 0);
  expect_rc(rc, ERR_RANGE, "vec_alloc(0) should return ERR_RANGE");
  assert(v == NULL);

#ifdef VECTOR_MAX_ELEMENTS
  rc = vec_alloc(&v, (size_t)VECTOR_MAX_ELEMENTS + 1);
  expect_rc(rc, ERR_RANGE, "vec_alloc(VECTOR_MAX_ELEMENTS+1) -> ERR_RANGE");
  assert(v == NULL);
#endif

  printf(" OK\n\n");
}

static void test_set_get(void) {
  printf("TEST: set/get\n");

  vec_t* v = NULL;
  util_error_t rc = vec_alloc(&v, 4);
  expect_rc(rc, ERR_OK, "vec_alloc(4) for set/get");
  assert(v && v->data);

  rc = vec_set(v, 0, 1.5);
  expect_rc(rc, ERR_OK, "vec_set(0)");
  rc = vec_set(v, 3, -2.25);
  expect_rc(rc, ERR_OK, "vec_set(3)");

  double out;
  rc = vec_get(v, 0, &out);
  expect_rc(rc, ERR_OK, "vec_get(0)");
  expect_double_eq(out, 1.5, 1e-12, "vec_get(0) value");

  rc = vec_get(v, 3, &out);
  expect_rc(rc, ERR_OK, "vec_get(3)");
  expect_double_eq(out, -2.25, 1e-12, "vec_get(3) value");

  rc = vec_set(NULL, 0, 1.0);
  expect_rc(rc, ERR_NULL, "vec_set(NULL) should be ERR_NULL");

  rc = vec_get(NULL, 0, &out);
  expect_rc(rc, ERR_NULL, "vec_get(NULL, ...) should be ERR_NULL");

  rc = vec_get(v, 99, &out);
  expect_rc(rc, ERR_RANGE, "vec_get(out-of-range) should be ERR_RANGE");

  rc = vec_get(v, 1, NULL);
  expect_rc(rc, ERR_NULL, "vec_get(..., NULL out_val) should be ERR_NULL");

  vec_free(v);
  printf(" OK\n\n");
}

static void test_add_subtract(void) {
  printf("TEST: add/subtract\n");

  vec_t *a = NULL, *b = NULL, *out = NULL;
  util_error_t rc;

  rc = vec_alloc(&a, 3);
  expect_rc(rc, ERR_OK, "alloc a");
  rc = vec_alloc(&b, 3);
  expect_rc(rc, ERR_OK, "alloc b");
  rc = vec_alloc(&out, 3);
  expect_rc(rc, ERR_OK, "alloc out");

  for (size_t i = 0; i < 3; ++i) {
    rc = vec_set(a, i, (double)(i + 1));
    expect_rc(rc, ERR_OK, "set a");
    rc = vec_set(b, i, (double)(i + 1) * 10.0);
    expect_rc(rc, ERR_OK, "set b");
  }

  rc = vec_add(a, b, out);
  expect_rc(rc, ERR_OK, "vec_add valid");
  double x;
  vec_get(out, 0, &x);
  expect_double_eq(x, 11.0, 1e-12, "add[0]");
  vec_get(out, 1, &x);
  expect_double_eq(x, 22.0, 1e-12, "add[1]");
  vec_get(out, 2, &x);
  expect_double_eq(x, 33.0, 1e-12, "add[2]");

  rc = vec_subtract(b, a, out);
  expect_rc(rc, ERR_OK, "vec_subtract valid");
  vec_get(out, 0, &x);
  expect_double_eq(x, 9.0, 1e-12, "sub[0]");
  vec_get(out, 1, &x);
  expect_double_eq(x, 18.0, 1e-12, "sub[1]");
  vec_get(out, 2, &x);
  expect_double_eq(x, 27.0, 1e-12, "sub[2]");

  vec_t* small = NULL;
  rc = vec_alloc(&small, 2);
  expect_rc(rc, ERR_OK, "alloc small");
  rc = vec_add(a, small, out);
  expect_rc(rc, ERR_DIM, "vec_add with dim mismatch should be ERR_DIM");

  rc = vec_add(NULL, b, out);
  expect_rc(rc, ERR_NULL, "vec_add(NULL,...) should be ERR_NULL");

  vec_free(a);
  vec_free(b);
  vec_free(out);
  vec_free(small);

  printf(" OK\n\n");
}

static void test_scale_dot(void) {
  printf("TEST: scale/dot\n");

  vec_t *a = NULL, *out = NULL;
  util_error_t rc = vec_alloc(&a, 3);
  expect_rc(rc, ERR_OK, "alloc a");
  rc = vec_alloc(&out, 3);
  expect_rc(rc, ERR_OK, "alloc out");

  vec_set(a, 0, 1.0);
  vec_set(a, 1, 2.0);
  vec_set(a, 2, 3.0);

  rc = vec_scale(a, out, 2.0);
  expect_rc(rc, ERR_OK, "vec_scale valid");
  double v;
  vec_get(out, 0, &v);
  expect_double_eq(v, 2.0, 1e-12, "scale[0]");
  vec_get(out, 1, &v);
  expect_double_eq(v, 4.0, 1e-12, "scale[1]");
  vec_get(out, 2, &v);
  expect_double_eq(v, 6.0, 1e-12, "scale[2]");

  double dot;
  rc = vec_dot(a, out, &dot);
  expect_rc(rc, ERR_OK, "vec_dot valid");
  expect_double_eq(dot, 28.0, 1e-12, "dot value");

  rc = vec_dot(a, out, NULL);
  expect_rc(rc, ERR_NULL, "vec_dot(NULL out_result) should be ERR_NULL");

  vec_t* short_v = NULL;
  rc = vec_alloc(&short_v, 2);
  expect_rc(rc, ERR_OK, "alloc short_v");
  rc = vec_dot(a, short_v, &dot);
  expect_rc(rc, ERR_DIM, "vec_dot dim mismatch should be ERR_DIM");

  vec_free(a);
  vec_free(out);
  vec_free(short_v);

  printf(" OK\n\n");
}

static void test_print_behavior(void) {
  printf("TEST: print\n");

  vec_t* v = NULL;
  util_error_t rc = vec_alloc(&v, 3);
  expect_rc(rc, ERR_OK, "alloc for print");
  vec_set(v, 0, 1.1);
  vec_set(v, 1, 2.2);
  vec_set(v, 2, 3.3);

  printf("Expected: (1.100000, 2.200000, 3.300000)\n");
  rc = vec_print(v);
  expect_rc(rc, ERR_OK, "vec_print valid");

  rc = vec_print(NULL);
  expect_rc(rc, ERR_NULL, "vec_print(NULL) should be ERR_NULL");

  vec_t tmp = {.n = 3, .data = NULL};
  rc = vec_print(&tmp);
  expect_rc(rc, ERR_NULL, "vec_print(invalid data) should be ERR_NULL");

  vec_free(v);

  printf(" OK\n\n");
}

static void test_from_array_and_deepcopy(void) {
  printf("TEST: vec_from_array (deep copy)\n");

  double src[] = {4.4, 5.5, 6.6};
  vec_t* v = NULL;
  util_error_t rc = vec_from_array(src, &v, 3);
  expect_rc(rc, ERR_OK, "vec_from_array valid");
  assert(v && v->data && v->n == 3);

  double x;
  rc = vec_get(v, 0, &x);
  expect_rc(rc, ERR_OK, "vec_get(v,0)");
  expect_double_eq(x, 4.4, 1e-12, "from_array[0]");
  rc = vec_get(v, 1, &x);
  expect_rc(rc, ERR_OK, "vec_get(v,1)");
  expect_double_eq(x, 5.5, 1e-12, "from_array[1]");
  rc = vec_get(v, 2, &x);
  expect_rc(rc, ERR_OK, "vec_get(v,2)");
  expect_double_eq(x, 6.6, 1e-12, "from_array[2]");

  src[0] = 0.0;
  rc = vec_get(v, 0, &x);
  expect_rc(rc, ERR_OK, "vec_get after src modify");
  expect_double_eq(x, 4.4, 1e-12, "deep copy check");

  rc = vec_from_array(NULL, &v, 3);
  expect_rc(rc, ERR_NULL, "vec_from_array(NULL,data)");
  vec_t* tmp = NULL;
  rc = vec_from_array(src, &tmp, 0);
  expect_rc(rc, ERR_RANGE, "vec_from_array(0) -> ERR_RANGE");
  expect_rc(rc, ERR_RANGE, "vec_from_array(0) should be ERR_RANGE");

  vec_free(v);

  printf(" OK\n\n");
}

static void test_freep_behavior(void) {
  printf("TEST: vec_freep (free + null)\n");

  vec_t* v = NULL;
  util_error_t rc = vec_alloc(&v, 2);
  expect_rc(rc, ERR_OK, "alloc for freep");
  assert(v != NULL);
  rc = vec_freep(&v);
  expect_rc(rc, ERR_OK, "vec_freep should return ERR_OK");
  assert(v == NULL);

  rc = vec_freep(NULL);
  expect_rc(rc, ERR_NULL, "vec_freep(NULL) should be ERR_NULL");

  vec_t* nullv = NULL;
  rc = vec_freep(&nullv);
  expect_rc(rc, ERR_OK, "vec_freep(&NULL) should be ERR_OK");

  printf(" OK\n\n");
}

static void test_aliasing_protection(void) {
  printf("TEST: aliasing protections (out == a or out == b)\n");

  vec_t *a = NULL, *b = NULL, *out = NULL;
  util_error_t rc;

  rc = vec_alloc(&a, 3);
  expect_rc(rc, ERR_OK, "alloc a");
  rc = vec_alloc(&b, 3);
  expect_rc(rc, ERR_OK, "alloc b");
  rc = vec_alloc(&out, 3);
  expect_rc(rc, ERR_OK, "alloc out");

  for (size_t i = 0; i < 3; ++i) {
    vec_set(a, i, (double)(i + 1));
    vec_set(b, i, (double)(i + 1) * 2.0);
  }

  rc = vec_add(a, b, a);
  expect_rc(rc, ERR_INVALID_ARG, "vec_add out==a should be ERR_INVALID_ARG");

  rc = vec_subtract(a, b, b);
  expect_rc(rc, ERR_INVALID_ARG,
            "vec_subtract out==b should be ERR_INVALID_ARG");

  rc = vec_scale(a, a, 3.0);
  expect_rc(rc, ERR_INVALID_ARG, "vec_scale out==a should be ERR_INVALID_ARG");

  vec_free(a);
  vec_free(b);
  vec_free(out);

  printf(" OK\n\n");
}

static void test_overflow_allocation(void) {
  printf("TEST: overflow allocation\n");

  vec_t* v = NULL;
  size_t n_overflow = SIZE_MAX / sizeof(double) + 1u;

  util_error_t rc = vec_alloc(&v, n_overflow);
  expect_rc(rc, ERR_RANGE,
            "vec_alloc(SIZE_MAX/sizeof(double)+1) should return ERR_RANGE");
  assert(v == NULL);

  printf(" OK\n\n");
}

int main() {
  printf("Running vectors_test...\n\n");

  test_alloc_and_free();
  test_set_get();
  test_add_subtract();
  test_scale_dot();
  test_print_behavior();

  test_from_array_and_deepcopy();
  test_freep_behavior();
  test_aliasing_protection();

  test_overflow_allocation();

  printf("All tests passed.\n");
  return EXIT_SUCCESS;
}
