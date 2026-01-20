# Project
PROJECT = linalg
CC = gcc

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# Sources
LIB_SOURCES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
LIB_OBJECTS = $(LIB_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Benchmark utilities
BENCH_UTILS = $(TEST_DIR)/benchmark_utils.c
BENCH_UTILS_OBJ = $(BUILD_DIR)/benchmark_utils.o

# Test sources
TEST_VECTOR = $(TEST_DIR)/vector_benchmark.c
TEST_MATRIX = $(TEST_DIR)/matrix_benchmark.c

TEST_VECTOR_OBJ = $(BUILD_DIR)/test_vector_benchmark.o
TEST_MATRIX_OBJ = $(BUILD_DIR)/test_matrix_benchmark.o

# Unity test framework
UNITY_SRC = $(TEST_DIR)/unity.c
UNITY_OBJ = $(BUILD_DIR)/unity.o

# Unit test sources
UNIT_TEST_VEC = $(TEST_DIR)/test_vec_rc.c
UNIT_TEST_VEC_OBJ = $(BUILD_DIR)/test_vec_rc.o

UNIT_TEST_VEC_WRAPPER = $(TEST_DIR)/test_vec.c
UNIT_TEST_VEC_WRAPPER_OBJ = $(BUILD_DIR)/test_vec.o

# Targets
TARGET_VECTOR_BENCH = $(BIN_DIR)/vector_bench
TARGET_MATRIX_BENCH = $(BIN_DIR)/matrix_bench
TARGET_UNIT_TEST_VEC = $(BIN_DIR)/test_vec_rc
TARGET_UNIT_TEST_VEC_WRAPPER = $(BIN_DIR)/test_vec

# ============================================================================
# FLAGS
# ============================================================================

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(TEST_DIR) -fopenmp

CFLAGS += -O3 -march=native -mtune=native -flto \
          -fno-math-errno -fomit-frame-pointer -fno-plt -pipe

CFLAGS += -DUNITY_INCLUDE_DOUBLE -DUNITY_DOUBLE_PRECISION=1e-12
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_INCLUDE_FLOAT

LDFLAGS = -lm -flto -fopenmp

# ============================================================================
# BUILD MODES
# ============================================================================

all: CFLAGS += -DNDEBUG
all: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(TEST_DIR) -fopenmp
debug: CFLAGS += -O0 -g -ggdb3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: CFLAGS += -DUNITY_INCLUDE_DOUBLE -DUNITY_DOUBLE_PRECISION=1e-12
debug: CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_INCLUDE_FLOAT
debug: LDFLAGS = -lm -fopenmp -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

release: CFLAGS += -DNDEBUG
release: CFLAGS += -fipa-pta -fprefetch-loop-arrays -floop-nest-optimize
release: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

# ============================================================================
# Unit Tests
# ============================================================================

# ============================================================================
# Unit Tests
# ============================================================================

test: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(TEST_DIR) -fopenmp
test: CFLAGS += -O2
test: CFLAGS += -DUNITY_INCLUDE_DOUBLE -DUNITY_DOUBLE_PRECISION=1e-12
test: CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_INCLUDE_FLOAT
test: LDFLAGS = -lm -fopenmp
test: clean $(TARGET_UNIT_TEST_VEC) $(TARGET_UNIT_TEST_VEC_WRAPPER)
	@echo "========================================="
	@echo "Running Unit Tests (vec_rc module)"
	@echo "========================================="
	@./$(TARGET_UNIT_TEST_VEC)
	@echo ""
	@echo "========================================="
	@echo "Running Unit Tests (vec wrapper module)"
	@echo "========================================="
	@./$(TARGET_UNIT_TEST_VEC_WRAPPER)

test-debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(TEST_DIR) -fopenmp
test-debug: CFLAGS += -O0 -g -ggdb3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
test-debug: CFLAGS += -DUNITY_INCLUDE_DOUBLE -DUNITY_DOUBLE_PRECISION=1e-12
test-debug: CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_INCLUDE_FLOAT
test-debug: LDFLAGS = -lm -fopenmp -fsanitize=address -fsanitize=undefined -fsanitize=leak
test-debug: clean $(TARGET_UNIT_TEST_VEC) $(TARGET_UNIT_TEST_VEC_WRAPPER)
	@echo "========================================="
	@echo "Running Unit Tests (Debug Mode)"
	@echo "========================================="
	@echo "--- vec_rc module ---"
	@./$(TARGET_UNIT_TEST_VEC)
	@echo ""
	@echo "--- vec wrapper module ---"
	@./$(TARGET_UNIT_TEST_VEC_WRAPPER)

test-valgrind: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -I$(TEST_DIR) -fopenmp
test-valgrind: CFLAGS += -O0 -g -ggdb3
test-valgrind: CFLAGS += -DUNITY_INCLUDE_DOUBLE -DUNITY_DOUBLE_PRECISION=1e-12
test-valgrind: CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_INCLUDE_FLOAT
test-valgrind: LDFLAGS = -lm -fopenmp
test-valgrind: clean $(TARGET_UNIT_TEST_VEC) $(TARGET_UNIT_TEST_VEC_WRAPPER)
	@echo "========================================="
	@echo "Running Unit Tests with Valgrind"
	@echo "========================================="
	@echo "--- vec_rc module ---"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--verbose ./$(TARGET_UNIT_TEST_VEC)
	@echo ""
	@echo "--- vec wrapper module ---"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--verbose ./$(TARGET_UNIT_TEST_VEC_WRAPPER)

# ============================================================================
# Run Targets
# ============================================================================
run-vector: $(TARGET_VECTOR_BENCH)
	@echo "Running vector benchmark..."
	@./$(TARGET_VECTOR_BENCH)

run-matrix: $(TARGET_MATRIX_BENCH)
	@echo "Running matrix benchmark..."
	@./$(TARGET_MATRIX_BENCH)

run-all: run-vector run-matrix

benchmark: all
	@echo "========================================="
	@echo "Running Vector Benchmark (3 iterations)"
	@echo "========================================="
	@for i in 1 2 3; do \
		echo ""; \
		echo "=== Vector Iteration $$i ==="; \
		./$(TARGET_VECTOR_BENCH) | tail -10; \
	done
	@echo ""
	@echo "========================================="
	@echo "Running Matrix Benchmark (3 iterations)"
	@echo "========================================="
	@for i in 1 2 3; do \
		echo ""; \
		echo "=== Matrix Iteration $$i ==="; \
		./$(TARGET_MATRIX_BENCH) | tail -10; \
	done

profile-vector: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -fopenmp
profile-vector: CFLAGS += -O2 -pg
profile-vector: LDFLAGS = -lm -fopenmp -pg
profile-vector: clean $(TARGET_VECTOR_BENCH)
	@echo "Run './$(TARGET_VECTOR_BENCH)' then 'gprof $(TARGET_VECTOR_BENCH) gmon.out'"

profile-matrix: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR) -fopenmp
profile-matrix: CFLAGS += -O2 -pg
profile-matrix: LDFLAGS = -lm -fopenmp -pg
profile-matrix: clean $(TARGET_MATRIX_BENCH)
	@echo "Run './$(TARGET_MATRIX_BENCH)' then 'gprof $(TARGET_MATRIX_BENCH) gmon.out'"

