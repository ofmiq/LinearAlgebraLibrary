#include "benchmark_utils.h"

double get_t(clock_t start) {
  return ((double)(clock() - start)) / CLOCKS_PER_SEC;
}