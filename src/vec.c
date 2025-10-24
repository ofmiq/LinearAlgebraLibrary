#include "vec.h"

#include <stdint.h>
#include <math.h>
#include <stdio.h>

/*
  TODO: FOR ALL FUNCTION ADD ERROR HANDLING
*/

vec_t* vec_alloc(size_t n) {
  if (n == 0) {
    return NULL;
  }

  if (n > SIZE_MAX) {
    return NULL;
  }

  vec_t* v = (vec_t*)malloc(sizeof(vec_t));
  if (v == NULL) {
    return NULL;
  }

  v->data = (double*)malloc(sizeof(double) * n);
  if (v->data == NULL) {
    vec_free(v);
    return NULL;
  }

  v->n = n;

  return v;
}

void vec_free(vec_t* v) {
  if (v == NULL) {
    return;
  }

  if (v->data != NULL) {
    free(v->data);
  }

  free(v);
}

void vec_set(vec_t* v, size_t i, double val) {
  if (v == NULL || i >= v->n || v->data == NULL) {
    return;
  }

  v->data[i] = val;
}

double vec_get(const vec_t* v, size_t i) {
  if (v == NULL || i >= v->n || v->data == NULL) {
    return NAN;
  }
  return v->data[i];
}

void vec_add(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return;
  }

  if (a->n != b->n || a->n != out->n) {
    return;
  }

  size_t length_of_vectors = a->n;
  for (size_t i = 0; i < length_of_vectors; ++i) {
    out->data[i] = a->data[i] + b->data[i];
  }
}

void vec_subtract(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return;
  }

  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return;
  }

  if (a->n != b->n || a->n != out->n) {
    return;
  }

  size_t length_of_vectors = a->n;
  for (size_t i = 0; i < length_of_vectors; ++i) {
    out->data[i] = a->data[i] - b->data[i];
  }
}

void vec_scale(const vec_t* a, vec_t* out, double scalar) {
  if (a == NULL || out == NULL) {
    return;
  }

  if (a->data == NULL || out->data == NULL) {
    return;
  }

  if (a->n != out->n) {
    return;
  }

  size_t length_of_vector = a->n;
  for (size_t i = 0; i < length_of_vector; ++i) {
    out->data[i] = a->data[i] * scalar;
  }
}

double vec_dot(const vec_t* a, const vec_t* b) {
  if (a == NULL || b == NULL || a->data == NULL || b->data == NULL) {
    return NAN;
  }

  if (a->n != b->n) {
    return NAN;
  }

  size_t length_of_vectors = a->n;
  double result = 0.0;

  for (size_t i = 0; i < length_of_vectors; ++i) {
    result += a->data[i] * b->data[i];
  }

  return result;
}

void vec_print(const vec_t* v) {
  if (v == NULL || v->data == NULL) {
    printf("(NULL vector)\n");
    return;
  }

  const size_t length_of_vector = v->n;

  printf("(");

  for (size_t i = 0; i < length_of_vector; ++i) {
    printf("%.6lf", v->data[i]);
    if (i < length_of_vector - 1) {
      printf(", ");
    }
  }

  printf(")\n");
}