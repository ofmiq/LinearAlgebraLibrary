#ifndef UTIL_H
#define UTIL_H

// Max number of elements (doubles). Approximately 76.3 MiB for 10,000,000 elements
#define VECTOR_MAX_ELEMENTS 10000000UL

/**
 * @brief Enumeration of standard error codes for the library.
 */
typedef enum {
  ERR_OK = 0,      ///< 0. Successful completion.
  ERR_ALLOC,       ///< 1. Memory allocation error.
  ERR_NULL,        ///< 2. A NULL pointer was passed as a required argument.
  ERR_DIM,         ///< 3. Mismatch in sizes, dimensions, or shapes of objects.
  ERR_RANGE,       ///< 4. Index or value outside the valid range.
  ERR_INVALID_ARG  ///< 5. Invalid argument it the function.
} util_error_t;

/**
 * @brief Return the description of the following error.
 * @param code Error code.
 * @return Safe string with error explanation
 */
const char* util_error_str(util_error_t code);

#endif