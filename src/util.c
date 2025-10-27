#include "util.h"

static const char* const ERROR_MESSAGES[] = {
    "Success",                                  // ERR_OK (0)
    "Memory allocation failed",                 // ERR_ALLOC (1)
    "NULL pointer argument",                    // ERR_NULL (2)
    "Dimension/size mismatch or invalid size",  // ERR_DIM (3)
    "Index or value out of range",              // ERR_RANGE (4)
    "Invalid argument"                          // ERR_INVALID_ARG (5)
};

#define MAX_ERROR_CODE \
  ((int)(sizeof(ERROR_MESSAGES) / sizeof(ERROR_MESSAGES[0])) - 1)

const char* util_error_str(util_error_t code) {
  if (code >= ERR_OK && code <= MAX_ERROR_CODE) {
    return ERROR_MESSAGES[code];
  }
  return "Unknown error code";
}