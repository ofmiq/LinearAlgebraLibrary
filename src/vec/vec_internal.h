#ifndef VEC_INTERNAL_H
#define VEC_INTERNAL_H

#include "config.h"
#include "vec_rc.h"

/* ==================== Preconditions ==================== */

/**
 * @brief Require pointer to be non-NULL.
 * @param p Pointer to check.
 * @return Returns ERR_NULL if p is NULL.
 */
#define VEC_REQUIRE_NON_NULL(p) \
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
#define VEC_REQUIRE_NON_NULL_2(a, b)   \
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
#define VEC_REQUIRE_NON_NULL_3(a, b, c)               \
  do {                                               \
    if ((a) == NULL || (b) == NULL || (c) == NULL) { \
      return ERR_NULL;                               \
    }                                                \
  } while (0)

/**
 * @brief Require a valid vector object.
 * Checks that the vector pointer and its data buffer are non-NULL.
 * @param v Vector to validate.
 * @return Returns ERR_NULL if vector or its data is NULL.
 */
#define VEC_REQUIRE_VALID_VEC(v)            \
  do {                                      \
    if ((v) == NULL || (v)->data == NULL) { \
      return ERR_NULL;                      \
    }                                       \
  } while (0)

/**
 * @brief Require two valid vector objects.
 * @param a First vector.
 * @param b Second vector.
 * @return Returns ERR_NULL if any vector data buffer is NULL.
 */
#define VEC_REQUIRE_VALID_VEC_2(a, b)             \
  do {                                            \
    if ((a)->data == NULL || (b)->data == NULL) { \
      return ERR_NULL;                            \
    }                                             \
  } while (0)

/**
 * @brief Require three valid vector objects.
 * @param a First vector.
 * @param b Second vector.
 * @param c Third vector.
 * @return Returns ERR_NULL if any vector data buffer is NULL.
 */
#define VEC_REQUIRE_VALID_VEC_3(a, b, c)                               \
  do {                                                                 \
    if ((a)->data == NULL || (b)->data == NULL || (c)->data == NULL) { \
      return ERR_NULL;                                                 \
    }                                                                  \
  } while (0)

/* ==================== Size requirements ==================== */

/**
 * @brief Require two vectors to have the same size.
 * @param a First vector.
 * @param b Second vector.
 * @return ERR_OK if sizes match, ERR_DIM otherwise.
 */
static inline util_error_t vec_require_same_size(const vec_t* a,
                                                 const vec_t* b) {
  return (a->n == b->n) ? ERR_OK : ERR_DIM;
}

/**
 * @brief Require vector size to be equal to a specific value.
 * @param v Vector to check.
 * @param n Expected size.
 * @return ERR_OK if size matches, ERR_DIM otherwise.
 */
static inline util_error_t vec_require_size(const vec_t* v, size_t n) {
  return (v->n == n) ? ERR_OK : ERR_DIM;
}

#endif /* VEC_INTERNAL_H */
