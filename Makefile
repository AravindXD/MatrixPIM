CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -I./include
LDFLAGS = -pthread

BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/frontend/*.cpp) \
       $(wildcard $(SRC_DIR)/optimizer/*.cpp) \
       $(wildcard $(SRC_DIR)/backend/*.cpp) \
       $(wildcard $(SRC_DIR)/pim_isa/*.cpp) \
       $(wildcard $(SRC_DIR)/memorymap/*.cpp) \
       $(wildcard $(SRC_DIR)/utils/*.cpp)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable
TARGET = $(BIN_DIR)/pim_compiler

# Default target
all: directories $(TARGET)

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/frontend
	@mkdir -p $(BUILD_DIR)/optimizer
	@mkdir -p $(BUILD_DIR)/backend
	@mkdir -p $(BUILD_DIR)/pim_isa
	@mkdir -p $(BUILD_DIR)/memorymap
	@mkdir -p $(BUILD_DIR)/utils
	@mkdir -p $(BIN_DIR)

# Build executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run tests
test: $(TARGET)
	$(TARGET) -v test/test_matrix_mul.cpp test/output.asm

# Phony targets
.PHONY: all clean test directories

# Dependency files
-include $(OBJS:.o=.d)

