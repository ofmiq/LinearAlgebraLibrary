#ifndef UTIL_H
#define UTIL_H

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

#endif  // UTIL_H