#ifndef MAT_H
#define MAT_H

#include <stdbool.h>

#include "mat_types.h"
#include "util.h"
#include "vec_types.h"

/* ============================================================ */
/*                     Lifecycle Management                     */
/* ============================================================ */

/**
 * @brief Creates a matrix of size rows x cols.
 * @param rows Number of rows to allocate.
 * @param cols Number of columns to allocate.
 * @return Pointer where the newly allocated matrix will be stored, or NULL on error.
 */
mat_t* mat_alloc(size_t rows, size_t cols);

/**
 * @brief Creates a new matrix by copying data from a C array (row-major order).
 * @param data Pointer to the C array of doubles.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return Pointer where the newly allocated matrix will be stored, or NULL on error.
 */
mat_t* mat_from_array(const double* data, size_t rows, size_t cols);

/**
 * @brief Deallocates the memory occupied by the matrix.
 * @param m Pointer to the matrix to be freed.
 */
void mat_free(mat_t* m);

/**
 * @brief Deallocates the memory occupied by the matrix and sets the pointer to NULL.
 * @param mp Double pointer to the matrix to be freed.
 */
void mat_freep(mat_t** mp);

/**
 * @brief Creates a NEW matrix with changed dimensions (copy and resize).
 * @param m Pointer to the source matrix.
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @return Pointer to the newly allocated resized matrix, or NULL on error.
 */
mat_t* mat_resize_new(const mat_t* m, size_t new_rows, size_t new_cols);

/**
 * @brief Changes the dimensions of a given matrix in-place.
 * @param m Pointer to the matrix (will be modified).
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @return True on success, False otherwise.
 */
bool mat_resize_inplace(mat_t* m, size_t new_rows, size_t new_cols);

/* ============================================================ */
/*                Data Access and Inspection                    */
/* ============================================================ */

/**
 * @brief Sets the value of an element in the matrix at a specific index (i, j).
 * @param m Pointer to the matrix.
 * @param i Index of the row.
 * @param j Index of the column.
 * @param val New value for the element.
 * @return True on success, False otherwise.
 */
bool mat_set(mat_t* m, size_t i, size_t j, double val);

/**
 * @brief Sets the values of a specific row in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param row Index of the row to set.
 * @param v Pointer to the source vector.
 * @return True on success, False otherwise.
 */
bool mat_set_row(mat_t* m, size_t row, const vec_t* v);

/**
 * @brief Sets the values of a specific column in the matrix using a vector.
 * @param m Pointer to the matrix.
 * @param col Index of the column to set.
 * @param v Pointer to the source vector.
 * @return True on success, False otherwise.
 */
bool mat_set_column(mat_t* m, size_t col, const vec_t* v);

/**
 * @brief Retrieves the value of an element in the matrix at a specific index.
 * @param m Pointer to the matrix.
 * @param i Index of the row.
 * @param j Index of the column.
 * @return Retrieved value, or NAN on error.
 */
double mat_get(const mat_t* m, size_t i, size_t j);

/**
 * @brief Retrieves a specific row from the matrix as a NEW vector.
 * WARNING: Allocates memory.
 * @param m Pointer to the matrix.
 * @param row Index of the row to retrieve.
 * @return Pointer to the newly allocated vector, or NULL on error.
 */
vec_t* mat_get_row(const mat_t* m, size_t row);

/**
 * @brief Retrieves a specific column from the matrix as a NEW vector.
 * WARNING: Allocates memory.
 * @param m Pointer to the matrix.
 * @param col Index of the column to retrieve.
 * @return Pointer to the newly allocated vector, or NULL on error.
 */
vec_t* mat_get_column(const mat_t* m, size_t col);

/**
 * @brief Retrieves the number of rows in the matrix.
 * @param m Pointer to the matrix.
 * @return Number of rows, or 0 on error.
 */
size_t mat_rows(const mat_t* m);

/**
 * @brief Retrieves the number of columns in the matrix.
 * @param m Pointer to the matrix.
 * @return Number of columns, or 0 on error.
 */
