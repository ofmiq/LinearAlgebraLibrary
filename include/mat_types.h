#ifndef MAT_TYPES_H
#define MAT_TYPES_H

#include <stddef.h>

// Macro for accessing a matrix element
#define MAT_AT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

/**
 * @brief Structure to represent a matrix.
 */
typedef struct mat_t {
  /** @brief Current number of rows. */
  size_t rows;
  /** @brief Current number of columns. */
  size_t cols;
  /** @brief Pointer to the dynamicly allocated array of double data (Row-major
   * order). */
  double* data;
} mat_t;

#endif  // MAT_TYPES_H