#ifndef VEC_TYPES_H
#define VEC_TYPES_H

#include <stdlib.h>

/**
 * @brief Structure to represent a vector.
 */
typedef struct vec_t {
  /** @brief Current length (dimension) of the vector. */
  size_t n;
  /** @brief Pointer to the dynamically allocated array of double data. */
  double* data;
} vec_t;

#endif