size_t mat_cols(const mat_t* m);

/**
 * @brief Provides a pointer to the underlying data array.
 * @param m Pointer to the matrix.
 * @return Pointer to data array, or NULL on error.
 */
const double* mat_data(const mat_t* m);

/* ============================================================ */
/*                    Initialization Helpers                    */
/* ============================================================ */

/**
 * @brief Fills a matrix with a specific value.
 * @param m Pointer to the matrix.
 * @param val The value to be used for filling.
 * @return True on success, False otherwise.
 */
bool mat_fill(mat_t* m, double val);

/**
 * @brief Creates a NEW matrix of size rows x cols filled with zeros.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_zeros(size_t rows, size_t cols);

/**
 * @brief Creates a NEW identity matrix of size n x n.
 * @param n Size of the square matrix.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_identity(size_t n);

/* ============================================================ */
/*                   Basic Matrix Arithmetic                    */
/* ============================================================ */

/**
 * @brief Creates a NEW matrix with the result of a + b.
 * WARNING: Allocates memory.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_add_new(const mat_t* a, const mat_t* b);

/**
 * @brief Adds the source matrix to the destination matrix in-place.
 * @param dest Pointer to the destination matrix (will be modified).
 * @param src Pointer to the source matrix.
 * @return True on success, False otherwise.
 */
bool mat_add_inplace(mat_t* dest, const mat_t* src);

/**
 * @brief Creates a NEW matrix with the result of a - b.
 * WARNING: Allocates memory.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_subtract_new(const mat_t* a, const mat_t* b);

/**
 * @brief Subtracts the source matrix from the destination matrix in-place.
 * @param dest Pointer to the destination matrix (will be modified).
 * @param src Pointer to the source matrix.
 * @return True on success, False otherwise.
 */
bool mat_subtract_inplace(mat_t* dest, const mat_t* src);

/* ============================================================ */
/*              Scalar and Element-wise Operations              */
/* ============================================================ */

/**
 * @brief Creates a NEW matrix scaled by a multiplier.
 * WARNING: Allocates memory.
 * @param a Pointer to the source matrix.
 * @param scalar The scalar multiplier value.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_scale_new(const mat_t* a, double scalar);

/**
 * @brief Scales a matrix by a scalar in-place.
 * @param dest Pointer to the matrix that will be scaled (will be modified).
 * @param scalar The scalar multiplier value.
 * @return True on success, False otherwise.
 */
bool mat_scale_inplace(mat_t* dest, double scalar);

/**
 * @brief Creates a NEW matrix with the Hadamard product (element-wise product)
 * of two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_hadamard_new(const mat_t* a, const mat_t* b);

/**
 * @brief Creates a NEW matrix by applying a function to every element of the source.
 * @param src Pointer to the source matrix.
 * @param func Function pointer to apply (e.g., sin, sqrt).
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_map_new(const mat_t* src, mat_map_func_t func);

/* ============================================================ */
/*                        Matrix Products                       */
/* ============================================================ */

/**
 * @brief Creates a NEW matrix with the product of two matrices.
 * WARNING: Allocates memory.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @return Pointer to the newly allocated matrix, or NULL on error.
 */
mat_t* mat_multiply_new(const mat_t* a, const mat_t* b);

/**
 * @brief Creates a NEW vector with the product of a matrix and a vector.
 * WARNING: Allocates memory.
 * @param m Pointer to the matrix.
 * @param v Pointer to the vector.
 * @return Pointer to the newly allocated vector, or NULL on error.
 */
vec_t* mat_vec_multiply_new(const mat_t* m, const vec_t* v);

/* ============================================================ */
/*                    Matrix Transformations                    */
/* ============================================================ */

/**
 * @brief Creates a NEW matrix which is the transpose of the input.
 * @param a Pointer to the source matrix.
 * @return Pointer to the newly allocated transposed matrix, or NULL on error.
 */
mat_t* mat_transpose_new(const mat_t* a);

