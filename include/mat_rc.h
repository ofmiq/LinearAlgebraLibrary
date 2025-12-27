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

#endif  // MAT_RC_H