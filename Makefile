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

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(BUILD_DIR)/test_%.o)

TARGET = $(BIN_DIR)/$(PROJECT)

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
all: $(TARGET)

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)
debug: CFLAGS += -O0 -g -ggdb3 -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: LDFLAGS = -lm -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: $(TARGET)

release: CFLAGS += -DNDEBUG
release: CFLAGS += -fipa-pta -fprefetch-loop-arrays -floop-nest-optimize
release: $(TARGET)

run: all
	@./$(TARGET)

benchmark: all
	@echo "Running 3 iterations..."
	@for i in 1 2 3; do \
		echo "=== Iteration $$i ==="; \
		./$(TARGET) | tail -10; \
	done

profile: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -I$(INC_DIR)
profile: CFLAGS += -O2 -pg
profile: LDFLAGS = -lm -pg
profile: clean $(TARGET)
	@echo "Run './$(TARGET)' then 'gprof $(TARGET) gmon.out'"

# ============================================================================
# RULES
# ============================================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(LIB_OBJECTS) $(TEST_OBJECTS) | $(BIN_DIR)
	$(CC) $(LIB_OBJECTS) $(TEST_OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# CLEAN
# ============================================================================

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all debug release clean
