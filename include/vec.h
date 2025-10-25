#ifndef VEC_H
#define VEC_H

#include <stdlib.h>

#include "util.h"

/**
 * @brief Structure to represent a vector.
 * @param n Length of the vector.
 * @param data Pointer to the array of data.
 */
typedef struct vec_t {
  size_t n;
  double* data;
} vec_t;

/**
 * @brief Allocates memory for a vector of length n.
 * @param n Length of the vector.
 * @return Pointer to the newly allocated vector, or NULL on failure.
 */
util_error_t vec_alloc(vec_t** out, size_t n);

/**
 * @brief Deallocates the memory occupied by the vector.
 * @param v Pointer to the vector to be freed.
 */
util_error_t vec_free(vec_t* v);

/**
 * @brief Sets the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @param val New value for the element.
 */
util_error_t vec_set(vec_t* v, size_t i, double val);

/**
 * @brief Returns the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @return The value of the element.
 */
util_error_t vec_get(const vec_t* v, size_t i, double* out_val);

/**
 * @brief Adds two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to the vector where the sum will be stored. Out cannot be
 * the same as a or b.
 */
util_error_t vec_add(const vec_t* a, const vec_t* b, vec_t* out);

/**
 * @brief Subtracts two vectors.
 * @param a Pointer to the vector to be subtracted from.
 * @param b Pointer to the vector to subtract.
 * @param out Pointer to the vector where the difference will be stored. Out
 * cannot be the same as a or b.
 */
util_error_t vec_subtract(const vec_t* a, const vec_t* b, vec_t* out);

/**
 * @brief Scales a vector by a scalar.
 * @param a Pointer to the vector that will be scaled.
 * @param out Pointer to the vector where the scalar multiplication will be
 * stored. Out cannot be the same as a or b.
 * @param scalar The scalar multiplier value.
 */
util_error_t vec_scale(const vec_t* a, vec_t* out, double scalar);

/**
 * @brief Computes the dot product of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return The value of the dot product.
 */
util_error_t vec_dot(const vec_t* a, const vec_t* b, double* out_result);

/**
 * @brief Prints the vector elements in the format (v0, v1, ..., vn).
 * @param v Pointer to the vector to be printed.
 */
util_error_t vec_print(const vec_t* v);

#endif