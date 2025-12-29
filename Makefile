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

# Targets
TARGET_VECTOR_BENCH = $(BIN_DIR)/vector_bench
TARGET_MATRIX_BENCH = $(BIN_DIR)/matrix_bench

# ============================================================================
# FLAGS
# ============================================================================

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)

CFLAGS += -O3 -march=native -mtune=native -flto \
          -fno-math-errno -fomit-frame-pointer -fno-plt -pipe

LDFLAGS = -lm -flto

# ============================================================================
# BUILD MODES
# ============================================================================

all: CFLAGS += -DNDEBUG
all: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)
debug: CFLAGS += -O0 -g -ggdb3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: LDFLAGS = -lm -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

release: CFLAGS += -DNDEBUG
release: CFLAGS += -fipa-pta -fprefetch-loop-arrays -floop-nest-optimize
release: $(TARGET_VECTOR_BENCH) $(TARGET_MATRIX_BENCH)

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

profile-vector: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)
profile-vector: CFLAGS += -O2 -pg
profile-vector: LDFLAGS = -lm -pg
profile-vector: clean $(TARGET_VECTOR_BENCH)
	@echo "Run './$(TARGET_VECTOR_BENCH)' then 'gprof $(TARGET_VECTOR_BENCH) gmon.out'"

profile-matrix: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)
profile-matrix: CFLAGS += -O2 -pg
profile-matrix: LDFLAGS = -lm -pg
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

# ============================================================================
# CLEAN
# ============================================================================

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) gmon.out

.PHONY: all debug release clean run-vector run-matrix run-all benchmark profile-vector profile-matrix