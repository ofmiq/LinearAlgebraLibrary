#ifndef CONFIG_H
#define CONFIG_H

// Max number of elements in the vector (doubles). Approximately 122.07 Mib for
// 16,000,000 elements
#define VECTOR_MAX_ELEMENTS 16000000UL

// Max number of elements in the matrix (doubles). Approximately 122.07 Mib for
// 16,000,000 elements
#define MATRIX_MAX_ELEMENTS 16000000UL

// Max number of rows in the matrix (doubles).
#define MATRIX_MAX_ROWS 4000

// Max number of columns in the matrix (doubles).
#define MATRIX_MAX_COLUMNS 4000

// Values ​​for comparing floating-point numbers
#define VEC_EPSILON 1e-15

// 32-byte alignment value
#define ALIGNMENT 32

#endif  // CONFIG_H