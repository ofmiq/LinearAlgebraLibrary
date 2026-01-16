#ifndef MAT_INTERNAL_H
#define MAT_INTERNAL_H

#include "config.h"
#include "mat_rc.h"
#include "vec.h"

/* ==================== Preconditions ==================== */

/**
 * @brief Require pointer to be non-NULL.
 * @param p Pointer to check.
 * @return Returns ERR_NULL if p is NULL.
 */
#define MAT_REQUIRE_NON_NULL(p) \
  do {                         \
    if ((p) == NULL) {         \
      return ERR_NULL;         \
    }                          \
  } while (0)

/**
 * @brief Require two pointers to be non-NULL.
 * @param a First pointer.
 * @param b Second pointer.
 * @return Returns ERR_NULL if any pointer is NULL.
 */
#define MAT_REQUIRE_NON_NULL_2(a, b)   \
  do {                                \
    if ((a) == NULL || (b) == NULL) { \
      return ERR_NULL;                \
    }                                 \
  } while (0)

/**
 * @brief Require three pointers to be non-NULL.
 * @param a First pointer.
 * @param b Second pointer.
 * @param c Third pointer.
 * @return Returns ERR_NULL if any pointer is NULL.
 */
#define MAT_REQUIRE_NON_NULL_3(a, b, c)               \
  do {                                               \
    if ((a) == NULL || (b) == NULL || (c) == NULL) { \
      return ERR_NULL;                               \
    }                                                \
  } while (0)

/**
 * @brief Require a valid matrix object.
 * Checks that the matrix pointer and its data buffer are non-NULL.
 * @param m Matrix to validate.
 * @return Returns ERR_NULL if matrix or its data is NULL.
 */
#define MAT_REQUIRE_VALID_MAT(m)            \
  do {                                      \
    if ((m) == NULL || (m)->data == NULL) { \
      return ERR_NULL;                      \
    }                                       \
  } while (0)

/**
 * @brief Require two valid matrix objects.
 * @param a First matrix.
 * @param b Second matrix.
 * @return Returns ERR_NULL if any matrix data buffer is NULL.
 */
#define MAT_REQUIRE_VALID_MAT_2(a, b)             \
  do {                                            \
    if ((a)->data == NULL || (b)->data == NULL) { \
      return ERR_NULL;                            \
    }                                             \
  } while (0)

/**
 * @brief Require three valid matrix objects.
 * @param a First matrix.
 * @param b Second matrix.
 * @param c Third matrix.
 * @return Returns ERR_NULL if any matrix data buffer is NULL.
 */
#define MAT_REQUIRE_VALID_MAT_3(a, b, c)                               \
  do {                                                                 \
    if ((a)->data == NULL || (b)->data == NULL || (c)->data == NULL) { \
      return ERR_NULL;                                                 \
    }                                                                  \
  } while (0)

/* ==================== Dimension requirements ==================== */

/**
 * @brief Require matrices to have identical shape.
 * @param a First matrix.
 * @param b Second matrix.
 * @return ERR_OK if dimensions match, ERR_DIM otherwise.
 */
static inline util_error_t mat_same_shape(const mat_t* a,
                                                  const mat_t* b) {
  return (a->rows == b->rows && a->cols == b->cols) ? ERR_OK : ERR_DIM;
}

/**
 * @brief Require matrix to be square.
 * @param m Matrix to check.
 * @return ERR_OK if matrix is square, ERR_DIM otherwise.
 */
static inline util_error_t mat_require_square(const mat_t* m) {
  return (m->rows == m->cols) ? ERR_OK : ERR_DIM;
}

#endif /* MAT_INTERNAL_H */
