#ifndef VEC_H
#define VEC_H

#include <stdbool.h>

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
 * @brief Adds the source vector to the destination vector in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @return True on success (dimensions match), False otherwise.
 */
bool vec_add_inplace(vec_t* dest, const vec_t* src);

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
 * @brief Subtracts the source vector from the destination vector in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @return True on success (dimensions match), False otherwise.
 */
bool vec_subtract_inplace(vec_t* dest, const vec_t* src);

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
 * @brief Scales the vector by a scalar in-place.
 * @param v Pointer to the vector (will be modified).
 * @param scalar The scalar multiplier value.
 * @return True on success, False otherwise.
 */
bool vec_scale_inplace(vec_t* v, double scalar);

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
 * @brief Computes the cross product of the destination and source vectors
 * in-place.
 * @param dest Pointer to the destination vector (will be modified).
 * @param src Pointer to the source vector.
 * @return True on success (dimensions must be 3), False otherwise.
 */
bool vec_cross_inplace(vec_t* dest, const vec_t* src);

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
 * @brief Normalizes the vector in-place.
 * @param v Pointer to the vector to be normalized.
 * @return True on success, False otherwise.
 */
bool vec_normalize(vec_t* v);

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
 * @brief Creates a new vector of length n filled with zeros.
 * @param n Length of the vector (must be > 0).
 * @return Pointer to a newly allocated vector owned by the caller, or NULL on
 * error.
 */
vec_t* vec_zeros(size_t n);

/**
 * @brief Creates a new vector of length n filled with ones.
 * @param n Length (dimension) of the vector to allocate.
 * @return Pointer where the newly allocated vector will be stored, or NULL on
 * error.
 */
vec_t* vec_ones(size_t n);

/**
 * @brief Fills the vector with a specific value.
 * @param v Pointer to the vector.
 * @param val The value to fill the vector with.
 * @return True on success, False otherwise.
 */
bool vec_fill(vec_t* v, double val);

/**
 * @brief Finds the minimum value in the vector.
 * @param v Pointer to the vector.
 * @return Minimum value, or NAN on error.
 */
double vec_min(const vec_t* v);

/**
 * @brief Finds the maximum value in the vector.
 * @param v Pointer to the vector.
 * @return Maximum value, or NAN on error.
 */
double vec_max(const vec_t* v);

/**
 * @brief Creates a NEW vector by applying a function to each element.
 * WARNING: Allocates memory.
 * @param v Pointer to the source vector.
 * @param func Function to apply (e.g., sin, cos).
 * @return Pointer to newly allocated vector, or NULL on error.
 */
vec_t* vec_map_new(const vec_t* v, vec_map_func_t func);

/**
 * @brief Find the size (dimension) of a given vector.
 * @param vec Pointer to the vector.
 * @return Size (dimension) of a given vector, or 0 on error.
 */
size_t vec_size(const vec_t* vec);

/**
 * @brief Provide a pointer to data array of a vector.
 * @param v Pointer to the vector.
 * @return Pointer to data of array, or NULL on error.
 */
const double* vec_data(const vec_t* v);

/**
 * @brief Change a size of a given array.
 * @param v Pointer to the vector.
 * @param new_n New size of a vector.
 * @return Vector with a new size.
 */
vec_t* vec_resize_new(const vec_t* v, size_t new_n);

/**
 * @brief Change a size of a given array in-place.
 * @param v Pointer to the vector (will be modified).
 * @param new_n New size of a vector.
 * @return True on success, false otherwise.
 */
bool vec_resize_inplace(vec_t* v, size_t new_n);

/**
 * @brief Creates a NEW vector computing the constant times a vector plus a
 * vector (y = a*x + y). WARNING: Allocates memory.
 * @param a The scalar constant 'a'.
 * @param x Pointer to the vector 'x'.
 * @param y Pointer to the vector 'y'.
 * @return Pointer to newly allocated vector, or NULL on error.
 */
vec_t* vec_axpy_new(double a, const vec_t* x, const vec_t* y);

/**
 * @brief Swaps the contents (data and size) of two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 */
void vec_swap(vec_t* a, vec_t* b);

/**
 * @brief Creates a NEW vector which is the negation of the input vector.
 * @param v Pointer to the source vector.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_negate_new(const vec_t* v);

/**
 * @brief Computes the sum of all elements in a vector.
 * @param v Pointer to the source vector.
 * @return Sum of elements, or NAN on error.
 */
double vec_sum(const vec_t* v);

/**
 * @brief Computes the angle (in radians) between two vectors.
 * @param a Pointer to the first vector.
 * @param b Pointer to the second vector.
 * @return Angle between vectors in radians, or NAN on error.
 */
double vec_angle(const vec_t* a, const vec_t* b);

/**
 * @brief Creates a NEW vector representing the projection of vector a onto b.
 * @param a Pointer to the vector to be projected.
 * @param b Pointer to the vector onto which the projection is computed.
 * @return Pointer to newly allocated vector on success, NULL on error.
 */
vec_t* vec_project_new(const vec_t* a, const vec_t* b);

/**
 * @brief Prints the vector elements to stdout in the format (v0, v1, ..., vn).
 * @param v Pointer to the vector to be printed.
 */
void vec_print(const vec_t* v);

#endif  // VEC_H