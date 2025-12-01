#ifndef VEC_H
#define VEC_H

#include <stdbool.h>
#include <stdlib.h>

#include "util.h"
#include "vec_types.h"

/**
 * @brief Creates a vector of length n.
 * @param n Length (dimension) of the vector to allocate.
 * @return Pointer where the newly allocated vector will be stored.
 */
vec_t* vec_alloc(size_t n);

/**
 * @brief Creates a new vector by copying data from a C array.
 * @param data Pointer to the C array of doubles.
 * @param n Number of elements to copy.
 * @return Pointer where the newly allocated vector will be stored.
 */
vec_t* vec_from_array(const double* data, size_t n);

/**
 * @brief Deallocates the memory occupied by the vector.
 * @param v Pointer to the vector to be freed.
 */
void vec_free(vec_t* v);

/**
 * @brief Deallocates the memory occupied by the vector and set NULL ptr.
 * @param vp Double pointer to the vector to be freed.
 */
void vec_freep(vec_t** vp);

/**
 * @brief Sets the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @param val New value for the element.
 * @return True on success, false otherwise.
 */
bool vec_set(vec_t* v, size_t i, double val);

/**
 * @brief Retrieves the value of an element in the vector at a specific index.
 * @param v Pointer to the vector.
 * @param i Index of the element.
 * @return Retrieved value, or NAN on error.
 */
double vec_get(const vec_t* v, size_t i);

/**
 * @brief Creates a NEW vector with the result of a + b.
 * WARNING: This function allocates memory. Do NOT use in nested calls -> memory
 * leak.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_add_new(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a NEW vector with the result of a - b.
 * WARNING: This function allocates memory. Do NOT use in nested calls -> memory
 * leak.
 * @param a Pointer to the vector to be subtracted from.
 * @param b Pointer to the vector to subtract.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_subtract_new(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a NEW vector with the result of a * scalar.
 * WARNING: This function allocates memory. Do NOT use in nested calls -> memory
 * leak.
 * @param a Pointer to the vector that will be scaled.
 * @param scalar The scalar multiplier value.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_scale_new(const vec_t* a, double scalar);

/**
 * @brief Computes the dot product of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Dot product or NAN on error.
 */
double vec_dot(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a NEW vector with the cross product of two vectors.
 * WARNING: This function allocates memory. Do NOT use in nested calls -> memory
 * leak.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_cross_new(const vec_t* a, const vec_t* b);

/**
 * @brief Computes the length (magnitude) of vector.
 * @param v Pointer to the vector.
 * @return Length (magnitude) of the vector, or NAN on error.
 */
double vec_len(const vec_t* v);

/**
 * @brief Duplicate the vector.
 * @param v Pointer to the source vector.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_duplicate(const vec_t* v);

/**
 * @brief Compares two vectors for equality within a given permissible error
 * (epsilon).  
 * @param a Pointer to the first vector.  
 * @param b Pointer to the second vector.  
 * @param epsilon Permissible error threshold.
 * @return True if two vectors are equal within the permissible error, False
 * otherwise.  
*/
bool vec_is_equal(const vec_t* a, const vec_t* b, double epsilon);

/**
 * @brief Create a NEW normalized vector.
 * @param v Pointer to the vector to be normalized.
 * @return Pointer to newly allocated vector on success, NULL on error.
*/
vec_t* vec_normalized_new(const vec_t* v);

/**
 * @brief Computes the Euclidean distance between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Euclidean distance between two vectors, or NAN on error.
 */
double vec_dist(const vec_t* a, const vec_t* b);

/**
 * @brief Computes the squared Euclidean distance between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Euclidean squared distance between two vectors, or NAN on error.
 */
double vec_dist_sq(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a NEW vector with the Hadamard product (element-wise
 * product) of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Pointer to newly allocated vector on success, or NULL on error.
 */
vec_t* vec_multiply_new(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a new vector of length n filled with zeroes.
 * @param n Length (dimension) of the vector to allocate.
 * @return Pointer where the newly allocated vector will be stored, or NULL on error.
 */
vec_t* vec_zeros(size_t n);

/**
 * @brief Creates a new vector of length n filled with ones.
 * @param n Length (dimension) of the vector to allocate.
 * @return Pointer where the newly allocated vector will be stored, or NULL on error.
 */
vec_t* vec_ones(size_t n);

/**
 * @brief Prints the vector elements to stdout in the format (v0, v1, ..., vn).
 * @param v Pointer to the vector to be printed.
 */
void vec_print(const vec_t* v);

#endif  // VEC_H