#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "vec.h"

static void test_alloc_free(void) {
  printf("=== test_alloc_free ===\n");
  vec_t* v = vec_alloc(3);
  assert(v != NULL);
  assert(v->data != NULL);
  assert(v->n == 3);
  vec_free(v);

  vec_t* v0 = vec_alloc(0);
  assert(v0 == NULL);
  printf("OK\n\n");
}

static void test_set_get(void) {
  printf("=== test_set_get ===\n");
  vec_t* v = vec_alloc(3);
  for (size_t i = 0; i < 3; ++i) vec_set(v, i, i + 1.0);
  assert(fabs(vec_get(v, 0) - 1.0) < 1e-9);
  assert(fabs(vec_get(v, 1) - 2.0) < 1e-9);
  assert(fabs(vec_get(v, 2) - 3.0) < 1e-9);
  assert(isnan(vec_get(v, 99)));

  vec_free(v);
  printf("OK\n\n");
}

static void test_add_subtract(void) {
  printf("=== test_add_subtract ===\n");
  vec_t* a = vec_alloc(3);
  vec_t* b = vec_alloc(3);
  vec_t* out = vec_alloc(3);

  for (size_t i = 0; i < 3; ++i) {
    vec_set(a, i, i + 1);
    vec_set(b, i, (i + 1) * 10);
  }

  vec_add(a, b, out);
  assert(fabs(vec_get(out, 0) - 11) < 1e-9);
  assert(fabs(vec_get(out, 1) - 22) < 1e-9);
  assert(fabs(vec_get(out, 2) - 33) < 1e-9);

  vec_subtract(b, a, out);
  assert(fabs(vec_get(out, 0) - 9) < 1e-9);
  assert(fabs(vec_get(out, 1) - 18) < 1e-9);
  assert(fabs(vec_get(out, 2) - 27) < 1e-9);

  vec_free(a);
  vec_free(b);
  vec_free(out);
  printf("OK\n\n");
}

static void test_scale_dot(void) {
  printf("=== test_scale_dot ===\n");
  vec_t* a = vec_alloc(3);
  vec_t* out = vec_alloc(3);

  vec_set(a, 0, 1.0);
  vec_set(a, 1, 2.0);
  vec_set(a, 2, 3.0);

  vec_scale(a, out, 2.0);
  assert(fabs(vec_get(out, 0) - 2.0) < 1e-9);
  assert(fabs(vec_get(out, 1) - 4.0) < 1e-9);
  assert(fabs(vec_get(out, 2) - 6.0) < 1e-9);

  double dot = vec_dot(a, out);
  assert(fabs(dot - 28.0) < 1e-9);

  vec_t* wrong = vec_alloc(2);
  assert(isnan(vec_dot(a, wrong)));
  vec_free(wrong);

  vec_free(a);
  vec_free(out);
  printf("OK\n\n");
}

static void test_print(void) {
  printf("=== test_print ===\n");
  vec_t* v = vec_alloc(3);
  vec_set(v, 0, 1.1);
  vec_set(v, 1, 2.2);
  vec_set(v, 2, 3.3);

  printf("Expected output: (1.100000, 2.200000, 3.300000)\n");
  printf("Actual output:   ");
  vec_print(v);

  vec_free(v);
  printf("OK\n\n");
}

static void stress_test() {
  for (int i = 0; i < 100000; ++i) {
    vec_t* v = vec_alloc(1000);
    vec_free(v);
  }
  printf("Stress test passed.\n");
}

int main(void) {
  printf("Running vector tests...\n\n");
  test_alloc_free();
  test_set_get();
  test_add_subtract();
  test_scale_dot();
  test_print();
  stress_test();
  printf("All tests passed.\n");
  return 0;
}
