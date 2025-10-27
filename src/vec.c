#include "vec.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

util_error_t vec_alloc(vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = malloc(sizeof *v);
  if (v == NULL) {
    return ERR_ALLOC;
  }

  v->n = 0;
  v->data = NULL;

  if (n > SIZE_MAX / sizeof *v->data) {
    free(v);
    return ERR_RANGE;
  }

  v->data = malloc(sizeof *v->data * n);
  if (v->data == NULL) {
    free(v);
    return ERR_ALLOC;
  }

  v->n = n;
  *out = v;

  return ERR_OK;
}

util_error_t vec_free(vec_t* v) {
  if (!v) {
    return ERR_OK;
  }

  free(v->data);
  free(v);

  return ERR_OK;
}

util_error_t vec_freep(vec_t** vp) {
  if (vp == NULL) {
    return ERR_NULL;
  }
  if (*vp == NULL) {
    return ERR_OK;
  }

  free((*vp)->data);
  free(*vp);
  *vp = NULL;

  return ERR_OK;
}

util_error_t vec_from_array(const double* data, vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (data == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = malloc(sizeof *v);
  if (v == NULL) {
    return ERR_ALLOC;
  }

  v->n = 0;
  v->data = NULL;

  if (n > SIZE_MAX / sizeof *v->data) {
    free(v);
    return ERR_RANGE;
  }

  v->data = malloc(sizeof *v->data * n);
  if (v->data == NULL) {
    free(v);
    return ERR_ALLOC;
  }

  memcpy(v->data, data, sizeof *v->data * n);

  v->n = n;
  *out = v;

  return ERR_OK;
}

util_error_t vec_set(vec_t* v, size_t i, double val) {
  if (v == NULL || v->data == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  v->data[i] = val;
  return ERR_OK;
}

util_error_t vec_get(const vec_t* v, size_t i, double* out_val) {
  if (v == NULL || v->data == NULL || out_val == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  *out_val = v->data[i];
  return ERR_OK;
}

util_error_t vec_add(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }
  if (out == a || out == b) {
    return ERR_INVALID_ARG;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] + b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }
  if (out == a || out == b) {
    return ERR_INVALID_ARG;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] - b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_scale(const vec_t* a, vec_t* out, double scalar) {
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != out->n) {
    return ERR_DIM;
  }
  if (out == a) {
    return ERR_INVALID_ARG;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t vec_dot(const vec_t* a, const vec_t* b, double* out_result) {
  if (a == NULL || b == NULL || out_result == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n) {
    return ERR_DIM;
  }

  double result = 0.0;
  for (size_t i = 0; i < a->n; ++i) {
    result += a->data[i] * b->data[i];
  }

  *out_result = result;
  return ERR_OK;
}

util_error_t vec_print(const vec_t* v) {
  if (v == NULL) {
    printf("(NULL vector)\n");
    return ERR_NULL;
  }
  if (v->data == NULL) {
    printf("(Invalid data pointer)\n");
    return ERR_NULL;
  }
  if (v->n == 0) {
    printf("(empty)\n");
    return ERR_OK;
  }

  printf("(");
  for (size_t i = 0; i < v->n; ++i) {
    printf("%.6lf", v->data[i]);
    if (i < v->n - 1) printf(", ");
  }
  printf(")\n");

  return ERR_OK;
}
