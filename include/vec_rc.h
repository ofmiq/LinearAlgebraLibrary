#ifndef VEC_RC_H
#define VEC_RC_H

#include <stdbool.h>

#include "util.h"
#include "vec_types.h"

/**
 * @brief Allocates memory for a vector of length n. The vector is uninitialized
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
 * @param out Pointer to a double where the retrieved value will be stored.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_get_rc(const vec_t* v, size_t i, double* out);

/**
 * @brief Adds two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to the vector where the sum will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_add_rc(const vec_t* restrict a, const vec_t* restrict b,
                        vec_t* restrict out);

/**
 * @brief Adds the source vector to the destination vector in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_add_inplace_rc(vec_t* restrict dest,
                                const vec_t* restrict src);

/**
 * @brief Subtracts two vectors.
 * @param a Pointer to the vector to be subtracted from.
 * @param b Pointer to the vector to subtract.
 * @param out Pointer to the vector where the difference will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_subtract_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out);

/**
 * @brief Subtracts the source vector from the destination vector in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_subtract_inplace_rc(vec_t* restrict dest,
                                     const vec_t* restrict src);

/**
 * @brief Scales a vector by a scalar.
 * @param a Pointer to the vector that will be scaled.
 * @param out Pointer to the vector where the scalar multiplication will be
 * stored.
 * @param scalar The scalar multiplier value.
 * @note Arguments 'a' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_scale_rc(const vec_t* restrict a, vec_t* restrict out,
                          double scalar);

/**
 * @brief Scales a vector by a scalar in-place.
 * @param v Pointer to the vector that will be scaled (will be modified).
 * @param scalar The scalar multiplier value.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_scale_inplace_rc(vec_t* restrict v, double scalar);

/**
 * @brief Computes the dot product of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to a double where the dot product result will be
 * stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_dot_rc(const vec_t* restrict a, const vec_t* restrict b,
                        double* restrict out);

/**
 * @brief Computes the cross product of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to the vector where the cross product will be stored.
 * @note Argument 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_cross_rc(const vec_t* restrict a, const vec_t* restrict b,
                          vec_t* restrict out);

/**
 * @brief Computes the cross product of the destination and source vectors
 * in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_cross_inplace_rc(vec_t* restrict dest,
                                  const vec_t* restrict src);

/**
 * @brief Computes the length (magnitude) of vector.
 * @param v Pointer to the vector.
 * @param out Pointer to a double where the computed length (magnitude) will be
stored.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_len_rc(const vec_t* restrict v, double* restrict out);

/**
 * @brief Deep copies values ​​from the source vector to the destination
vector.  
 * @param src Pointer to the source vector.
 * @param dest Pointer to the destination vector.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.  
*/
util_error_t vec_copy_rc(const vec_t* restrict src, vec_t* restrict dest);

/**
 * @brief Compares two vectors for equality within a given permissible error
 * (epsilon).  
 * @param a Pointer to the first vector.  
 * @param b Pointer to the second vector.  
 * @param epsilon Permissible error threshold.
 * @param out Pointer to a bool where the result will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_is_equal_rc(const vec_t* restrict a, const vec_t* restrict b,
                             double epsilon, bool* restrict out);

/**
 * @brief Normalizes the vector in-place.
 * @param v Pointer to the vector to be normalized.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_normalize_inplace_rc(vec_t* restrict v);

/**
 * @brief Computes the Euclidean distance between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to a double where the distance will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_dist_rc(const vec_t* restrict a, const vec_t* restrict b,
                         double* restrict out);

/**
 * @brief Computes the squared Euclidean distance between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to a double where the squared distance will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_dist_sq_rc(const vec_t* restrict a, const vec_t* restrict b,
                            double* restrict out);

/**
 * @brief Computes the Hadamard product (element-wise product) of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to a vector where the Hadamard product will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_multiply_rc(const vec_t* restrict a, const vec_t* restrict b,
                             vec_t* restrict out);

/**
 * @brief Fill a vector by value.
 * @param v Pointer to the vector.
 * @param val The value that will be vector filled.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_fill_rc(vec_t* restrict v, double val);

/**
 * @brief Finds the minimum value in the vector.
 * @param v Pointer to the vector.
 * @param out Pointer to store the minimum value.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_min_rc(const vec_t* restrict v, double* restrict out);

/**
 * @brief Finds the maximum value in the vector.
 * @param v Pointer to the vector.
 * @param out Pointer to store the maximum value.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_max_rc(const vec_t* restrict v, double* restrict out);

/**
 * @brief Applies a function to every element of the source vector.
 * @param src Pointer to the source vector.
 * @param dest Pointer to the destination vector.
 * @param func Function pointer to apply (e.g., sin, sqrt).
 * @note Arguments 'src' and 'dest' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_map_rc(const vec_t* restrict src, vec_t* restrict dest,
                        vec_map_func_t func);

/**
 * @brief Find a size (dimension) of a given vector.
 * @param v Pointer to the vector.
 * @param out Pointer to a size_t where the size of vector will be stored.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_size_rc(const vec_t* restrict v, size_t* restrict out);

/**
 * @brief Provide a pointer to an pointer data array.
 * @param v Pointer to the vector.
 * @param out Double pointer where a pointer will be saved.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_data_rc(const vec_t* restrict v, const double** restrict out);

/**
 * @brief Change a size of a given array.
 * @param vp Double pointer to the vector.
 * @param new_n New size of a vector.
 * @note If shrinking, tail elements are discarded; if expanding, new elements
 * are zeroed.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_resize_rc(vec_t** vp, size_t new_n);

/**
 * @brief Computes the constant times a vector plus a vector (y = a*x + y).
 * @param a The scalar constant 'a'.
 * @param x Pointer to the vector 'x'.
 * @param y Pointer to the vector 'y' (will be modified).
 * @note Arguments 'x' and 'y' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_axpy_rc(double a, const vec_t* restrict x, vec_t* restrict y);

/**
 * @brief Swaps the contents of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_swap_rc(vec_t* a, vec_t* b);

/**
 * @brief Computes the negation of a vector.
 * @param v Pointer to the source vector.
 * @param out Pointer to the destination vector where the result will be stored.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_negate_rc(const vec_t* restrict v, vec_t* restrict out);

/**
 * @brief Computes the sum of all elements in a vector.
 * @param v Pointer to the source vector.
 * @param out Pointer to a double where the result will be stored.
 * @note Arguments 'v' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_sum_rc(const vec_t* restrict v, double* restrict out);

/**
 * @brief Computes the angle (in radians) between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @param out Pointer to a double where the result will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_angle_rc(const vec_t* restrict a, const vec_t* restrict b,
                          double* restrict out);

/**
 * @brief Computes the projection of vector a onto vector b.
 * @param a Pointer to the vector to be projected.
 * @param b Pointer to the vector onto which the projection is computed.
 * @param out Pointer to the destination vector where the result will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_project_rc(const vec_t* restrict a, const vec_t* restrict b,
                            vec_t* restrict out);

/**
 * @brief Prints the vector elements to stdout in the format (v0, v1, ..., vn).
 * @param v Pointer to the vector to be printed.
 * @return ERR_OK on success, or an error code.
 */
util_error_t vec_print_rc(const vec_t* v);

#endif  // VEC_RC_H