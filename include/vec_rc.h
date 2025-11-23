#ifndef VEC_RC_H
#define VEC_RC_H

#include <stdbool.h>
#include "vec_types.h"

#include "util.h"

/**
 * @brief Allocates memory for a vector of length n.
 * @param out Double pointer where the newly allocated vector will be stored.
 * @param n Length (dimension) of the vector to allocate.
 * @return ERR_OK on success, or an error code. On error, *out is left
 * unchanged.
 */
util_error_t vec_alloc_rc(vec_t** out, size_t n);

/**
 * @brief Creates a new vector by copying data from a C array.
 * @param data Pointer to the C array of doubles.
 * @param out Double pointer where the newly allocated vector will be stored.
 * @param n Number of elements to copy.
 * @return ERR_OK on success, or an error code. On error, *out is left
 * unchanged.
 */
util_error_t vec_from_array_rc(const double* data, vec_t** out, size_t n);

/**
 * @brief Deallocates the memory occupied by the vector.
 * @param v Pointer to the vector to be freed.
 */
void vec_free_rc(vec_t* v);

/**
 * @brief Deallocates the memory occupied by the vector and set NULL ptr.
 * @param vp Double pointer to the vector to be freed.
 */
void vec_freep_rc(vec_t** vp);

/**
 * @brief Sets the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @param val New value for the element.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_set_rc(vec_t* v, size_t i, double val);

/**
 * @brief Retrieves the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @param out_val Pointer to a double where the retrieved value will be stored.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_get_rc(const vec_t* v, size_t i, double* out_val);

/**
 * @brief Adds two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to the vector where the sum will be stored.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_add_rc(const vec_t* a, const vec_t* b, vec_t* out);

/**
 * @brief Subtracts two vectors.
 * @param a Pointer to the vector to be subtracted from.
 * @param b Pointer to the vector to subtract.
 * @param out Pointer to the vector where the difference will be stored.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_subtract_rc(const vec_t* a, const vec_t* b, vec_t* out);

/**
 * @brief Scales a vector by a scalar.
 * @param a Pointer to the vector that will be scaled.
 * @param out Pointer to the vector where the scalar multiplication will be
 * stored.
 * @param scalar The scalar multiplier value.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_scale_rc(const vec_t* a, vec_t* out, double scalar);

/**
 * @brief Computes the dot product of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out_result Pointer to a double where the dot product result will be
 * stored.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_dot_rc(const vec_t* a, const vec_t* b, double* out_result);

/**
 * @brief Prints the vector elements to stdout in the format (v0, v1, ..., vn).
 * @param v Pointer to the vector to be printed.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_print_rc(const vec_t* v);

#endif  // VEC_RC_H