/**
 * @brief Reshapes the matrix to new dimensions in-place.
 * @param m Pointer to the matrix (will be modified).
 * @param new_rows New number of rows.
 * @param new_cols New number of columns.
 * @return True on success (total elements must match), False otherwise.
 */
bool mat_reshape(mat_t* m, size_t new_rows, size_t new_cols);

/* ============================================================ */
/*                        Linear Algebra                        */
/* ============================================================ */

/**
 * @brief Creates a NEW matrix containing the LU decomposition factors.
 * @param src Pointer to the source matrix.
 * @param piv Pointer to an array (allocated by caller) to store permutation indices.
 * @param sign Pointer to an integer to store the sign of the permutation.
 * @return Pointer to newly allocated matrix (L + U), or NULL on error.
 */
mat_t* mat_lu_decompose_new(const mat_t* src, size_t* piv, int* sign);

/**
 * @brief Performs LU decomposition of a matrix in-place.
 * @param a Pointer to the matrix (will be replaced by LU factors).
 * @param piv Pointer to an array (allocated by caller) to store permutation indices.
 * @param sign Pointer to an integer to store the sign of the permutation.
 * @return True on success, False otherwise.
 */
bool mat_lu_decompose_inplace(mat_t* a, size_t* piv, int* sign);

/**
 * @brief Computes the determinant of the matrix.
 * @param m Pointer to the matrix.
 * @return Determinant value, or NAN on error.
 */
double mat_det(const mat_t* m);

/**
 * @brief Computes the inverse of the matrix into a NEW matrix.
 * @param m Pointer to the source matrix.
 * @return Pointer to the newly allocated inverse matrix, or NULL on error.
 */
mat_t* mat_inverse_new(const mat_t* m);

/**
 * @brief Computes the inverse of the matrix in-place.
 * @param m Pointer to the matrix (will be modified).
 * @return True on success, False otherwise.
 */
bool mat_inverse_inplace(mat_t* m);

/**
 * @brief Solves a system of linear equations (Ax = b) and returns a NEW vector x.
 * @param a Pointer to the coefficient matrix.
 * @param b Pointer to the right-hand side vector.
 * @return Pointer to newly allocated solution vector, or NULL on error.
 */
vec_t* mat_solve_new(const mat_t* a, const vec_t* b);

/**
 * @brief Computes the trace of the matrix.
 * @param m Pointer to the matrix.
 * @return Trace value, or NAN on error.
 */
double mat_trace(const mat_t* m);

/* ============================================================ */
/*              Properties, Comparison and Utility              */
/* ============================================================ */

/**
 * @brief Checks whether the matrix is square.
 * @param m Pointer to the matrix.
 * @return True if square, False otherwise.
 */
bool mat_is_square(const mat_t* m);

/**
 * @brief Compares two matrices for equality within a given permissible error (epsilon).
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 * @param epsilon Permissible error threshold.
 * @return True if equal, False otherwise.
 */
bool mat_is_equal(const mat_t* a, const mat_t* b, double epsilon);

/**
 * @brief Computes the sum of all elements in the matrix.
 * @param m Pointer to the matrix.
 * @return Sum of elements, or NAN on error.
 */
double mat_sum(const mat_t* m);

/**
 * @brief Swaps the contents of two matrices.
 * @param a Pointer to the first matrix.
 * @param b Pointer to the second matrix.
 */
void mat_swap(mat_t* a, mat_t* b);

/**
 * @brief Swaps two rows in a matrix.
 * @param m Pointer to the matrix.
 * @param row_a Index of the first row.
 * @param row_b Index of the second row.
 * @return True on success, False otherwise.
 */
bool mat_swap_rows(mat_t* m, size_t row_a, size_t row_b);

/**
 * @brief Creates a duplicate (deep copy) of the matrix.
 * @param src Pointer to the source matrix.
 * @return Pointer to the newly allocated duplicate matrix, or NULL on error.
 */
mat_t* mat_duplicate(const mat_t* src);

/**
 * @brief Prints the matrix elements to stdout.
 * @param m Pointer to the matrix to be printed.
 */
void mat_print(const mat_t* m);

#endif // MAT_H