# ============================================================================
# RULES
# ============================================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Vector benchmark executable
$(TARGET_VECTOR_BENCH): $(LIB_OBJECTS) $(BENCH_UTILS_OBJ) $(TEST_VECTOR_OBJ) | $(BIN_DIR)
	$(CC) $(LIB_OBJECTS) $(BENCH_UTILS_OBJ) $(TEST_VECTOR_OBJ) $(LDFLAGS) -o $@

# Matrix benchmark executable
$(TARGET_MATRIX_BENCH): $(LIB_OBJECTS) $(BENCH_UTILS_OBJ) $(TEST_MATRIX_OBJ) | $(BIN_DIR)
	$(CC) $(LIB_OBJECTS) $(BENCH_UTILS_OBJ) $(TEST_MATRIX_OBJ) $(LDFLAGS) -o $@

# Unit test executable for vec_rc
$(TARGET_UNIT_TEST_VEC): $(LIB_OBJECTS) $(UNITY_OBJ) $(UNIT_TEST_VEC_OBJ) | $(BIN_DIR)
	$(CC) $(LIB_OBJECTS) $(UNITY_OBJ) $(UNIT_TEST_VEC_OBJ) $(LDFLAGS) -o $@

# Unit test executable for vec wrapper
$(TARGET_UNIT_TEST_VEC_WRAPPER): $(LIB_OBJECTS) $(UNITY_OBJ) $(UNIT_TEST_VEC_WRAPPER_OBJ) | $(BIN_DIR)
	$(CC) $(LIB_OBJECTS) $(UNITY_OBJ) $(UNIT_TEST_VEC_WRAPPER_OBJ) $(LDFLAGS) -o $@

# Compile library sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile benchmark utilities
$(BUILD_DIR)/benchmark_utils.o: $(TEST_DIR)/benchmark_utils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile vector benchmark
$(BUILD_DIR)/test_vector_benchmark.o: $(TEST_DIR)/vector_benchmark.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile matrix benchmark
$(BUILD_DIR)/test_matrix_benchmark.o: $(TEST_DIR)/matrix_benchmark.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Unity framework
$(BUILD_DIR)/unity.o: $(TEST_DIR)/unity.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile unit test for vec_rc
$(BUILD_DIR)/test_vec_rc.o: $(TEST_DIR)/test_vec_rc.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile unit test for vec wrapper
$(BUILD_DIR)/test_vec.o: $(TEST_DIR)/test_vec.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# CLEAN
# ============================================================================

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) gmon.out

.PHONY: all debug release clean run-vector run-matrix run-all benchmark \
        profile-vector profile-matrix test test-debug test-valgrind