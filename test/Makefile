# Compiler and flags
CC := gcc
CFLAGS := -Wall -g
DEPFLAGS := -MMD -MP
GCOVFLAGS := -fprofile-arcs -ftest-coverage

# Default logging mode (simple logging) unless overridden
LOG_MODE ?= MU_LOG_ENABLE

ifeq ($(LOG_MODE), MU_LOG_ENABLE)
	CFLAGS += -DMU_LOG_ENABLE
endif

ifeq ($(LOG_MODE), MU_LOG_ENABLE_FORMATTED)
	CFLAGS += -DMU_LOG_ENABLE_FORMATTED
endif

# Directories
SRC_DIR := ../src
TEST_DIR := ../test
UNITY_DIR := ../test_support

OBJ_DIR := $(TEST_DIR)/obj
BIN_DIR := $(TEST_DIR)/bin
COVERAGE_DIR := $(TEST_DIR)/coverage

# Source files
SRC_FILES := $(SRC_DIR)/mu_log.c
TEST_FILES := $(TEST_DIR)/test_mu_log.c
UNITY_FILES := $(UNITY_DIR)/unity.c

SRC_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_FILES))
UNITY_OBJS := $(patsubst $(UNITY_DIR)/%.c, $(OBJ_DIR)/%.o, $(UNITY_FILES))

EXECUTABLES := $(BIN_DIR)/test_mu_log

.PHONY: all log_simple log_formatted log_disabled tests coverage-simple clean

# Default to simple logging if not specified
all: log_simple

log_simple:
	$(MAKE) tests LOG_MODE=MU_LOG_ENABLE

log_formatted:
	$(MAKE) tests LOG_MODE=MU_LOG_ENABLE_FORMATTED

log_disabled:
	$(MAKE) tests LOG_MODE=NONE

# Test target: build and run tests
tests: $(EXECUTABLES)
	@for test in $(EXECUTABLES); do \
		echo "Running $$test..."; \
		./$$test; \
	done

# Coverage testing (simple logging only)
coverage_simple:
	$(MAKE) clean
	$(MAKE) tests LOG_MODE=MU_LOG_ENABLE CFLAGS="$(CFLAGS) $(GCOVFLAGS)"
	@for test in $(EXECUTABLES); do \
		./$$test; \
	done
	lcov --capture --directory $(OBJ_DIR) --output-file coverage.info
	genhtml coverage.info --output-directory $(COVERAGE_DIR)
	@echo "Coverage report generated in $(COVERAGE_DIR)"

# Coverage testing (formatted logging)
coverage_formatted:
	$(MAKE) clean
	$(MAKE) tests LOG_MODE=MU_LOG_ENABLE_FORMATTED CFLAGS="-DMU_LOG_ENABLE_FORMATTED $(GCOVFLAGS)"
	@for test in $(EXECUTABLES); do \
		./$$test; \
	done
	lcov --capture --directory $(OBJ_DIR) --output-file coverage.info
	genhtml coverage.info --output-directory $(COVERAGE_DIR)
	@echo "Coverage report generated in $(COVERAGE_DIR)"

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(COVERAGE_DIR) coverage.info *.gcda *.gcno

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRC_DIR) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_DIR) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UNITY_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I$(UNITY_DIR) $(DEPFLAGS) -c $< -o $@

# Linking
$(BIN_DIR)/test_mu_log: $(TEST_OBJS) $(SRC_OBJS) $(UNITY_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(GCOVFLAGS) $^ -o $@

-include $(OBJ_DIR)/*.d
