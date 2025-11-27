#include "vec_rc.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

util_error_t vec_alloc_rc(vec_t** out, size_t n) {
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

void vec_free_rc(vec_t* v) {
  if (!v) {
    return;
  }

  free(v->data);
  free(v);
}

void vec_freep_rc(vec_t** vp) {
  if (vp == NULL || *vp == NULL) {
    return;
  }

  free((*vp)->data);
  free(*vp);
  *vp = NULL;
}

util_error_t vec_from_array_rc(const double* data, vec_t** out, size_t n) {
  if (out == NULL) {
    return ERR_NULL;
  }

  if (data == NULL) {
    return ERR_NULL;
  }

  if (n == 0 || n > VECTOR_MAX_ELEMENTS) {
    return ERR_RANGE;
  }

  vec_t* v = NULL;
  util_error_t err = vec_alloc_rc(&v, n);
  if (err != ERR_OK) {
    return err;
  }

  memcpy(v->data, data, sizeof(double) * n);
  *out = v;
  return ERR_OK;
}

util_error_t vec_set_rc(vec_t* v, size_t i, double val) {
  if (v == NULL || v->data == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  v->data[i] = val;
  return ERR_OK;
}

util_error_t vec_get_rc(const vec_t* v, size_t i, double* out) {
  if (v == NULL || v->data == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (i >= v->n) {
    return ERR_RANGE;
  }

  *out = v->data[i];
  return ERR_OK;
}

util_error_t vec_add_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] + b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_subtract_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || b->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != b->n || a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] - b->data[i];
  }

  return ERR_OK;
}

util_error_t vec_scale_rc(const vec_t* a, vec_t* out, double scalar) {
  if (a == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (a->data == NULL || out->data == NULL) {
    return ERR_NULL;
  }
  if (a->n != out->n) {
    return ERR_DIM;
  }

  for (size_t i = 0; i < a->n; ++i) {
    out->data[i] = a->data[i] * scalar;
  }

  return ERR_OK;
}

util_error_t vec_dot_rc(const vec_t* a, const vec_t* b, double* out) {
  if (a == NULL || b == NULL || out == NULL) {
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

  *out = result;
  return ERR_OK;
}

util_error_t vec_cross_rc(const vec_t* a, const vec_t* b, vec_t* out) {
  if (a == NULL || b == NULL || out == NULL) return ERR_NULL;
  if (a->data == NULL || b->data == NULL || out->data == NULL) return ERR_NULL;

  if (a->n != 3 || b->n != 3 || out->n != 3) return ERR_DIM;

  double x = a->data[1] * b->data[2] - a->data[2] * b->data[1];
  double y = a->data[2] * b->data[0] - a->data[0] * b->data[2];
  double z = a->data[0] * b->data[1] - a->data[1] * b->data[0];

  out->data[0] = x;
  out->data[1] = y;
  out->data[2] = z;

  return ERR_OK;
}

util_error_t vec_len_rc(const vec_t* v, double* out) {
  if (v == NULL || out == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }

  double dot_product = 0.0;

  util_error_t err = vec_dot_rc(v, v, &dot_product);
  if (err != ERR_OK) {
    return err;
  }

  *out = sqrt(dot_product);

  return ERR_OK;
}

util_error_t vec_copy_rc(const vec_t* src, vec_t* dest) {
  if (src == NULL || dest == NULL) {
    return ERR_NULL;
  }
  if (src->data == NULL || dest->data == NULL) {
    return ERR_NULL;
  }
  if (src->n != dest->n) {
    return ERR_DIM;
  }

  memcpy(dest->data, src->data, src->n * sizeof(double));
  return ERR_OK;
}

bool vec_is_equal_rc(const vec_t* a, const vec_t* b, double epsilon) {
  if (a == NULL || b == NULL) {
    return false;
  }
  if (a->data == NULL || b->data == NULL) {
    return false;
  }
  if (a->n != b->n) {
    return false;
  }

  for (size_t i = 0; i < a->n; ++i) {
    if (fabs(a->data[i] - b->data[i]) > epsilon) {
      return false;
    }
  }
  
  return true;
}

util_error_t vec_print_rc(const vec_t* v) {
  if (v == NULL) {
    return ERR_NULL;
  }
  if (v->data == NULL) {
    return ERR_NULL;
  }
  if (v->n == 0) {
    return ERR_OK;
  }

  printf("(");
  for (size_t i = 0; i < v->n; ++i) {
    printf("%g", v->data[i]);
    if (i < v->n - 1) printf(", ");
  }
  printf(")\n");

  return ERR_OK;
}
