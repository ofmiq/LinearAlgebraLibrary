#ifndef MAT_RC_H
#define MAT_RC_H

#include "mat_types.h"
#include "util.h"
#include "vec_types.h"

/* ============================================================ */
/*                      Lifecycle Management                    */
/* ============================================================ */

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
 * @param m Pointer to the matrix to be freed.
 */
void mat_free_rc(mat_t* m);

/**
 * @brief Deallocates the memory occupied by the matrix and sets the pointer to
 * NULL.
 * @param mp Double pointer to the matrix to be freed.
 */
void mat_freep_rc(mat_t** mp);

/**
 * @brief Changes the size (rows, columns) of a given matrix.
 * @param mp Double pointer to the matrix.
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @note If shrinking, tail elements are discarded; if expanding, new elements
 * are zeroed.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_resize_rc(mat_t** mp, size_t new_rows, size_t new_cols);

/* ============================================================ */
/*                Data Access and Inspection                    */
/* ============================================================ */

/**
 * @brief Sets the value of an element in the matrix at a specific index (i, j).
 * @param m Pointer to the matrix.
 * @param i Index of the element (row).
 * @param j Index of the element (column).
 * @param val New value for the element.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_rc(mat_t* m, size_t i, size_t j, double val);

/**
 * @brief Sets the values of a specific row in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param row Index of the row to set.
 * @param v Pointer to the source vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_row(const mat_t* m, size_t row, const vec_t* v);

/**
 * @brief Sets the values of a specific column in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param col Index of the column to set.
 * @param v Pointer to the source vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_column(const mat_t* m, size_t col, const vec_t* v);

/**
 * @brief Retrieves the value of an element in the matrix at a specific index.
 * @param m Pointer to the matrix.
 * @param i Index of the element (row).
 * @param j Index of the element (column).
 * @param out Pointer to a double where the retrieved value will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_rc(const mat_t* m, size_t i, size_t j, double* out);

/**
 * @brief Retrieves a specific row from the matrix and stores it in a vector.
 * @param m Pointer to the matrix.
 * @param row Index of the row to retrieve.
 * @param out Pointer to the destination vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_row(const mat_t* m, size_t row, vec_t* out);

/**
 * @brief Retrieves a specific column from the matrix and stores it in a vector.
 * @param m Pointer to the matrix.
 * @param col Index of the column to retrieve.
 * @param out Pointer to the destination vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_column(const mat_t* m, size_t col, vec_t* out);

/**
 * @brief Retrieves the number of rows in the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a size_t where the number of rows will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_rows_rc(const mat_t* m, size_t* out);

/**
 * @brief Retrieves the number of columns in the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a size_t where the number of columns will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_cols_rc(const mat_t* m, size_t* out);

/**
 * @brief Provides a pointer to the underlying data array.
 * @param m Pointer to the matrix.
 * @param out Double pointer where the pointer to the data will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t vec_data_rc(const mat_t* m, const double** out);

/* ============================================================ */
/*                   Basic Matrix Arithmetic                    */
/* ============================================================ */

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

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

/**
 * @brief Scales a matrix by a scalar.
 * @param a Pointer to the matrix that will be scaled.
 * @param out Pointer to the matrix where the result will be stored.
 * @param scalar The scalar multiplier value.
 * @note Arguments 'a' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_scale_rc(const mat_t* restrict a, mat_t* restrict out,
                          double scalar);

/**
 * @brief Scales a matrix by a scalar in-place.
 * @param dest Pointer to the matrix that will be scaled (will be modified).
 * @param scalar The scalar multiplier value.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_scale_inplace_rc(mat_t* restrict dest, double scalar);

/**
 * @brief Computes the Hadamard product (element-wise product) of two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param out Pointer to the matrix where the Hadamard product will be stored.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_hadamard_new(const mat_t* restrict a, const mat_t* restrict b,
                              mat_t* restrict out);

/* ============================================================ */
/*                        Matrix Products                       */
/* ============================================================ */

/**
 * @brief Computes the product of two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param out Pointer to the matrix where the product will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_multiply_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out);

/**
 * @brief Computes the product of a matrix and a vector.
 * @param m Pointer to the matrix.
 * @param v Pointer to the vector.
 * @param out Pointer to the vector where the product will be stored.
 * @note Arguments 'm', 'v', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_vec_multiply_rc(const mat_t* restrict m,
                                 const vec_t* restrict v, vec_t* restrict out);

#endif  // MAT_RC_H