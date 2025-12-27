#ifndef MAT_RC_H
#define MAT_RC_H

#include "mat_types.h"
#include "util.h"

/**
 * @brief Allocates memory for a matrix. Matrix is uninitialized.
 * @param out Double pointer where the newly allocated matrix will be stored.
 * @param rows Number of rows to allocate.
 * @param cols Number of columns to allocate.
 * @return ERR_OK on success, or an error code otherwise. On error, *out is left
 * unchanged.
 */
util_error_t mat_alloc_rc(mat_t** out, size_t rows, size_t cols);

/**
 * @brief Creates a new matrix by copying data from a C array (row-major order).
 * @param data Pointer to the C array of doubles.
 * @param out Double pointer where the newly allocated matrix will be stored.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return ERR_OK on success, or an error code otherwise. On error, *out is left
 * unchanged.
 */
util_error_t mat_from_array_rc(const double* data, mat_t** out, size_t rows,
                               size_t cols);

/**
 * @brief Deallocates the memory occupied by the matrix.
 * @param m Pointer to the vector to be freed.
 */
void mat_free_rc(mat_t* m);

/**
 * @brief Deallocates the memory occupied by the matrix and set NULL pointer.
 * @param mp Double pointer to the vector to be freed.
 */
void mat_freep_rc(mat_t** mp);

/**
 * @brief Sets the value of an element in the matrix at a specific index (i, j).
 * @param m Pointer to the matrix.
 * @param i Index of the element (rows).
 * @param j Index of the element (columns).
 * @param val New value for the element.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_rc(mat_t* m, size_t i, size_t j, double val);

/**
 * @brief Retrieves the value of an element in the matrix at a specific index.
 * @param m Pointer to the matrix.
 * @param i Index of the element (rows).
 * @param j Index of the element (columns).
 * @param out Pointer to a double where the retrieved value will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_rc(const mat_t* m, size_t i, size_t j, double* out);

/**
 * @brief Adds two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param out Pointer to the matrix where the sum will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_add_rc(const mat_t* restrict a, const mat_t* b,
                        mat_t* restrict out);

/**
 * @brief Adds the source matrix to the destination matrix in-place.
 * @param dest Pointer to the destination matrix (will be modified).
 * @param src Pointer to the source matrix.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_add_inplace_rc(mat_t* restrict dest,
                                const mat_t* restrict src);

/**
 * @brief Subtracts two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param out Pointer to the matrix where the difference will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_subtract_rc(const mat_t* restrict a, const mat_t* b,
                             mat_t* restrict out);

/**
 * @brief Subtracts the source matrix from the destination matrix in-place.
 * @param dest Pointer to the destination matrix (will be modified).
 * @param src Pointer to the source matrix.
 * @note Arguments 'dest' and 'src' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_subtract_inplace_rc(mat_t* restrict dest,
                                     const mat_t* restrict src);

/**
 * @brief Scales a matrix by a scalar.
 * @param a Pointer to the matrix that will be scaled.
 * @param out Pointer to the matrix where the scalar multiplication will be
 * stored.
 * @param scalar The scalar multiplier value.
 * @note Arguments 'a' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_scale_rc(const mat_t* restrict a, mat_t* restrict out,
                          double scalar);

/**
 * @brief Scales a matrix by a scalar in-place.
 * @param v Pointer to the matrix that will be scaled (will be modified).
 * @param scalar The scalar multiplier value.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_scale_inplace_rc(mat_t* restrict dest, double scalar);

#endif  // MAT_RC_H