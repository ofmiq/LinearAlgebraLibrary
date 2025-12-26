#ifndef CONFIG_H
#define CONFIG_H

// Max number of elements (doubles). Approximately 76.3 MiB for 10,000,000 elements
#define VECTOR_MAX_ELEMENTS 10000000UL

// Values ​​for comparing floating-point numbers
#define VEC_EPSILON 1e-15

// 32-byte alignment value
#define VEC_ALIGNMENT 32

#endif // CONFIG_H