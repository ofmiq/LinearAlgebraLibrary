#ifndef MAT_RC_H
#define MAT_RC_H

#include <stdbool.h>

#include "mat_types.h"
#include "util.h"
#include "vec_types.h"

/* ============================================================ */
/*                      Lifecycle Management                    */
/* ============================================================ */

/**
 * @brief Allocates memory for a matrix. The matrix is uninitialized.
 * @param out Double pointer where the newly allocated matrix will be stored.
 * @param rows Number of rows to allocate.
 * @param cols Number of columns to allocate.
 * @return ERR_OK on success, or an error code otherwise. On error, *out is left
 * unchanged.
 */
util_error_t mat_alloc_rc(mat_t** restrict out, size_t rows, size_t cols);

/**
 * @brief Creates a new matrix by copying data from a C array (row-major order).
 * @param data Pointer to the C array of doubles.
 * @param out Double pointer where the newly allocated matrix will be stored.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @note Arguments 'data' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise. On error, *out is left
 * unchanged.
 */
util_error_t mat_from_array_rc(const double* restrict data,
                               mat_t** restrict out, size_t rows, size_t cols);

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
void mat_freep_rc(mat_t** restrict mp);

/**
 * @brief Changes the dimensions (rows, columns) of a given matrix.
 * @param mp Double pointer to the matrix.
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @note If shrinking, tail elements are discarded; if expanding, new elements
 * are zeroed.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_resize_rc(mat_t** restrict mp, size_t new_rows,
                           size_t new_cols);

/* ============================================================ */
/*                Data Access and Inspection                    */
/* ============================================================ */

/**
 * @brief Sets the value of an element in the matrix at a specific index (i, j).
 * @param m Pointer to the matrix.
 * @param i Index of the row.
 * @param j Index of the column.
 * @param val New value for the element.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_rc(mat_t* restrict m, size_t i, size_t j, double val);

/**
 * @brief Sets the values of a specific row in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param row Index of the row to set.
 * @param v Pointer to the source vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_row(mat_t* restrict m, size_t row,
                         const vec_t* restrict v);

/**
 * @brief Sets the values of a specific column in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param col Index of the column to set.
 * @param v Pointer to the source vector.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_set_column(mat_t* restrict m, size_t col,
                            const vec_t* restrict v);

/**
 * @brief Retrieves the value of an element in the matrix at a specific index.
 * @param m Pointer to the matrix.
 * @param i Index of the row.
 * @param j Index of the column.
 * @param out Pointer to a double where the retrieved value will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_rc(const mat_t* restrict m, size_t i, size_t j,
                        double* restrict out);

/**
 * @brief Retrieves a specific row from the matrix and stores it in a vector.
 * @param m Pointer to the matrix.
 * @param row Index of the row to retrieve.
 * @param out Pointer to the destination vector.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_row(const mat_t* restrict m, size_t row,
                         vec_t* restrict out);

/**
 * @brief Retrieves a specific column from the matrix and stores it in a vector.
 * @param m Pointer to the matrix.
 * @param col Index of the column to retrieve.
 * @param out Pointer to the destination vector.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_get_column(const mat_t* restrict m, size_t col,
                            vec_t* restrict out);

/**
 * @brief Retrieves the number of rows in the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a size_t where the number of rows will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_rows_rc(const mat_t* restrict m, size_t* restrict out);

/**
 * @brief Retrieves the number of columns in the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a size_t where the number of columns will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_cols_rc(const mat_t* restrict m, size_t* restrict out);

/**
 * @brief Provides a pointer to the underlying data array.
 * @param m Pointer to the matrix.
 * @param out Double pointer where the pointer to the data will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_data_rc(const mat_t* restrict m, const double** restrict out);

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
util_error_t mat_add_rc(const mat_t* restrict a, const mat_t* restrict b,
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
 * @brief Subtracts one matrix from another.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param out Pointer to the matrix where the difference will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_subtract_rc(const mat_t* restrict a, const mat_t* restrict b,
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
 * @brief Scales a matrix by a scalar multiplier.
 * @param a Pointer to the source matrix.
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
 * @param out Pointer to the matrix where the result will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_hadamard_rc(const mat_t* restrict a, const mat_t* restrict b,
                             mat_t* restrict out);

/**
 * @brief Applies a function to every element of the source matrix.
 * @param src Pointer to the source matrix.
 * @param dest Pointer to the destination matrix.
 * @param func Function pointer to apply (e.g., sin, sqrt).
 * @note Arguments 'src' and 'dest' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_map_rc(const mat_t* restrict src, mat_t* restrict dest,
                        mat_map_func_t func);

/* ============================================================ */
/*                        Matrix Products                       */
/* ============================================================ */

/**
 * @brief Computes the matrix product of two matrices.
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

/* ============================================================ */
/*                    Matrix Transformations                    */
/* ============================================================ */

/**
 * @brief Performs matrix transposition.
 * @param a Pointer to the source matrix.
 * @param out Pointer to the matrix where the transposed matrix will be stored.
 * @note Arguments 'a' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_transpose_rc(const mat_t* restrict a, mat_t* restrict out);

/**
 * @brief Reshapes the matrix to new dimensions.
 * @param m Pointer to the matrix.
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_reshape_rc(mat_t* restrict m, size_t new_rows,
                            size_t new_cols);

/* ============================================================ */
/*                        Linear Algebra                        */
/* ============================================================ */

/**
 * @brief Computes the determinant of the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a double where the determinant will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_det_rc(const mat_t* restrict m, double* restrict out);

/**
 * @brief Computes the inverse of the matrix.
 * @param m Pointer to the source matrix.
 * @param out Pointer to the matrix where the inverse will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_inverse_rc(const mat_t* restrict m, mat_t* restrict out);

/**
 * @brief Solves a system of linear equations (Ax = b).
 * @param a Pointer to the coefficient matrix.
 * @param b Pointer to the right-hand side vector.
 * @param out Pointer to the vector where the solution will be stored.
 * @note Arguments 'a', 'b', and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_solve_rc(const mat_t* restrict a, const vec_t* restrict b,
                          vec_t* restrict out);
/**
 * @brief Computes the trace of the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a double where the result will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_trace_rc(const mat_t* restrict m, double* restrict out);

/* ============================================================ */
/*              Properties, Comparison and Utility              */
/* ============================================================ */

/**
 * @brief Checks whether the matrix is square.
 * @param m Pointer to the matrix.
 * @param out Pointer to a bool where the result will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_is_square_rc(const mat_t* restrict m, bool* restrict out);

/**
 * @brief Computes the sum of all elements in the matrix.
 * @param m Pointer to the matrix.
 * @param out Pointer to a double where the result will be stored.
 * @note Arguments 'm' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_sum_rc(const mat_t* restrict m, double* restrict out);

/**
 * @brief Swaps the contents of two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @note Arguments 'a' and 'b' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_swap_rc(mat_t* restrict a, mat_t* restrict b);

/**
 * @brief Creates a deep copy of the source matrix.
 * @param src Pointer to the source matrix.
 * @param out Double pointer where the newly allocated copy will be stored.
 * @note Arguments 'src' and 'out' must not overlap (restrict pointers).
 * @return ERR_OK on success, or an error code otherwise.
 */
util_error_t mat_copy_rc(const mat_t* restrict src, mat_t** restrict out);

#endif  // MAT_RC_H