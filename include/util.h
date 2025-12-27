#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/**
 * @brief Enumeration of standard error codes for the library.
 */
typedef enum {
  ERR_OK = 0,           ///< 0. Successful completion.
  ERR_ALLOC = 1,        ///< 1. Memory allocation error.
  ERR_NULL = 2,         ///< 2. A NULL pointer was passed as a required argument.
  ERR_DIM = 3,          ///< 3. Mismatch in sizes, dimensions, or shapes of objects.
  ERR_RANGE = 4,        ///< 4. Index or value outside the valid range.
  ERR_INVALID_ARG = 5,  ///< 5. Invalid argument in the function.
  ERR_DIV_ZERO = 6      ///< 6. Division by zero.
} util_error_t;

/**
 * @brief Return the description of the following error.
 * @param code Error code.
 * @return Safe string with error explanation
 */
const char* util_error_str(util_error_t code);

/**
 * @brief Function pointer type for mapping operations.
 * Takes a double and returns a double (e.g., sin, cos, abs).
 */
typedef double (*vec_map_func_t)(double);

/**
 * @brief Calculates the aligned memory size in bytes required for n double elements.
 * @param n The number of elements of type double.
 * @return size_t The total size in bytes, aligned to the ALIGNMENT boundary.
 */
size_t get_aligned_size(size_t n);

#endif  // UTIL_H